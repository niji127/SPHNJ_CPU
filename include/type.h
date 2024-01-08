#ifndef _TYPE_H
#define _TYPE_H

enum Phase_Type
{
    LIQUID,
    GAS
};

enum Boundary_Type
{
    SLIP,
    NO_SLIP,
    MOVING
};

enum Fluid_Boundary_Type
{
    SPLASH,
    FREE_SURFACE,
    VICINITY,
    INNER
};

enum Material_Type
{
    LINEAR_ELASTIC,
    AUXETIC
};

enum Plastic_Type
{
    HYPER_ELASTIC,
    PLASTIC
};

enum Solid_Boundary_Type
{
    UNCONSTRAINED_PARTICLE,
    FIXED_PARTICLE
};

enum Constitution_Type
{
    STRESS_2D,
    STRAIN_2D
};

enum Near_Particle
{
    NO_PARTICLE,
    HAS_PARTICLE,
    HAS_NEAR_PARTICLE,
};

enum Transient_Type
{
    TRANSIENT,
    STEADY
};

#endif