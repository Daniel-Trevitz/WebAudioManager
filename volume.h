#ifndef VOLUME_H
#define VOLUME_H

#include <httpserverpp>

class Volume : public httpserver::http_resource
{
    static const int amps = 3;
    static const char *address[amps];

    int m_volume[amps] = { 0, 0, 0 };
    void set(int speaker, int volume);
public:
    Volume();
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request &req);
    void increaseVolumes(int delta);
    void increaseVolume(int speaker, int delta);
    void setVolume(int speaker, int volume);
    void getVolumes(); // update volume from ipc
    void setMute(bool mute = true);
};

#endif // VOLUME_H
