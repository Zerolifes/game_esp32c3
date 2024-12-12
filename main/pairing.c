/*
 * pairing.c
 *
 *  Created on: Dec 10, 2024
 *      Author: ro
 */

#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

#define TAG "CLIENT"
#define SERVER_IP "192.168.52.253" 
#define SERVER_PORT 5000
#define SERVER_UDP_PORT 3333      
   
const char *ssid = "Bus";
const char *password = "12345678";
char map_maze[134];
char sync_pos_send[4];
char sync_pos_recv[2];

SemaphoreHandle_t wifiSemaphore;
SemaphoreHandle_t tcp_maze;

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        char ip_str[16]; 
        esp_ip4addr_ntoa(&event->ip_info.ip, ip_str, sizeof(ip_str));
        ESP_LOGI(TAG, "Got IP: %s", ip_str);
        xSemaphoreGive(wifiSemaphore);
    }
}

void wifi_init_sta(void *pvParameters) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished. Connecting to SSID: %s", ssid);

	vTaskDelete(NULL);
}

void tcp_client_task(void *pvParameters) {
    if (xSemaphoreTake(wifiSemaphore, portMAX_DELAY) == pdTRUE) {
		const char *message = (const char *)pvParameters;
        struct sockaddr_in dest_addr;
        int sock = -1;  
        int len = 0;

        dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(SERVER_PORT);

        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelete(NULL);
            return;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            close(sock);
            vTaskDelete(NULL);
            return;
        }
        ESP_LOGI(TAG, "Successfully connected to server");

        err = send(sock, message, strlen(message), 0);
        if (err < 0) {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        } else {
            ESP_LOGI(TAG, "message sent: %s", message);
        }

        len = recv(sock, map_maze, sizeof(map_maze) - 1, 0);
        if (len < 0) {
            ESP_LOGE(TAG, "Receive failed: errno %d", errno);
        } else {
            map_maze[len] = '\0'; // Đảm bảo chuỗi nhận được có kết thúc hợp lệ
            ESP_LOGI(TAG, "Received from server: %s", map_maze);
        }

        close(sock);
        ESP_LOGI(TAG, "Socket closed");
        xSemaphoreGive(tcp_maze);
        vTaskDelete(NULL);
    }
}

static void udp_client_task(void *pvParameters)
{
	if (xSemaphoreTake(tcp_maze, portMAX_DELAY) == pdTRUE) {
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    while (1) 
    {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(SERVER_UDP_PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) 
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, sending to %s:%d", SERVER_IP, SERVER_UDP_PORT);

        struct timeval timeout;
        timeout.tv_sec = 0;  
        timeout.tv_usec = 200000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        while (1) 
        {
            int err = sendto(sock, sync_pos_send, strlen(sync_pos_send), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err < 0) 
            {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }
            ESP_LOGI(TAG, "Message sent");

            struct sockaddr_in source_addr;
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(sock, sync_pos_recv, sizeof(sync_pos_recv), 0, (struct sockaddr *)&source_addr, &socklen);

            if (len < 0) 
            {
                if (errno == EWOULDBLOCK) 
                {
                    ESP_LOGW(TAG, "Receive timeout");
                } 
                else 
                {
                    ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
                    break;
                }
            } 
            else 
            {
                sync_pos_recv[len] = 0;  // 
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", sync_pos_recv);
            }

            vTaskDelay(100 / portTICK_PERIOD_MS);

            if (sock != -1) 
            {
                ESP_LOGE(TAG, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
            }
            
            vTaskDelay(100 / portTICK_PERIOD_MS);
    	}
    }
    vTaskDelete(NULL);
    }
}

void sync()
{
	xTaskCreate(udp_client_task, "udp_client_task", 4096, NULL, 5, NULL);
}

void connect_wifi()
{
 	wifiSemaphore = xSemaphoreCreateBinary();
 	tcp_maze = xSemaphoreCreateBinary();
    xTaskCreate(wifi_init_sta, "wifi_init_sta", 4096, NULL, 5, NULL);
}

void pair_with_another(char *message) {
    xTaskCreate(tcp_client_task, "tcp_client_task", 4096, (void*)message, 5, NULL);
}


