#include "version.h"

const unsigned Version::major_    {1};
const unsigned Version::minor_    {0};
const unsigned Version::revision_ {0};

Version::Version()
{

}

std::string Version::version()
{
    std::string ver {std::to_string(major_) + "." + std::to_string(minor_)};
    if (revision_ > 0) ver += "." + std::to_string(revision_);
    return ver;
}
