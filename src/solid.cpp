#include "calculation.h"

void Calculation::coupleCalculation()
{
    if (!(sim->parameter.hasFluid()))
        return;
    if (!(sim->parameter.hasSolid()))
        return;
}
void Calculation::solidCalculation()
{
    if (!(sim->parameter.hasSolid()))
        return;
}