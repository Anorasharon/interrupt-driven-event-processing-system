#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define PIR_GPIO     GPIO_NUM_22
#define BUZZER_GPIO  GPIO_NUM_23

static const char *TAG = "PIR_BUZZER";

typedef enum {
    STATE_IDLE,
    STATE_ALERT
} system_state_t;

typedef enum {
    EVENT_MOTION
} event_t;

QueueHandle_t eventQueue;
system_state_t current_state = STATE_IDLE;

void IRAM_ATTR pir_isr_handler(void *arg)
{
    event_t event = EVENT_MOTION;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xQueueSendFromISR(eventQueue, &event, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

void event_task(void *arg)
{
    event_t event;
    static TickType_t last_trigger_time = 0;
    const TickType_t cooldown = pdMS_TO_TICKS(5000);

    while (1)
    {
        if (xQueueReceive(eventQueue, &event, portMAX_DELAY))
        {
            TickType_t now = xTaskGetTickCount();

            if (now - last_trigger_time < cooldown) {
                continue;
            }

            last_trigger_time = now;

            ESP_LOGI(TAG, "Motion detected");

            if (current_state == STATE_IDLE)
            {
                current_state = STATE_ALERT;

                ESP_LOGI(TAG, "Buzzer ON");
                gpio_set_level(BUZZER_GPIO, 1);

                vTaskDelay(pdMS_TO_TICKS(2000));

                gpio_set_level(BUZZER_GPIO, 0);
                ESP_LOGI(TAG, "Buzzer OFF");

                current_state = STATE_IDLE;
            }
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "System booting...");
    ESP_LOGI(TAG, "Waiting for PIR warm-up (8 seconds)");

    gpio_reset_pin(PIR_GPIO);
    gpio_set_direction(PIR_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIR_GPIO, GPIO_FLOATING);

    gpio_reset_pin(BUZZER_GPIO);
    gpio_set_direction(BUZZER_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER_GPIO, 0);

    eventQueue = xQueueCreate(5, sizeof(event_t));

    xTaskCreate(event_task, "event_task", 2048, NULL, 5, NULL);

    vTaskDelay(pdMS_TO_TICKS(8000));

    ESP_LOGI(TAG, "PIR ready. Enabling interrupt.");

    gpio_set_intr_type(PIR_GPIO, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIR_GPIO, pir_isr_handler, NULL);
}
