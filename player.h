#ifndef PLAYER_H
#define PLAYER_H

#include <httpserverpp>

class Player : public httpserver::http_resource
{
    class async_process *proc = nullptr;
public:
    Player();
    ~Player();
    bool play(const std::string &stream);
    bool play_cd();
    bool stop();
    bool isPlaying();
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request &req);
};

#endif // PLAYER_H
