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
#include "samplestoetalon.h"
#include "qsdlib.h"
#include "parameters.h"
#include "version.h"

#include <limits>
#include <fstream>
#include <QMenuBar>
#include <QSettings>
#include <QSize>
#include <QHeaderView>
#include <QFile>
#include <QTableWidgetItem>
#include <QDebug>

MainWindow::MainWindow(Parameters params, QWidget *parent) :
    QMainWindow(parent),
    parameters(params)
{
    QAction* action {nullptr};
    QMenuBar* menuBar = new QMenuBar(this);

    // File menu
    QMenu* menuFile = menuBar->addMenu(tr("&File"));

    action = menuFile->addAction(tr("&Read Selected Etalons"));
    connect(action, SIGNAL(triggered(bool)), SLOT(readSelectedEtalons()));
    menuFile->addSeparator();
    action = menuFile->addAction(tr("E&xit"));
    connect(action,SIGNAL(triggered(bool)), SLOT(close()));
    setMenuBar(menuBar);

    // Edit menu
    QMenu* menuEdit = menuBar->addMenu(tr("&Edit"));

    action = menuEdit->addAction(tr("Select &All"));
    connect(action, &QAction::triggered, [this](){
        for (int r=0; r<table->rowCount(); r++)
            table->item(r,0)->setCheckState(Qt::Checked);});
    action = menuEdit->addAction(tr("Select &None"));
    connect(action, &QAction::triggered, [this](){
        for (int r=0; r<table->rowCount(); r++)
            table->item(r,0)->setCheckState(Qt::Unchecked);});
    menuEdit->addSeparator();
    action = menuEdit->addAction(tr("&Show/Hide Notes"));
    connect(action, &QAction::triggered,
        [this](){ if (notes->isHidden()) notes->show(); else notes->hide(); });
    action = menuEdit->addAction(tr("&Parameters"));
    action->setEnabled(false);

    // Analyze menu
    QMenu* menuAnalyze = menuBar->addMenu(tr("&Analyze"));
    action = menuAnalyze->addAction(tr("&Unmixing Samples"));
    connect(action, &QAction::triggered, [this](){unmixingSamples();});
    action = menuAnalyze->addAction(tr("&Linear Etalon Search"));
    connect(action, SIGNAL(triggered(bool)),SLOT(linearEtalonSearch()));
    action = menuAnalyze->addAction(tr("&Condition Number of Etalons Matrix"));
    connect(action,SIGNAL(triggered(bool)),SLOT(conditionNumberOfEtalonsMatrix()));

    // Tools menu
    QMenu* menuTools = menuBar->addMenu(tr("&Tools"));

    // Help menu
    QMenu* menuHelp = menuBar->addMenu(tr("&Help"));
    action = menuHelp->addAction(tr("&About Qsdata"));
    connect(action, &QAction::triggered, [this](){
        QMessageBox msgBox;
        msgBox.setText("Qsdata version " + QString(Version::version().c_str()));
        msgBox.exec();});

    SamplesToEtalon* samples = new SamplesToEtalon(this);
    samples->setEtalonsMinX(parameters.etalonsMinX());
    action = menuTools->addAction(tr("&Samples to Etalon"));
    connect(action, &QAction::triggered, [samples](){samples->show();});


    auto bothPositive = [](int a, int b) {return a >0 && b>0; };
    QSettings settings;

    int w = settings.value("mainWindow/sizeWidth" ).toInt();
    int h = settings.value("mainWindow/sizeHeight").toInt();
    if (bothPositive(w, h)) resize(w, h);
    else                    resize(300,100);

    int px = settings.value("mainWindow/posX").toInt();
    int py = settings.value("mainWindow/posY").toInt();
    if (bothPositive(px, py)) move(px, py);

    notes = new Notes(this);

    // checkable table of etalons
    table = new QTableWidget;
    table->setColumnCount(2);
    table->hideColumn(1);
    table->setHorizontalHeaderLabels(QStringList() << tr("Etalons"));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);

    table->hide();

    QGridLayout* grid = new QGridLayout;
    grid->addWidget(table);

    QWidget* window = new QWidget;
    window->setLayout(grid);
    setCentralWidget(window);

    readAllEtalons();

    if (parameters.continuumRemoval())
        setWindowTitle("qsdata - continuum removal is used to normalize reflectance spectra");
    else
        setWindowTitle("qsdate - continuum removal is not used to normalize reflectance spectra");
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("mainWindow/sizeWidth" , size().width());
    settings.setValue("mainWindow/sizeHeight", size().height());
    settings.setValue("mainWindow/posX",       x());
    settings.setValue("mainWindow/posY",       y());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void MainWindow::conditionNumberOfEtalonsMatrix()
{
    Index cols = tableActiveEtalons();
    if (cols == 0) return;
    Index rows = etalons[0]->x.size() - etalonsMinIndex;

    Mat A(rows, cols);
    activeEtalonsToSubMatrix(A);
    SVD svd(A);
    Vec W = svd.SVD_W();
    double min {std::numeric_limits<double>::max()};
    double max {std::numeric_limits<double>::min()};
    double tol {svd.tol()};
    for (auto w : W) {
        if (w <= tol) continue;
        if (w <= min) min = w;
        if (w >= max) max = w;
    }

    bool first {true};
    QString text =
        "<h2>Condition Number of Etalon Matrix</h2>"
        "<table><tr><th>Condition number</th><th>Nullity</th><th>Etalons</th></tr>";
    for (int i=0; i<table->rowCount(); i++) {
        if (table->item(i,0)->checkState() != Qt::Checked) continue;
        if (first) {
             text +=
                "<tr>"
                "<td>" + QString::number(max/min) + "</td>"
                "<td>" + QString::number(svd.nullity()) + "</td>"
                "<td>" + table->item(i,0)->text() + "</td>"
                "</tr>";
             first = false;
        } else {
             text += "<tr><td></td><td></td><td>" + table->item(i,0)->text() + "</td></tr>";
        }
    }
    text += "</table>";
    notes->edit->append(text);
    notes->show();
}

void MainWindow::readEtalons(const QStringList &files)
{
    if (files.size() == 0) return;

    for (auto sd : etalons) delete sd;
    etalons.clear();
    table->clearContents();
    table->setRowCount(files.size());
    tableHasChanged = true;
    connect(table, &QTableWidget::itemChanged, [this](){tableHasChanged=true;});
    int row {0};
    for (auto name : files) {
        QFile file(name);
        file.open(QFile::ReadOnly);
        SpectralData* sd = new SpectralData;
        sd->readXML(file);
        removeContinuum(*sd);
        etalons.push_back(sd);

        QTableWidgetItem* item = new QTableWidgetItem(sd->name);
        item->setCheckState(Qt::Checked);
        table->setItem(row, 0, item);
        table->setItem(row, 1, new QTableWidgetItem(QString::number(row)));
        row++;
    }

    table->sortByColumn(0, Qt::SortOrder::AscendingOrder);
    table2etalon.clear();
    for (int i=0; i<table->rowCount(); i++) {
        table2etalon.push_back(QString(table->item(i,1)->text()).toInt());
    }

    table->show();

    etalonsMinIndex = 0;
    if (etalons.size() > 0) {
        const QVector<double>& x = etalons[0]->x;
        for (etalonsMinIndex=0; etalonsMinIndex<x.size(); etalonsMinIndex++)
            if (x[etalonsMinIndex] >= parameters.etalonsMinX()) break;
    }
}

void MainWindow::readAllEtalons()
{
    QSettings settings;
    QString dir = settings.value("directory/etalons").toString();
    if (dir.isEmpty()) return;

    QStringList files;
    QDir etalons(dir);
    etalons.setNameFilters(QStringList() << "*.xml");

    QDirIterator iter(etalons, QDirIterator::Subdirectories);
    while (iter.hasNext()) {
        files.push_back(iter.next());
    }

    readEtalons(files);
}

void MainWindow::readSelectedEtalons()
{
    QSettings settings;
    QString directoryEtalons = settings.value("directory/etalons").toString();

    QFileDialog dialog(this, tr("Read Etalons"));
    dialog.setDirectory(directoryEtalons);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (!dialog.exec()) return;
    QStringList files = dialog.selectedFiles();

    if (files.size() > 0) {
        QFileInfo info(files[0]);
        QDir dir = info.dir();
        settings.setValue("directory/etalons", dir.absolutePath());
    }
    else {
        return;   // this should never happen
    }

    readEtalons(files);
}

void MainWindow::activeEtalonsToSubMatrix(Mat &A)
{
    activeEtalonsIndexes.clear();
    activeEtalonsIndexes.push_back(-1);   // shift for 1-based indexes
    Index cindex = 1;
    for (Index c=0; c<Index(table->rowCount()); c++)
    {
        if (table->item(c,0)->checkState() != Qt::Checked) continue;

        int eind = tableToEtalonIndex(c);
        activeEtalonsIndexes.push_back(eind);
        for (Index rindex=1, r=etalonsMinIndex; r<Index(etalons[eind]->y.size()); r++)
        {
            double a = etalons[eind]->y[r];
            A(rindex++,cindex) = a;
        }
        cindex++;
    }
}

void MainWindow::linearEtalonSearch()
{
    qDebug() << "MainWindow::linearEtalonSearch()";

    QSettings settings;
    QString directorySamples = settings.value("directory/samples").toString();

    QFileDialog dialog(this, tr("Read Samples"));
    dialog.setDirectory(directorySamples);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (!dialog.exec()) return;
    QStringList files = dialog.selectedFiles();

    if (files.size() > 0) {
        //setWindowTitle(basicWindowTitle + " : " + files[0]);

        QFileInfo info(files[0]);
        QDir dir = info.dir();
        dir.cdUp();
        settings.setValue("directory/samples", dir.absolutePath());
    }

    QString text("<h2>Linear Etalon Search</h2>");

    for (QString f : files) {
        text += "<p>" + QFileInfo(f).fileName() + "</p>";
        std::fstream stream(f.toStdString());
        SpectralData sample;
        sample.readData(stream);
        removeContinuum(sample);

        using tuple = std::tuple<double,double,QString>;
        std::vector<tuple> list;

        for (Index c=0; c<Index(table->rowCount()); c++)
        {
            if (table->item(c,0)->checkState() != Qt::Checked) continue;

            double dp {0};
            double sa {0};
            double sb {0};
            double ed {0};
            int etalon_index = tableToEtalonIndex(c);
            SpectralData* etalon = etalons[etalon_index];
            for (int r=etalonsMinIndex; r<etalon->y.size(); r++)
            {
                double a = etalon->y[r];
                double b = sample.y[r];
                dp += a*b;
                sa += a*a;
                sb += b*b;
                ed += (a-b)*(a-b);
            }
            double angle = std::acos(dp/std::sqrt(sa*sb))/M_PI*200;
            double dist  = std::sqrt(ed);
            list.push_back(std::make_tuple(angle, dist, table->item(c,0)->text()));
        }
        std::sort(list.begin(),list.end());

        text +=
            "<table border=1>"
            "<tr><th>Angle [g]</th><th>Distance</th><th>Etalon</th></tr>";
        for (auto x : list) {
            text +=
                "<tr><td>"  + QString::number(std::get<0>(x),'f',2) +
                "</td><td>" + QString::number(std::get<1>(x),'f',1) +
                "</td><td>" + std::get<2>(x) + "</td></tr>";
        }
        text += "</table>";
    }

    notes->edit->append(text);
    notes->show();
}

int MainWindow::tableToEtalonIndex(int index) const
{
    return table2etalon[index];
}

int MainWindow::tableActiveEtalons()
{
    if (!tableHasChanged) return table_active_etalons;

    table_active_etalons = 0;
    for (int i=0; i<table->rowCount(); i++)
        if (table->item(i,0)->checkState() == Qt::Checked) table_active_etalons++;

    tableHasChanged = false;
    return table_active_etalons;
}

void MainWindow::unmixingSamples()
{
    // qDebug() << "MainWindow::unmixingSamples()";
    Index cols = tableActiveEtalons();
    if (cols == 0) return;

    QSettings settings;
    QString directorySamples = settings.value("directory/samples").toString();

    QFileDialog dialog(this, tr("Read Samples"));
    dialog.setDirectory(directorySamples);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (!dialog.exec()) return;
    QStringList files = dialog.selectedFiles();

    if (files.size() > 0) {
        //setWindowTitle(basicWindowTitle + " : " + files[0]);

        QFileInfo info(files[0]);
        QDir dir = info.dir();
        dir.cdUp();
        settings.setValue("directory/samples", dir.absolutePath());
    }

    QString text("<h2>Unmixing samples</h2>");

    for (QString f : files) {
        QFileInfo info(f);
        text += "<p>" + info.fileName() + "</p>";
        std::fstream stream(f.toStdString());
        SpectralData sample;
        sample.readData(stream);
        removeContinuum(sample);

        Index rows = sample.y.size()-etalonsMinIndex;
        Vec b(rows);
        for (Index i=1, r=etalonsMinIndex; r<Index(sample.y.size()); i++, r++) {
            b(i) = sample.y[r];
        }
        //std::cerr << trans(b);

        Mat A(rows, cols);
        activeEtalonsToSubMatrix(A);

        NonNegativeLeastSquares nnls(A,b);
        Vec x = trans(nnls.parameters());

        using tuple = std::tuple<double,QString>;
        std::vector<tuple> list;

        for (Index i=1; i<=x.dim(); i++) {
            list.push_back(std::make_tuple(x(i),etalons[activeEtalonsIndexes[i]]->name));
        }

        std::sort(list.begin(),list.end());

        text +=
            "<table border=1>"
            "<tr><th>Similarity</th><th>Etalon</th></tr>";
        for (auto t=list.rbegin(); t!=list.rend(); ++t)
        {
            text += "<tr><td>" + QString::number(std::get<0>(*t),'f',4) + "</td>"
                    "<td>" +  std::get<1>(*t) + "</td></tr>";
        }
        text += "</table>";
    }

    notes->edit->append(text);
    notes->show();
}

void MainWindow::removeContinuum(SpectralData &sd)
{
    if (parameters.continuumRemoval()) sd.continuumRemoval();
}
