#ifndef ROCHE_LIMIT_OBJECT_H
#define ROCHE_LIMIT_OBJECT_H

#include "helper/Vector.h"

namespace LH
{
    /**
     * \brief Definition of a circular object (think planet, asteroid, mass point).
     */
    struct Object
    {
        LH::Vector pos;
        LH::Vector vel;
        long double radius;
        long double mass;
    };
}

#endif //ROCHE_LIMIT_OBJECT_H