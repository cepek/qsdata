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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QString>

class Parameters
{
public:
    Parameters();

    void setImplicitValues();

    double etalonsMinX() const;
    bool   continuumRemoval() const;

    void setEtalonsMinx(double minx=0);
    void setContinuumRemovat(bool contrem=false);

private:
    double etalonsMinX_;
    bool   continuumRemoval_;
};

#endif // PARAMETERS_H
