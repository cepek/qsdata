#include "parameters.h"
#include <QDebug>

Parameters::Parameters()
{
        setImplicitValues();
}

void Parameters::setImplicitValues()
{
    etalonsMinX_ = 0;
    continuumRemoval_ = false;
}

double Parameters::etalonsMinX() const
{
    return etalonsMinX_;
}

bool Parameters::continuumRemoval() const
{
    return continuumRemoval_;
}

void Parameters::setEtalonsMinx(double minx)
{
    etalonsMinX_ = minx;
}

void Parameters::setContinuumRemovat(bool contrem)
{
    continuumRemoval_ = contrem;
}
