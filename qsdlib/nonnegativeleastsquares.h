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

#ifndef NONNEGATICELEASTSQUARES_H
#define NONNEGATICELEASTSQUARES_H

#include <matvec/matvec.h>

using Mat = GNU_gama::Mat<double>;
using Vec = GNU_gama::Vec<double>;

class NonNegativeLeastSquares
{
public:
    NonNegativeLeastSquares(const Mat& M, const Vec& v);

    Vec  parameters();
    Vec  residuals();
    int  iterations();
    void setMaxIterations(int mit=0);

private:
    Mat A;
    Vec b, x, r;
    bool solved {false};
    int iters {};
    int max_iters {0};
    double tol {1e-12};

    void solve();
};

#endif // NONNEGATICELEASTSQUARES_H
