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
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

QSDataSettings::QSDataSettings(Parameters& params, QWidget *parent)
    : QDialog(parent), parameters(params)
{
    QLayout* vbox = new QGridLayout;
    QFormLayout* form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignLeft);
    form->setHorizontalSpacing(15);
    lineEdit_etalon_min_x = new QLineEdit;
    form->addRow(tr("Etalon min. X"), lineEdit_etalon_min_x);
    lineEdit_etalon_max_x = new QLineEdit;
    form->addRow(tr("Etalon max. X (ignored if 0)"), lineEdit_etalon_max_x);
    checkBox_continuum_removal = new QCheckBox;
    form->addRow(tr("Continuum removal"), checkBox_continuum_removal);
    vbox->addItem(form);

    auto setDialogButtonBoxValues = [this](bool initParameters){
        if(initParameters) parameters.setImplicitValues();
        lineEdit_etalon_min_x->setText(QString::number(parameters.etalonsMinX()));
        lineEdit_etalon_max_x->setText(QString::number(parameters.etalonsMaxX()));
        checkBox_continuum_removal->setChecked(parameters.continuumRemoval());
    };

    setDialogButtonBoxValues(false);

    QDialogButtonBox* bbox = new QDialogButtonBox(
                //QDialogButtonBox::Apply |
                //QDialogButtonBox::Help |
                QDialogButtonBox::Ok |
                QDialogButtonBox::Reset,
                this
                );
    vbox->addWidget(bbox);
    setLayout(vbox);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    connect(bbox, &QDialogButtonBox::accepted, [this](){
        bool ok;
        double minx = lineEdit_etalon_min_x->text().toDouble(&ok);
        if (!ok) {
            QMessageBox::critical(this,"Etalon min. x","not a number");
            return;
        }
        if (minx < 0) {
            QMessageBox::critical(this,"Etalon min. x","cannot be negative");
            return;
        }
        double maxx = lineEdit_etalon_max_x->text().toDouble(&ok);
        if (!ok) {
            QMessageBox::critical(this,"Etalon max. x","not a number");
            return;
        }
        if (maxx != 0 && (maxx < 0 || maxx <= minx)) {
            QMessageBox::critical(this,"Etalon max. x","cannot be negative or less than or equal to min. x");
            return;
        }
        parameters.setEtalonsMinX(minx);
        parameters.setEtalonsMaxX(maxx);
        parameters.setContinuumRemoval(checkBox_continuum_removal->isChecked());
        this->close();});
    connect(bbox, &QDialogButtonBox::clicked,
            [this,bbox,setDialogButtonBoxValues](QAbstractButton* b){
        if (bbox->button(QDialogButtonBox::Reset) == b) {
            setDialogButtonBoxValues(true);
        }
    });
}
