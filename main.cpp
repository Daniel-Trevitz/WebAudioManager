#include "directory_server.h"
#include "channel.h"
#include "status.h"
#include "player.h"
#include "ripper.h"
#include "volume.h"
#include "radio.h"
#include "async_process.h"
#include "cdinfo.h"

#include <iostream>
#include <unistd.h>

#include <httpserver.hpp>

int main()
{
    async_process dbus_launch("/usr/bin/dbus-launch", "--binary-syntax");
    dbus_launch.exec();

    httpserver::webserver ws = httpserver::create_webserver(8080);

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    directory_server::add_dir(ws, cwd);

    Volume volume;

    Channel ch(volume); // changing channels requires muting
    ws.register_resource("/cgi-bin/channel", &ch);

    Player player;
    Ripper ripper;
    Radio radio(player);
    Status status(player, ripper);

    ws.register_resource("/cgi-bin/status", &status);
    ws.register_resource("/cgi-bin/player", &player);
    ws.register_resource("/cgi-bin/ripper", &ripper);
    ws.register_resource("/cgi-bin/volume", &volume);
    ws.register_resource("/cgi-bin/radio",  &radio);

    {
        dbus_launch.wait();
        std::string key = dbus_launch.readStdOut();
        setenv("DBUS_SESSION_BUS_ADDRESS", key.c_str(), 1);
        std::cout << key << std::endl;
    }

    std::cout << "Starting webserver" << std::endl;

    try {
        ws.start(true);
    } catch (...) {
        std::cerr << "Failed to start webserver" << std::endl;
    }

    return 0;
}
