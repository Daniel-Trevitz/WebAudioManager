#ifndef INDEX_H
#define INDEX_H

#include <httpserver.hpp>

class index_html : public httpserver::http_resource
{
public:
    index_html();

    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request&);
};

#endif // INDEX_H
