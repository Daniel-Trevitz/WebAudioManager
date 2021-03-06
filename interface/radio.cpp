#include "radio.h"
#include "sources/tunein.h"
#include "httpserver/bad_request.h"

#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace httpserver;

class pRadio
{
public:
    Player &player;
    std::vector<TuneIN::result> lastResults;
    TuneIN::result nowPlaying;

    pRadio(Player *player) :
        player(*player)
    {

    }

    bool begin()
    {
        lastResults = TuneIN::navigate("https://opml.radiotime.com/Browse.ashx");
        return !lastResults.empty();
    }

    bool select(size_t idx)
    {
        const auto &e = lastResults.at(idx);

        if(e.isStation)
            return false; // this is a station... so why did you ask?

        const auto r = TuneIN::navigate(e.url);
        if(r.empty())
            return false; // No results! So now what?!

        lastResults = r;
        return true;
    }

    bool play(size_t idx)
    {
        const auto &r = lastResults.at(idx);

        if(!r.isStation)
            return false; // not a station... so why did you ask?

        //if(!player.play(TuneIN::getPlayStream(r))) // when not using vlc use this
        if(!player.play(r.url)) // vlc can handle these urls, but nothing else can
            return false;

        nowPlaying = r;
        return true;
    }

    // Use the lastResults to create a table for the user to continue further actions
    // If playing make a table showing the back button and
    std::string makeTable()
    {
        if(player.isPlaying())
            return makeTable_playing();
        return makeTable_notPlaying();
    }

    std::string makeTable_playing()
    {
        std::stringstream s;
        s << "<table id=radio>"
             "<tr>"
             "<td><button class=radio id=start onclick='startPlayer()'>Start</button></td>"
             "<td><button class=radio id=pause onclick='pausePlayer()'>Pause</button></td>"
             "<td><button class=radio id=stop  onclick='stopRadioPlayer()' >Stop</button></td>"
             "</tr>"
             "</table>";


        s << "<table id=radio>"
             "<tr>";

        if(!nowPlaying.img.empty())
        {
            s << "<td><img class=radio_img src=\"" << nowPlaying.img << "\"></img></td>";
        }

        s << "<td>"
          << nowPlaying.name
          << "</td>"
          << "</tr>"
          << "</table>";
        return s.str();
    }

    std::string makeTable_notPlaying()
    {
        std::stringstream s;
        s << "<table id=radio width=60%>";

        std::stringstream stations,menu;

        if(lastResults.empty() && !begin())
            return "Failed to get radio stations!";

        for(size_t i = 0; i < lastResults.size(); i++)
        {
            const auto &e = lastResults.at(i);
            if(e.isStation)
            {
                makeStation(i, e, stations);
            }
            else
            {
                makeMenu(i, e, menu);
            }
        }

        auto m = menu.str();
        if(!m.empty())
        {
            s << "<select class=radio_select id=searchoptions onchange='selectRadioSearch(this);'>"
                 "<option value='' selected disabled hidden>Select Search Type</option>";

            s << m;

            s << "</select>";
        }

        s << stations.str();

        s << "</table>";
        return s.str();
    }

    void makeStation(size_t i, const TuneIN::result &r, std::stringstream &s)
    {
        s << "<tr>"
             "<td>"
             "<button class=radio_btn onclick=\"selectRadioStation(" << i << ");\">";

        s << "<table style='width:80%;'>"
             "<tr>"
             "<td style='width:25px;'>";

        if(!r.img.empty())
        {
            s << "<img class=radio_img src=\"" << r.img << "\"></img>";
        }

        s << "</td>"
             "<td width=100%>"
          << r.name;

        s << "</button>"
           "</td></tr></table>"
           "</td>"
           "</tr>";
    }

    void makeMenu(size_t i, const TuneIN::result &r, std::stringstream &s)
    {
        s << "<option value='" << i << "'>" << r.name << "</option>";
    }
};

Radio::Radio(Player *player) : p(new pRadio(player))
{

}

Radio::~Radio()
{
    delete p;
}

const std::shared_ptr<http_response> Radio::render(const http_request &req)
{
    try
    {
        auto args = req.get_args();
        if(args.count("begin_nav"))
        {
            if(!p->player.isPlaying()) // don't reload all the time
                p->begin(); // Start navigation from the base URL - called when Radio.html is loaded
        }
        else if(args.count("play"))
        {
            size_t idx = std::stoul(args["play"]);
            if(idx > p->lastResults.size())
                throw std::out_of_range("not a valid id");

            p->play(idx);
        }
        else if(args.count("select"))
        {
            // Navigate

            size_t idx = std::stoul(args["select"]);
            if(idx > p->lastResults.size())
                throw std::out_of_range("not a valid id");
            p->select(idx);
        }
    }
    catch (...)
    {
        return bad_request();
    }

    return std::shared_ptr<http_response>(new string_response(p->makeTable()));
}
