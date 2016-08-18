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

#include "notes.h"

#include <QTextEdit>
#include <QLayout>
#include <QSettings>

Notes::Notes(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(tr("Notes"));

    edit = new QTextEdit(this);
    QGridLayout* grid = new QGridLayout;
    grid->addWidget(edit);

    QWidget* window = new QWidget(this);
    window->setLayout(grid);
    setCentralWidget(window);

    edit->insertHtml(
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Notes</title>"
        "</head>"
        "<body>"
        "<h1>Notes</h1>"
        );

    // restore window's settings from the last session
    auto bothPositive = [](int a, int b) {return a >0 && b>0; };
    QSettings settings;

    int w = settings.value("notes/sizeWidth" ).toInt();
    int h = settings.value("notes/sizeHeight").toInt();
    if (bothPositive(w, h)) resize(w, h);
    else                    resize(600,400);

    int px = settings.value("notes/posX").toInt();
    int py = settings.value("notes/posY").toInt();
    if (bothPositive(px, py)) move(px, py);
}

Notes::~Notes()
{
    QSettings settings;
    settings.setValue("notes/sizeWidth" , size().width());
    settings.setValue("notes/sizeHeight", size().height());
    settings.setValue("notes/posX",       x());
    settings.setValue("notes/posY",       y());
}
