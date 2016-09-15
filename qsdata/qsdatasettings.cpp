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

#include "qsdatasettings.h"
#include "parameters.h"
#include "parameterseditor.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QPushButton>
#include <QMessageBox>
#include <QTabWidget>
#include <QDebug>

QSDataSettings::QSDataSettings(QWidget *parent) : QDialog(parent)
{
    ParametersEditor* editorExisting = new ParametersEditor(ParametersEditor::Role::existingSettings, this);
    ParametersEditor* editorNew      = new ParametersEditor(ParametersEditor::Role::newSettings, this);
    ParametersEditor* editorRemove   = new ParametersEditor(ParametersEditor::Role::removeSettings, this);

    /*auto setDialogButtonBoxValues = [this](bool initParameters){
        if(initParameters) parameters.setImplicitValues();
        lineEdit_etalon_min_x->setText(QString::number(parameters.etalonsMinX()));
        lineEdit_etalon_max_x->setText(QString::number(parameters.etalonsMaxX()));
        checkBox_continuum_removal->setChecked(parameters.continuumRemoval());
    };*/

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->addTab(editorExisting, tr("Existing Settings"));
    tabWidget->addTab(editorNew,      tr("New Settings"));
    tabWidget->addTab(editorRemove,   tr("Remove Settings"));

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(tabWidget);
    setLayout(vbox);

    connect(editorExisting, &ParametersEditor::rejected, [this](){emit rejected(); close();});
    connect(editorNew,      &ParametersEditor::rejected, [this](){emit rejected(); close();});
    connect(editorRemove,   &ParametersEditor::rejected, [this](){emit rejected(); close();});

    connect(editorExisting, &ParametersEditor::accepted, [this](){emit accepted(); close();});
    connect(editorNew,      &ParametersEditor::accepted, [this](){emit accepted(); close();});
    connect(editorRemove,   &ParametersEditor::accepted, [this,editorExisting,editorRemove](){
        editorExisting->buildComboBox();
        editorExisting->readParametersFromSettings();
        editorRemove  ->buildComboBox();
        editorRemove  ->readParametersFromSettings();
        emit accepted();
    });
}
