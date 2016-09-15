#ifndef PARAMETERSEDITOR_H
#define PARAMETERSEDITOR_H

#include "parameters.h"
#include "qsdatasettings.h"
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>


class ParametersEditor : public QWidget
{
    Q_OBJECT
public:
    enum class Role {existingSettings, newSettings, removeSettings};

    explicit ParametersEditor(ParametersEditor::Role role_, QSDataSettings *parent);

    Parameters parameters;

signals:
    void accepted();
    void rejected();

public slots:
    void readParametersFromSettings();
    void buttonAccepted();
    void setDialogButtonBoxValues(bool initParameters);

private:
    QLineEdit* lineEdit_etalon_name {};
    QComboBox* comboBox_etalons_names {};
    QLineEdit* lineEdit_etalon_min_x {};
    QLineEdit* lineEdit_etalon_max_x {};
    QCheckBox* checkBox_continuum_removal {};

    ParametersEditor::Role role;

    friend class QSDataSettings;
    void buildComboBox();
};

#endif // PARAMETERSEDITOR_H
