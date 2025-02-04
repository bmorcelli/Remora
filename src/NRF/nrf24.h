#ifndef __NRF24_H_
#define __NRF24_H_
#include <RF24.h>
#include <config.h>
#include <SPI.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

class NRF24: RF24 {
private:
    RF24 radio;
    SPIClass NRF_spi;
    SPIClass* _NRF_spi;
    gpio_num_t _cs;
    gpio_num_t _ce;
    TaskHandle_t handler = nullptr;
    static void Jammertask(void *parameter);

public:
    NRF24();
    ~NRF24();

    bool begin(gpio_num_t sck = (gpio_num_t)SPI_SCK_PIN, gpio_num_t mosi=(gpio_num_t)SPI_MOSI_PIN, gpio_num_t miso = (gpio_num_t)SPI_MISO_PIN, gpio_num_t cs = (gpio_num_t)NRF24_1_CS, gpio_num_t ce = (gpio_num_t)NRF24_1_CE);
    bool begin(SPIClass* spi, gpio_num_t cs = (gpio_num_t)NRF24_1_CS, gpio_num_t ce = (gpio_num_t)NRF24_1_CE);
    bool end();

    bool start_jamming();
    bool stop_jamming();

};

#endif