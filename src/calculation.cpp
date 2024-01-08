#include <windows.h>

#include "calculation.h"

void Calculation::calculation()
{
    Sim_Time *time = sim->parameter.getTime();

    clock_t start, end,start_l,end_l;

    fillCellFirst();
    virtStaticCalculation();

    omp_set_num_threads(20);
    start = clock();
    start_l = clock();
    while (time->addStep())
    {
        // out.outputData();
        if (!(time->i%1))
        {
            end_l = clock();
            printf("dt=%dms\n", end_l - start_l);
            start_l = clock();
        }
        particleCellUpdate();
        fillCell();
        coupleCalculation();
        virtUpdate();
        fluidCalculation();
        solidCalculation();
    }
    end = clock();
    printf("total time=%dms\n", end - start);
    out.outputData();
}