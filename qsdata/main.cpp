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

#include "mainwindow.h"
#include "parameters.h"
#include "qsdatasettings.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName  ("qsdata");
    QCoreApplication::setOrganizationDomain("geomatics.fsv.cvut.cz");
    QCoreApplication::setApplicationName   ("qsdata");

    Parameters parameters;
    parameters.setSettingsValues();

    QSDataSettings qsds(parameters);
    qsds.setSizeGripEnabled(true);
    qsds.exec();

    MainWindow w(parameters);
    w.show();

    return a.exec();
}
