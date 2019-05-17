#include "player.h"

#include "async_process.h"
#include <iostream>
using namespace httpserver;

Player::Player()
{
//    std::vector<std::string> argv;
//    argv.push_back("--help");
//    async_process test("/usr/bin/mplayer", argv);
//    test.exec();
//    while(true)
//    {
//        auto s = test.readStdOut();
//        auto r = test.readStdErr();
//        if(s.size())
//            std::cout << "DAN:\t" << s << std::flush;
//        if(r.size())
//            std::cout << "Err:\t" << r << std::flush;
//    }
}

const std::shared_ptr<http_response> Player::render(const http_request &req)
{
//    auto args = req.get_args();

    return std::shared_ptr<http_response>(new string_response("Hello, World!"));
}
