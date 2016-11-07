#ifndef XMLTOASCII_H
#define XMLTOASCII_H

#include <QMainWindow>

class Notes;

class XmlToAscii : public QWidget
{
    Q_OBJECT
public:
    explicit XmlToAscii(Notes* pnotes, QWidget *parent = 0);
    ~XmlToAscii();
    void run();

signals:

public slots:
private:
    Notes* notes;
};

#endif // XMLTOASCII_H
