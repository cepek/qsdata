#include "spectralsampleetalon.h"

SpectralSampleEtalon::SpectralSampleEtalon(std::vector<std::shared_ptr<SpectralData>> samples)
    : samples_(samples)
{

}

std::vector<std::shared_ptr<SpectralData>> SpectralSampleEtalon::samples() const
{
    return samples_;
}

void SpectralSampleEtalon::push_back(std::shared_ptr<SpectralData> sd)
{
    samples_.push_back(std::shared_ptr<SpectralData>(sd));
}

SpectralData SpectralSampleEtalon::average()
{
    SpectralData average_;
    if (samples_.size() == 0) return average_;

    const size_t M = samples_.size();
    const size_t N = samples_[0]->sdx.size();
    average_.sdx.resize(N);
    average_.sdy.resize(N);
    for (size_t i=0; i<N; i++)
    {
        average_.sdx[i] = samples_[0]->sdx[i];
        average_.sdy[i] = 0;
    }

    for (size_t i=0; i<N; i++)
    {
        double s = 0;
        for (size_t j=0; j<M; j++)
        {
            s += samples_[j]->sdy[i];
        }
        average_.sdy[i] = s/M;
    }

    return average_;
}

SpectralData SpectralSampleEtalon::median()
{
    SpectralData median_;
    if (samples_.size() == 0) return median_;

    const size_t M = samples_.size();
    const size_t N = samples_[0]->sdx.size();
    const size_t L = (M-1)/2;
    const size_t U =  M/2;
    median_ .sdx.resize(N);
    median_ .sdy.resize(N);
    for (size_t i=0; i<N; i++)
    {
        median_ .sdx[i] = samples_[0]->sdx[i];
        median_ .sdy[i] = 0;
    }

    for (size_t i=0; i<N; i++)
    {
        QVector<double> t;
        for (size_t j=0; j<M; j++)
        {
            t.push_back(samples_[j]->sdy[i]);
        }

        std::sort(t.begin(), t.end());
        median_ .sdy[i] = (t[L] + t[U])/2;
    }

    return median_;
}

void SpectralSampleEtalon::clear()
{
    samples_.clear();
}

