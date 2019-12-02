#ifndef LIBRARY_H
#define LIBRARY_H

#include <httpserverpp>

class Library : public httpserver::http_resource
{
    class pLibrary *p;
public:
    Library(class MediaManager *mm, class Player *player);
    ~Library();
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request&);
};

#endif // LIBRARY_H
