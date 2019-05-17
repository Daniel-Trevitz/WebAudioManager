#ifndef FILE_RESPONSE_H
#define FILE_RESPONSE_H

#include <string>

#include <httpserver.hpp>

class file_response : public httpserver::http_resource
{
    const std::string m_filename;
public:
    file_response(const std::string &filename);
    const std::shared_ptr<httpserver::http_response> render(const httpserver::http_request&);
};

#endif // FILE_RESPONSE_H
