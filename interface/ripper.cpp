#include "ripper.h"

#include "sources/cdinfo.h"

#include <iostream>
using namespace httpserver;

Ripper::Ripper()
{

}

const std::shared_ptr<http_response> Ripper::render(const http_request &req)
{
    auto args = req.get_args();
    std::string resp;

    if(args.count("form"))
    {

        if(!CDInfo::cdInstalled())
        {
            resp =  "No CD Found - insert CD and refresh the page";
        }
        else
        {
            CDInfo cd(true);

            if(!cd.valid())
            {
                resp = "Failed to get the CD information" "<BR/>" + cd.getError();
            }
            else
            {
                auto tracks = cd.tracks();

                resp += "<form id='myform' action='/cgi-bin/ripper' method='post' enctype='multipart/form-data'>"
                        "<input type='file' name='image' accept='image/*' capture='environment'></input>";

                resp += "<input type='text' id='title'>" + cd.title() + "</input>";
                resp += "<input type='text' id='genre'>" + cd.genre() + "</input>";

                for(size_t i = 0; i < tracks.size(); i++)
                    resp += "<input type='text' id='track-" + std::to_string(i) + "'>" + tracks.at(i) + "</input>";

                resp += "<input type='submit'>"
                        "</form>";
            }
        }
    }
    else if(args.empty() || args.count("status"))
    {

    }

    return std::shared_ptr<http_response>(new string_response(resp));
}

const std::shared_ptr<http_response> Ripper::render_POST(const http_request &req)
{
    auto r = req.get_content();

    return std::shared_ptr<http_response>(new string_response("Received"));
}
