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
#include "parameterseditor.h"
#include "qsdatasettings.h"
#include "spectraldata.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon icon(":/img/sd-icon.png");
    a.setWindowIcon(icon);

    QCoreApplication::setOrganizationName  ("qsdata");
    QCoreApplication::setOrganizationDomain("geomatics.fsv.cvut.cz");
    QCoreApplication::setApplicationName   ("qsdata");

    bool accepted {false};
    {
        QSDataSettings qsds;
        QObject::connect(&qsds, &QSDataSettings::rejected, [&accepted](){accepted=false;});
        QObject::connect(&qsds, &QSDataSettings::accepted, [&accepted](){accepted=true;});
        qsds.setSizeGripEnabled(true);
        qsds.exec();
    }

    if (accepted)
    {
        SpectralData::addSdxVector();

        MainWindow w;
        w.show();
        return a.exec();
    }
}
