#ifndef __CONFIG_h_
#define __CONFIG_h_

// Pinout configuration
#if defined(ESP32S3_ZERO)

    #define SPI_MOSI_PIN    1
    #define SPI_MISO_PIN    2
    #define SPI_SCK_PIN     3

    #define CC1101_CS       4
    #define CC1101_GDO0     5

    #define NRF24_1_CS      8
    #define NRF24_1_CE      9

    #define NRF24_2_CS      10
    #define NRF24_2_CE      11

    #define IR_TX_PIN       6
    #define IR_RX_PIN       7

    #define PN532_CS        12

    #define UART_TX         43
    #define UART_RX         44

#elif defined(ESP32C3_SUPERMINI)

    #define SPI_MOSI_PIN    10
    #define SPI_MISO_PIN    9
    #define SPI_SCK_PIN     8

    #define CC1101_CS       6
    #define CC1101_GDO0     7

    #define NRF24_1_CS      0
    #define NRF24_1_CE      1

    #define NRF24_2_CS      -1  // not available
    #define NRF24_2_CE      -1  // not available

    #define IR_TX_PIN       2
    #define IR_RX_PIN       3

    #define PN532_CS        5

    #define UART_TX         21
    #define UART_RX         20

#endif

#endif