#ifndef VARS_H
#define VARS_H


#include "qcolor.h"
#include "qglobal.h"
#include "qrgb.h"
class Vars
{
public:
    Vars();

    static qreal far_distance;
    static qreal small_distance;
    static qreal small_float;
    static QColor bg_color;
    static bool enable_shadows;
    static qreal min_parameter;
    static std::atomic_bool stop;

};

#endif // VARS_H
