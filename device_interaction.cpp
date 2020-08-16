#include "device_interaction.h"

#include "sources/led.h"
#include "sources/touch.h"

int ledBrightness = 50;

#include <iostream>
#include <fstream>

void * device_interaction(void *)
{
    // leds.setColor(3, 255,0,0);

    static const int LED_Channel = 0;
    static const int LED_PlayerState = 1;

    constexpr int key0 = 8;
    constexpr int key1 = 16;
    constexpr int key2 = 1;
    constexpr int key3 = 4;
    constexpr int key4 = 2;

    constexpr int key_Pause = key0;
    constexpr int key_MasterVolUp = key1;
    constexpr int key_MasterVolDn = key2;
    constexpr int key_ChangeChannel = key3;
    constexpr int key_Mute = key4;

    enum PlayerState
    {
        stopped,
        playing,
        paused
    } oldPlayerState = stopped;
    int oldChannel = -1;
    int oldLEDBright = 0;
    int oldKeyState = 0;


    LED leds;
    Touch touch/*(3)*/;

    // samples before we allow a new press
    const int debounceLimit = 2;
    int debounceCnt = debounceLimit;

    int cnt = 0;
    bool found = false;
    while(true)
    {
        // once every milisecond check for user input and update the LEDs
        usleep(100);

        /*
            if(touch->poweroff())
            {
                std::cout << "Poweroff requested!" << std::endl;
                touch->clear();
            }
            */

        if(ledBrightness != oldLEDBright)
        {
            oldLEDBright = ledBrightness;
            for(int i = 0; i < 5; i++)
                leds.setBrightness(i, ledBrightness);
        }

        if(player->isPlaying())
        {
            if(player->isPaused())
            {
                if(oldPlayerState != paused)
                    leds.setColor(LED_PlayerState, 255, 0, 0);
                oldPlayerState = paused;
            }
            else
            {

                leds.setColor(LED_PlayerState, 0, 255, 0);
                oldPlayerState = playing;
            }
        }
        else if(oldPlayerState != stopped)
        {
            leds.setColor(LED_PlayerState, 0,0,0);
            oldPlayerState = stopped;
        }


        const int keyState = touch.getKeys();

        if(keyState)
        {
            oldKeyState = keyState;
            // std::cout << keyState << " " << std::flush;
            touch.clear();
            debounceCnt = 0;
        }
        else
        {
            debounceCnt++;
        }

        if(debounceCnt == debounceLimit)
        {
            debounceCnt++;
            // std::cout << " Released!" << std::endl;

            if(oldKeyState == key_Pause)
            {
                if(player->isPlaying())
                    player->pause(!player->isPaused());
            }
            else if(oldKeyState == key_MasterVolUp)
            {
                volume->increaseVolumes(1);
            }
            else if(oldKeyState == key_MasterVolDn)
            {
                volume->increaseVolumes(-1);
            }
            else if(oldKeyState == key_ChangeChannel)
            {
                int nch = ch->get() + 1;
                if(nch == 5)
                    nch = -1;
                ch->set(nch);
            }
            else if(oldKeyState == key_Mute)
            {
                volume->setMute(!volume->muted());
            }

            //After an LED changes state we need to recalibrate
            touch.recalibrate();
        }


        if(oldChannel != ch->get())
        {
            switch(oldChannel = ch->get())
            {
            case -1:
            {
                volume->setMute(true);
                player->stop();
                leds.setColor(LED_Channel,   0,  0,    0);
                break;
            }
            case  0: leds.setColor(LED_Channel, 255,  0,    0); break;
            case  1: leds.setColor(LED_Channel,   0, 255,   0); break;
            case  2: leds.setColor(LED_Channel,   0,   0, 255); break;
            case  3: leds.setColor(LED_Channel, 255,   0, 255); break;
            case  4: leds.setColor(LED_Channel, 255, 255, 255); break;
            }
        }


        cnt++;
        if(cnt < 10000)
            continue;
        cnt = 0;

        // once a second check for theCD
        std::ifstream in;
        in.open("/dev/sr0");

        // TODO: Thread safety...
        if(!in.is_open())
        {
            if(found && player->isPlayingCD())
            {
                std::cout << "CD missing - Stoping playback" << std::endl;
                player->stop();
            }
            found = false;
        }
        else if(found)
        {
            // continue; - wait for state change
        }
        else
        {
            std::cout << "Found a CD - Playing" << std::endl;
            found = true;
            ch->play_from_pi();
            player->play_cd();
        }
    }
}
