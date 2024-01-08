#ifndef _CELL_H
#define _CELL_H

#include "parameter.h"
#include "global.h"

class Cell
{
public:
    int id;
    std::array<Cell *, 9> neighbor_cell;

    std::vector<Fluid_Particle *> fluid_list;
    std::vector<Solid_Particle *> solid_list;
    std::vector<Virt_Particle *> virt_list;

    std::vector<Fluid_Particle *> fluid_list_previous;
    std::vector<Solid_Particle *> solid_list_previous;
    std::vector<Virt_Particle *> virt_list_previous;

    Cell()
    {
        for (auto &cell : neighbor_cell)
            cell = nullptr;
    }
};

#endif
