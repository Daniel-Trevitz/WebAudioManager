#ifndef VOLUME_H
#define VOLUME_H

#include <httpserverpp>

class Volume : public httpserver::http_resource
{
public:
    Volume();
    ~Volume();
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request &req);
    void increaseVolumes(int delta);
    void increaseVolume(int speaker, int delta);
    void setVolume(int speaker, int volume);
    void setMaxVolume(int maxVolume);
    int getMaxVolume() const;
    int getVolume(int speaker);
    void setMute(bool mute = true);
    bool muted() const;

    static const int amps = 3;

private:
    const int address[amps] = { 0x49, 0x4A, 0x4B };
    int maxVolume = 100;

    bool m_muted = false;
    int file_i2c[amps];
    int m_volume[amps] = { 0, 0, 0 };
    void set(int speaker, int volume);
};

#endif // VOLUME_H
