/*
  Qsdata project
  Copyright (C) 2016 Ales Cepek <cepek@gnu.org>

  This file is part of Qsdata.

  Qsdata is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Qsdata is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Qsdata.  If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef SPECTRAL_DATA_H
#define SPECTRAL_DATA_H

#include <QVector>
#include <QFile>
#include <vector>
#include <utility>
#include <iostream>

class SpectralData
{
public:
    QVector<double> x, y;
    QString         name;

    SpectralData();
    void readData(std::istream& istr, std::string* spectrometerHeader=nullptr);
    void getLimits(double& minx, double& maxx, double& miny, double& maxy, bool init=false);

    void readXML (QFile& file);
    void writeXML(QFile& file);

    void continuumRemoval();
};

#endif // SPECTRAL_DATA_H