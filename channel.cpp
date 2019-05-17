#include "channel.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace httpserver;

Channel::Channel(Volume &volume) : m_channel(-1), m_volume(volume)
{

}

bool Channel::set(std::string channel)
{
    try
    {
        return set(std::stoi(channel));
    }
    catch (...)
    {
    }

    return false;
}

static void write(const char *value, int pin)
{
    auto file = "/sys/class/gpio/gpio" + std::to_string(pin) + "/value";

    std::ofstream myfile;
    myfile.open (file);
    myfile << value;
    myfile.close();
}

bool Channel::set(int channel)
{
    const int outputs[] =
    {
        26,
        25,
        24,
        23,
        22
    };

    m_volume.setMute(true);

    for(int i = 0; i < 5; i++)
    {
        write("0", outputs[i]);
    }

    m_channel = channel;
    if(channel == -1)
    {
        m_volume.setMute(false);
        return true;
    }

    if((channel < 0) || (channel > 5))
    {
        m_volume.setMute(false);
        return false;
    }

    write("1", outputs[channel]);
    m_volume.setMute(false);

    return true;
}


int Channel::get()
{
    return m_channel;
}

const std::shared_ptr<http_response> Channel::render(const http_request &req)
{
    std::string resp;

    auto args = req.get_args();
    if(args.count("set"))
    {
       if(!set(args["set"]))
       {
           return std::shared_ptr<http_response>(new http_response(
                                                     http::http_utils::http_bad_request,
                                                     http::http_utils::text_plain));
       }

    }

    resp = "channel=" + std::to_string(get()) + ";";

    return std::shared_ptr<http_response>(new string_response(resp));
}
