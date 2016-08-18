#include "nonnegativeleastsquares.h"
#include <matvec/gso.h>
#include <set>
#include <limits>
#include <cmath>

using GSO = GNU_gama::GSO<double>;

NonNegativeLeastSquares::NonNegativeLeastSquares(const Mat& m, const Vec& v)
    : A(m), b(v)
{

}

Vec NonNegativeLeastSquares::parameters()
{
    if (!solved) solve();
    return x;
}

Vec NonNegativeLeastSquares::residuals()
{
    if (!solved) solve();
    return r;
}

int NonNegativeLeastSquares::iterations()
{
    if (!solved) solve();
    return iters;
}

void NonNegativeLeastSquares::setMaxIterations(int mit)
{
    max_iters = mit > 0 ? mit : 0;
}

void NonNegativeLeastSquares::solve()
{
    // using std::cerr;
    using Index = GNU_gama::Index;
    const Index N = A.cols();
    const Index M = A.rows();

    /* Z and P will hold indexes of zero and positive variables respectively */
    std::set<Index> Z;            // Set Z = {1, ..., n}
    for (Index i=1; i<=N; i++) Z.insert(i);
    std::set<Index> P;            // Set P = ∅

    x.reset(N);                   // Set x to an all-zero vector of dimension n
    for (double& t : x) t = 0;

    Vec   w;                      // gradient vector
    Index maxw_ind {};            // index of max(w_i), for i in Z

    auto max_wi = [&]() {
        double maxwi = 0; //-std::numeric_limits<double>::max();
        maxw_ind = 0;
        for (Index i : Z) {
            if (w(i) > maxwi) {
                maxwi = w(i);
                maxw_ind = i;
            }
        }
        return maxwi;
    };

    auto move_from_Z_to_P = [&](Index j) {
        Z.erase(j);
        P.insert(j);
    };

    auto move_from_P_to_Z = [&](Index i) {
        Z.insert(i);
        P.erase(i);
    };

    auto min = [&](const Vec& y) {
        double miny = std::numeric_limits<double>::max();
        for (double t : y) if (t < miny) miny = t;
        return miny;
    };

    int maxiters = max_iters > 0 ? max_iters : 3*N;
    iters = 0;
    while (true)
    {
        // Compute negative gradient w
        w = trans(A)*(b - A*x);

        // if Z is empty or w_j <= 0 for all j : computation is complete
        if (Z.size() != 0 && max_wi() > 0)
        {
            move_from_Z_to_P(maxw_ind);
            while (++iters <= maxiters)
            {
                // Least square solution for Ap
                Mat G(M+P.size(),P.size()+1);
                G.set_zero();               
                Index p {1};
                for (Index i : P) {
                    for (Index k=1; k<=M; k++) G(k,p) = A(k,i);
                    p++;
                }
                for (Index i=1; i<=P.size(); i++) {
                    G(M+i,i) = 1;
                }
                for (Index i=1; i<=b.dim(); i++) {
                    G(i,P.size()+1) = -b(i);
                }

                GSO gso(G,M,P.size());
                gso.gso1();
                Vec y(P.size());
                for (Index i=1; i<=P.size(); i++) {
                    y(i) = G(M+i, P.size()+1);
                }

                Index alpha_ind {};
                if (min(y) <= 0) {
                    double alpha = 2;
                    Index q {1};
                    for (Index i : P) {
                        if (y(q) <= 0) {
                            double t = -x(i)/(y(q)-x(i));
                            if (alpha > t) {
                                alpha = t;
                                alpha_ind = i;
                            }
                        }
                        q++;
                    }

                    if (alpha == 2) break;

                    // update x = x + alpha*(y - x);
                    Index k {1};
                    for (Index i : P) {
                        x(i) += alpha*(y(k++) - x(i));
                    }
                    x(alpha_ind) = 0;
                    for (Index i : P) {
                        if (x(i) == 0)
                        //if (std::abs(x(i) < tol))
                            move_from_P_to_Z(i);
                    }
                } else {
                    // Update x = y;
                    Index k {1};
                    for (Index i : P) {
                       x(i) = y(k++);
                    }
                    break;
                }
            }   // inner loop

        } else {
            // return x;
            break;
        }
    }

    r = A*x - b;
    solved = true;
}
