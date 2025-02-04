#include <config.h>
#include <Arduino.h>
#include <SPI.h>
#include "NRF/nrf24.h"

SPIClass spi;
NRF24* nrf24;
HardwareSerial mySerial(2);
bool NRF24_on=false;



void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
  spi.begin(SPI_SCK_PIN,SPI_MISO_PIN,SPI_MOSI_PIN);
}

void loop() {

  while (mySerial.available() > 0){
    // get the byte data from the master
    String data = mySerial.readString();
    Serial.println(data);

    if(data.startsWith("NRF24 START") && NRF24_on==false) {
      nrf24 = new NRF24();
      nrf24->begin(&spi,(gpio_num_t)NRF24_1_CS,(gpio_num_t)NRF24_1_CE);
      nrf24->start_jamming();
    }
    if(data.startsWith("NRF24 STOP") && NRF24_on==true) {
      nrf24->stop_jamming();
      nrf24->end();
      nrf24 = nullptr;
    }
  }

  delay(1);
  
}
