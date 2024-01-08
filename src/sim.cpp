#include <omp.h>
#include "sim.h"

void Sim::readData()
{
    std::cout << "\nReading Particles..." << std::endl;
    bool error_flag(false);
    std::string path = "..\\data";

    clock_t start, end;
    start = clock();

    error_flag = readFluidData(path);
    error_flag = readSolidData(path) || error_flag;
    error_flag = readVirtData(path) || error_flag;

    end = clock();
    printf("read time=%dms\n", end - start);

    if (error_flag)
        throw std::invalid_argument("FAILED TO READ PARTICLES DATA");
}

bool Sim::readFluidData(const std::string &path)
{
    int fluid_num = parameter.fluid.number_total;
    if (fluid_num == 0)
    {
        std::cout << "\nno fluid particle" << std::endl;
        return false;
    }

    if (fluid_num < 0)
    {
        std::cout << "fluid particle number should >= 0" << std::endl;
        return true;
    }

    std::ifstream infile;
    infile.open(path + "\\fluid_particle.dat", std::ios::in);
    if (!infile.is_open())
    {
        std::cout << "failed to read fluid particles data" << std::endl;
        return true;
    }

    std::string str;
    std::getline(infile, str);
    while (std::getline(infile, str))
        fluids.push_back(Fluid_Particle(str));
    infile.close();

    return false;
}
bool Sim::readSolidData(const std::string &path)
{
    int solid_num = parameter.solid.number;
    if (solid_num == 0)
    {
        std::cout << "\nno solid particle" << std::endl;
        return false;
    }
    if (solid_num < 0)
    {
        std::cout << "solid particle number should >= 0" << std::endl;
        return true;
    }
    std::ifstream infile;
    infile.open(path + "\\solid_particle.dat", std::ios::in);
    if (!infile.is_open())
    {
        std::cout << "failed to read solid particles data" << std::endl;
        return true;
    }

    std::string str;
    std::getline(infile, str);
    while (std::getline(infile, str))
        solids.push_back(Solid_Particle(str));
    infile.close();

    return false;
}
bool Sim::readVirtData(const std::string &path)
{
    int virt_num = parameter.virt.number;
    if (virt_num == 0)
    {
        std::cout << "\nno virtual particle" << std::endl;
        return false;
    }

    if (virt_num < 0)
    {
        std::cout << "virtual particle number should >= 0" << std::endl;
        return true;
    }

    std::ifstream infile;
    infile.open(path + "\\virt_particle.dat", std::ios::in);
    if (!infile.is_open())
    {
        std::cout << "failed to read virtual particles data" << std::endl;
        return true;
    }

    std::string str;
    std::getline(infile, str);
    while (std::getline(infile, str))
        virts.push_back(Virt_Particle(str, parameter));
    infile.close();

    return false;
}

void Sim::createCell()
{
    int cell_number = parameter.domain.cell_number_total;
    int(&cell_n)[2] = parameter.domain.cell_number;
    float(&dx)[2] = parameter.domain.interval;

    cells.resize(parameter.domain.cell_number_total);

#pragma omp parallel for
    for (int i = 0; i < cell_n[0]; i++)
    {
        for (int j = 0; j < cell_n[1]; j++)
        {
            int id = i * cell_n[1] + j;
            Cell &cell = cells[id];
            cell.id = id;
            cell.neighbor_cell[4] = &cell;
            if (i != 0)
                cell.neighbor_cell[1] = &cell - cell_n[1];
            if (i != cell_n[0] - 1)
                cell.neighbor_cell[7] = &cell + cell_n[1];
            if (j != 0)
                cell.neighbor_cell[3] = &cell - 1;
            if (j != cell_n[1] - 1)
                cell.neighbor_cell[5] = &cell + 1;

            if ((i != 0) && (j != 0))
                cell.neighbor_cell[0] = &cell - 1 - cell_n[1];
            if ((i != 0) && (j != cell_n[1] - 1))
                cell.neighbor_cell[2] = &cell + 1 - cell_n[1];
            if ((i != cell_n[0] - 1) && (j != 0))
                cell.neighbor_cell[6] = &cell - 1 + cell_n[1];
            if ((i != cell_n[0] - 1) && (j != cell_n[1] - 1))
                cell.neighbor_cell[8] = &cell + 1 + cell_n[1];
        }
    }
}
