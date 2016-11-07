#include "xmltoascii.h"
#include "notes.h"
#include "spectraldata.h"
#include <QGridLayout>
#include <QFileDialog>
#include <QDebug>

XmlToAscii::XmlToAscii(Notes* pnotes, QWidget *parent)
    : QWidget(parent), notes(pnotes)
{
    qDebug() << "XmlToAscii::XmlToAscii()";
}

XmlToAscii::~XmlToAscii()
{
    qDebug() << "XmlToAscii::~XmlToAscii()";
}

void XmlToAscii::run()
{
    auto canceled = [this](){
        notes->textEdit()->append("<p>XML to ASCII canceled by user</p>");
    };

    qDebug() << "XmlToAscii::run()";
    QFileDialog fdialog;
    fdialog.setWindowTitle(tr("XML to ASCII - input file(s)"));
    fdialog.setFileMode(QFileDialog::ExistingFiles);
    fdialog.setNameFilters(QStringList("XML files (*.xml)"));
    if (fdialog.exec() == QDialog::Rejected) {
        return canceled();
    }

    QFileDialog ddialog;
    ddialog.setWindowTitle(tr("XML to ASCII - output directory"));
    ddialog.setFileMode(QFileDialog::DirectoryOnly);
    if (ddialog.exec() == QDialog::Rejected) {
        return canceled();
    }

    QString inputd  = fdialog.directory().path();
    QString outputd = ddialog.selectedFiles()[0];

    notes->textEdit()->append("<h2>XML to ASCII</h2>");
    QString text = "<table border=0>";
    text += "<tr><td>input  directory &nbsp;</td><td>" + inputd  + "</td></tr>";
    text += "<tr><td>output directory &nbsp;</td><td>" + outputd + "</td></tr>";

    for (int N = fdialog.selectedFiles().size(), i=0; i<N; i++) {
        text += "<tr><td>";
        if (i == 0) {
            if (N == 1) text += "file";
            else        text += "files";
        }
        text += "</td><td>";
        QString file = fdialog.selectedFiles()[i];
        QString name = QFileInfo(file).fileName();
        text += name;

        QFile inp(file);
        inp.open(QFile::ReadOnly);
        SpectralData data;
        data.readXML(inp);

        name = QFileInfo(name).baseName() + ".txt";
        name = QFileInfo(outputd, name).absoluteFilePath();
        QFile out(name);
        out.open(QFile::WriteOnly);
        QTextStream stream(&out);
        stream.setRealNumberPrecision(16);
        for (int i=0; i<data.x.size(); i++)
        {
            stream << data.x[i] << "\t" << data.y[i] << "\n";
        }

        text += "</td></tr>";
    }

    text += "</table>";
    notes->textEdit()->append(text);

    /*
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
    */
}
