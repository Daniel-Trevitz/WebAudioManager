#include "status.h"

using namespace httpserver;

Status::Status(Player &player, Ripper &ripper) :
    player(player),
    ripper(ripper)
{

}

const std::shared_ptr<httpserver::http_response> Status::render(const httpserver::http_request &req)
{
//    auto args = req.get_args();

    return std::shared_ptr<http_response>(new string_response("Hello, World!"));
}
