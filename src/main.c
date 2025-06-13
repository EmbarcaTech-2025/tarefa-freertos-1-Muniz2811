#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "FreeRTOS.h"
#include "task.h"

#define LED_RED_PIN     13  
#define LED_GREEN_PIN   11  
#define LED_BLUE_PIN    12 
#define BUZZER_PIN      21  
#define BUTTON_A_PIN    5  
#define BUTTON_B_PIN    6   


#define BUZZER_FREQUENCY 2000


TaskHandle_t ledTaskHandle = NULL;
TaskHandle_t buzzerTaskHandle = NULL;
TaskHandle_t buttonTaskHandle = NULL;


void vLedTask(void *pvParameters);
void vBuzzerTask(void *pvParameters);
void vButtonTask(void *pvParameters);
void pwm_init_buzzer(uint pin);
void beep(uint pin, uint duration_ms);


void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_config config = pwm_get_default_config();
    
    float divider = clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096.0f);
    pwm_config_set_clkdiv(&config, divider);
    
    pwm_init(slice_num, &config, true);


    pwm_set_gpio_level(pin, 0);
}

void beep(uint pin, uint duration_ms) {

    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_set_gpio_level(pin, 2048);

    vTaskDelay(pdMS_TO_TICKS(duration_ms));

    pwm_set_gpio_level(pin, 0);
}


void vLedTask(void *pvParameters) {
    gpio_init(LED_RED_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_init(LED_BLUE_PIN);
    
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);

    gpio_put(LED_RED_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_BLUE_PIN, 0);
    
    // Cores (0: Red, 1: Green, 2: Blue)
    int color_state = 0;
    
    for (;;) {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 0);
        
        switch (color_state) {
            case 0: // Red
                gpio_put(LED_RED_PIN, 1);
                break;
            case 1: // Green
                gpio_put(LED_GREEN_PIN, 1);
                break;
            case 2: // Blue
                gpio_put(LED_BLUE_PIN, 1);
                break;
        }
        
        // ir pra proxima cor
        color_state = (color_state + 1) % 3;
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

//Task do buzzer
void vBuzzerTask(void *pvParameters) {
    pwm_init_buzzer(BUZZER_PIN);
    
    for (;;) {
        beep(BUZZER_PIN, 100);
        
        vTaskDelay(pdMS_TO_TICKS(900)); 
    }
}

void vButtonTask(void *pvParameters) {
    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    
    gpio_pull_up(BUTTON_A_PIN);
    gpio_pull_up(BUTTON_B_PIN);
    
    bool led_suspended = false;
    bool buzzer_suspended = false;
    
    for (;;) {
        bool button_a_state = gpio_get(BUTTON_A_PIN);
        bool button_b_state = gpio_get(BUTTON_B_PIN);
        
        // Botao A controla o led
        // quando o botao for pressionado (LOW), suspende a tarefa se nao estiver suspensa
        if (!button_a_state && !led_suspended) {
            vTaskSuspend(ledTaskHandle);
            led_suspended = true;
        }
        // quando o botao for liberado (HIGH), retoma a tarefa se estiver suspensa
        else if (button_a_state && led_suspended) {
            vTaskResume(ledTaskHandle);
            led_suspended = false;
        }
        
        // Botao B controla o buzzer
        // quando o botao for pressionado (LOW), suspende a tarefa se nao estiver suspensa
        if (!button_b_state && !buzzer_suspended) {
            vTaskSuspend(buzzerTaskHandle);
            buzzer_suspended = true;
        }
        // quando o botao for liberado (HIGH), retoma a tarefa se estiver suspensa
        else if (button_b_state && buzzer_suspended) {
            vTaskResume(buzzerTaskHandle);
            buzzer_suspended = false;
        }
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

int main() {
    stdio_init_all();
    
    // Criando as tasks
    xTaskCreate(
        vLedTask,           
        "LED Task",         
        256,               
        NULL,              
        2,                 
        &ledTaskHandle      
    );
    
    xTaskCreate(
        vBuzzerTask,        
        "Buzzer Task",      
        256,                
        NULL,               
        2,                  
        &buzzerTaskHandle   
    );
    
    xTaskCreate(
        vButtonTask,        
        "Button Task",      
        256,                
        NULL,               
        3,                  
        &buttonTaskHandle   
    );
    
    vTaskStartScheduler();
    
    for (;;) {
    }
    
    return 0;
}
