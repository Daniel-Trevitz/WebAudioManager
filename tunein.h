#ifndef TUNEIN_H
#define TUNEIN_H

#include <string>
#include <vector>

namespace TuneIN
{
    struct result
    {
        std::string url;
        std::string img;
        std::string playing;
        std::string name;
        bool isStation;
    };

    std::vector<result> navigate(std::string url);
};

#endif // TUNEIN_H
