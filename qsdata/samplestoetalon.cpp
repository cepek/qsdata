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
    basicWindowTitle = tr("Samples ⟶ Etalon / Sample");
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

    QMenu* saveAverage = menuFile->addMenu(tr("Save &Average"));
    action = saveAverage->addAction(tr("as &Etalon (xml)"));
    connect(action, &QAction::triggered, [this](){save(Algorithm::Average, Format::Etalon);});
    action = saveAverage->addAction(tr("as &Sample (txt)"));
    connect(action, &QAction::triggered, [this](){save(Algorithm::Average, Format::Sample);});

    QMenu* saveMedian = menuFile->addMenu(tr("Save &Median"));
    action = saveMedian->addAction(tr("as &Etalon (xml)"));
    connect(action, &QAction::triggered, [this](){save(Algorithm::Median, Format::Etalon);});
    action = saveMedian->addAction(tr("as &Sample (txt)"));
    connect(action, &QAction::triggered, [this](){save(Algorithm::Median, Format::Sample);});

    menuFile->addSeparator();
    action = menuFile->addAction(tr("&Close"));
    connect(action,SIGNAL(triggered(bool)), SLOT(close()));

    QMenu* menuEdit = menuBar->addMenu(tr("&Edit"));
    action = menuEdit->addAction(tr("Select &All"));
    connect(action, &QAction::triggered, [this](){
        table->blockSignals(true);
        for (int row=0; row<table->rowCount(); row++) table->item(row, 0)->setCheckState(Qt::Checked);
        table->blockSignals(false);
        if (table->rowCount() > 0)emit table->itemChanged(table->item(0,0));
    });

    action = menuEdit->addAction(tr("Select &None"));
    connect(action, &QAction::triggered, [this](){
        table->blockSignals(true);
        for (int row=0; row<table->rowCount(); row++) table->item(row, 0)->setCheckState(Qt::Unchecked);
        table->blockSignals(false);
        if (table->rowCount() > 0)emit table->itemChanged(table->item(0,0));
    });

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

    connect(table, &QTableWidget::itemChanged, this, &SamplesToEtalon::setCustomPlot);
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

    xMin = std::numeric_limits<double>::max();
    xMax = std::numeric_limits<double>::min();
    yMin = std::numeric_limits<double>::max();
    yMax = std::numeric_limits<double>::min();

    table->blockSignals(true);
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
        etalon.push_back(std::shared_ptr<SpectralData>(sp));

        sp->getLimits(xMin, xMax, yMin, yMax);

        QTableWidgetItem* item = new QTableWidgetItem(sp->name);
        item->setCheckState(Qt::Checked);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, 0, item);
        table->setItem(row, 1, new QTableWidgetItem(QString::number(row)));
        row++;
    }
    table->blockSignals(false);

    setCustomPlot();
}

void SamplesToEtalon::setCustomPlot()
{
    qDebug() << "void SamplesToEtalon::setCustomPlot()";
    table->blockSignals(true);

    selected.clear();
    customPlot->clearGraphs();
    customPlot->xAxis->setLabel("wavelength");
    customPlot->yAxis->setLabel("intensity");
    customPlot->xAxis->setRange(xMin, xMax);
    customPlot->yAxis->setRange(yMin, yMax);

    int row {0};
    unsigned igraph {0};
    for (auto sp : etalon.samples())
    {
        qDebug() << row << table->item(row,0)->checkState();
        if (table->item(row++,0)->checkState() == Qt::CheckState::Unchecked) continue;

        selected.push_back(sp);

        /* http://www.qcustomplot.com
         * --------------------------
         */
        customPlot->addGraph();
        customPlot->graph(igraph)->setPen(QPen(Qt::lightGray));
        customPlot->graph(igraph)->setData(sp->x, sp->y);
        igraph++;
    }

    SpectralData a = selected.average();
    customPlot->addGraph();
    customPlot->graph(igraph)->setPen(QPen(Qt::darkBlue));
    customPlot->graph(igraph++)->setData(a.x, a.y);

    SpectralData b = selected.median();
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

    table->blockSignals(false);
}

void SamplesToEtalon::save(Algorithm alg, Format fmt)
{
    qDebug() << "SamplesToEtalon::save()";
    QSettings settings;
    QString directoryEtalons = settings.value("directory/etalons").toString();

    QString extension = (fmt == Format::Etalon) ? "*.xml" : "*.txt";
    QString title;
    if (alg == Algorithm::Average) title =  tr("Save Average as ");
    else                           title =  tr("Save Median as ");
    if (fmt == Format::Etalon)     title += tr("XML");
    else                           title += tr("Text");

    QFileDialog dialog(this, title, directoryEtalons, extension);
    dialog.setDirectory(directoryEtalons);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (fmt == Format::Etalon) dialog.setDefaultSuffix("xml");
    else                       dialog.setDefaultSuffix("txt");

    if (!dialog.exec()) return;

    QStringList slist = dialog.selectedFiles();
    if (slist.size() != 1) return;

    directoryEtalons = dialog.directory().path();
    settings.setValue("directory_etalons", directoryEtalons);

    QString name = slist[0];
    QFileInfo info(name);
    QString baseName = info.baseName();

    QFile file(name);
    file.open(QIODevice::WriteOnly|QIODevice::Text);

    setWindowTitle(basicWindowTitle + " : " + baseName);

    if (alg == Algorithm::Average) {
        SpectralData sd = selected.average();
        sd.name = baseName;
        if (fmt == Format::Etalon) sd.writeXML(file);
        else                       sd.writeASCII(file);
    } else {
        SpectralData sd = selected.median();
        sd.name = baseName;
        if (fmt == Format::Etalon) sd.writeXML(file);
        else                       sd.writeASCII(file);
    }
}
