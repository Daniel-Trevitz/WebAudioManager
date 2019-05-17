#ifndef PLAYER_H
#define PLAYER_H

#include <httpserverpp>

class Player : public httpserver::http_resource
{
public:
    Player();
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request &req);
};

#endif // PLAYER_H
