# Projeto 2 – Sintetizador de Áudio  
**Residência Tecnológica em Sistemas Embarcados – EmbarcaTech 2025**

Autor: **João Nogueira**  
Instituição: EmbarcaTech - HBr  
Brasília, 02 de junho de 2025

---

## Descrição

Este projeto implementa um **sintetizador de áudio digital** para a placa BitDogLab com microcontrolador Raspberry Pi Pico W (RP2040), atendendo 100% dos requisitos do Projeto 2 da disciplina.  
O sistema permite **gravar áudio do microfone**, armazenar em buffer, e **reproduzir o áudio** via PWM em buzzer ou alto-falante, com controle por botões e feedback visual por LED RGB.

---

## Funcionalidades

- **Gravação de áudio** do microfone (ADC) por tempo pré-definido (2s, 8kHz)
- **Armazenamento** das amostras em buffer na RAM
- **Reprodução** do áudio gravado via PWM (buzzer ou alto-falante)
- **Controle por botões**:  
  - Botão REC: inicia gravação  
  - Botão PLAY: inicia reprodução
- **Feedback visual**:  
  - LED vermelho aceso durante gravação  
  - LED verde aceso durante reprodução
- **Mensagens no terminal** (Serial Monitor) indicando estado e progresso
- **Estrutura pronta para visualização da forma de onda no display OLED**
- **Código modular, limpo e comentado**

---

## Esquemático de Ligações

| Função           | Pino Pico W | BitDogLab | Observação                  |
|------------------|-------------|-----------|-----------------------------|
| Microfone        | GP26 (ADC0) | MIC       | Entrada de áudio            |
| Botão REC        | GP14        | BTN1      | Inicia gravação             |
| Botão PLAY       | GP15        | BTN2      | Inicia reprodução           |
| LED RGB (Verm.)  | GP16        | LED_R     | Indica gravação             |
| LED RGB (Verde)  | GP17        | LED_G     | Indica reprodução           |
| Saída de Áudio   | GP18        | AUDIO_OUT | PWM + filtro RC             |

---

## Como Usar

1. **Compile e grave o firmware** na placa Pi Pico W.
2. **Conecte o Serial Monitor** (baudrate 115200) para acompanhar mensagens.
3. **Pressione o botão REC** para iniciar a gravação (LED vermelho acende).
4. **Após o término da gravação**, pressione o botão PLAY para ouvir o áudio (LED verde acende).
5. **Aguarde a reprodução terminar** (LEDs apagam).
6. **Repita o processo** para novas gravações.

---

## Estrutura do Projeto

```
projeto-orientado-2-sintetizador-de-udio-noggyamamoto/
│
├── src/
│   ├── main.c
│   └── sintetizador.c
├── include/
│   └── sintetizador.h
├── README.md
├── CMakeLists.txt

```

---

## Observações

- Para melhor qualidade de áudio, recomenda-se usar um filtro RC na saída PWM e, se possível, um amplificador classe D.
- O projeto pode ser expandido com filtros digitais, compressão ou armazenamento externo (SD).

---

## Licença

GNU GPL-3.0

---