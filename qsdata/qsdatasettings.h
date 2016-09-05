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

#ifndef QSDATASETTINGS_H
#define QSDATASETTINGS_H

#include "parameters.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QDialog>

class QSDataSettings : public QDialog
{
    Q_OBJECT
public:
    explicit QSDataSettings(Parameters& params, QWidget *parent = 0);

signals:

public slots:

private:
    Parameters& parameters;
    QLineEdit* lineEdit_etalon_min_x;
    QLineEdit* lineEdit_etalon_max_x;
    QCheckBox* checkBox_continuum_removal;
};

#endif // QSDATASETTINGS_H
