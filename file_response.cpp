#include "file_response.h"

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

// TODO: Is there a mime library we can take advantage of?
// TODO: Cache results?
std::string file_response::getType(std::string filename)
{
    // lowercase please
    transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

    if(ends_with(filename, ".html"))
        return "text/html";

    if(ends_with(filename, ".js"))
        return "text/javascript";

    if(ends_with(filename, ".css"))
        return "text/css";

    if(ends_with(filename, ".png"))
        return "text/png";

    if(ends_with(filename, ".jpeg") || ends_with(filename, ".jpg"))
        return "text/jpeg";

    if(ends_with(filename, ".gif"))
        return "text/gif";

    return "text/plain";
}

file_response::file_response(const std::string &filename) :
    m_filename(filename),
    m_type(getType(filename))
{

}

const std::shared_ptr<httpserver::http_response> file_response::render(const httpserver::http_request &)
{
    auto resp = new httpserver::file_response(
                m_filename, httpserver::http::http_utils::http_ok, m_type);

    return std::shared_ptr<httpserver::http_response>(
                reinterpret_cast<httpserver::http_response *>(resp));
}
