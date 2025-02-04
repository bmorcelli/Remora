#ifndef __RF_H_
#define __RF_H_

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include "../rmt_common.h"
#include "driver/gpio.h"
#include <SPI.h>

class CC1101 : private RMT_COMMON {
private:
    char* data_read;
    static SPIClass _spi;
    SPIClass *__spi;
    float frequency=433.92;

public:

    CC1101(gpio_num_t miso=GPIO_NUM_NC, gpio_num_t mosi=GPIO_NUM_NC, gpio_num_t sck=GPIO_NUM_NC, gpio_num_t cs=GPIO_NUM_NC, gpio_num_t gdo0=GPIO_NUM_NC, SPIClass* spi = &_spi);
    ~CC1101();

    bool setup(gpio_num_t miso=GPIO_NUM_NC, gpio_num_t mosi=GPIO_NUM_NC, gpio_num_t sck=GPIO_NUM_NC, gpio_num_t cs=GPIO_NUM_NC, gpio_num_t gdo0=GPIO_NUM_NC, SPIClass* spi = &_spi);
    void scan();
    char* send_data();

    inline float getFrequency() { return frequency; }
    bool setFrequency(float freq);

};




#endif