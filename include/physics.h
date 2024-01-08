#ifndef _PHYSICS_H
#define _PHYSICS_H

#include "Eigen/Dense"

#include "config.h"
#include "type.h"

class Physics
{
public:
    Eigen::Vector2f gravity;
    Transient_Type trasient_type;

    bool readNumber(const std::string &path);
    bool readParameter(const Config &config);
    void initiate();
};

#endif