#ifndef STATUS_H
#define STATUS_H

#include "ripper.h"
#include "player.h"

#include <httpserverpp>

class Status : public httpserver::http_resource
{
    Player &player;
    Ripper &ripper;
public:
    Status(Player &player, Ripper &ripper);
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request &req);
};

#endif // STATUS_H
