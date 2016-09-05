#include "spectraldata.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMessageBox>

#include <iterator>
#include <sstream>
#include <QDebug>

SpectralData::SpectralData()
{

}

void SpectralData::readData(std::istream &istr, std::string *spectrometerHeader)
{
    x.clear();
    y.clear();
    name.clear();

    std::string node((std::istreambuf_iterator<char>(istr)),
                      std::istreambuf_iterator<char>());
    for (char& c : node) if (c == ',') c = '.';
    std::istringstream istream(node);

    if (spectrometerHeader != nullptr) spectrometerHeader->clear();
    const std::string sep {">>>>>Begin Spectral Data<<<<<"};
    std::string line;
    while(std::getline(istream, line)) {
        if (line.substr(0,sep.length()) == sep) break;

        if (spectrometerHeader != nullptr) *spectrometerHeader += line + "\n";
    }

    double ix, iy;
    while(istream >> ix >> iy) {
        x.push_back(ix);
        y.push_back(iy);
    }
}

void SpectralData::getLimits(double &xMin, double &xMax, double &yMin, double &yMax, bool init)
{
    if (init)
    {
        xMin = yMin = std::numeric_limits<double>::max();
        xMax = yMax = std::numeric_limits<double>::min();
    }

    for (double t : x) {
        if (t < xMin) xMin = t;
        if (t > xMax) xMax = t;
    }
    for (double t : y) {
        if (t < yMin) yMin = t;
        if (t > yMax) yMax = t;
    }
}

void SpectralData::readXML(QFile &file)
{
    x.clear();
    y.clear();
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
            x.push_back(tx.toDouble());
            xml.readNext();

            xml.readNextStartElement();
            auto ty = xml.readElementText();
            y.push_back(ty.toDouble());
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

    for (int i=0; i<x.size(); i++)
    {
         xml.writeStartElement("point");
         xml.writeTextElement("x", QString("%1").arg(x[i]));
         xml.writeTextElement("y", QString("%1").arg(y[i]));
         xml.writeEndElement();
    }

    xml.writeEndElement();

    xml.writeEndElement();
    xml.writeEndDocument();
}

void SpectralData::continuumRemoval()
{
    const int N = x.size();
    QVector<double> r(N);

    int minIndX {};
    r = y; // ************
#if 0
    for (minIndX = 0; minIndX<N; minIndX++) {
        r[minIndX] = y[minIndX];
    }
#endif
    double px = x[minIndX];
    double py = y[minIndX];
    r[minIndX] = py;

    for (int i=minIndX+1; i<N-1; i++)
    {
        bool test = true;
        double dx1 = x[i] - px;
        double dy1 = y[i] - py;
        double a1  = std::atan2(dy1,dx1);

        for (int j=i+1; j<N; j++)
        {
            double dx2 = x[j] - px;
            double dy2 = y[j] - py;
            double a2  = std::atan2(dy2,dx2);

            if (a1 < a2)
            {
                test = false;
                break;
            }
        }
        if (test)
        {
            px = x[i];
            py = y[i];
            int j = minIndX+1;
            double dx = x[i] - x[minIndX];
            double x0 = x[minIndX];
            double y0 = y[minIndX];
            double dy = (y[i] - r[minIndX])/dx;
            while (j <= i) {
                r[minIndX+1] = (y0 + dy*(x[j]-x0));
                minIndX++;
                j++;
            }
        }
    }
    int i = N - 1;
    int j = minIndX+1;
    double dx = x[i] - x[minIndX];
    double x0 = x[minIndX];
    double y0 = r[minIndX];
    double dy = (y[i] - r[minIndX])/dx;
    while (j <= i) {
        r[minIndX+1] = (y0 + dy*(x[j]-x0));
        minIndX++;
        j++;
    }
    for (int i=0; i<r.size(); i++)
    {
        y[i] = y[i]/r[i];
    }
}
