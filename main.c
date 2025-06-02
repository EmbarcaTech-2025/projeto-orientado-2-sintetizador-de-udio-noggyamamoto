#include <stdio.h> // Biblioteca padrão
#include <math.h> // Biblioteca de matemática (função "round" foi utilizada)

#include "pico/stdlib.h" // Biblioteca padrão pico
#include "hardware/gpio.h" // Biblioteca de GPIOs
#include "hardware/adc.h" // Biblioteca do ADC
#include "hardware/pwm.h" // Biblioteca do PWM

int main() {
    stdio_init_all();
    sintetizador_init();

    while (true) {
        sintetizador_update();
        sleep_ms(1); // Pequeno delay para evitar uso excessivo da CPU
    }
    return 0;
}