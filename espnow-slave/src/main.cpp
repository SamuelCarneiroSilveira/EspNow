#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Definição dos pinos para os segmentos dos displays
int display1[7] = {2, 4, 15, 16, 14, 5, 13}; // Display 1 (A-G)
int display2[7] = {33, 32, 22, 19, 17, 27, 26}; // Display 2 (A-G)

// Mapeamento dos números para os segmentos (0-9)
bool numMap[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

unsigned long previousMillis = 0;        // Armazena a última vez que a atualização do display foi feita
const long interval = 5000;              // Intervalo em que o display mostra o número (5 segundos)

// Função para exibir um número em um display específico
void displayNumber(int display[], int number) {
  for (int segment = 0; segment < 7; segment++) {
    digitalWrite(display[segment], numMap[number][segment] ? LOW : HIGH);
  }
}

// Função para apagar todos os LEDs
void clearDisplays() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(display1[i], HIGH);
    digitalWrite(display2[i], HIGH);
  }
}

// Função para verificar se a string é um número de dois dígitos
bool isTwoDigitNumber(const char* str) {
  return isdigit(str[0]) && isdigit(str[1]) && str[2] == '\0';
}

bool displayActive = false; // Indica se o número está sendo exibido no display

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char buffer[250];
  memset(buffer, 0, sizeof(buffer));

  if (len < sizeof(buffer)) {
    memcpy(buffer, incomingData, len);
    buffer[len] = '\0';

    Serial.print("Dados recebidos: ");
    Serial.println(buffer);

    if (isTwoDigitNumber(buffer)) {
      int digit1 = buffer[0] - '0'; // Converte o primeiro caractere para número
      int digit2 = buffer[1] - '0'; // Converte o segundo caractere para número

      displayNumber(display1, digit1);
      displayNumber(display2, digit2);

      previousMillis = millis(); // Atualiza o tempo de início
      displayActive = true; // Ativa a exibição no display
    }
  } else {
    Serial.println("Erro: Tamanho do dado recebido excede o buffer");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao inicializar ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  // Configura os pinos dos displays como saída e inicializa como desligados
  for (int i = 0; i < 7; i++) {
    pinMode(display1[i], OUTPUT);
    digitalWrite(display1[i], HIGH); // Desliga o segmento do display 1
    pinMode(display2[i], OUTPUT);
    digitalWrite(display2[i], HIGH); // Desliga o segmento do display 2
  }

  Serial.print("Endereço MAC do Escravo: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Verifica se o display está ativo e se já passaram 5 segundos desde a última atualização
  if (displayActive && millis() - previousMillis >= interval) {
    clearDisplays();      // Apaga os displays
    displayActive = false; // Desativa a exibição no display
    ESP.restart(); // Reinicia o ESP para limpar o estado e iniciar de novo
  }
}
