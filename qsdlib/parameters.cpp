#include "parameters.h"
#include <QSettings>
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

void Parameters::setSettingsValues()
{
    QSettings settings;
    if (settings.childGroups().contains("parameters",Qt::CaseInsensitive)) {
        etalonsMinX_ = settings.value("parameters/etalonsMinX").toDouble();
        if (etalonsMinX_ < 0) etalonsMinX_ = 0;
        continuumRemoval_ = settings.value("parameters/continuumRemoval").toBool();
    } else {
        setImplicitValues();
    }
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

void Parameters::setContinuumRemoval(bool contrem)
{
    continuumRemoval_ = contrem;
}
