#ifndef _PARTICLE_H
#define _PARTICLE_H

#include "Eigen/Dense"

#include "type.h"
#include "parameter.h"

class Fluid_Particle
{
public:
    Eigen::Vector2f position, velocity;
    float density, pressure;
    Phase_Type phase_type;
    int cell_id;

    Fluid_Boundary_Type fluid_boundary_type;
    Near_Particle near_particle;
    Eigen::Vector2f gradient, du_dt;
    float color, drho_dt;

    Fluid_Particle(const std::string &str)
    {
        std::istringstream string_to_num(str);
        int phase_id;

        string_to_num >> position[0] >> position[1] >> velocity[0] >> velocity[1] >> density >> pressure >> phase_id;

        phase_type = Phase_Type(phase_id);
        cell_id = -1;
        fluid_boundary_type = FREE_SURFACE;
        near_particle = NO_PARTICLE;
        gradient.setZero();
        du_dt.setZero();
        color = 0.0f;
        drho_dt = 0.0f;
    }
};

class Solid_Particle
{
public:
    Eigen::Vector2f position, reference_position, velocity;
    Solid_Boundary_Type boundary_type;
    Eigen::Matrix2d max_strain;
    Material_Type material_type;
    float pressure;
    int cell_id;

    Eigen::Matrix2d deformation, stress_1st_piola, correction;
    Eigen::Vector2d du_dt;
    Eigen::Vector2f normal, couple_dudt;

    Solid_Particle(const std::string &str)
    {
        std::istringstream string_to_num(str);
        int material_type_id, boundary_type_id;

        string_to_num >> position[0] >> position[1] >> velocity[0] >> velocity[1] >> reference_position[0] >> reference_position[1] >> material_type_id >> boundary_type_id;

        boundary_type = Solid_Boundary_Type(boundary_type_id);
        max_strain = Eigen::Matrix2d::Zero();
        material_type = Material_Type(material_type_id);
        pressure = 0.0f;
        cell_id = -1;

        deformation.setIdentity();
        stress_1st_piola.setZero();
        correction.setIdentity();
        du_dt.setZero();
        normal.setZero();
        couple_dudt.setZero();
    }
};

class Virt_Particle
{
public:
    Eigen::Vector2f position, normal, velocity;
    float density, pressure;
    Phase_Type phase_type;
    Boundary_Type boundary_type;
    int cell_id;

    Virt_Particle(const std::string &str, const Parameter &parameter)
    {
        std::istringstream string_to_num(str);

        int boundary_type_id;
        string_to_num >> position[0] >> position[1] >> boundary_type_id;

        normal.setZero();
        velocity.setZero();
        density = parameter.fluid.reference_density[0];
        pressure = 0.0f;
        phase_type = LIQUID;
        boundary_type = Boundary_Type(boundary_type_id);
        cell_id = -1;
    }
};

#endif