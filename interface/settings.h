#ifndef SETTINGS_H
#define SETTINGS_H

#include <httpserverpp>

class Settings : public httpserver::http_resource
{
    static void set(const std::string &parameter, const std::string &value);
    static void load(const std::string &parameter);
public:
    Settings();
    static void SaveVolumes();
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request &req);
};

#endif // SETTINGS_H
