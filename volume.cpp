#include "volume.h"

#include <iostream>
#include "async_process.h"

const char *Volume::address[Volume::amps] =
{
    "0x49",
    "0x4A",
    "0x4B"
};

Volume::Volume()
{
    for(int i = 0; i < amps; i++)
        setVolume(i,0);
}

const std::shared_ptr<httpserver::http_response> Volume::render(const httpserver::http_request &req)
{
    auto args = req.get_args();

    if(args.size() == 0)
    {
        getVolumes();
    }
    else if(args.count("inc"))
    {
        increaseVolumes(+1);
    }
    else if(args.count("dec"))
    {
        increaseVolumes(-1);
    }
    else
    {
        try {
            if(args.count("volume0"))
            {
                setVolume(0, std::stoi(args["volume0"]));
            }
            if(args.count("volume1"))
            {
                setVolume(1, std::stoi(args["volume1"]));
            }
            if(args.count("volume2"))
            {
                setVolume(2, std::stoi(args["volume2"]));
            }
        }
        catch (...) {
            using namespace httpserver;
            return std::shared_ptr<http_response>(new http_response(
                                                      http::http_utils::http_bad_request,
                                                      http::http_utils::text_plain));
        }

        if(args.count("inc0"))
        {
            increaseVolume(0, 1);
        }
        else if(args.count("dec0"))
        {
            increaseVolume(0, -1);
        }

        if(args.count("inc1"))
        {
            increaseVolume(1, 1);
        }
        else if(args.count("dec1"))
        {
            increaseVolume(1, -1);
        }

        if(args.count("inc2"))
        {
            increaseVolume(2, 1);
        }
        else if(args.count("dec2"))
        {
            increaseVolume(2, -1);
        }
    }

    const std::string str =
            "volume_0 = " + std::to_string(m_volume[0]) + ";\n" +
            "volume_1 = " + std::to_string(m_volume[1]) + ";\n" +
            "volume_2 = " + std::to_string(m_volume[2]) + ";";

    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(str));
}

void Volume::increaseVolumes(int delta)
{
    if(delta == 0)
        return;

    if(delta < 0)
    {
        for(int i = 0; i < amps; i++)
        {
            if((delta + m_volume[i]) < 0)
            {
                delta = -m_volume[i];
                if(delta == 0)
                {
                    return;
                }
                else if(delta > 0)
                {
                    std::cerr << __PRETTY_FUNCTION__ << " " << __LINE__ << "Probabbly bad" << std::endl;
                    return;
                }
                i = 0;
            }
        }
    }
    else if(delta > 0)
    {
        for(int i = 0; i < amps; i++)
        {
            if((delta + m_volume[i]) > 100)
            {
                delta = 100 - m_volume[i];
                if(delta == 0)
                {
                    return;
                }
                else if(delta < 0)
                {
                    std::cerr << __PRETTY_FUNCTION__ << " " << __LINE__ << "Probabbly bad" << std::endl;
                    return;
                }
                i = 0;
            }
        }
    }

    for(int i = 0; i < amps; i++)
    {
        m_volume[i] += delta;
        if(m_volume[i] < 0)
        {
            std::cerr << __PRETTY_FUNCTION__ << " " << __LINE__ << "Definitely bad" << std::endl;
            m_volume[i] = 0;
        }
        else if(m_volume[i] > 100)
        {
            std::cerr << __PRETTY_FUNCTION__ << " " << __LINE__ << "Definitely bad" << std::endl;
            m_volume[i] = 100;
        }
        setVolume(i, m_volume[i]);
    }
}

void Volume::increaseVolume(int speaker, int delta)
{
    setVolume(speaker, m_volume[speaker] + delta);
}

// Mute by settin the volumes to zero, but not overwriting the previous state
void Volume::setMute(bool mute)
{
    if(mute)
    {
        for(int i = 0; i < amps; i++)
            set(i, 0);
    }
    else
    {
        for(int i = 0; i < amps; i++)
            set(i, m_volume[i]);
    }
}

void Volume::setVolume(int speaker, int volume)
{
    if(speaker < 0 || speaker >= amps)
        return;

    m_volume[speaker] = std::min(100, std::max(0, volume));

    set(speaker, volume);
}

void Volume::getVolumes()
{
    // TODO: implement - not actually needed (even a little bit)
}

// TODO: Replace with real i2c call?
void Volume::set(int speaker, int volume)
{
    const int cmdvol = int(double(volume) * 0.60);

    std::vector<std::string> args;
        args.push_back("-y");
        args.push_back("1");
        args.push_back(address[speaker]);
        args.push_back(std::to_string(cmdvol));

    async_process proc("/usr/sbin/i2cget", args);
    proc.exec();
    proc.wait();
}
