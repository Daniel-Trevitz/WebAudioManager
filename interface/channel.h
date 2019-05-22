#ifndef CHANNEL_H
#define CHANNEL_H

#include <httpserverpp>
#include "volume.h"

class Channel : public httpserver::http_resource
{
    int m_channel;
    Volume &m_volume;
public:
    Channel(Volume &volume);

    // Set the current channel
    bool set(int channel);
    bool set(std::string channel);

    // get the current channel
    int get();

    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request&);
};

#endif // CHANNEL_H
