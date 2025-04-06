#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

// Configurações de Wi-Fi
#define WIFI_SSID "EDNA"
#define WIFI_PASSWORD "wwork197"

// Definição dos pinos dos LEDs
#define LED_BLUE_PIN 12
#define LED_RED_PIN 13
#define BUTTON_A 5


static bool status_button = false;
static char msg_status_button[17] = "Não pressionado";
char *point_mgs_status_button = msg_status_button;

// Função de callback para processar requisições HTTP
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    char *request = (char *)malloc(p->len + 1);
    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    printf("Request: %s\n", request);

    // Resposta HTML completa
    char html[1024];
    snprintf(html, sizeof(html),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "<meta charset='UTF-8'>\n"
                "<title>LED Control</title>\n"
                "<style>\n"
                "body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
                "h1 { font-size: 64px; margin-bottom: 30px; }\n"
                "button { font-size: 36px; margin: 10px; padding: 20px 40px; border-radius: 10px; }\n"
                ".temperature { font-size: 48px; margin-top: 30px; color: #333; }\n"
                "</style>\n"
                "</head>\n"
                "<body>\n"
                "<h1>Status do Botão A</h1>\n"
                "<p class=\"temperature\" id=\"status\">Status do botão: %s</p>\n"
                "</body>\n"
                "</html>\n",
                point_mgs_status_button);

    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    

    free(request);
    pbuf_free(p);
    tcp_output(tpcb);
    return ERR_OK;
}

// Função de callback ao aceitar conexões TCP
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}


// Função principal
int main() {
    stdio_init_all();

    // Configuração dos LEDs como saída
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_put(LED_BLUE_PIN, false);

    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_put(LED_RED_PIN, false);

    // Configuração do Botão
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_put(LED_RED_PIN, 1);

    while (cyw43_arch_init()) {
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Conectando ao Wi-Fi...\n");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    printf("Conectado ao Wi-Fi\n");

    if (netif_default) {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    // Configura o servidor TCP
    struct tcp_pcb *server = tcp_new();
    if (!server) {
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    if (tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Falha ao associar servidor TCP à porta 80\n");
        return -1;
    }

    server = tcp_listen(server);
    tcp_accept(server, tcp_server_accept);

    printf("Servidor ouvindo na porta 80\n");
    while (true) {
        cyw43_arch_poll();

        bool button_pressed = gpio_get(BUTTON_A) == 0;

        if (button_pressed && !status_button) {
            gpio_put(LED_RED_PIN, 0);
            gpio_put(LED_BLUE_PIN, 1);
            strcpy(msg_status_button, "Pressionado");
            status_button = true;
        } else if (!button_pressed && status_button) {
            gpio_put(LED_BLUE_PIN, 0);
            gpio_put(LED_RED_PIN, 1);
            strcpy(msg_status_button, "Não pressionado");
            status_button = false;
        }

        printf("Status do botão: %s\n", msg_status_button);
        sleep_ms(1000);
    }

    cyw43_arch_deinit();
    return 0;
}
