#ifndef RIPPER_H
#define RIPPER_H

#include <httpserverpp>

class Ripper : public httpserver::http_resource
{
public:
    Ripper();
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request &req);

    const std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request& req);
};

#endif // RIPPER_H
