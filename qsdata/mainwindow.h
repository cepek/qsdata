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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QList>
#include <QCloseEvent>
#include <QTableWidget>
#include <QVector>

#include "parameters.h"
#include "spectraldata.h"
#include "notes.h"
#include "matvec/svd.h"

using Mat   = GNU_gama::Mat<double>;
using Vec   = GNU_gama::Vec<double>;
using SVD   = GNU_gama::SVD<double>;
using Index = GNU_gama::Index;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QTableWidget* table;
    bool tableHasChanged   {false};
    QVector<SpectralData*> etalons;    
    int    etalonsMinIndex {0};   //   corresonding vector index
    Notes* notes {nullptr};

    int tableToEtalonIndex(int index) const;
    QVector<int> table2etalon;
    int tableActiveEtalons();
    int table_active_etalons {0};
    QVector<int> activeEtalonsIndexes;

    void removeContinuum(SpectralData& sd);

private slots:
    void closeEvent(QCloseEvent *event);
    // Analyze
    void unmixingSamples();
    void linearEtalonSearch();
    void conditionNumberOfEtalonsMatrix();
    // Tools
    void readEtalons(const QStringList& files);
    void readAllEtalons();
    void readSelectedEtalons();
    void activeEtalonsToSubMatrix(Mat& A);
};

#endif // MAINWINDOW_H
