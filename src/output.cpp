#include <windows.h>

#include "output.h"

void Output::outputData()
{
    Sim_Time *time = sim->parameter.getTime();
    if (!(time->isOutputData()))
        return;

    std::cout << "Outputing Data..." << std::endl;
    std::cout << "--time=" << time->current_time - time->dt << "s step=" << time->i - 1 << "--" << std::endl;
    std::string file_path = "..\\out";

    if (sim->parameter.hasFluid())
    {
        std::string fluid_file_path = file_path + "\\fluid";
        CreateDirectory(fluid_file_path.c_str(), NULL);
        writeFluidVelocity(fluid_file_path);
        writeFluidDensity(fluid_file_path);
        writeFluidPressure(fluid_file_path);
    }
    if (sim->parameter.hasSolid())
    {
        std::string solid_file_path = file_path + "\\solid";
        CreateDirectory(solid_file_path.c_str(), NULL);
        writeSolidVelocity(solid_file_path);
        writeSolidPressure(solid_file_path);
        writeSolidDensity(solid_file_path);
        writeSolidStress(solid_file_path);
    }
    if (sim->parameter.hasVirtual())
    {
        std::string virt_file_path = file_path + "\\virtual";
        CreateDirectory(virt_file_path.c_str(), NULL);
        writeVirtNormal(virt_file_path);
        writeVirtPressure(virt_file_path);
    }
}

void Output::writeFluidVelocity(const std::string &path)
{
    int fluid_num = sim->fluids.size();
    Sim_Time *time = sim->parameter.getTime();
    std::fstream output;
    output.open(path + "\\fluid_velocity-" + std::to_string(time->file_step) + ".vtk", std::ios::out);

    output << "# vtk DataFile Version 2.0" << std::endl;
    output << "Velocity vector file" << std::endl;
    output << "ASCII" << std::endl;
    output << "DATASET POLYDATA" << std::endl;
    output << "POINTS " << fluid_num << " float" << std::endl;
    for (auto &fluid : sim->fluids)
        output << fluid.position(0) << " " << fluid.position(1) << " 0.0\n";
    output << "POINT_DATA " << fluid_num << std::endl;
    output << "VECTORS velocity float " << std::endl;
    for (auto &fluid : sim->fluids)
        output << fluid.velocity(0) << " " << fluid.velocity(1) << " " << fluid.density << "\n";
    output.close();
}
void Output::writeFluidDensity(const std::string &path) {}
void Output::writeFluidPressure(const std::string &path) {}

void Output::writeSolidVelocity(const std::string &path) {}
void Output::writeSolidPressure(const std::string &path) {}
void Output::writeSolidDensity(const std::string &path) {}
void Output::writeSolidStress(const std::string &path) {}

void Output::writeVirtNormal(const std::string &path)
{
    int virt_num = sim->virts.size();
    Sim_Time *time = sim->parameter.getTime();
    std::fstream output;
    output.open(path + "\\virt_normal-" + std::to_string(time->file_step) + ".vtk", std::ios::out);

    output << "# vtk DataFile Version 2.0" << std::endl;
    output << "Velocity vector file" << std::endl;
    output << "ASCII" << std::endl;
    output << "DATASET POLYDATA" << std::endl;
    output << "POINTS " << virt_num << " float" << std::endl;
    for (auto &virt : sim->virts)
        output << virt.position(0) << " " << virt.position(1) << " 0.0\n";
    output << "POINT_DATA " << virt_num << std::endl;
    output << "VECTORS normal float " << std::endl;
    for (auto &virt : sim->virts)
        output << virt.normal(0) << " " << virt.normal(1) << " 0.0\n";
    output.close();
}
void Output::writeVirtPressure(const std::string &path) {}