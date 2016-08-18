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

#ifndef SAMPLESTOETALON_H
#define SAMPLESTOETALON_H

#include <QMainWindow>
#include "qsdlib.h"

class SamplesToEtalon : public QMainWindow
{
    Q_OBJECT
public:
    explicit SamplesToEtalon(QWidget *parent = 0);
    ~SamplesToEtalon();

    void setEtalonsMinX(double min=0) {etalonsMinX=min;}

private:
    void read();
    void save();

    QCustomPlot* customPlot;
    SpectralSampleEtalon etalon;
    QString basicWindowTitle;
    double etalonsMinX {0};

signals:
public slots:
};

#endif // SAMPLESTOETALON_H
