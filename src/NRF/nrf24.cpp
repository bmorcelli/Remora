#include "nrf24.h"


bool NRF24::begin(gpio_num_t sck, gpio_num_t mosi, gpio_num_t miso, gpio_num_t cs, gpio_num_t ce) {
    NRF_spi.begin(sck,miso,mosi,cs);
    
    return begin(&NRF_spi, cs, ce);
}
bool NRF24::begin(SPIClass* spi, gpio_num_t cs, gpio_num_t ce) {
    pinMode(cs,OUTPUT);
    pinMode(ce,OUTPUT);
    digitalWrite(cs,HIGH);
    digitalWrite(ce,LOW);
    _NRF_spi = spi;
    if(radio.begin(_NRF_spi,ce,cs)) return true;
    else return false;
}
bool NRF24::end() {
    radio.stopConstCarrier();
    digitalWrite(_cs,HIGH);
    digitalWrite(_ce,LOW);
}

void NRF24::Jammertask(void *parameter) {
    NRF24* self = static_cast<NRF24*>(parameter);
    byte hopping_channel[] = {32,34, 46,48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80, 82, 84,86 };  // channel to hop
    byte ptr_hop = 0;  // Pointer to the hopping array
    while(1) {
        ptr_hop++;                                            /// perform next channel change
        if (ptr_hop >= sizeof(hopping_channel)) ptr_hop = 0;  // To avoid array indexing overflow
        self->radio.setChannel(hopping_channel[ptr_hop]);           // Change channel 
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

bool NRF24::start_jamming() {
    radio.setPALevel(RF24_PA_MAX);
    radio.startConstCarrier(RF24_PA_MAX, 45);
    radio.setAddressWidth(3);
    radio.setPayloadSize(2);
    if(!radio.setDataRate(RF24_2MBPS)) { Serial.println("Fail setting data Rate"); return false; } 
    xTaskCreate(
        Jammertask,   // Task function
        "JammerTask",     // Task Name
        4096,               // Stack size
        this,               // Task parameters
        2,                  // Task priority (0 to 3), loopTask has priority 2.
        &handler          // Task handle (not used)
    );
    return true;
}


bool NRF24::stop_jamming() {
    if(handler != nullptr)vTaskDelete(handler);
    radio.stopConstCarrier();
    handler = nullptr;
    return true;
}