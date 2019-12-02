#include "library.h"
#include "player.h"

#include "sources/mediamanager.h"
#include "utils/json.hpp"

using namespace httpserver;

class pLibrary
{
public:
    MediaManager &mm;
    Player &player;

    pLibrary(MediaManager &mm, Player &player) :
        mm(mm),
        player(player)
    {

    }

    std::string getLibrary()
    {
        nlohmann::json j;
        const auto &c = mm.getCache();
        for(const auto &a : c)
        {
            auto &item = j[a.cdid];
            item["titles"] = a.titles;
            item["genre"]  = a.genre;
            item["name"] = a.albumName;
            item["year"] = a.year;
            item["playtime_s"] = a.playTime_s;
        }

        return j.dump(1);
    }
};

Library::Library(MediaManager *mm, Player *player) :
    p(new pLibrary(*mm, *player))
{

}

Library::~Library()
{
    delete p;
}

const std::shared_ptr<http_response> Library::render(const http_request &req)
{
    auto args = req.get_args();

    if(args.count("img"))
    {
        auto img = std::string(p->mm.mediaPath()) + "/" + args["img"] + "/artwork.png";
        //TODO: Check if the file exists and is a plain old file?
        //TODO: This is a path escalation bug
        return std::shared_ptr<http_response>(new file_response(img));
    }

    if(args.count("get_all"))
    {
        return std::shared_ptr<http_response>(new string_response(p->getLibrary()));
    }

    return std::shared_ptr<http_response>(new string_response("Hello, World!"));
}
