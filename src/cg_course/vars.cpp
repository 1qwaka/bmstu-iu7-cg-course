#include "vars.h"
#include "qrgb.h"
#include "qglobal.h"

Vars::Vars()
{

}

qreal Vars::far_distance = 1e10;
qreal Vars::small_distance = 0.0001;
qreal Vars::small_float = 0.0001;
QColor Vars::bg_color = qRgb(55, 55, 55);
bool Vars::enable_shadows = false;
qreal Vars::min_parameter = 1;
std::atomic_bool Vars::stop = false;

