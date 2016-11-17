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

#include "samplestoetalon.h"

#include <fstream>
#include <QMenuBar>
#include <QSettings>
#include <QSplitter>
#include <QDebug>

SamplesToEtalon::SamplesToEtalon(QWidget *parent) : QMainWindow(parent)
{
    basicWindowTitle = tr("Samples ⟶ Etalon");
    setWindowTitle(basicWindowTitle);
    customPlot = new QCustomPlot(this);
    //customPlot->hide();

    table = new QTableWidget;
    table->setColumnCount(2);
    table->hideColumn(1);
    table->setHorizontalHeaderLabels(QStringList() << tr("Samples"));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);

    QSplitter* splitter = new QSplitter;
    splitter->addWidget(table);
    splitter->addWidget(customPlot);
    QGridLayout* grid = new QGridLayout;
    grid->addWidget(splitter);
    QWidget* window = new QWidget;
    window->setLayout(grid);
    setCentralWidget(window);


    // menu bar
    QAction * action {nullptr};
    QMenuBar* menuBar = new QMenuBar(this);

    QMenu* menuFile = menuBar->addMenu(tr("&File"));
    action = menuFile->addAction(tr("&Read"));
    connect(action, &QAction::triggered, [this](){read();});

    action = menuFile->addAction(tr("&Save"));
    connect(action, &QAction::triggered, [this](){save();});

    menuFile->addSeparator();
    action = menuFile->addAction(tr("&Close"));
    connect(action,SIGNAL(triggered(bool)), SLOT(close()));

    QMenu* menuEdit = menuBar->addMenu(tr("&Edit"));
    action = menuEdit->addAction(tr("Select &All"));
    connect(action, &QAction::triggered, [this](){;});

    action = menuEdit->addAction(tr("Select &None"));
    connect(action, &QAction::triggered, [this](){;});

    setMenuBar(menuBar);


    // restore window's settings from the last session
    auto bothPositive = [](int a, int b) {return a >0 && b>0; };
    QSettings settings;

    int w = settings.value("sampleToEtalon/sizeWidth" ).toInt();
    int h = settings.value("sampleToEtalon/sizeHeight").toInt();
    if (!bothPositive(w, h)) w=600, h=400;
    resize(w, h);

    QList<int> sizes {int(0.2*w),int(0.8*w)};
    splitter->setSizes(sizes);

    int px = settings.value("sampleToEtalon/posX").toInt();
    int py = settings.value("sampleToEtalon/posY").toInt();
    if (bothPositive(px, py)) move(px, py);
}

SamplesToEtalon::~SamplesToEtalon()
{
    QString name = Parameters::global.settingsName();
    if (!name.isEmpty()) {
        QSettings settings;
        settings.beginGroup(name);
        settings.setValue("sampleToEtalon_sizeWidth" , size().width());
        settings.setValue("sampleToEtalon_sizeHeight", size().height());
        settings.setValue("sampleToEtalon_posX",       x());
        settings.setValue("sampleToEtalon_posY",       y());
        settings.endGroup();
    }
}

void SamplesToEtalon::read()
{
    qDebug() << "SamplesToEtalon::read()";
    QSettings settings;
    QString directorySamples = settings.value("directory/samples").toString();

    QFileDialog dialog(this, tr("Read Samples"));
    dialog.setDirectory(directorySamples);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (!dialog.exec()) return;
    QStringList files = dialog.selectedFiles();

    if (files.size() > 0) {
        setWindowTitle(basicWindowTitle + " : " + files[0]);

        QFileInfo info(files[0]);
        QDir dir = info.dir();
        dir.cdUp();
        settings.setValue("directory_samples", dir.absolutePath());
    }

    double xMin {std::numeric_limits<double>::max()};
    double xMax {std::numeric_limits<double>::min()};
    double yMin {std::numeric_limits<double>::max()};
    double yMax {std::numeric_limits<double>::min()};

    etalon.clear();
    table->clearContents();
    table->setRowCount(files.size());
    int row {0};
    for (auto name : files)
    {
        //std::ifstream file(name.toStdString());

        SpectralData * sp = new SpectralData;
        QFile file(name);
        file.open(QIODevice::ReadOnly|QIODevice::Text);
        sp->readData(file);
        etalon.push_back(sp);

        sp->getLimits(xMin, xMax, yMin, yMax);

        QTableWidgetItem* item = new QTableWidgetItem(sp->name);
        item->setCheckState(Qt::Checked);
        table->setItem(row, 0, item);
        table->setItem(row, 1, new QTableWidgetItem(QString::number(row)));
        row++;
    }

    customPlot->clearGraphs();
    unsigned igraph {0};
    for (auto sp : etalon.samples())
    {
        /* http://www.qcustomplot.com
         * --------------------------
         */
        customPlot->addGraph();
        customPlot->graph(igraph)->setPen(QPen(Qt::lightGray));
        customPlot->graph(igraph)->setData(sp->x, sp->y);
        igraph++;

        if (igraph == 1)
        {
            customPlot->xAxis->setLabel("wavelength");
            customPlot->yAxis->setLabel("intensity");

            customPlot->xAxis->setRange(xMin, xMax);
            customPlot->yAxis->setRange(yMin, yMax);
        }
    }

    SpectralData a = etalon.average();
    customPlot->addGraph();
    customPlot->graph(igraph)->setPen(QPen(Qt::darkBlue));
    customPlot->graph(igraph++)->setData(a.x, a.y);

    SpectralData b = etalon.median();
    customPlot->addGraph();
    customPlot->graph(igraph)->setPen(QPen(Qt::green));
    customPlot->graph(igraph++)->setData(b.x, b.y);

    if (etalonsMinX > 0)
    {
        QCPItemLine* xminLine = new QCPItemLine(customPlot);
        xminLine->start->setCoords(etalonsMinX, QCPRange::minRange);
        xminLine->end  ->setCoords(etalonsMinX, QCPRange::maxRange);
        QPen pen;
        pen.setColor(Qt::red);
        pen.setStyle(Qt::DotLine);
        xminLine->setPen(pen);
    }

    customPlot->replot();

    customPlot->show();
}

void SamplesToEtalon::save()
{
    qDebug() << "SamplesToEtalon::save()";
    QSettings settings;
    QString directoryEtalons = settings.value("directory/etalons").toString();

    QFileDialog dialog(this, tr("Save Etalon"), directoryEtalons, "*.xml");
    dialog.setDirectory(directoryEtalons);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("xml");

    if (!dialog.exec()) return;

    QStringList slist = dialog.selectedFiles();
    if (slist.size() != 1) return;

    directoryEtalons = dialog.directory().path();
    settings.setValue("directory_etalons", directoryEtalons);

    QString name = slist[0];
    QFileInfo info(name);
    QString baseName = info.baseName();
    //QString suffix = info.suffix();
    //if (suffix.isEmpty()) name += ".xml";

    QFile file(name);
    file.open(QIODevice::WriteOnly|QIODevice::Text);

    setWindowTitle(basicWindowTitle + " : " + baseName);

    SpectralData sd = etalon.average();
    sd.name = baseName;
    sd.writeXML(file);
}
