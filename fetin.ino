#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h> // Inclua a biblioteca padrão de tempo

// Defina suas credenciais Wi-Fi
#define WIFI_SSID "KYLA"
#define WIFI_PASSWORD "UK513B60HP45R"

// Configurações do Firestore
#define FIREBASE_API_KEY "AIzaSyAEeuYXv8-meyp1ACslr_jALYSksZmRcZ0"
#define FIREBASE_PROJECT_ID "carereminder-10bab"

// URL do Firestore (REST API)
#define FIRESTORE_URL "https://firestore.googleapis.com/v1/projects/" FIREBASE_PROJECT_ID "/databases/(default)/documents/TabelaRemedios"

// Configuração do NTP
const long utcOffsetInSeconds = -3 * 3600; // Offset para o horário de Brasília
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", utcOffsetInSeconds);

WiFiClientSecure client;

unsigned long previousMillis = 0;  // Armazena o último tempo de atualização
const long interval = 60000;       // Intervalo de 1 minuto

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conectado ao Wi-Fi!");

  timeClient.begin();
  timeClient.update(); // Atualiza a hora imediatamente

  client.setInsecure(); // Para ignorar problemas de certificado SSL (não recomendado para produção)
}

void listarDocumentosTabelaRemedios() {
  if (client.connect("firestore.googleapis.com", 443)) {
    String url = String("/v1/projects/") + FIREBASE_PROJECT_ID + "/databases/(default)/documents/TabelaRemedios";
    client.print(String("GET ") + url + "?key=" + FIREBASE_API_KEY + " HTTP/1.1\r\n" +
                 "Host: firestore.googleapis.com\r\n" +
                 "Connection: close\r\n\r\n");

    delay(1000);

    bool dentroDeDocumento = false; // Variável para controlar quando estamos dentro de um documento
    String fieldName;  // Armazena o nome do campo
    String fieldValue; // Armazena o valor do campo
    String documentId; // Armazena o ID do documento
    String nome;       // Armazena o valor do campo 'nome'
    String compartimento; // Armazena o valor do campo 'compartimento'

    // Imprimir a data e hora atual
    time_t now = timeClient.getEpochTime();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    char dateBuffer[11]; // Buffer para armazenar a data no formato DD/MM/YYYY
    char timeBuffer[6];  // Buffer para armazenar a hora no formato HH:MM

    sprintf(dateBuffer, "%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
    sprintf(timeBuffer, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

    Serial.print("Data e Hora Atual: ");
    Serial.print(dateBuffer);
    Serial.print(" ");
    Serial.println(timeBuffer);
    Serial.println("------------------------------------------");

    // Variáveis para armazenar a data e a hora prevista dos documentos
    String diaPrevisto;
    String horarioPrevisto;
    bool found = false; // Flag para indicar se algum documento foi encontrado

    while (client.available()) {
      String line = client.readStringUntil('\n');
      line.trim(); // Remove espaços em branco

      // Verifica se a linha marca o início de um novo documento
      if (line.indexOf("\"name\": \"projects/") != -1) {
        if (dentroDeDocumento) {
          // Verifica se o dia e hora previstos são iguais à data e hora atuais
          if (diaPrevisto == dateBuffer && horarioPrevisto == timeBuffer) {
            Serial.println("Documento com data e hora prevista iguais à atual:");
            Serial.println("ID do Documento: " + documentId);
            Serial.println("Nome: " + nome);
            Serial.println("Compartimento: " + compartimento);
            Serial.println("------------------------------------------");
            found = true;
          }
        }

        // Extrai a ID do documento sem o prefixo "TabelaRemedios/"
        int idStart = line.indexOf("documents/") + 10; // A posição começa após "documents/"
        int idEnd = line.indexOf("\"", idStart);
        documentId = line.substring(idStart, idEnd);

        dentroDeDocumento = true;
        fieldName = "";
        fieldValue = "";
        nome = "";
        compartimento = "";
        continue;
      }

      // Se a linha contém "createTime" ou "updateTime", pule-a
      if (line.indexOf("\"createTime\"") != -1 || line.indexOf("\"updateTime\"") != -1) {
        continue;
      }

      // Se a linha contém "stringValue", capture o valor do campo
      int posStringValue = line.indexOf("\"stringValue\":");
      if (posStringValue != -1) {
        fieldValue = line.substring(posStringValue + 14);
        fieldValue.trim();
        fieldValue.replace("\"", ""); // Remove aspas do valor
        continue;
      }

      // Se a linha contém "fieldName", extraia o nome do campo
      int posFieldName = line.indexOf("\"");
      if (posFieldName != -1 && line.indexOf("\": {") != -1) {
        fieldName = line.substring(posFieldName + 1, line.indexOf("\": {"));
        fieldName.trim();
        continue;
      }

      // Se a linha contém "fieldValue", atribua o valor ao campo atual
      if (line.indexOf("}") != -1) {
        if (fieldName == "dia_previsto") {
          diaPrevisto = fieldValue;
        } else if (fieldName == "horario_previsto") {
          horarioPrevisto = fieldValue;
        } else if (fieldName == "nome") {
          nome = fieldValue;
        } else if (fieldName == "compartimento") {
          compartimento = fieldValue;
        }
        fieldName = "";
        fieldValue = "";
      }
    }

    // Exibe o último documento capturado se ele corresponder
    if (dentroDeDocumento) {
      if (diaPrevisto == dateBuffer && horarioPrevisto == timeBuffer) {
        Serial.println("Documento com data e hora prevista iguais à atual:");
        Serial.println("ID do Documento: " + documentId);
        Serial.println("Nome: " + nome);
        Serial.println("Compartimento: " + compartimento);
        Serial.println("------------------------------------------");
        found = true;
      }
    }

    // Se nenhum documento foi encontrado
    if (!found) {
      Serial.println("Nenhum documento encontrado com data e hora previstas iguais à atual.");
      Serial.println("------------------------------------------");
    }

    client.stop();
  } else {
    Serial.println("Falha na conexão com o Firestore.");
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    listarDocumentosTabelaRemedios();
  }
}
