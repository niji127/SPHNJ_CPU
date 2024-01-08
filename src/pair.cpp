#include "calculation.h"

int Calculation::findPositionCell(const Eigen::Vector2f &pos)
{
    Domain *domain = &(sim->parameter.domain);
    float xmin = domain->cell_xmin[0];
    float ymin = domain->cell_xmin[1];
    float inter_inv_x = domain->interval_inv[0];
    float inter_inv_y = domain->interval_inv[1];
    int block_nx = domain->cell_number[0];
    int block_ny = domain->cell_number[1];

    return (int)((pos[1] - ymin) * inter_inv_y) + (int)((pos[0] - xmin) * inter_inv_x) * block_ny;
}

void Calculation::particleCellUpdate()
{
    int fluid_num = sim->fluids.size();
    int solid_num = sim->solids.size();
    int virt_num = sim->virts.size();

#pragma omp parallel for
    for (int i = 0; i < fluid_num; i++)
    {
        Fluid_Particle &fluid = sim->fluids[i];
        fluid.cell_id = findPositionCell(fluid.position);
    }

#pragma omp parallel for
    for (int i = 0; i < solid_num; i++)
    {
        Solid_Particle &solid = sim->solids[i];
        solid.cell_id = findPositionCell(solid.position);
    }

#pragma omp parallel for
    for (int i = 0; i < virt_num; i++)
    {
        Virt_Particle &virt = sim->virts[i];
        virt.cell_id = findPositionCell(virt.position);
    }
}
void Calculation::fillCell()
{
    int cell_num = sim->cells.size();
#pragma omp parallel for
    for (int i = 0; i < cell_num;i++)
    {
        Cell &cell_i = sim->cells[i];
        cell_i.fluid_list_previous = cell_i.fluid_list;
        cell_i.fluid_list.resize(0);
        cell_i.solid_list_previous = cell_i.solid_list;
        cell_i.solid_list.resize(0);
        cell_i.virt_list_previous = cell_i.virt_list;
        cell_i.virt_list.resize(0);
    }

#pragma omp parallel for
    for (int i = 0; i < cell_num; i++)
    {
        Cell &cell_i = sim->cells[i];
        for (auto cell_j : cell_i.neighbor_cell)
        {
            if (cell_j == nullptr)
                continue;
            for (auto fluid : cell_j->fluid_list_previous)
            {
                if (fluid->cell_id == cell_i.id)
                    cell_i.fluid_list.push_back(fluid);
            }
            for (auto solid : cell_j->solid_list_previous)
            {
                if (solid->cell_id == cell_i.id)
                    cell_i.solid_list.push_back(solid);
            }
            for (auto virt : cell_j->virt_list_previous)
            {
                if (virt->cell_id == cell_i.id)
                    cell_i.virt_list.push_back(virt);
            }
        }
    }
}
void Calculation::fillCellFirst()
{
    std::cout << "Filling cells" << std::endl;
    clock_t start, end;
    start = clock();

    int fluid_num = sim->fluids.size();
    int solid_num = sim->solids.size();
    int virt_num = sim->virts.size();

    for (int i = 0; i < fluid_num; i++)
    {
        Fluid_Particle &fluid = sim->fluids[i];
        fluid.cell_id = findPositionCell(fluid.position);
        sim->cells[fluid.cell_id].fluid_list.push_back(&fluid);
    }

    for (int i = 0; i < solid_num; i++)
    {
        Solid_Particle &solid = sim->solids[i];
        solid.cell_id = findPositionCell(solid.position);
        sim->cells[solid.cell_id].solid_list.push_back(&solid);
    }

    for (int i = 0; i < virt_num; i++)
    {
        Virt_Particle &virt = sim->virts[i];
        virt.cell_id = findPositionCell(virt.position);
        sim->cells[virt.cell_id].virt_list.push_back(&virt);
    }

//     particleCellUpdate();

//     int cell_num = sim->cells.size();
// #pragma omp parallel for
//     for (int i = 0; i < cell_num;i++)
//     {
//         Cell &cell = sim->cells[i];
//         for (auto &fluid : sim->fluids)
//         {
//             if (fluid.cell_id == cell.id)
//                 cell.fluid_list.push_back(&fluid);
//         }
//         for (auto &solid : sim->solids)
//         {
//             if (solid.cell_id == cell.id)
//                 cell.solid_list.push_back(&solid);
//         }
//         for (auto &virt : sim->virts)
//         {
//             if (virt.cell_id == cell.id)
//                 cell.virt_list.push_back(&virt);
//         }
//     }

    end = clock();
    printf("Completed in %dms\n", end - start);
}