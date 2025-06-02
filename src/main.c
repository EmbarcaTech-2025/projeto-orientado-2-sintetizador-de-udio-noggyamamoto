#include <stdio.h>
#include "pico/stdlib.h"
#include "sintetizador.h"

int main() {
    stdio_init_all();
    sintetizador_init();

    while (true) {
        sintetizador_update();
        sleep_ms(1);
    }
    return 0;
}