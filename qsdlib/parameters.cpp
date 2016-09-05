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
    etalonsMaxX_ = 0;
    continuumRemoval_ = false;
}

void Parameters::setSettingsValues()
{
    QSettings settings;
    if (settings.childGroups().contains("parameters",Qt::CaseInsensitive)) {
        etalonsMinX_ = settings.value("parameters/etalonsMinX").toDouble();
        if (etalonsMinX_ < 0) etalonsMinX_ = 0;
        etalonsMaxX_ = settings.value("parameters/etalonsMaxX").toDouble();
        if (etalonsMaxX_ == 0 || etalonsMaxX_ <= etalonsMinX_) etalonsMaxX_ = 0;
        continuumRemoval_ = settings.value("parameters/continuumRemoval").toBool();
    } else {
        setImplicitValues();
    }
}

double Parameters::etalonsMinX() const
{
    return etalonsMinX_;
}

double Parameters::etalonsMaxX() const
{
    return etalonsMaxX_;
}

bool Parameters::continuumRemoval() const
{
    return continuumRemoval_;
}

void Parameters::setEtalonsMinX(double minx)
{
    etalonsMinX_ = minx;
}

void Parameters::setEtalonsMaxX(double maxx)
{
    etalonsMaxX_ = maxx;
}

void Parameters::setContinuumRemoval(bool contrem)
{
    continuumRemoval_ = contrem;
}
