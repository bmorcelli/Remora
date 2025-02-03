#ifndef __RMT_COMMON_H_
#define __RMT_COMMON_H_

#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"

class RMT_COMMON {
private:
    rmt_channel_handle_t rx_chan = NULL;
    rmt_channel_handle_t tx_chan = NULL;
    rmt_encoder_handle_t encoder = NULL;
    rmt_symbol_word_t rx_buffer[512]; // Buffer de recepção
    
public:
    enum rmt_mode {
        RX_MODE,
        TX_MODE,
        IDLE,
    };
    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    RMT_COMMON();
    RMT_COMMON(gpio_num_t pin, rmt_mode mode);
    ~RMT_COMMON();

    /////////////////////////////////////////////////////////////////////////////////////
    // Life Cycle
    /////////////////////////////////////////////////////////////////////////////////////
    esp_err_t initRxRMT(gpio_num_t pin);
    esp_err_t initTxRMT(gpio_num_t pin);
    esp_err_t deinitRMT();

    esp_err_t rmt_rf_receiver();
    esp_err_t rmt_rf_transmitter(uint16_t *data, size_t len);
    esp_err_t rmt_transmitter(rmt_symbol_word_t *symbols);
    esp_err_t convert_raw_data_to_rmt(uint16_t *raw_data, size_t len, rmt_symbol_word_t *symbols, size_t *symbol_count);
    esp_err_t convert_rmt_to_rawData(rmt_symbol_word_t *symbols, size_t symbol_count, uint16_t *raw_data, size_t *len);
};




#endif