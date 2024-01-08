#include <iostream>
#include <omp.h>

#include "sim.h"
#include "calculation.h"

int main()
{
    omp_set_num_threads(1);
    Sim *sim = new Sim;
    Calculation *calculation = new Calculation(sim);
    calculation->calculation();

    delete sim, calculation;

    return 0;
}