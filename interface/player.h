#ifndef PLAYER_H
#define PLAYER_H

#include <httpserverpp>

class Player : public httpserver::http_resource
{
    void dbus(std::string cmd);
    class mpv_handle *ctx;
    class libvlc_instance_t *inst;
    class libvlc_media_player_t *mp = nullptr;
    std::string cur_stream, prev_stream;
    class async_process *proc = nullptr;
    bool m_playing = false;
    bool m_paused = false;
public:
    Player();
    ~Player();
    bool play(); // play last stream
    bool play(const std::string &stream);
    bool play_cd();
    bool eject_cd();
    bool pause(bool c_pause = true);
    bool changeChapter(bool fwd);
    bool stop();
    bool isPaused() const;
    bool isPlaying() const; // returns true when paused or playing (not stopped)
    bool isPlayingCD() const;//returns true when paused or playinga cd stream (not stopped or playing other streams)
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request &req);
};

#endif // PLAYER_H
