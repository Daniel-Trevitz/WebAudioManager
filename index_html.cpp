#include "index_html.h"

using namespace httpserver;

index_html::index_html()
{

}

const std::shared_ptr<http_response> index_html::render(const http_request &) {
    return std::shared_ptr<http_response>(new string_response("Hello, World!"));
}
