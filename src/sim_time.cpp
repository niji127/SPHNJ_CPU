#include "sim_time.h"

bool Sim_Time::readNumber(const std::string &path)
{
    return false;
}

bool Sim_Time::readParameter(const Config &config)
{
    dt = config.Read("dt", 0.0f);
    istart = config.Read("istart", 0);
    iend = config.Read("iend", 0);
    solid_sub_step = config.Read("solid_sub_step", 1);
    result_interval = config.Read("result_interval", 1);
    sort_every_step = config.Read("sort_every_step", 1);
    shift_per_step = config.Read("shift_per_step", 1);

    if (dt == 0.0f || istart - iend == 0)
    {
        std::cout << "failed to read time information" << std::endl;
        return true;
    }

    return false;
}

void Sim_Time::initiate()
{
    i = istart;
    current_time = (float)istart * dt;
    file_step = i / result_interval;
}

bool Sim_Time::addStep()
{
    i++;
    current_time = i * dt;
    file_step = i / result_interval;
    return i <= iend ? true : false;
}

bool Sim_Time::isFirstStep()
{
    return i == istart + 1 ? true : false;
}

bool Sim_Time::isOutputData()
{
    return !((i - 1) % result_interval);
}

bool Sim_Time::isSort()
{
    return !((i - 1) % sort_every_step);
}
