#ifndef _OUTPUT_H
#define _OUTPUT_H

#include "sim.h"

class Output
{
public:
    Sim *sim;
    void outputData();

    void writeFluidVelocity(const std::string &path);
    void writeFluidDensity(const std::string &path);
    void writeFluidPressure(const std::string &path);

    void writeSolidVelocity(const std::string &path);
    void writeSolidPressure(const std::string &path);
    void writeSolidDensity(const std::string &path);
    void writeSolidStress(const std::string &path);

    void writeVirtNormal(const std::string &path);
    void writeVirtPressure(const std::string &path);
};

#endif