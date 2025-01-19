#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <SPIFFS.h>
#include <ctime>

#define KY037_PIN 35 // Pino analógico
#define LIMITE_DECIBEIS 70.0 // Limite de decibéis
#define LED_PIN 2 // Pino do LED
#define BUZZER_PIN 4 // Pino do Buzzer

// Configurações da rede Wi-Fi
const char* ssid = "";        
const char* password = "";    

// Configurações do Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "seu_usuario"       // Substitua pelo seu nome de usuário do Adafruit IO
#define AIO_KEY         "sua_chave_aio"    // Substitua pela sua chave do Adafruit IO

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Feeds do Adafruit IO
Adafruit_MQTT_Publish decibeisFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/decibeis");
Adafruit_MQTT_Publish alertaFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/alerta");

void setup_wifi() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  if (mqtt.connected()) {
    Serial.println("MQTT já está conectado. Nenhuma ação necessária.");
    return;
  }

  while (mqtt.connected() == false) {
    Serial.print("Conectando ao Adafruit IO...");
    if (mqtt.connect()) {
      Serial.println("Conectado ao Adafruit IO!");
    } else {
      Serial.println("Falha ao conectar ao Adafruit IO. Verifique as configurações.");
      delay(2000);
    }
  }
}

String getFormattedTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Hora não disponível";
  }
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

void salvaNoSPIFFS(String mensagem) {
  File file = SPIFFS.open("/log.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Erro ao abrir o arquivo para escrita.");
    return;
  }
  file.println(mensagem);
  file.close();
  Serial.println("Log salvo no SPIFFS.");
}

float converteParaDecibeis(int leituraADC) {
  float tensao = (leituraADC / 4095.0) * 3.3; 
  return tensao * 50.0; 
}

void setup() {
  Serial.begin(115200);

  // Inicializa SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Erro ao montar o sistema de arquivos SPIFFS.");
    return;
  }

  // Configura ADC
  analogSetAttenuation(ADC_11db);
  analogReadResolution(12);

  // Configura pinos do LED e do Buzzer
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Conecta ao Wi-Fi
  setup_wifi();

  // Configura fuso horário
  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void loop() {
  if (!mqtt.connected()) {
    reconnectMQTT();
  }
  mqtt.processPackets(1000);
  mqtt.ping();

  int leitura = analogRead(KY037_PIN); 
  float tensao = (leitura / 4095.0) * 3.3; 
  float decibeis = converteParaDecibeis(leitura); 

  // Publica no feed de decibéis em tempo real
  if (!decibeisFeed.publish(decibeis)) {
    Serial.println("Falha ao publicar no feed de decibéis.");
  } else {
    Serial.println("Decibéis publicados com sucesso.");
  }

  if (decibeis > LIMITE_DECIBEIS) {
    // Publica no feed de alerta indicando que o limite foi excedido
    if (!alertaFeed.publish("Limite de decibéis excedido!")) {
      Serial.println("Falha ao publicar no feed de alerta.");
    } else {
      Serial.println("Alerta publicado com sucesso.");
    }

    // Salva no SPIFFS
    String mensagem = "[" + getFormattedTime() + "] Decibéis excedidos: " + String(decibeis);
    salvaNoSPIFFS(mensagem);

    // Ativa LED e Buzzer
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500); // Mantém ativos por 500ms
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

  } else {
    // Publica no feed de alerta indicando que o limite não foi excedido
    if (!alertaFeed.publish("Limite de decibéis não excedido.")) {
      Serial.println("Falha ao publicar no feed de alerta.");
    } else {
      Serial.println("Status de alerta publicado: Limite não excedido.");
    }
  }

  // Exibe os valores no monitor serial
  Serial.print("Leitura do ADC: ");
  Serial.print(leitura);
  Serial.print(" | Tensão medida: ");
  Serial.print(tensao, 3); 
  Serial.print(" V | Decibéis: ");
  Serial.println(decibeis);

  delay(500);
}
