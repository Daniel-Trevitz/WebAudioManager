#ifndef RADIO_H
#define RADIO_H

#include <httpserverpp>
#include "player.h"

class Radio : public httpserver::http_resource
{
    class pRadio *p;
public:
    Radio(Player &player);
    ~Radio();
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request &req);
};

#endif // RADIO_H
