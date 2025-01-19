# PROJETO DE IOT DECIBELÍMETRO

Bem-vindo(a) ao repositório do **Projeto IoT Decibelímetro**!  
Este projeto foi desenvolvido em **C**, utiliza **SPIFFS** para armazenamento de arquivos na memória flash e permite o envio dos dados via MQTT tanto para o **Maqiatto** quanto para o **Adafruit IO** (ou outro broker compatível).  

> **Observação**: O sensor de som **não** é da Adafruit; trata-se de um sensor genérico ou de outra fabricante. As referências à Adafruit neste projeto relacionam-se ao possível uso do **Adafruit IO** como broker MQTT ou a bibliotecas auxiliares, **não** ao sensor de som.

---

## Visão Geral

O **Decibelímetro IoT** possibilita a medição de níveis de som (decibéis) e o envio dos dados para a nuvem via MQTT. Com isso, você pode monitorar remotamente, em tempo real, o ruído de um ambiente.

- **Linguagem**: C (para ESP8266/ESP32 ou similar)  
- **Armazenamento de Arquivos**: **SPIFFS** (para guardar configurações e logs)  
- **Broker MQTT**: Pode ser tanto **Maqiatto** quanto **Adafruit IO** (ou outro de sua preferência)  
- **Sensor de Som**: **Não** é da Adafruit, sendo um sensor genérico ou de outra fabricante  
- **Bibliotecas**: Algumas bibliotecas da Adafruit podem ser utilizadas para recursos adicionais, porém não especificamente para o sensor

---

## Funcionalidades Principais

1. **Leitura de Som (dB)**  
   - Capta o nível de ruído ambiente utilizando um sensor genérico (ex.: módulo de microfone com saída analógica).
2. **Publicação MQTT**  
   - Envia periodicamente as leituras de dB para um broker MQTT (Maqiatto ou Adafruit IO).
3. **Armazenamento em SPIFFS**  
   - Guarda arquivos de configuração (ex.: Wi-Fi, broker MQTT) ou pequenos logs na memória flash da placa.
4. **Conexão Wi-Fi Automática**  
   - Conecta à rede Wi-Fi conforme SSID e senha definidos no código ou em arquivo de configuração.
5. **Monitoramento Remoto**  
   - As medições ficam disponíveis em tempo real para qualquer cliente que assine o tópico MQTT configurado.

---

## Requisitos e Dependências

- **Placa de Desenvolvimento**  
  - ESP8266, ESP32 ou similar com suporte a SPIFFS
- **IDE de Programação**  
  - Arduino IDE ou PlatformIO (para compilação e upload)
- **Bibliotecas**  
  - [ESP8266/ESP32 Core](https://github.com/esp8266/Arduino) (se estiver usando a Arduino IDE)  
  - [SPIFFS](https://github.com/espressif/arduino-esp32fs-plugin) (para leitura/escrita de arquivos)
  - <ctime> ( para a inclusão do horário nos logs salvos no SPIFFS
  - **(Opcional)** Biblioteca de MQTT (como PubSubClient ou a da própria Adafruit)  
- **Broker MQTT**  
  - [Maqiatto](https://www.maqiatto.com/) ou [Adafruit IO](https://io.adafruit.com/) — ou outro compatível
- **Sensor de Som**  
  - Sensor genérico (p. ex.: microfone com saída analógica)
  - Foi utilizado o KY-037

---

