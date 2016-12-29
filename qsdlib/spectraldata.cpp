#include "spectraldata.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileInfo>
#include <QMessageBox>

#include <iterator>
#include <sstream>
#include <fstream>
#include <QDebug>

SpectralData::SpectralData()
{

}

void SpectralData::readData(QFile &file)
{
    const std::string sep { ">>>>>Begin Spectral Data<<<<<" };

    std::string sname = file.fileName().toStdString();
    std::ifstream istr(sname);

    sdx.clear();
    sdy.clear();
    name = QFileInfo(file).baseName();   // .clear();

    std::string node((std::istreambuf_iterator<char>(istr)),
                      std::istreambuf_iterator<char>());

    if (node.find("<?xml") != std::string::npos) return readXML(file);

    bool spectrometer_format = node.find(sep) != std::string::npos;

    if (spectrometer_format) for (char& c : node) if (c == ',') c = '.';

    std::istringstream istream(node);

    if (spectrometer_format)
    {
        std::string line;
        while(std::getline(istream, line)) {
            if (line.substr(0,sep.length()) == sep) break;
        }
    }

    double ix, iy;
    while(istream >> ix >> iy) {
        sdx.push_back(ix);
        sdy.push_back(iy);
    }
}

void SpectralData::getLimits(double &xMin, double &xMax, double &yMin, double &yMax, bool init)
{
    if (init)
    {
        xMin = yMin = std::numeric_limits<double>::max();
        xMax = yMax = std::numeric_limits<double>::min();
    }

    for (double t : sdx) {
        if (t < xMin) xMin = t;
        if (t > xMax) xMax = t;
    }
    for (double t : sdy) {
        if (t < yMin) yMin = t;
        if (t > yMax) yMax = t;
    }
}

void SpectralData::readXML(QFile &file)
{
    sdx.clear();
    sdy.clear();
    name.clear();

    QXmlStreamReader xml(&file);

    do {
        auto read_start_element = [&](const char* tag){
            xml.readNextStartElement();
            if (xml.name() != tag) {
                QString err = QObject::tr("Missing tag <%1>");
                xml.raiseError(err.arg(tag));
            }
            return !xml.hasError();
            };

        if (!read_start_element("endmember")) break;
        if (!read_start_element("header")) break;

        while (!xml.atEnd()) {
            // read untill end element or error
            bool start_element = xml.readNextStartElement();
            if (!start_element) break;

            auto atr = xml.name();
            auto val = xml.readElementText();

            if (atr == "name") name = val;
        }

        if (!read_start_element("spectraldata")) break;

        while (!xml.atEnd()) {
            // read start element <point>
            bool start_element = xml.readNextStartElement();
            if (!start_element) break;

            xml.readNextStartElement();
            auto tx = xml.readElementText();
            sdx.push_back(tx.toDouble());
            xml.readNext();

            xml.readNextStartElement();
            auto ty = xml.readElementText();
            sdy.push_back(ty.toDouble());
            xml.readNext();

            // read end element </point>
            xml.readNext();
        }
    } while (false);

    if (xml.hasError()) {
        qDebug() << "zpracovani xml skoncilo chybou" << xml.errorString();
        QMessageBox::critical(0,QObject::tr("XML Error"), xml.errorString());
    }
}

void SpectralData::writeXML(QFile &file)
{
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);

    xml.writeStartDocument("1.0");  // xml version 1.0
    xml.writeStartElement("endmember");

    xml.writeStartElement("header");
    xml.writeTextElement("name", name);
    xml.writeEndElement();

    xml.writeStartElement("spectraldata");

    for (int i=0; i<sdx.size(); i++)
    {
         xml.writeStartElement("point");
         xml.writeTextElement("x", QString("%1").arg(sdx[i],0,'G',16));
         xml.writeTextElement("y", QString("%1").arg(sdy[i],0,'G',16));
         xml.writeEndElement();
    }

    xml.writeEndElement();

    xml.writeEndElement();
    xml.writeEndDocument();
}

void SpectralData::writeASCII(QFile &file)
{
    QTextStream stream(&file);
    stream.setRealNumberPrecision(real_number_precision);
    for (int i=0; i<sdx.size(); i++)
    {
        stream << sdx[i] << "\t" << sdy[i] << "\n";
    }
}

void SpectralData::continuumRemoval()
{
    const int N = sdx.size();
    QVector<double> r(N);

    int minIndX {};
    r = sdy; // ************
#if 0
    for (minIndX = 0; minIndX<N; minIndX++) {
        r[minIndX] = y[minIndX];
    }
#endif
    double px = sdx[minIndX];
    double py = sdy[minIndX];
    r[minIndX] = py;

    for (int i=minIndX+1; i<N-1; i++)
    {
        bool test = true;
        double dx1 = sdx[i] - px;
        double dy1 = sdy[i] - py;
        double a1  = std::atan2(dy1,dx1);

        for (int j=i+1; j<N; j++)
        {
            double dx2 = sdx[j] - px;
            double dy2 = sdy[j] - py;
            double a2  = std::atan2(dy2,dx2);

            if (a1 < a2)
            {
                test = false;
                break;
            }
        }
        if (test)
        {
            px = sdx[i];
            py = sdy[i];
            int j = minIndX+1;
            double dx = sdx[i] - sdx[minIndX];
            double x0 = sdx[minIndX];
            double y0 = sdy[minIndX];
            double dy = (sdy[i] - r[minIndX])/dx;
            while (j <= i) {
                r[minIndX+1] = (y0 + dy*(sdx[j]-x0));
                minIndX++;
                j++;
            }
        }
    }
    int i = N - 1;
    int j = minIndX+1;
    double dx = sdx[i] - sdx[minIndX];
    double x0 = sdx[minIndX];
    double y0 = r[minIndX];
    double dy = (sdy[i] - r[minIndX])/dx;
    while (j <= i) {
        r[minIndX+1] = (y0 + dy*(sdx[j]-x0));
        minIndX++;
        j++;
    }
    for (int i=0; i<r.size(); i++)
    {
        sdy[i] = sdy[i]/r[i];
    }
}
