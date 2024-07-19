#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Substitua pelo endereço MAC do dispositivo escravo
uint8_t slaveAddress[] = {0xD8, 0xBC, 0x38, 0xE5, 0xA2, 0xF4};

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Status do envio: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sucesso" : "Falha");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao inicializar ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Inicializa a estrutura esp_now_peer_info_t
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, slaveAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Falha ao adicionar peer");
    return;
  }
}

void loop() {
  if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      input.trim(); // Remove espaços em branco e novas linhas

    if (input.length() == 2 && isDigit(input[0]) && isDigit(input[1])) {
      // Se a entrada for de dois dígitos, envie essa string
      Serial.println("Enviando: " + input);
      esp_now_send(slaveAddress, (uint8_t *)input.c_str(), input.length() + 1);
    } else {
      // Caso contrário, informe ao usuário
      Serial.println("Entrada inválida. Por favor, insira dois dígitos.");
    }
  }

  delay(500); // Ajuste conforme necessário
}



