#include "directory_server.h"
#include "channel.h"
#include "status.h"
#include "player.h"
#include "ripper.h"
#include "volume.h"

#include <iostream>
#include <unistd.h>

#include <httpserver.hpp>

int main()
{
    httpserver::webserver ws = httpserver::create_webserver(8080);

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    directory_server::add_dir(ws, cwd);

    Volume volume;

    Channel ch(volume); // changing channels requires muting
    ws.register_resource("/cgi-bin/channel", &ch);

    Player player;
    Ripper ripper;
    Status status(player, ripper);

    ws.register_resource("/cgi-bin/status", &status);
    ws.register_resource("/cgi-bin/player", &player);
    ws.register_resource("/cgi-bin/ripper", &ripper);
    ws.register_resource("/cgi-bin/volume", &volume);



    ws.start(true);

    return 0;
}
