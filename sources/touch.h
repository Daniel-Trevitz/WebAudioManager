#ifndef TOUCH_H
#define TOUCH_H

#include <cstdint>

class Touch
{
private:
    static const int gpio_interupt_pin = 7;
    int fd_i2c;
    int fd_interupt;
public:
    // 0 <= pwrKeyIdx <= 5
    Touch(/*uint8_t pwrKeyIdx*/);
    ~Touch();
    // check if power off has been requested
    bool poweroff();
    // Check the gpio for an interupt
    int interuptDetected();
    // Return the device state - bitmask of the keys
    int getKeys();
    // Clear the key state
    void clear();
    // recalibrate the keys now - required after LED changes
    void recalibrate();
};

#endif // TOUCH_H
