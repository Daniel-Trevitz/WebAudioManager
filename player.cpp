#include "player.h"
#include "async_process.h"
#include "bad_request.h"

#include <iostream>
using namespace httpserver;

Player::Player()
{
}

Player::~Player()
{
    delete proc;
}

bool Player::play(const std::string &stream)
{
    if(proc && !stop())
        return false;
    proc = new async_process("/usr/bin/cvlc", stream);
    return proc->exec();
}

bool Player::play_cd()
{
    return play("cdda://dev/cdrom");
}

bool Player::stop()
{
    if(proc == nullptr)
        return false; // nothing to stop!
    delete proc;
    proc = nullptr;
    return true;
}

bool Player::isPlaying()
{
    if(proc && !proc->active())
    {
        delete proc;
        proc = nullptr;
    }
    return proc != nullptr;
}

const std::shared_ptr<http_response> Player::render(const http_request &req)
{
    auto args = req.get_args();
    if(args.count("stop"))
    {
        // Stop whatever is currently playing
        if(!stop())
            return bad_request();
        return std::shared_ptr<http_response>(new string_response("playing=0"));
    }
    else if(args.count("play_cd"))
    {
        // Stop whatever is currently playing
        if(!play_cd())
            return bad_request();
        return std::shared_ptr<http_response>(new string_response("playing=1"));
    }
    else if(args.count("status"))
    {
        std::string str = "playing=";
        if(proc && proc->active())
            str += "1";
        else
            str += "0";

        return std::shared_ptr<http_response>(new string_response(str));
    }

    // No action is invalid
    return bad_request();
}
