#include "parameters.h"
#include <QSettings>
#include <QDebug>

Parameters Parameters::global;

void Parameters::setGlobal() const
{
    global = *this;
}

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
    if (!settingsName().isEmpty()) {
        settings.beginGroup(settingsName());
        etalonsMinX_ = settings.value("parameters_etalonsMinX").toDouble();
        if (etalonsMinX_ < 0) etalonsMinX_ = 0;
        etalonsMaxX_ = settings.value("parameters_etalonsMaxX").toDouble();
        if (etalonsMaxX_ == 0 || etalonsMaxX_ <= etalonsMinX_) etalonsMaxX_ = 0;
        continuumRemoval_ = settings.value("parameters_continuumRemoval").toBool();
        settings.endGroup();
    } else {
        setImplicitValues();
    }
}

QString Parameters::settingsName() const
{
    return settingsName_;
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

void Parameters::setSettingsName(QString name)
{
    settingsName_ = name;
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
