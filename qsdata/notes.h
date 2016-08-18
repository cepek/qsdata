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

#ifndef NOTES_H
#define NOTES_H

#include <QMainWindow>
#include <QTextEdit>

class Notes : public QMainWindow
{
    Q_OBJECT
public:
    explicit Notes(QWidget *parent = 0);
    ~Notes();

private:
    friend class MainWindow;
    QTextEdit* edit {nullptr};

signals:

public slots:
};

#endif // NOTES_H
