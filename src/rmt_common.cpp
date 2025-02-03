#include "rmt_common.h"
#include "esp_err.h"
#include "esp_log.h"

#define TAG "RMT_COMMON"

#define RMT_CLK_DIV   80 /*!< RMT counter clock divider */
#define RMT_1US_TICKS (80000000 / RMT_CLK_DIV / 1000000)
#define RMT_1MS_TICKS (RMT_1US_TICKS * 1000)

RMT_COMMON::RMT_COMMON() {
    #ifdef CONFIG_IDF_TARGET_ESP32C3
        #define _PIN 7
    #elif CONFIG_IDF_TARGET_ESP32S3
        #define _PIN 5
    #endif
    initRxRMT((gpio_num_t)_PIN);
}

RMT_COMMON::RMT_COMMON(gpio_num_t pin, rmt_mode mode) {
    if(mode == RX_MODE) {
        initRxRMT(pin);
    }
    else if(mode == TX_MODE) {
        initTxRMT(pin);
    }
    else ESP_LOGW(TAG,"RMT not set/initialized by constructor");
}

RMT_COMMON::~RMT_COMMON() {
    deinitRMT();
}

esp_err_t RMT_COMMON::deinitRMT() {
    esp_err_t err = ESP_OK;
    if (rx_chan) {
        err = rmt_del_channel(rx_chan);
        rx_chan = NULL;
    }
    if (tx_chan) {
        err = rmt_del_channel(tx_chan);
        tx_chan = NULL;
    }
    return err;
}

esp_err_t RMT_COMMON::initRxRMT(gpio_num_t pin) {
    esp_err_t err = ESP_OK;
    deinitRMT();

    rmt_rx_channel_config_t rx_config = {
        .gpio_num = pin,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000000, // 1 MHz (1us/tick)
        .mem_block_symbols = 512, // RMT memory
        .flags = {
            .invert_in = false,
            .with_dma = false
        }
    };

    err = rmt_new_rx_channel(&rx_config, &rx_chan);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error creating RX channel: %d", err);
        return err;
    }

    err = rmt_enable(rx_chan);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error enabling RMT RX: %d", err);
        return err;
    }

    ESP_LOGI(TAG, "RMT RX configured on GPIO %d", pin);
    return ESP_OK;
}

esp_err_t RMT_COMMON::initTxRMT(gpio_num_t pin) {
    esp_err_t err = ESP_OK;
    deinitRMT();

    rmt_tx_channel_config_t tx_config = {
        .gpio_num = pin,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000000, // 1 MHz (1us/tick)
        .mem_block_symbols = 512,
        .trans_queue_depth = 1, // Depth of internal transfer queue, increase this value can support more transfers pending in the background
        .flags = {
            .invert_out = false,
            .with_dma = false
        }
    };

    err = rmt_new_tx_channel(&tx_config, &tx_chan);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error creating TX channel: %d", err);
        return err;
    }

    err = rmt_enable(tx_chan);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error enabling RMT TX: %d", err);
        return err;
    }

    ESP_LOGI(TAG, "RMT TX configured on GPIO %d", pin);
    return ESP_OK;
}

/**
 * @brief Inicia a recepção de dados via RMT
 */
esp_err_t RMT_COMMON::rmt_rf_receiver() {
    if (rx_chan == NULL) {
        ESP_LOGE(TAG, "Canal RX não está inicializado!");
        return ESP_ERR_INVALID_STATE;
    }

    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,  // 1.25µs
        .signal_range_max_ns = 2000000 // 1ms
    };

    size_t received_size = 0;

    esp_err_t err = rmt_receive(rx_chan, rx_buffer, sizeof(rx_buffer), &receive_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao iniciar recepção: %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Recepção iniciada...");
    return ESP_OK;
}

/**
 * @brief Envia um array de pulsos via RMT
 * @param data Vetor de tempos de pulso (em microssegundos)
 * @param len Tamanho do vetor
 */
esp_err_t RMT_COMMON::rmt_rf_transmitter(uint16_t *data, size_t len) {
    if (tx_chan == NULL) {
        ESP_LOGE(TAG, "Canal TX não está inicializado!");
        return ESP_ERR_INVALID_STATE;
    }

    rmt_transmit_config_t tx_config = {
        .loop_count = 0 // Enviar apenas uma vez
    };

    rmt_symbol_word_t tx_symbols[len];

    size_t count=0;
    for (size_t i = 0; i < len; i+=2) {
        tx_symbols[count].duration0 = data[i];
        tx_symbols[count].level0 = 1;
        tx_symbols[count].duration1 = data[i+1];
        tx_symbols[count].level1 = 0;
        count++;
    }

    esp_err_t err = rmt_transmit(tx_chan, encoder, tx_symbols, sizeof(tx_symbols), &tx_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao transmitir: %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Transmissão iniciada...");
    return ESP_OK;
}

esp_err_t RMT_COMMON::rmt_transmitter(rmt_symbol_word_t *symbols) {
    rmt_transmit_config_t tx_config = {
        .loop_count = 0 // Enviar apenas uma vez
    };
    esp_err_t err = rmt_transmit(tx_chan, encoder, symbols, sizeof(symbols), &tx_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao transmitir: %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Transmissão iniciada...");
    return ESP_OK;
}

esp_err_t RMT_COMMON::convert_raw_data_to_rmt(uint16_t *raw_data, size_t len, rmt_symbol_word_t *symbols, size_t *symbol_count) {
    if (len % 2 != 0) {
        ESP_LOGE(TAG, "Dados brutos devem ter pares de tempos ALTO-BAIXO!");
        return ESP_ERR_INVALID_ARG;
    }

    size_t count = 0;
    for (size_t i = 0; i < len; i += 2) {
        if (count >= *symbol_count) {
            ESP_LOGE(TAG, "Buffer de símbolos RMT muito pequeno!");
            return ESP_ERR_INVALID_SIZE;
        }

        symbols[count].duration0 = raw_data[i];       // Tempo ALTO
        symbols[count].level0 = 1;                    // Pulso alto
        symbols[count].duration1 = raw_data[i + 1];   // Tempo BAIXO
        symbols[count].level1 = 0;                    // Pulso baixo

        count++;
    }

    *symbol_count = count;
    return ESP_OK;
}

esp_err_t RMT_COMMON::convert_rmt_to_rawData(rmt_symbol_word_t *symbols, size_t symbol_count, uint16_t *raw_data, size_t *len) {
    if(*len<2*symbol_count) {
        ESP_LOGE(TAG,"rawData lenght must be 2xsymbol_count");
        return ESP_ERR_INVALID_ARG;
    }
    size_t j=0;
    for(size_t i=0; i<symbol_count; i++) {
        if(j==*len) break;
        raw_data[j++] = symbols->duration0;
        if(j==*len) break;
        raw_data[j++] = symbols->duration1;
    }
    return ESP_OK;
}