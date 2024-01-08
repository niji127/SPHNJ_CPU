#include "calculation.h"
#include <float.h>

void Calculation::virtStaticCalculation()
{
    if (!sim->parameter.hasVirtual())
        return;

    Kernel &kernel = sim->parameter.kernel;
    float impact_dis_2 = kernel.impact_distance_square;
    float coef = kernel.kernel_differential;
    float h_inv_2 = kernel.smoothing_length_inverse_square;
    float coef3to2 = kernel.coefficient_3d_to_2d;

    int virt_num = sim->virts.size();

#pragma omp parallel for
    for (int i = 0; i < virt_num;i++)
    {
        Virt_Particle &virt_i = sim->virts[i];
        virt_i.normal.setZero();
        Eigen::Vector2f &x_i = virt_i.position;

        Cell &cell_i = sim->cells[virt_i.cell_id];
        for (auto cell_j : cell_i.neighbor_cell)
        {
            if (cell_j == nullptr)
                continue;
            for (auto &virt_j : cell_j->virt_list)
            {
                Eigen::Vector2f dx = virt_j->position - x_i;
                if (dx.squaredNorm() < impact_dis_2)
                {
                    float q_2 = dx.squaredNorm() * h_inv_2;
                    float kernel_coef = expf(-q_2) * coef * coef3to2;
                    virt_i.normal -= dx * kernel_coef;
                }
            }
        }
        if (virt_i.normal.norm() < EPS_FOR_SUM)
            virt_i.normal.setZero();
        else
            virt_i.normal.normalize();
    }
}

void Calculation::virtUpdate()
{
    if (!sim->parameter.hasVirtual())
        return;

    Eigen::Vector2f &gravity = sim->parameter.physics.gravity;
    int virt_num = sim->virts.size();

#pragma omp parallel for
    for (int i = 0; i < virt_num; i++)
    {
        Virt_Particle &virt_i = sim->virts[i];
        Cell &cell_i = sim->cells[virt_i.cell_id];
        Eigen::Vector2f &x_i = virt_i.position;
        float nearest_distance_square = FLT_MAX;

        for (auto cell_j : cell_i.neighbor_cell)
        {
            if (cell_j == nullptr)
                continue;
            for (auto fluid_j : cell_j->fluid_list)
            {
                Eigen::Vector2f dx = fluid_j->position - x_i;
                if (dx.squaredNorm() < nearest_distance_square)
                {
                    nearest_distance_square = dx.squaredNorm();

                    float rho_j = fluid_j->density;
                    float p_j = fluid_j->pressure;
                    Eigen::Vector2f &u_j = fluid_j->velocity;
                    Phase_Type phase_j = fluid_j->phase_type;

                    virt_i.phase_type = phase_j;
                    virt_i.pressure = p_j - rho_j * gravity.dot(dx);
                    virt_i.density = pressureToDensity(p_j, phase_j);
                    switch (virt_i.boundary_type)
                    {
                    case SLIP:
                    {
                        Eigen::Vector2f &n = virt_i.normal;
                        virt_i.velocity = u_j - 2.0f * u_j.dot(n) * n;
                    }
                    break;
                    case NO_SLIP:
                        virt_i.velocity = -u_j;
                        break;
                    }
                }
            }
        }
    }
}