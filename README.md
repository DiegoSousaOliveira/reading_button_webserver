# Leitura do Estado do Botão em um Servidor
Este projeto demonstra como configurar e programar o microcontrolador RP2040 para ler o estado de um botão e disponibilizar essa informação através de um servidor web. A seguir, apresentamos um guia detalhado para replicar este projeto, incluindo o código-fonte, instruções de configuração e explicações dos resultados obtidos.

## 1. Passo a Passo

### 1.1. Configuração do Ambiente

- Para desenvolver este projeto, é necessário configurar o ambiente de desenvolvimento para o RP2040. Siga os passos abaixo:
- Instale o SDK do Pico: O Raspberry Pi Pico SDK fornece as bibliotecas e ferramentas necessárias para programar o RP2040.
- Configure o CMake: Utilize o CMake para gerenciar o processo de construção do projeto.
- Clone o Repositório: Obtenha o código-fonte do projeto através do repositório GitHub:
        ```
        git clone https://github.com/DiegoSousaOliveira/reading_button_webserver
        ```
### 1.2. Hardware Necessário

- Raspberry Pi Pico: Microcontrolador baseado no RP2040.
- Botão: Para simular a entrada digital.
- Resistor de Pull-down: Garante que a entrada seja lida como LOW quando o botão não está pressionado.
- LED (Opcional): Pode ser utilizado para indicar o estado do botão.

### 1.3. Esquema de Ligação

Conecte os componentes conforme o esquema abaixo:
- Botão: Conecte um terminal ao pino GP15 do Pico e o outro terminal ao GND. Adicione um resistor de pull-down entre o pino GP15 e o GND.
- LED (Opcional): Conecte o anodo ao pino GP14 através de um resistor e o catodo ao GND.

### 1.4. Código-Fonte

- O código principal do projeto está no arquivo `ligar_desligar_led_webserver.c`. Este programa configura o RP2040 para:
- Inicializar uma interface de rede utilizando a biblioteca lwIP.
- Configurar um servidor HTTP que responde com o estado atual do botão.
- Ler continuamente o estado do botão conectado ao pino GP15.
- Atualizar o estado do LED no pino GP14 com base no estado do botão.

### 1.5. Compilação e Gravação

Para compilar e gravar o código no RP2040:

1. Navegue até o diretório do projeto:
       ```
       cd reading_button_webserver
       ```
  2. Crie um diretório de build e acesse-o:​
      ```
      mkdir build
      cd build
      ```
  3. Configure o projeto com o CMake:​
      ```
      cmake ..
      ```
  4. Compile o projeto:​
      ```
      make
      ```
## ✅ Resultados Obtidos

Ao finalizar a implementação do projeto, obtivemos os seguintes resultados:

- O botão físico conectado ao pino GP15 pôde ser lido corretamente pelo RP2040.
- O LED (conectado ao pino GP14) acende quando o botão é pressionado, funcionando como um feedback visual direto do estado do botão.
- Um servidor web é iniciado diretamente no microcontrolador, permitindo acessar via navegador o estado do botão em tempo real.
- A leitura é atualizada continuamente a cada 100ms, garantindo baixa latência e boa responsividade na visualização.
- O código funciona de forma eficiente mesmo com recursos limitados do RP2040, graças ao uso de C bare metal e a leveza da lwIP.

## 🎯 Impacto de Cada Configuração

### 1. `gpio_init() + gpio_set_dir()`

- Função: Inicializa o pino do botão e LED.
- Impacto: Define se o pino será usado como entrada (`GPIO_IN`) ou saída (`GPIO_OUT`). Sem isso, o hardware não se comportaria corretamente.

### 2. `gpio_pull_down(BUTTON_PIN)`

- Função: Ativa um resistor interno de pull-down.
- Impacto: Garante que o pino fique em nível lógico baixo quando o botão não estiver pressionado. Evita leituras incorretas causadas por flutuações (ruído) no sinal.

### 3. `gpio_get(BUTTON_PIN)`

- Função: Lê o valor atual do botão.
- Impacto: Permite saber se o botão está pressionado (1) ou solto (0), base para atualizar tanto o LED quanto a resposta no servidor web.

### 4. `gpio_put(LED_PIN, button_state)`

- Função: Atualiza o estado do LED conforme o botão.
- Impacto: Dá ao usuário um retorno visual imediato da leitura do botão, útil para testes e feedback em tempo real.

### 5. `sleep_ms(100)`

- Função: Aguarda 100 milissegundos entre as leituras.
- Impacto: Reduz o consumo de CPU e evita leituras excessivas desnecessárias (debounce parcial). Ainda assim mantém boa responsividade.

### 6. `lwIP + httpd_init()`

- Função: Inicializa a pilha TCP/IP e o servidor HTTP.
- Impacto: Permite que o microcontrolador atue como servidor web, fornecendo uma interface acessível por navegador. Sem isso, não haveria visualização remota do estado do botão.
    
