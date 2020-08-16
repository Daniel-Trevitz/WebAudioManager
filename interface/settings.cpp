#include "settings.h"

#include "device_interaction.h"

#define SETTINGS_PATH "/home/pi/settings/"

#include <iostream>
#include <fstream>

using namespace httpserver;

Settings::Settings()
{
    load("led_brightness");

    for(int i = 0; i < Volume::amps; i++)
        load("volume_" + std::to_string(i));

    load("max_volume");
}

void Settings::SaveVolumes()
{
    for(int i = 0; i < Volume::amps; i++)
        set("volume_" + std::to_string(i), std::to_string(volume->getVolume(i)));
}

const std::shared_ptr<httpserver::http_response> Settings::render(const httpserver::http_request &req)
{
    auto args = req.get_args();
    std::string resp;

    if(args.count("led_brightness"))
    {
        if(args["led_brightness"].size() > 0)
            ledBrightness = atoi(args["led_brightness"].c_str());
        resp = std::to_string(ledBrightness);
        set("led_brightness", resp);
    }
    else if(args.count("poweroff"))
    {
        system("sudo poweroff");
    }
    else if(args.count("reboot"))
    {
        system("sudo reboot");
    }
    else if(args.count("save_volumes"))
    {
        SaveVolumes();
    }
    else if(args.count("max_volume"))
    {
        if(args["max_volume"].size() > 0)
        {
            const int mv = atoi(args["max_volume"].c_str());
            std::cout << "Got max volume request: " << mv << ":" << args["max_volume"] << std::endl;
            if((mv > 10) && (mv <= 100))
            {
                volume->setMaxVolume(mv);
                set("max_volume", std::to_string(mv));
            }
        }
        resp = std::to_string(volume->getMaxVolume());
    }

    return std::shared_ptr<http_response>(new string_response(resp));
}

void Settings::set(const std::string &parameter, const std::string &value)
{
    std::ofstream out;

    out.open(SETTINGS_PATH + parameter);
    if(!out.is_open())
    {
        std::cerr << "Failed to open for write " << parameter << std::endl;
        return;
    }

    out << value;
    out.close();

    system("sync");
}

void Settings::load(const std::string &parameter)
{
    std::string value;

    std::ifstream in;
    in.open(SETTINGS_PATH + parameter);
    if(!in.is_open())
    {
        std::cerr << "Failed to open for read " << parameter << std::endl;
        return;
    }

    const size_t length = 1000;
    char data[length];
    memset(data, 0, length);
    in.read(data, length);
    value = data;

    std::cout << parameter << "=" << value << std::endl;

    if(parameter == "led_brightness")
        ledBrightness = atoi(value.c_str());
    else if(parameter == "volume_0")
        volume->setVolume(0, atoi(value.c_str()));
    else if(parameter == "volume_1")
        volume->setVolume(1, atoi(value.c_str()));
    else if(parameter == "volume_2")
        volume->setVolume(2, atoi(value.c_str()));
    else if(parameter == "max_volume")
        volume->setMaxVolume(atoi(value.c_str()));
}
