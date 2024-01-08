#ifndef _SIM_H
#define _SIM_H

#include <vector>

#include "particle.h"
#include "parameter.h"
#include "cell.h"

class Sim
{
public:
    std::vector<Fluid_Particle> fluids;
    std::vector<Solid_Particle> solids;
    std::vector<Virt_Particle> virts;
    std::vector<Cell> cells;
    Parameter parameter;
    Sim()
    {
        parameter.initiate();
        parameter.showInfo();
        createCell();
        fluids.reserve(parameter.fluid.number_total);
        solids.reserve(parameter.solid.number);
        virts.reserve(parameter.virt.number);
        readData();
        std::cout << "\nInitialization completed" << std::endl;
    }

    void readData();
    void createCell();
    bool readFluidData(const std::string &path);
    bool readSolidData(const std::string &path);
    bool readVirtData(const std::string &path);
};

#endif