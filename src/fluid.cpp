
#include "calculation.h"

void Calculation::fluidCalculation()
{
    if (!(sim->parameter.hasFluid()))
        return;
    float impact_dis_2 = sim->parameter.kernel.impact_distance_square;
    int fluid_num = sim->fluids.size();

#pragma omp parallel for
    for (int i = 0; i < fluid_num;i++)
    {
        Fluid_Particle &fluid_i = sim->fluids[i];
        Cell &cell_i = sim->cells[fluid_i.cell_id];
        Eigen::Vector2f &x_i = fluid_i.position;

        clearFluidVariables(fluid_i);
        for (auto cell_j : cell_i.neighbor_cell)
        {
            if (cell_j == nullptr)
                continue;
            for (auto fluid_j : cell_j->fluid_list)
            {
                Eigen::Vector2f dx = fluid_j->position - x_i;
                if (dx.squaredNorm() < impact_dis_2)
                    fluidFluidCalculation(fluid_i, *fluid_j);
            }
            for (auto solid_j : cell_j->solid_list)
            {
                Eigen::Vector2f dx = solid_j->position - x_i;
                if (dx.squaredNorm() < impact_dis_2)
                    fluidSolidCalculation(fluid_i, *solid_j);
            }
            for (auto virt_j : cell_j->virt_list)
            {
                Eigen::Vector2f dx = virt_j->position - x_i;
                if (dx.squaredNorm() < impact_dis_2)
                    fluidVirtCalculation(fluid_i, *virt_j);
            }
        }
    }

    Eigen::Vector2f &gravity = sim->parameter.physics.gravity;
    Domain &domain = sim->parameter.domain;
    float xmin = domain.domain_xmin[0];
    float xmax = domain.domain_xmax[0];
    float ymin = domain.domain_xmin[1];
    float ymax = domain.domain_xmax[1];
    float dt = sim->parameter.time.dt;

#pragma omp parallel for
    for (int i = 0; i < fluid_num; i++)
    {
        Fluid_Particle &fluid = sim->fluids[i];
        Phase_Type phase = fluid.phase_type;
        Eigen::Vector2f correction = shiftDisplacementCalculate(fluid);
        fluid.position += fluid.velocity * dt + correction;
        fluid.velocity += (fluid.du_dt + gravity) * dt;
        fluid.density += fluid.drho_dt * dt;
        fluid.density = std::max(fluid.density, sim->parameter.fluid.min_density[phase]);
        fluid.pressure = densityToPressure(fluid.density, phase);

        fluid.position(0) = std::max(fluid.position(0), xmin);
        fluid.position(0) = std::min(fluid.position(0), xmax);
        fluid.position(1) = std::max(fluid.position(1), ymin);
        fluid.position(1) = std::min(fluid.position(1), ymax);
    }
}

Eigen::Vector2f Calculation::shiftDisplacementCalculate(Fluid_Particle &particle)
{
    float coef = sim->parameter.fluid.particle_shift_coef;
    float d_2 = sim->parameter.kernel.diameter_square;
    Eigen::Vector2f displacement = -coef * d_2 * particle.gradient;

    if (particle.phase_type == GAS)
    {
        particle.fluid_boundary_type = INNER;
        return displacement;
    }

    if (particle.color < 1.5f)
    {
        if (particle.near_particle == NO_PARTICLE)
        {
            particle.fluid_boundary_type = SPLASH;
            return Eigen::Vector2f::Zero();
        }
        else
        {
            particle.fluid_boundary_type = FREE_SURFACE;
            Eigen::Vector2f normal = particle.gradient.normalized();
            if (normal.norm() < EPS_FOR_SUM)
                normal.setZero();
            return (Eigen::Matrix2f::Identity() - normal * normal.transpose()) * displacement;
        }
    }

    if (particle.color < 2.0f)
    {
        if (particle.near_particle == HAS_NEAR_PARTICLE)
        {
            particle.fluid_boundary_type = VICINITY;
            Eigen::Vector2f normal = particle.gradient.normalized();
            if (normal.norm() < EPS_FOR_SUM)
                normal.setZero();
            return (Eigen::Matrix2f::Identity() - normal * normal.transpose()) * displacement;
        }
    }
    particle.fluid_boundary_type = INNER;
    return displacement;
}
float Calculation::densityToPressure(float density, Phase_Type phase)
{
    Fluid_Properties &fluid = sim->parameter.fluid;
    float density_inverse = fluid.reference_density_inverse[phase];
    float coef = fluid.coefficient_rho2p[phase];
    float gamma = fluid.gamma[phase];

    float pressure = (powf(density * density_inverse, gamma) - 1.0f) * coef;
    return std::max(pressure, fluid.min_pressure);
}
float Calculation::pressureToDensity(float pressure, Phase_Type phase)
{
    Fluid_Properties &fluid = sim->parameter.fluid;
    float density_reference = fluid.reference_density[phase];
    float coef = fluid.coefficient_p2rho[phase];
    float gamma_inv = fluid.gamma_inv[phase];

    return density_reference * powf(pressure * coef + 1.0f, gamma_inv);
}

void Calculation::fluidFluidCalculation(Fluid_Particle &particle_i, Fluid_Particle &particle_j)
{
    Kernel &kernel = sim->parameter.kernel;
    Fluid_Properties &fluid = sim->parameter.fluid;
    float coef = kernel.kernel_differential;
    float h_inv_2 = kernel.smoothing_length_inverse_square;
    float coef3to2 = kernel.coefficient_3d_to_2d;
    float impact_dis_2 = kernel.impact_distance_square;

    Eigen::Vector2f &x_i = particle_i.position;
    Eigen::Vector2f &x_j = particle_j.position;

    Eigen::Vector2f &u_i = particle_i.velocity;
    Eigen::Vector2f &u_j = particle_j.velocity;

    float p_i = particle_i.pressure;
    float p_j = particle_j.pressure;

    float rho_i = particle_i.density;
    float rho_j = particle_j.density;

    Phase_Type phase_i = particle_i.phase_type;
    Phase_Type phase_j = particle_j.phase_type;
    float cs = fluid.sound_speed[phase_i];

    Eigen::Vector2f dx = x_j - x_i;
    float q_2 = dx.squaredNorm() * h_inv_2;
    float kernel_coef = expf(-q_2) * coef * coef3to2;

    float ps = (rho_i * p_j + rho_j * p_i) / (rho_i + rho_j);
    float du = (u_i - u_j).dot(dx.normalized());
    if (du >= 0.0f)
        ps += rho_i * rho_j * du / (rho_i + rho_j) * std::min(3.0f * du, cs);

    Eigen::Vector2f vs = (rho_i * u_i + rho_j * u_j) / (rho_i + rho_j);
    vs += (p_i - p_j) / (rho_i + rho_j) / cs * dx.normalized();

    particle_i.du_dt += -2.0f * ps / rho_i * dx * kernel_coef;
    particle_i.drho_dt += 2.0f * rho_i * (u_i - vs).dot(dx) * kernel_coef;

    if (!(phase_i == LIQUID && phase_j == GAS))
    {
        particle_i.color += dx.dot(dx) * kernel_coef;
        particle_i.gradient += dx * kernel_coef;
    }
    if (particle_j.fluid_boundary_type == FREE_SURFACE && phase_i == LIQUID && &particle_i != &particle_j)
    {
        if (dx.squaredNorm() < 0.25f * impact_dis_2)
            particle_i.near_particle = HAS_NEAR_PARTICLE;
        else if (particle_i.near_particle == NO_PARTICLE)
            particle_i.near_particle = HAS_PARTICLE;
    }

    if (!fluid.is_fluid_viscid)
        return;
    float vis = (u_j - u_i).dot(dx);
    vis /= dx.squaredNorm() + 0.01f * kernel.smoothing_length_square;
    particle_i.du_dt += 8.0f * fluid.viscosity[phase_i] * vis / rho_i * dx * kernel_coef;
}

void Calculation::fluidSolidCalculation(Fluid_Particle &particle_i, Solid_Particle &particle_j) {}
void Calculation::fluidVirtCalculation(Fluid_Particle &particle_i, Virt_Particle &particle_j)
{
    Kernel &kernel = sim->parameter.kernel;
    Fluid_Properties &fluid = sim->parameter.fluid;
    float coef = kernel.kernel_differential;
    float h_inv_2 = kernel.smoothing_length_inverse_square;
    float coef3to2 = kernel.coefficient_3d_to_2d;
    float impact_dis_2 = kernel.impact_distance_square;

    Eigen::Vector2f &x_i = particle_i.position;
    Eigen::Vector2f &x_j = particle_j.position;

    Eigen::Vector2f &u_i = particle_i.velocity;
    Eigen::Vector2f &u_j = particle_j.velocity;

    float p_i = particle_i.pressure;
    float p_j = particle_j.pressure;

    float rho_i = particle_i.density;
    float rho_j = particle_j.density;

    Phase_Type phase_i = particle_i.phase_type;
    Phase_Type phase_j = particle_j.phase_type;
    float cs = fluid.sound_speed[phase_i];

    Eigen::Vector2f dx = x_j - x_i;
    float q_2 = dx.squaredNorm() * h_inv_2;
    float kernel_coef = expf(-q_2) * coef * coef3to2;

    float ps = (rho_i * p_j + rho_j * p_i) / (rho_i + rho_j);
    float du = (u_i - u_j).dot(dx.normalized());
    if (du >= 0.0f)
        ps += rho_i * rho_j * du / (rho_i + rho_j) * std::min(3.0f * du, cs);

    Eigen::Vector2f vs = (rho_i * u_i + rho_j * u_j) / (rho_i + rho_j);
    vs += (p_i - p_j) / (rho_i + rho_j) / cs * dx.normalized();

    particle_i.du_dt += -2.0f * ps / rho_i * dx * kernel_coef;
    particle_i.drho_dt += 2.0f * rho_i * (u_i - vs).dot(dx) * kernel_coef;

    if (!(phase_i == LIQUID && phase_j == GAS))
    {
        particle_i.color += dx.dot(dx) * kernel_coef;
        particle_i.gradient += dx * kernel_coef;
    }

    if (!fluid.is_fluid_viscid)
        return;
    float vis = (u_j - u_i).dot(dx);
    vis /= dx.squaredNorm() + 0.01f * kernel.smoothing_length_square;
    particle_i.du_dt += 8.0f * fluid.viscosity[phase_i] * vis / rho_i * dx * kernel_coef;
}
void Calculation::clearFluidVariables(Fluid_Particle &particle)
{
    particle.du_dt.setZero();
    particle.drho_dt = 0.0f;
    particle.color = 0.0f;
    particle.gradient.setZero();
    particle.near_particle = NO_PARTICLE;
}