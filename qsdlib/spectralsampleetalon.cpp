#include "spectralsampleetalon.h"

SpectralSampleEtalon::SpectralSampleEtalon(std::vector<SpectralData*> samples)
    : samples_(samples)
{

}

std::vector<SpectralData*> SpectralSampleEtalon::samples() const
{
    return samples_;
}

void SpectralSampleEtalon::push_back(SpectralData *sd)
{
    samples_.push_back(sd);
}

SpectralData SpectralSampleEtalon::average()
{
    SpectralData average_;
    if (samples_.size() == 0) return average_;

    const size_t M = samples_.size();
    const size_t N = samples_[0]->x.size();
    average_.x.resize(N);
    average_.y.resize(N);
    for (size_t i=0; i<N; i++)
    {
        average_.x[i] = samples_[0]->x[i];
        average_.y[i] = 0;
    }

    for (size_t i=0; i<N; i++)
    {
        double s = 0;
        for (size_t j=0; j<M; j++)
        {
            s += samples_[j]->y[i];
        }
        average_.y[i] = s/M;
    }

    return average_;
}

SpectralData SpectralSampleEtalon::median()
{
    SpectralData median_;
    if (samples_.size() == 0) return median_;

    const size_t M = samples_.size();
    const size_t N = samples_[0]->x.size();
    const size_t L = (M-1)/2;
    const size_t U =  M/2;
    median_ .x.resize(N);
    median_ .y.resize(N);
    for (size_t i=0; i<N; i++)
    {
        median_ .x[i] = samples_[0]->x[i];
        median_ .y[i] = 0;
    }

    for (size_t i=0; i<N; i++)
    {
        QVector<double> t;
        for (size_t j=0; j<M; j++)
        {
            t.push_back(samples_[j]->y[i]);
        }

        std::sort(t.begin(), t.end());
        median_ .y[i] = (t[L] + t[U])/2;
    }

    return median_;
}

void SpectralSampleEtalon::clear()
{
    for (auto data : samples_) delete data;
    samples_.clear();
}

