#include "utils/async_process.h"

#include "httpserver/directory_server.h"

#include "interface/channel.h"
#include "interface/player.h"
#include "interface/ripper.h"
#include "interface/volume.h"
#include "interface/radio.h"
#include "interface/library.h"

#include "sources/led.h"

#include <iostream>
#include <unistd.h>
#include <pthread.h>

#include <httpserver.hpp>

#include "sources/mediamanager.h"


#include <fstream>

static Player player;
static Volume volume;
static Channel ch(volume); // changing channels requires muting

[[noreturn]] void * cd_detect(void *)
{
    bool found = false;
    while(true)
    {
        sleep(1);
        std::ifstream in;
        in.open("/dev/sr0");

        // TODO: Thread safety...
        if(!in.is_open())
        {
            if(found && player.isPlayingCD())
            {
                std::cout << "CD missing - Stoping playback" << std::endl;
                player.stop();
            }
            found = false;
        }
        else if(found)
        {
            // continue;
        }
        else
        {
            std::cout << "Found a CD - Playing" << std::endl;
            found = true;
            ch.play_from_pi();
            player.play_cd();

        }
    }
}

int main()
{
    pthread_t cd_detect_thread;
    MediaManager media(MEDIA_PATH);
    volume.setVolume(0,25);
    volume.setVolume(1,25);
    volume.setVolume(2,25);


    int iret1 = pthread_create( &cd_detect_thread, nullptr, cd_detect, nullptr);
    if(iret1)
    {
        std::cerr << "Failed to create the cd thread" << std::endl;
    }

#ifdef SIMULATION
    Album a[10];
    for(int i = 0; i < 10; i++)
    {
        a[i].cdid = std::to_string(i);
        a[i].albumName = "Album " + std::to_string(i);
        for(int j = 0; j < (5 + i % 4); j++)
        {
            a[i].titles.push_back(a[i].albumName + " Title " + std::to_string(j));
        }
        if(!media.cacheAlbum(a[i]))
            std::cerr << "sim album failed?! " << i << std::endl;
    }
#endif

//    LED led;
//    led.delayUpdate(true);
//    led.update();
//const double m = 0.1;
//char oR = 0, oG = 0, oB = 0;
//    while(true)
//    {
//        int l = rand() % 5;
//        int a1 = rand() % 2;
//        int a2 = rand() % 2;
//        int a3 = rand() % 2;
//        int t = rand() % 100;

//        char r = a1 ? 255 : 0;
//        char g = a2 ? 255 : 0;
//        char b = a3 ? 255 : 0;

//        if(!a1 && !a2 && !a3)
//            continue;
//        if(a1 && a2 && a3)
//            continue;
//        if((oR == r) && (oB == b) && (oG == g))
//            continue;

//        oR = r;
//        oG = g;
//        oB = b;

//        led.setColor(l, r,g,b);
//    std::cout << int(r) << " " << int(g) << " " << int(b) << std::endl;
//        for(int i = 0; i < 32; i++)
//        {
//            led.setBrightness(l, char(m*double(8*i)));
//            led.update();
//            usleep(10000);
//        }

//        usleep(75000);
//        // led.setBrightness(l, 255);
//        led.update();
//        for(int i = 0; i < 32; i++)
//        {
//            led.setBrightness(l, char(m*double(256-(8*i))));
//            led.update();
//            usleep(10000);
//        }

//        // usleep(100000 + 5*t);
//        led.setBrightness(l, 0);
//        led.update();
//    }

#ifndef SIMULATION
    async_process dbus_launch("/usr/bin/dbus-launch", "--binary-syntax");
    dbus_launch.exec();
#endif

    httpserver::webserver ws = httpserver::create_webserver(7070);

#ifdef SIMULATION
    const char *cwd = "/home/daniel/Projects/EE/ThorntonAudio/WebAudioManager/www/";
#else
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
#endif

    directory_server::add_dir(ws, cwd);

    ws.register_resource("/cgi-bin/channel", &ch);

    Ripper ripper;
    Radio radio(&player);
    Library library(&media, &player);

    ws.register_resource("/cgi-bin/player", &player);
    ws.register_resource("/cgi-bin/ripper", &ripper);
    ws.register_resource("/cgi-bin/volume", &volume);
    ws.register_resource("/cgi-bin/radio",  &radio);
    ws.register_resource("/cgi-bin/library",  &library);

#ifndef SIMULATION
    {
        dbus_launch.wait();
        std::string key = dbus_launch.readStdOut();
        setenv("DBUS_SESSION_BUS_ADDRESS", key.c_str(), 1);
        std::cout << key << std::endl;
    }
#endif

    std::cout << "Starting webserver" << std::endl;

    try {
        ws.start(true);
    } catch (...) {
        std::cerr << "Failed to start webserver" << std::endl;
    }

    return 0;
}
