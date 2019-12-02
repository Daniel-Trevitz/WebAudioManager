#include "led.h"

#include <unistd.h>                     //Needed for I2C port
#include <fcntl.h>                      //Needed for I2C port
#include <sys/ioctl.h>                  //Needed for I2C port
#include <linux/i2c-dev.h>              //Needed for I2C port

#include <iostream>
#include <cmath>

LED::brgb LED::normalize(int led)
{
    // TODO: The following doesn't do chromagraphic stuff
    // Return a color in terms of led brightness, normalizing for chromagraphic difference

    const brgb in = ledState[led];
    brgb res;

    // TODO: Refactor the math
    // TODO: Get a real algorithm for value to color
    double bright_percent = double(in.brightness) / 255.0;
    double r = double(in.r) / 255.0;
    double g = double(in.g) / 255.0;
    double b = double(in.b) / 255.0;

    res.r = char(bright_percent * r * 255.0);
    res.g = char(bright_percent * g * 255.0);
    res.b = char(bright_percent * b * 255.0);

    return res;
}

void LED::update()
{
    char buffer[0x17];

    buffer[0x00] = 1;
    buffer[0x13] = 0x3f;
    buffer[0x14] = 0x3f;
    buffer[0x15] = 0x3f;
    buffer[0x16] = 1;

    brgb colors[6] =
    {
        normalize(0),
        normalize(1),
        normalize(2),
        normalize(3),
        normalize(4),
        normalize(5)
    };

    buffer[0x01] = colors[0].b;
    buffer[0x02] = colors[0].g;
    buffer[0x03] = colors[0].r;

    buffer[0x07] = colors[1].b;
    buffer[0x08] = colors[1].g;
    buffer[0x09] = colors[1].r;

    buffer[0x06] = colors[2].b;
    buffer[0x04] = colors[2].g;
    buffer[0x05] = colors[2].r;

    buffer[0x0a] = colors[3].b;
    buffer[0x0c] = colors[3].g;
    buffer[0x0b] = colors[3].r;

    buffer[0x0f] = colors[4].b;
    buffer[0x0d] = colors[4].g;
    buffer[0x0e] = colors[4].r;

    buffer[0x10] = colors[5].b;
    buffer[0x11] = colors[5].g;
    buffer[0x12] = colors[5].r;

    size_t length = 0x17;
    if (write(file_i2c, buffer, length) != ssize_t(length))
    {
        std::cout << "Failed to write to the i2c bus. " << length << " " << errno << std::endl;
    }
}

void LED::delayUpdate(bool yes)
{
    delayedUpdate = yes;
}

LED::LED()
{
    //----- OPEN THE I2C BUS -----
    if ((file_i2c = open("/dev/i2c-1", O_RDWR)) < 0)
    {
        //ERROR HANDLING: you can check errno to see what went wrong
        printf("Failed to open the i2c bus");
        return;
    }

    int addr = 0x54;          //<<<<<The I2C address of the slave
    if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
    {
        std::cout << "Failed to acquire bus access and/or talk to slave." << std::endl;
        //ERROR HANDLING; you can check errno to see what went wrong
        return;
    }

    shutdown(false);
    /*
    0x13 0x1    1 B
    0x13 0x2    1 G
    0x13 0x4    1 R

    0x13 0x8    3 G
    0x13 0x10   3 R
    0x13 0x20   3 B

    0x14 0x1    2 B
    0x14 0x2    2 G
    0x14 0x4    2 R

    0x14 0x8    4 B
    0x14 0x10   4 R
    0x14 0x20   4 G

    0x15 0x1    5 G
    0x15 0x2    5 R
    0x15 0x4    5 B
    */
}

void LED::shutdown(bool off)
{
    char buffer[2];
    buffer[0] = 0;
    buffer[1] = off ? 0 : 1;
    size_t length = 0x2;                   //<<< Number of bytes to write
    if (write(file_i2c, buffer, length) != ssize_t(length))          //write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
        /* ERROR HANDLING: i2c transaction failed */
        std::cout << "Failed to write %i to the i2c bus." << std::endl;
    }
}

void LED::setBrightness(int led, int brightness)
{
    if(led < 0 || led > 6)
        return;

    ledState[led].brightness = char(std::min(255, std::max(0, brightness)));

    if(!delayedUpdate)
        update();
}

void LED::setColor(int led, char r, char g, char b)
{
    if(led < 0 || led > 6)
        return;

    ledState[led].r = r;
    ledState[led].g = g;
    ledState[led].b = b;

    if(!delayedUpdate)
        update();
}

/*
void LED::setBrightness(int led, int brightness)
{
    char regR = 0;
    char regG = 0;
    char regB = 0;
    switch(led)
    {
    case 1:
        regB = 0x01;
        regG = 0x02;
        regR = 0x03;

        break;
    case 2:
        regB = 0x07;
        regG = 0x08;
        regR = 0x09;

        break;
    case 3:
        regB = 0x06;
        regG = 0x04;
        regR = 0x05;

        break;
    case 4:
        regB = 0x0A;
        regG = 0x0C;
        regR = 0x0B;

        break;
    case 5:
        regB = 0x0F;
        regG = 0x0D;
        regR = 0x0E;
        break;
    }

    if(regR == 0)
        return;

    size_t length = 0x2;                   //<<< Number of bytes to write

    char buffer[2];
    buffer[0] = regR;
    buffer[1] = char(brightness);
    if (write(file_i2c, buffer, length) != ssize_t(length))          //write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
            printf("Failed to write %i to the i2c bus.\n", 0);
    }
    buffer[0] = regG;
    if (write(file_i2c, buffer, length) != ssize_t(length))          //write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
            printf("Failed to write %i to the i2c bus.\n", 0);
    }
    buffer[0] = regB;
    if (write(file_i2c, buffer, length) != ssize_t(length))          //write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
            printf("Failed to write %i to the i2c bus.\n", 0);
    }

    buffer[0] = 0x16;
    buffer[1] = 0;
    if (write(file_i2c, buffer, length) != ssize_t(length))          //write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
            printf("Failed to write %i to the i2c bus.\n", 0);
    }
}
*/


/*
void LED::setOn(int led, enum Color r, bool on)
{
    int reg = -1;
    int bit = 0;
    switch(led)
    {
    case 1:
        reg = 0;
        switch(r)
        {
        case red:   bit = 0x04; break;
        case green: bit = 0x02; break;
        case blue:  bit = 0x01; break;
        }

        break;
    case 2:
        reg = 1;
        switch(r)
        {
        case red:   bit = 0x04; break;
        case green: bit = 0x02; break;
        case blue:  bit = 0x01; break;
        }

        break;
    case 3:
        reg = 0;
        switch(r)
        {
        case red:   bit = 0x10; break;
        case green: bit = 0x08; break;
        case blue:  bit = 0x20; break;
        }
        break;
    case 4:
        reg = 1;
        switch(r)
        {
        case red:   bit = 0x10; break;
        case green: bit = 0x20; break;
        case blue:  bit = 0x08; break;
        }
        break;
    case 5:
        reg = 2;
        switch(r)
        {
        case red:   bit = 0x02; break;
        case green: bit = 0x01; break;
        case blue:  bit = 0x04; break;
        }
        break;
    }

    if(reg == -1 || bit == 0)
        return;

    if(on)
    {
        statReg[reg] |= bit;
    }
    else
    {
        statReg[reg] &= ~bit;
    }

    size_t length = 0x2;                   //<<< Number of bytes to write

    char buffer[2];
    buffer[0] = char(0x13 + reg);
    buffer[1] = char(statReg[reg]);
    if (write(file_i2c, buffer, length) != ssize_t(length))          //write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
            // ERROR HANDLING: i2c transaction failed
            printf("Failed to write %i to the i2c bus.\n", 0);
    }

    buffer[0] = 0x16;
    buffer[1] = 0;
    if (write(file_i2c, buffer, length) != ssize_t(length))          //write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
            // ERROR HANDLING: i2c transaction failed
            printf("Failed to write %i to the i2c bus.\n", 0);
    }
}
*/
