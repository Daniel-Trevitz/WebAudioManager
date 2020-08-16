#include "volume.h"
#include "utils/async_process.h"
#include "httpserver/bad_request.h"
#include "device_interaction.h"

#include "utils/json.hpp"

#include <iostream>

#include <unistd.h>                     //Needed for I2C port
#include <fcntl.h>                      //Needed for I2C port
#include <sys/ioctl.h>                  //Needed for I2C port
#include <linux/i2c-dev.h>              //Needed for I2C port

Volume::Volume()
{
    for(int i = 0; i < amps; i++)
    {
        //----- OPEN THE I2C BUS -----
        if ((file_i2c[i] = open("/dev/i2c-1", O_RDWR)) < 0)
        {
            //ERROR HANDLING: you can check errno to see what went wrong
            std::cerr << "Failed to open the i2c bus: " << errno << " " << file_i2c[i] << std::endl;
            return;
        }

        if (ioctl(file_i2c[i], I2C_SLAVE, address[i]) < 0)
        {
            //ERROR HANDLING; you can check errno to see what went wrong
            std::cerr << "Failed to acquire bus access and/or talk to slave. " << errno << std::endl;
            return;
        }
    }
}

Volume::~Volume()
{
    for(int i = 0; i < amps; i++)
        close(file_i2c[i]);
}

const std::shared_ptr<httpserver::http_response> Volume::render(const httpserver::http_request &req)
{
    auto args = req.get_args();

    if(args.size() == 0)
    {

    }
    else if(args.count("mute"))
    {
        setMute(!m_muted);
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
            return bad_request();
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


    nlohmann::json j;

    for(size_t i = 0; i < 3; i++)
        j[std::to_string(i)] = m_volume[i];

    j["muted"] = (m_muted ? "1" : "0");
    j["max"] = maxVolume;

    using namespace httpserver;
    return std::shared_ptr<http_response>(new string_response(j.dump(1)));
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
    // std::cout << "Muting " << mute << std::endl;
    m_muted = mute;
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

bool Volume::muted() const
{
    // std::cout << "Muted? " << std::endl;
    return m_muted;
}

void Volume::setVolume(int speaker, int volume)
{
    if(speaker < 0 || speaker >= amps)
        return;

    m_volume[speaker] = std::min(100, std::max(0, volume));

    set(speaker, volume);
}

void Volume::setMaxVolume(int maxVolume)
{
    this->maxVolume = maxVolume;
}

int Volume::getMaxVolume() const
{
    return maxVolume;
}

int Volume::getVolume(int speaker)
{
    if(speaker < 0 || speaker >= amps)
        return -1;

    return m_volume[speaker];
}

// TODO: Replace with real i2c call?
void Volume::set(int speaker, int volume)
{
    const uint8_t cmdvol = uint8_t(double(volume) * 0.60);

    if((speaker < 0) || (speaker >= amps) || (file_i2c[speaker] < 0))
        return;

    //std::cout << "Vol CMD: " << speaker << " " << cmdvol << std::endl;

    int ret = write(file_i2c[speaker], &cmdvol, 1);
    if(ret != 1)
        std::cerr << "Failed i2c write for volume! " << errno << " " << ret<< file_i2c[speaker] << std::endl;

    Settings::SaveVolumes();
}
