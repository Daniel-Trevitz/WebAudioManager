#include "bad_request.h"

using namespace httpserver;
std::shared_ptr<http_response> bad_request()
{
    return std::shared_ptr<http_response>(new http_response(
                                              http::http_utils::http_bad_request,
                                              http::http_utils::text_plain));
}
