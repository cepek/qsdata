#include "parameterseditor.h"
#include "qsdatasettings.h"
#include <algorithm>
#include <utility>
#include <vector>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>


ParametersEditor::ParametersEditor(ParametersEditor::Role role_, QSDataSettings *parent)
    : QWidget(parent), role(role_)
{
    QFormLayout* form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignLeft);
    form->setHorizontalSpacing(15);

    comboBox_etalons_names = new QComboBox;

    if (role == ParametersEditor::Role::newSettings) {
        lineEdit_etalon_name = new QLineEdit;
        form->addRow(tr("Settings' name"), lineEdit_etalon_name);
    } else {
        form->addRow(tr("Settings' name"), comboBox_etalons_names);
    }
    lineEdit_etalon_min_x = new QLineEdit;
    form->addRow(tr("Etalon min. X"), lineEdit_etalon_min_x);
    lineEdit_etalon_max_x = new QLineEdit;
    form->addRow(tr("Etalon max. X (ignored if 0)"), lineEdit_etalon_max_x);
    checkBox_continuum_removal = new QCheckBox;
    form->addRow(tr("Continuum removal"), checkBox_continuum_removal);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addLayout(form);

    setDialogButtonBoxValues(true);

    QDialogButtonBox* bbox {};
    if (role == ParametersEditor::Role::removeSettings) {
        bbox = new QDialogButtonBox(
                   QDialogButtonBox::Cancel |
                   QDialogButtonBox::Ok,
                   this
                   );
        lineEdit_etalon_min_x->setEnabled(false);
        lineEdit_etalon_max_x->setEnabled(false);
        checkBox_continuum_removal->setEnabled(false);
    }
    else
    {
        bbox = new QDialogButtonBox(
                   QDialogButtonBox::Cancel |
                   QDialogButtonBox::Ok |
                   QDialogButtonBox::Reset,
                   this
                   );
    }

    connect(bbox, &QDialogButtonBox::clicked,
            [this,bbox](QAbstractButton* b){
                if (bbox->button(QDialogButtonBox::Reset) == b) {
                    setDialogButtonBoxValues(true);
                }
    });
    connect(bbox, SIGNAL(accepted()), SLOT(buttonAccepted()));

    connect(bbox, &QDialogButtonBox::rejected,[this](){
        emit rejected();
    });

    buildComboBox();
    setDialogButtonBoxValues(false);

    connect(comboBox_etalons_names, SIGNAL(currentIndexChanged(int)),
            this, SLOT(readParametersFromSettings()));

    vbox->addWidget(bbox);
    setLayout(vbox);
}

void ParametersEditor::readParametersFromSettings()
{
    QString name = comboBox_etalons_names->currentText();
    QSettings settings;
    settings.beginGroup(name);
    lineEdit_etalon_min_x->setText(settings.value("parameters_etalonsMinX").toString());
    lineEdit_etalon_max_x->setText(settings.value("parameters_etalonsMaxX").toString());
    checkBox_continuum_removal->setChecked(settings.value("parameters_continuumRemoval").toBool());
    settings.endGroup();
}

void ParametersEditor::buildComboBox()
{
    if (role == ParametersEditor::Role::newSettings) return;

    QSettings settings;
    QStringList names = settings.childGroups();
    using unsignedQString = std::pair<unsigned, QString>;
    std::vector<unsignedQString> pairs;
    for (auto name : names)
    {
        settings.beginGroup(name);
        unsignedQString p;
        p.first = settings.value("mainWindow_dateTime").toUInt();
        p.second = name;
        pairs.push_back(p);
        settings.endGroup();
    }
    std::sort(pairs.begin(),pairs.end());

    comboBox_etalons_names->clear();
    bool descending = (role == ParametersEditor::Role::existingSettings);
    if (descending) {
        for (auto p=pairs.rbegin(); p!=pairs.rend(); p++) comboBox_etalons_names->addItem(p->second);
    } else {
        for (auto p=pairs.begin(); p!=pairs.end(); p++)   comboBox_etalons_names->addItem(p->second);
    }

    parameters.setSettingsName(comboBox_etalons_names->currentText());
    parameters.setSettingsValues();
}

void ParametersEditor::buttonAccepted()
{
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

    QString name;
    if (role == ParametersEditor::Role::newSettings) {
        name = lineEdit_etalon_name->text().simplified();
        if (lineEdit_etalon_name->text().isEmpty()) {
            QMessageBox::critical(this,"Settings's name","cannot be empty");
            return;
        }
    } else {
        name = comboBox_etalons_names->currentText().simplified();
    }

    if (role == ParametersEditor::Role::removeSettings) {
        QSettings settings;
        settings.beginGroup(name);
        settings.remove("");
        settings.endGroup();
        emit accepted();
    } else {
        parameters.setSettingsName(name);
        parameters.setEtalonsMinX(minx);
        parameters.setEtalonsMaxX(maxx);
        parameters.setContinuumRemoval(checkBox_continuum_removal->isChecked());
        parameters.setGlobal();
        emit accepted();
    }
}

void ParametersEditor::setDialogButtonBoxValues(bool initParameters)
{
        if(initParameters) parameters.setImplicitValues();
        lineEdit_etalon_min_x->setText(QString::number(parameters.etalonsMinX()));
        lineEdit_etalon_max_x->setText(QString::number(parameters.etalonsMaxX()));
        checkBox_continuum_removal->setChecked(parameters.continuumRemoval());
}
