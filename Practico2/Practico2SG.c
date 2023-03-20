#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define AHT10_ADDRESS 0x38 // AHT10 I2C address

int main()
{
    int file;
    char *filename = "/dev/i2c-1"; // I2C bus device file
    if ((file = open(filename, O_RDWR)) < 0)
    {
        printf("Failed to open I2C bus %s\n", filename);
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, AHT10_ADDRESS) < 0)
    {
        printf("Failed to select AHT10 sensor\n");
        exit(1);
    }

    // Send command to measure temperature and humidity
    char command[3] = {0xAC, 0x33, 0x00};
    write(file, command, 3);

    usleep(50000); // Wait for measurement to complete (50ms)

    // Read temperature and humidity data
    /*char data[6];
    read(file, data, 6);
    int temperature = ((data[3] << 12) | (data[1] << 4) | (data[2] & 0x0F));
    temperature = temperature * 200 / 4096 - 50;
    int humidity = ((data[3] << 12) | (data[4] << 4) | (data[5] & 0x0F));
    humidity = humidity * 100 / 4096;*/

    char data[6];
    read(file, data, 6);
    float cur_temp, ctmp;

    cur_temp = (((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5]);
    cur_temp = ((cur_temp * 200) / 1048576) - 50;
    printf("Temperature: %2.2f\n", cur_temp);
    ctmp = ((data[1] << 16) | (data[2] << 8) | data[3]) >> 4;
    ctmp = ctmp * 100 / 1048576;
    printf("Humidity: %1.f %\n", ctmp);

    // printf("Temperature: %d.%d C\n", temperature / 10, temperature % 10);
    // printf("Humidity: %d%%\n", humidity);

    close(file);
    return 0;
}
