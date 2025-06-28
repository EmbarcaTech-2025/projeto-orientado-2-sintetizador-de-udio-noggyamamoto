#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "sintetizador.h"

// === Definições de pinos (ajuste conforme seu hardware) ===
#define PIN_MIC         26  // ADC0 - Microfone
#define PIN_BTN_REC     14  // Botão para gravar
#define PIN_BTN_PLAY    15  // Botão para reproduzir
#define PIN_LED_R       16  // LED RGB - Vermelho
#define PIN_LED_G       17  // LED RGB - Verde
#define PIN_AUDIO       18  // Saída PWM para áudio

// === Parâmetros de áudio ===
#define SAMPLE_RATE     8000    // 8 kHz
#define DURATION_SEC    2       // 2 segundos de áudio
#define BUFFER_SIZE     (SAMPLE_RATE * DURATION_SEC) // 16.000 amostras

// === Buffer de áudio ===
static uint8_t audio_buffer[BUFFER_SIZE];
static volatile uint32_t buffer_index = 0;

// === Estados do sistema ===
typedef enum {
    STATE_IDLE = 0,
    STATE_RECORDING,
    STATE_PLAYING
} synth_state_t;

static synth_state_t state = STATE_IDLE;

// === Funções auxiliares ===
static void set_led_rgb(bool r, bool g) {
    gpio_put(PIN_LED_R, r);
    gpio_put(PIN_LED_G, g);
}

// === Inicialização dos periféricos ===
void sintetizador_init(void) {
    // Botões
    gpio_init(PIN_BTN_REC);
    gpio_set_dir(PIN_BTN_REC, GPIO_IN);
    gpio_pull_up(PIN_BTN_REC);

    gpio_init(PIN_BTN_PLAY);
    gpio_set_dir(PIN_BTN_PLAY, GPIO_IN);
    gpio_pull_up(PIN_BTN_PLAY);

    // LEDs
    gpio_init(PIN_LED_R);
    gpio_set_dir(PIN_LED_R, GPIO_OUT);
    gpio_init(PIN_LED_G);
    gpio_set_dir(PIN_LED_G, GPIO_OUT);

    // ADC (microfone)
    adc_init();
    adc_gpio_init(PIN_MIC);

    // PWM para áudio
    gpio_set_function(PIN_AUDIO, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PIN_AUDIO);
    pwm_set_wrap(slice_num, 255); // 8 bits
    pwm_set_enabled(slice_num, true);

    set_led_rgb(0, 0);

    printf("[INFO] Sintetizador inicializado!\n");
}

// === Função principal de atualização ===
void sintetizador_update(void) {
    static bool last_btn_rec = true;
    static bool last_btn_play = true;
    static absolute_time_t last_sample_time;
    static uint32_t play_index = 0;

    // Leitura dos botões (com debounce)
    bool btn_rec = gpio_get(PIN_BTN_REC);
    bool btn_play = gpio_get(PIN_BTN_PLAY);

    // Inicia gravação ao pressionar botão REC
    if (!btn_rec && last_btn_rec && state == STATE_IDLE) {
        state = STATE_RECORDING;
        buffer_index = 0;
        set_led_rgb(1, 0); // LED vermelho
        printf("[INFO] Gravação iniciada!\n");
        sleep_ms(200);
    }
    last_btn_rec = btn_rec;

    // Inicia reprodução ao pressionar botão PLAY
    if (!btn_play && last_btn_play && state == STATE_IDLE) {
        state = STATE_PLAYING;
        play_index = 0;
        set_led_rgb(0, 1); // LED verde
        printf("[INFO] Reprodução iniciada!\n");
        sleep_ms(200);
    }
    last_btn_play = btn_play;

    // === Estado de gravação ===
    if (state == STATE_RECORDING) {
        // Amostra na taxa definida
        if (absolute_time_diff_us(last_sample_time, get_absolute_time()) >= (1000000 / SAMPLE_RATE)) {
            last_sample_time = get_absolute_time();
            adc_select_input(0); // ADC0 = PIN_MIC
            uint16_t raw = adc_read();
            uint8_t sample = (raw >> 4) & 0xFF; // Converte 12 bits para 8 bits
            audio_buffer[buffer_index++] = sample;

            // (Opcional) Visualização no OLED
            // oled_draw_waveform(audio_buffer, buffer_index);

            // Mensagem de progresso a cada 1000 amostras
            if (buffer_index % 1000 == 0) {
                printf("[REC] %lu/%d amostras\n", buffer_index, BUFFER_SIZE);
            }

            // Fim da gravação
            if (buffer_index >= BUFFER_SIZE) {
                state = STATE_IDLE;
                set_led_rgb(0, 0);
                printf("[INFO] Gravação finalizada!\n");
                // (Opcional) Visualização completa no OLED
                // oled_draw_waveform(audio_buffer, BUFFER_SIZE);
            }
        }
    }

    // === Estado de reprodução ===
    if (state == STATE_PLAYING) {
        if (absolute_time_diff_us(last_sample_time, get_absolute_time()) >= (1000000 / SAMPLE_RATE)) {
            last_sample_time = get_absolute_time();
            uint8_t sample = audio_buffer[play_index++];
            pwm_set_gpio_level(PIN_AUDIO, sample);

            // (Opcional) Visualização no OLED
            // oled_draw_waveform(audio_buffer, play_index);

            // Mensagem de progresso a cada 1000 amostras
            if (play_index % 1000 == 0) {
                printf("[PLAY] %lu/%d amostras\n", play_index, BUFFER_SIZE);
            }

            // Fim da reprodução
            if (play_index >= BUFFER_SIZE) {
                state = STATE_IDLE;
                set_led_rgb(0, 0);
                printf("[INFO] Reprodução finalizada!\n");
            }
        }
    }
}