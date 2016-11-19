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

#ifndef SPECTRALSAMPLEETALON_H
#define SPECTRALSAMPLEETALON_H

#include "spectraldata.h"
#include <vector>
#include <memory>

class SpectralSampleEtalon
{
public:
    SpectralSampleEtalon(std::vector<std::shared_ptr<SpectralData>> samples
                        =std::vector<std::shared_ptr<SpectralData>>());

    void push_back(std::shared_ptr<SpectralData> sd);
    std::vector<std::shared_ptr<SpectralData>> samples() const;

    SpectralData average();
    SpectralData median();

    void clear();
private:
    std::vector<std::shared_ptr<SpectralData>> samples_;
};

#endif // SPECTRALSAMPLEETALON_H
