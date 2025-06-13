# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: **Pedro Henrique Muniz de Oliveira**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, 06 de 2025

---

# BitDogLab FreeRTOS Project

Este projeto implementa um sistema multitarefa embarcado usando FreeRTOS na plataforma BitDogLab com Raspberry Pi Pico.

## Funcionalidades

O sistema controla três periféricos de forma concorrente:

1. **LED RGB**: Alterna ciclicamente entre as cores vermelho, verde e azul a cada 500ms.
2. **Buzzer**: Emite bipes periódicos de 100ms a cada 1 segundo.
3. **Botões**:
   - **Botão A**: Suspende ou retoma a tarefa do LED.
   - **Botão B**: Suspende ou retoma a tarefa do buzzer.

## Estrutura do Projeto

```
RTOS/
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── FreeRTOS/            ← clone do kernel
├── src/
│   └── main.c
├── include/
│   └── FreeRTOSConfig.h
```

## Requisitos

- BitDogLab com Raspberry Pi Pico W ou Pico 2
- Cabo micro-USB ou USB-C para gravação
- Ambiente configurado:
  - VSCode com extensão C/C++
  - ARM GCC Toolchain (arm-none-eabi-gcc)
  - CMake
  - pico-sdk e FreeRTOS já clonados no projeto

## Compilação e Gravação

1. Clone o repositório do FreeRTOS:
```bash
git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS
```

2. Configure a variável de ambiente para o FreeRTOS:
```bash
export FREERTOS_KERNEL_PATH=/caminho/para/FreeRTOS
```

3. Compile o projeto:
```bash
mkdir build
cd build
cmake ..
make -j4
```

4. Copie o arquivo `.uf2` gerado para a placa BitDogLab em modo bootloader.

## Reflexão

### O que acontece se todas as tarefas tiverem a mesma prioridade?

Se todas as tarefas tiverem a mesma prioridade, o escalonador do FreeRTOS usará um algoritmo de escalonamento Round-Robin com fatias de tempo iguais. Isso significa que cada tarefa receberá aproximadamente o mesmo tempo de CPU, alternando entre elas a cada tick do sistema. 

No nosso projeto, isso poderia causar um comportamento menos responsivo dos botões, pois a tarefa de monitoramento dos botões não teria prioridade sobre as outras tarefas. Isso poderia resultar em atrasos na detecção de pressionamentos de botões, especialmente se as outras tarefas estiverem executando operações que consomem muito tempo de processamento.

### Qual tarefa consome mais tempo da CPU?

A tarefa do LED RGB consome menos tempo de CPU, pois realiza apenas operações simples de GPIO e depois entra em estado de espera. 

A tarefa do buzzer consome um pouco mais de recursos devido ao uso do PWM, mas ainda assim é relativamente eficiente.

A tarefa dos botões, embora tenha uma lógica simples, é a que mais consome CPU proporcionalmente ao seu tempo de execução, pois está constantemente fazendo polling dos pinos GPIO a cada 100ms. Em um sistema mais complexo, seria mais eficiente usar interrupções para detectar mudanças nos botões em vez de polling.

### Quais seriam os riscos de usar polling sem prioridades?

Usar polling sem prioridades adequadas pode levar a:

1. **Tempo de resposta inconsistente**: Sem prioridades adequadas, o tempo de resposta aos eventos (como pressionamentos de botões) pode variar significativamente, dependendo do que outras tarefas estão fazendo.

2. **Starvation de tarefas**: Tarefas que realizam polling frequente podem consumir muito tempo de CPU, potencialmente "roubando" tempo de outras tarefas importantes.

3. **Aumento da latência**: Eventos críticos podem sofrer atrasos inaceitáveis se o sistema estiver ocupado com tarefas de menor importância.

4. **Comportamento imprevisível**: O sistema pode se comportar de maneira inconsistente sob diferentes condições de carga, tornando difícil prever seu comportamento em todas as situações.

5. **Desperdício de energia**: O polling constante consome energia mesmo quando não há eventos para processar, o que é especialmente problemático em sistemas embarcados alimentados por bateria.

Uma abordagem melhor seria usar interrupções para eventos como pressionamentos de botões, permitindo que o processador entre em estados de baixo consumo quando não estiver realizando trabalho útil, e garantindo que eventos importantes sejam tratados prontamente.

---

## 📜 Licença
GNU GPL-3.0.
