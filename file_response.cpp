#include "file_response.h"


file_response::file_response(const std::string &filename) : m_filename(filename)
{

}

const std::shared_ptr<httpserver::http_response> file_response::render(const httpserver::http_request &)
{
    return std::shared_ptr<httpserver::http_response>(
                reinterpret_cast<httpserver::http_response *>(
                    new httpserver::file_response(m_filename,
                                                  httpserver::http::http_utils::http_ok,
                                                  "text/html")
                    )
                );
}
