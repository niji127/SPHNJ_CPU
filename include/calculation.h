#ifndef _CALCULATION_H
#define _CALCULATION_H

#include "sim.h"
#include "output.h"

class Calculation
{
public:
    Sim *sim;
    Output out;
    Calculation(Sim *sim)
    {
        out.sim = sim;
        this->sim = sim;
    }
    void calculation();

    // pair
    int findPositionCell(const Eigen::Vector2f &pos);
    void particleCellUpdate();
    void fillCell();
    void fillCellFirst();
    
    // fluid
    void fluidCalculation();

    void clearFluidVariables(Fluid_Particle &particle);
    void fluidFluidCalculation(Fluid_Particle &particle_i, Fluid_Particle &particle_j);
    void fluidSolidCalculation(Fluid_Particle &particle_i, Solid_Particle &particle_j);
    void fluidVirtCalculation(Fluid_Particle &particle_i, Virt_Particle &particle_j);

    Eigen::Vector2f shiftDisplacementCalculate(Fluid_Particle &particle);
    float densityToPressure(float density, Phase_Type phase);
    float pressureToDensity(float pressure, Phase_Type phase);

    // virt
    void virtStaticCalculation();
    void virtUpdate();

    void clearVirtVariables(Virt_Particle &particle);

    void coupleCalculation();
    void solidCalculation();
};

#endif