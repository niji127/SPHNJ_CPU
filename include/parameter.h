#ifndef _PARAMETER_H
#define _PARAMETER_H

#include "config.h"
#include "global.h"
#include "properties.h"
#include "physics.h"
#include "domain.h"
#include "sim_time.h"
#include "kernel.h"

class Parameter
{
public:
    Physics physics;
    Fluid_Properties fluid;
    Solid_Properties solid;
    Virt_Properties virt;
    Domain domain;
    Sim_Time time;
    Kernel kernel;
    void initiate()
    {
        readParticleNumber();
        readFieldParameter();
        initiateParameter();
    }

    void showInfo();
    void initiateParameter();

    // read parameters in input.ini
    void readFieldParameter();

    // read particle number (different files)
    void readParticleNumber();

    bool hasFluid();
    bool hasSolid();
    bool hasVirtual();

    Sim_Time *getTime();
};

#endif