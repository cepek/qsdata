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
    QLayout* vbox = new QGridLayout; //QVBoxLayout(this);
    QFormLayout* form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignLeft);
    form->setHorizontalSpacing(15);
    lineEdit_etalon_min_x = new QLineEdit;
    form->addRow(tr("Etalon min. X"), lineEdit_etalon_min_x);
    checkBox_continuum_removal = new QCheckBox;
    form->addRow(tr("Continuum removal"), checkBox_continuum_removal);
    vbox->addItem(form);

    auto setImplicitValues = [this](){
        parameters.setImplicitValues();
        lineEdit_etalon_min_x->setText(QString::number(parameters.etalonsMinX()));
        checkBox_continuum_removal->setChecked(parameters.continuumRemoval());
    };

    setImplicitValues();

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
        parameters.setEtalonsMinx(minx);
        parameters.setContinuumRemovat(checkBox_continuum_removal->isChecked());
        this->close();});
    connect(bbox, &QDialogButtonBox::clicked,
            [this,bbox,setImplicitValues](QAbstractButton* b){
        if (bbox->button(QDialogButtonBox::Reset) == b) {
            setImplicitValues();
        }
    });
}
