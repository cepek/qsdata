#include "version.h"

const unsigned Version::major_    {0};
const unsigned Version::minor_    {9};
const unsigned Version::revision_ {7};

Version::Version()
{

}

std::string Version::version()
{
    std::string ver {std::to_string(major_) + "." + std::to_string(minor_)};
    if (revision_ > 0) ver += "." + std::to_string(revision_);
    return ver;
}
