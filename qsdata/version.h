#ifndef VERSION_H
#define VERSION_H

#include <string>


class Version
{
public:
    Version();

    static unsigned major()     { return major_; }
    static unsigned minor()     { return minor_; }
    static unsigned revision()  { return revision_; }
    static std::string version();

private:
    static const unsigned major_;
    static const unsigned minor_;
    static const unsigned revision_;
};

#endif // VERSION_H
