#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "sintetizador.h"

// Definições de pinos (ajuste conforme seu hardware)
#define PIN_POT 26
#define PIN_BTN_WAVE 14
#define PIN_BTN_PLAY 15
#define PIN_LED1 16
#define PIN_LED2 17
#define PIN_AUDIO 18

#define PWM_WRAP 255
#define SAMPLE_RATE 8000

typedef enum {
    WAVE_SINE = 0,
    WAVE_SQUARE,
    WAVE_TRIANGLE,
    WAVE_SAWTOOTH,
    WAVE_MAX
} wave_t;

static wave_t current_wave = WAVE_SINE;
static bool playing = false;

// Funções auxiliares
static void update_leds(void) {
    gpio_put(PIN_LED1, current_wave & 0x01);
    gpio_put(PIN_LED2, (current_wave >> 1) & 0x01);
}

static uint16_t generate_sample(wave_t wave, float phase) {
    float sample = 0.0f;
    switch (wave) {
        case WAVE_SINE:
            sample = (sinf(2 * M_PI * phase) + 1.0f) * 0.5f;
            break;
        case WAVE_SQUARE:
            sample = (phase < 0.5f) ? 1.0f : 0.0f;
            break;
        case WAVE_TRIANGLE:
            sample = (phase < 0.5f) ? (phase * 2.0f) : (2.0f - phase * 2.0f);
            break;
        case WAVE_SAWTOOTH:
            sample = phase;
            break;
        default:
            sample = 0.0f;
    }
    return (uint16_t)(sample * PWM_WRAP);
}

void sintetizador_init(void) {
    // Inicialização dos GPIOs
    gpio_init(PIN_BTN_WAVE);
    gpio_set_dir(PIN_BTN_WAVE, GPIO_IN);
    gpio_pull_up(PIN_BTN_WAVE);

    gpio_init(PIN_BTN_PLAY);
    gpio_set_dir(PIN_BTN_PLAY, GPIO_IN);
    gpio_pull_up(PIN_BTN_PLAY);

    gpio_init(PIN_LED1);
    gpio_set_dir(PIN_LED1, GPIO_OUT);
    gpio_init(PIN_LED2);
    gpio_set_dir(PIN_LED2, GPIO_OUT);

    // Inicialização do ADC
    adc_init();
    adc_gpio_init(PIN_POT);

    // Inicialização do PWM para áudio
    gpio_set_function(PIN_AUDIO, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PIN_AUDIO);
    pwm_set_wrap(slice_num, PWM_WRAP);
    pwm_set_enabled(slice_num, true);

    update_leds();
}

void sintetizador_update(void) {
    static bool last_btn_wave = true;
    static bool last_btn_play = true;

    // Leitura dos botões (com debounce simples)
    bool btn_wave = gpio_get(PIN_BTN_WAVE);
    bool btn_play = gpio_get(PIN_BTN_PLAY);

    if (!btn_wave && last_btn_wave) {
        current_wave = (current_wave + 1) % WAVE_MAX;
        update_leds();
        sleep_ms(200); // debounce
    }
    last_btn_wave = btn_wave;

    if (!btn_play && last_btn_play) {
        playing = !playing;
        sleep_ms(200); // debounce
    }
    last_btn_play = btn_play;

    // Leitura do potenciômetro (frequência)
    adc_select_input(0); // Canal 0 = GPIO 26
    uint16_t pot_value = adc_read();
    float freq = 220.0f + (pot_value / 4095.0f) * 880.0f; // 220Hz a 1100Hz

    // Geração de áudio
    if (playing) {
        static absolute_time_t last_sample_time;
        if (absolute_time_diff_us(last_sample_time, get_absolute_time()) >= (1000000 / SAMPLE_RATE)) {
            last_sample_time = get_absolute_time();
            static float phase = 0.0f;
            float phase_step = freq / SAMPLE_RATE;
            uint16_t sample = generate_sample(current_wave, phase);
            pwm_set_gpio_level(PIN_AUDIO, sample);
            phase += phase_step;
            if (phase >= 1.0f) phase -= 1.0f;
        }
    } else {
        pwm_set_gpio_level(PIN_AUDIO, PWM_WRAP / 2); // Silêncio
    }
}