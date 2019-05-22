#include "tunein.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "utils/json.hpp"

using json = nlohmann::json;

namespace pTuneIN
{
std::string getURL(const std::string &url, bool &ok)
{
    try
    {
        using namespace curlpp::options;

        std::ostringstream os;
        os << Url(url);
        ok = true;
        return os.str();
    }
    catch(curlpp::RuntimeError & e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(curlpp::LogicError & e)
    {
        std::cout << e.what() << std::endl;
    }

    ok = false;
    return "";
}

std::vector<TuneIN::result> procArray(const json &j)
{
    std::vector<TuneIN::result> data;
    for (auto& e : j) {
        TuneIN::result r;

        if(e.find("children") != e.end())
        {
            auto d = procArray(e["children"]);
            data.insert(data.end(), d.begin(), d.end());
            continue;
        }

        if(e.find("URL") == e.end())
            continue; // not an optional field

        r.url = e["URL"];

        if(e.find("image") != e.end())
            r.img = e["image"];

        if(e.find("text") != e.end())
            r.name = e["text"];
        else
            r.name = "no name given";

        if(e.find("type") != e.end())
            r.isStation = (e["type"] == "audio");
        else
            r.isStation = false;

        if(e.find("current_track") != e.end())
            r.playing = e["current_track"];

        data.push_back(r);
    }
    return data;
}
} // namespace pTuneIN


namespace TuneIN
{

using namespace pTuneIN;

std::vector<result> navigate(std::string url)
{
    std::vector<result> data;

    // Allways request json format. Add the ? if not there
    if(url.find("render=json") == std::string::npos)
    {
        size_t pos = url.find('?');
        if(pos == std::string::npos)
        {
            url += "?render=json";
        }
        else
        {
            url.insert(pos+1, "render=json&");
        }
    }

    bool ok;
    std::string resp = getURL(url, ok);
    if(!ok || resp.empty())
    {
        std::cerr << "Bad response - no data: " << url << std::endl;
        return data;
    }

    // All responses must contain a body
    try
    {
        auto j = json::parse(resp);
        if(j.find("body") == j.end())
        {
            std::cerr << "Bad response - no body: " << url << std::endl;
            return data; // no valid response
        }

        j = j["body"];

        if(!j.size())
        {
            std::cerr << "Bad response - empty body: " << url << std::endl;
            return data;
        }

        return procArray(j);
    }
    catch(...)
    {
        std::cerr << "Exception! " << url << std::endl;
    }
    return data;
}

std::string getPlayStream(const result &res)
{
    if(!res.isStation)
    {
        std::cerr << __PRETTY_FUNCTION__ << " Invalid use: " << res.url << std::endl;
        return ""; // tut tut
    }

    bool ok;
    std::string s = getURL(res.url, ok);
    if(!ok)
    {
        std::cerr << __PRETTY_FUNCTION__ << " Failed to load stream: " << res.url << std::endl;
    }

    return s.substr(0, s.find('\n'));
}

} // namespace TuneIN
