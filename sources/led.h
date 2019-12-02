#ifndef LED_H
#define LED_H

class LED
{
    int file_i2c;

    struct brgb
    {
        char brightness = 0;
        char r = 0, g = 0, b = 0;
    };

    brgb ledState[6];

    bool delayedUpdate = false;

    brgb normalize(int led);

public:
    LED();

    void update();
    void delayUpdate(bool yes = true);
    void shutdown(bool off = true);
    void setBrightness(int led, int brightness);
    void setColor(int led, char r, char g, char b);
};

#endif // LED_H
