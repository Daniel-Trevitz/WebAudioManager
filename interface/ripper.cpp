#include "ripper.h"

using namespace httpserver;

Ripper::Ripper()
{

}

const std::shared_ptr<http_response> Ripper::render(const http_request &req)
{
//    auto args = req.get_args();

    return std::shared_ptr<http_response>(new string_response("Hello, World!"));
}
