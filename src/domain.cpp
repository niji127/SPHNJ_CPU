#include "domain.h"

bool Domain::readNumber(const std::string &path)
{
    return false;
}

bool Domain::readParameter(const Config &config)
{
    domain_xmin[0] = config.Read("xmin", 0.0f);
    domain_xmin[1] = config.Read("ymin", 0.0f);
    domain_xmax[0] = config.Read("xmax", 0.0f);
    domain_xmax[1] = config.Read("ymax", 0.0f);

    if (domain_xmax[0] - domain_xmin[0] == 0.0f || domain_xmax[1] - domain_xmin[1] == 0.0f)
    {
        std::cout << "failed to read domain information" << std::endl;
        return true;
    }

    return false;
}

void Domain::initiate(const Kernel &kernel)
{
    int layer_number = 8;
    float boudary_thick = (float)layer_number * kernel.particle_diameter;
    for (int i = 0; i < 2; i++)
    {
        cell_xmin[i] = domain_xmin[i] - boudary_thick;
        cell_xmax[i] = domain_xmax[i] + boudary_thick;
    }

    float smoothing_length = kernel.smoothing_length;
    float cell_size_max = kernel.impact_length_hsml_ratio * smoothing_length;

    for (int i = 0; i < 2; i++)
    {
        cell_number[i] = (int)((cell_xmax[i] - cell_xmin[i]) / cell_size_max);
        interval[i] = (cell_xmax[i] - cell_xmin[i]) / (float)cell_number[i];
        interval_inv[i] = 1.0f / interval[i];
    }
    cell_number_total = cell_number[0] * cell_number[1];
}