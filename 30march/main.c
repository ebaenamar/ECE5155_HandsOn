#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define UART_NUM UART_NUM_2
#define BUF_SIZE 256
#define FRAME_SIZE 32

// PMS5003 frame structure
typedef struct {
    uint8_t start1;
    uint8_t start2;
    uint16_t frame_len;
    uint16_t pm1_0;
    uint16_t pm2_5;
    uint16_t pm10;
    uint16_t pm1_0_env;
    uint16_t pm2_5_env;
    uint16_t pm10_env;
    uint8_t reserved[14];
    uint16_t checksum;
} __attribute__((packed)) pms_frame_t;

void app_main() {
    printf("========================================\n");
    printf("PMS5003 Custom Chip Reader (ESP-IDF)\n");
    printf("========================================\n\n");
    
    // Configure UART2 (pins 16=RX, 17=TX)
    uart_config_t config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM, &config);
    uart_set_pin(UART_NUM, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    
    printf("UART2 initialized: 9600 baud, 8N1, pins 16(RX)/17(TX)\n\n");
    printf("Hour | PM1.0 | PM2.5 | PM10 | Checksum | Status\n");
    printf("-----|-------|-------|------|----------|-------\n");
    
    uint8_t data[BUF_SIZE];
    int sample_count = 0;
    
    while (true) {
        // Read UART data
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 100 / portTICK_PERIOD_MS);
        
        if (len > 0) {
            // Look for start bytes 0x42 0x4D
            for (int i = 0; i < len - 1; i++) {
                if (data[i] == 0x42 && data[i + 1] == 0x4D) {
                    // Found frame start
                    if (i + FRAME_SIZE <= len) {
                        pms_frame_t *frame = (pms_frame_t *)&data[i];
                        
                        // Verify checksum
                        uint16_t calc_sum = 0;
                        uint8_t *ptr = (uint8_t *)frame;
                        for (int j = 0; j < 30; j++) {
                            calc_sum += ptr[j];
                        }
                        uint16_t recv_sum = (frame->checksum >> 8) | (frame->checksum << 8);
                        
                        // Extract values (big-endian)
                        uint16_t pm1 = (frame->pm1_0 >> 8) | (frame->pm1_0 << 8);
                        uint16_t pm25 = (frame->pm2_5 >> 8) | (frame->pm2_5 << 8);
                        uint16_t pm10 = (frame->pm10 >> 8) | (frame->pm10 << 8);
                        
                        int hour = (sample_count / 60) % 24;
                        sample_count++;
                        
                        printf("%02d:00 | %5d | %5d | %4d | %8s | %s\n",
                               hour, pm1, pm25, pm10,
                               calc_sum == recv_sum ? "OK" : "FAIL",
                               pm25 < 12 ? "GOOD" : pm25 < 35 ? "MODERATE" : "UNHEALTHY");
                    }
                    break;
                }
            }
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}