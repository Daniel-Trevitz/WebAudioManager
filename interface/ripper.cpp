#include "ripper.h"

#include <iostream>

using namespace httpserver;

Ripper::Ripper()
{

}

const std::shared_ptr<http_response> Ripper::render(const http_request &req)
{
//    auto args = req.get_args();

    return std::shared_ptr<http_response>(new string_response("Hello, World!"));
}

const std::shared_ptr<http_response> Ripper::render_POST(const http_request &req)
{
    std::cout << req.get_content() << std::endl;
    return std::shared_ptr<http_response>(new string_response("Received"));
}
