#include "rf.h"


bool CC1101::setFrequency(float frequency) {
        if(frequency>928 || frequency < 300)  {
            frequency = 433.92;
            Serial.println("Frequency out of band");
            return false;
        }
        ELECHOUSE_cc1101.setMHZ(frequency);
        return true;
}


#define RMT_RX_CHANNEL  RMT_CHANNEL_6
#define RMT_BLOCK_NUM


#define RMT_CLK_DIV   80 /*!< RMT counter clock divider */
#define RMT_1US_TICKS (80000000 / RMT_CLK_DIV / 1000000)
#define RMT_1MS_TICKS (RMT_1US_TICKS * 1000)


void deinitRMT() {
    // Deinit RMT channels
    for (int i = 0; i < RMT_CHANNEL_MAX; i++) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_uninstall((rmt_channel_t)i));
    }
}

void initRMT() {
    deinitRMT();

    rxconfig.rmt_mode            = RMT_MODE_RX;
    rxconfig.channel             = RMT_RX_CHANNEL;
    rxconfig.gpio_num            = gpio_num_t(CC1101_GDO0_PIN);
    rxconfig.clk_div             = RMT_CLK_DIV; // RMT_DEFAULT_CLK_DIV=32
    rxconfig.mem_block_num       = 1;
    rxconfig.flags               = 0;
    rxconfig.rx_config.idle_threshold = 3 * RMT_1MS_TICKS,
    rxconfig.rx_config.filter_ticks_thresh = 200 * RMT_1US_TICKS;
    rxconfig.rx_config.filter_en = true;

    ESP_ERROR_CHECK(rmt_config(&rxconfig));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_driver_install(rxconfig.channel, 2048, 0));

}

bool CC1101::setup(gpio_num_t miso, gpio_num_t mosi, gpio_num_t sck, gpio_num_t cs, gpio_num_t gdo0, SPIClass* spi = &_spi) {
    if(miso<0 || mosi<0 || sck<0 || cs<0 || gdo0<0) { 
        Serial.println("One or more pins are not set");
        return false;
    }
    __spi = spi; // use the SPI instance given to it
    ELECHOUSE_cc1101.setSPIinstance(__spi);
    ELECHOUSE_cc1101.setSpiPin(sck,miso,mosi,cs);
    ELECHOUSE_cc1101.setGDO0(gdo0);
    if(!ELECHOUSE_cc1101.getCC1101()) {
        Serial.println("CC1101 not found on SPI");
        CC1101::~CC1101();
        return false;
    }
    ELECHOUSE_cc1101.setRxBW(256);      // narrow band for better accuracy
    ELECHOUSE_cc1101.setClb(1,13,15);   // Calibration Offset
    ELECHOUSE_cc1101.setClb(2,16,19);   // Calibration Offset
    ELECHOUSE_cc1101.setModulation(2);  // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
    ELECHOUSE_cc1101.setDRate(50);      // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
    ELECHOUSE_cc1101.setPktFormat(3);   // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX.
                                        // 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
                                        // 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX.
                                        // 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
    ELECHOUSE_cc1101.setMHZ(frequency);


    
    return true;
}

CC1101::CC1101(gpio_num_t miso, gpio_num_t mosi, gpio_num_t sck, gpio_num_t cs, gpio_num_t gdo0, SPIClass* spi = &_spi) {
    setup(miso, mosi, sck,cs,gdo0, spi);
}

CC1101::~CC1101() {
    // remove configurations of GPIO pins
}