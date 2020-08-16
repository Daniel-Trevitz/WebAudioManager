#include "touch.h"

#include <unistd.h>                     //Needed for I2C port
#include <fcntl.h>                      //Needed for I2C port
#include <sys/ioctl.h>                  //Needed for I2C port
#include <linux/i2c-dev.h>              //Needed for I2C port

#include <fstream>
#include <iostream>

Touch::Touch(/*uint8_t pwrKeyIdx*/)
{
    //----- OPEN THE I2C BUS -----
    if ((fd_i2c = open("/dev/i2c-1", O_RDWR)) < 0)
    {
        //ERROR HANDLING: you can check errno to see what went wrong
        printf("Failed to open the i2c bus");
        return;
    }

    int addr = 0x28;          //<<<<<The I2C address of the slave
    if (ioctl(fd_i2c, I2C_SLAVE, addr) < 0)
    {
        //ERROR HANDLING; you can check errno to see what went wrong
        std::cerr << "Failed to acquire bus access and/or talk to slave." << std::endl;
        return;
    }

    std::ofstream gpio_export;
    gpio_export.open ("/sys/class/gpio/export");
    gpio_export << gpio_interupt_pin << std::endl;
    gpio_export.close();


    usleep(100);
    fd_interupt = ::open("/sys/class/gpio/gpio7/value", O_RDWR);
    while(fd_interupt < 0)
    {
        usleep(100);
        std::cerr << "Failed to open gpio 7 for touch interupt!" << std::endl;
        fd_interupt = ::open("/sys/class/gpio/gpio7/value", O_RDWR);
    }


    // i2c_smbus_write_byte_data(fd_i2c, 0x60, pwrKeyIdx); // set power button on the requested key
    // i2c_smbus_write_byte_data(fd_i2c, 0x61, 0x07); // longest hold time and enable

    i2c_smbus_write_byte_data(fd_i2c, 0x21, 0x1F); // Disable the unused 6th input

    i2c_smbus_write_byte_data(fd_i2c, 0x24, 0x31); // cycle as fast as possible (35ms)
    // force a recalibration now
    recalibrate();
}

Touch::~Touch()
{
    close(fd_interupt);
    close(fd_i2c);
}

int Touch::interuptDetected()
{
    lseek(fd_interupt, 0, SEEK_SET);
    char state = '1';
    read(fd_interupt, &state, 1);
    return state == '0';
}

int Touch::getKeys()
{
    int ret = i2c_smbus_read_byte_data(fd_i2c, 0x03);
    if(ret < 0)
    {
        std::cerr << "i2c read failed " << errno << " " << fd_i2c << std::endl;
        return 0;
    }
    return int(ret);
}

bool Touch::poweroff()
{
    const int ret = i2c_smbus_write_byte_data(fd_i2c, 0, 2);
    return (ret == -1) ? false : ((ret & 0x10) == 0x10);
}

void Touch::clear()
{
    i2c_smbus_write_byte_data(fd_i2c, 0, 0);
}

void Touch::recalibrate()
{
    i2c_smbus_write_byte_data(fd_i2c, 0x26, 0x1F);
}
