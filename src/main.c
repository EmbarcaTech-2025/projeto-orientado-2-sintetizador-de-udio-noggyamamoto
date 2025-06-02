#include <stdio.h>
#include "pico/stdlib.h"
#include "sintetizador.h"

// Função principal do programa
int main() {
    stdio_init_all();           // Inicializa a comunicação serial via USB
    sintetizador_init();        // Inicializa o sintetizador (GPIOs, ADC, PWM, etc.)

    while (true) {
        sintetizador_update();  // Atualiza o estado do sintetizador (leitura de botões, geração de áudio, etc.)
        sleep_ms(1);           // Pequeno delay para evitar uso excessivo da CPU
    }
    return 0;
}