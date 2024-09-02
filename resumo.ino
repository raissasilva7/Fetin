#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Defina suas credenciais Wi-Fi
#define WIFI_SSID "KYLA"
#define WIFI_PASSWORD "UK513B60HP45R"

// Configurações do Firestore
#define FIREBASE_API_KEY "AIzaSyAEeuYXv8-meyp1ACslr_jALYSksZmRcZ0"
#define FIREBASE_PROJECT_ID "carereminder-10bab"

// URL do Firestore (REST API)
#define FIRESTORE_URL "https://firestore.googleapis.com/v1/projects/" FIREBASE_PROJECT_ID "/databases/(default)/documents/TabelaRemedios"

WiFiClientSecure client;

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

  client.setInsecure(); // Para ignorar problemas de certificado SSL (não recomendado para produção)
  listarDocumentosTabelaRemedios();
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

    while (client.available()) {
      String line = client.readStringUntil('\n');
      line.trim(); // Remove espaços em branco

      // Verifica se a linha marca o início de um novo documento
      if (line.indexOf("\"name\": \"projects/") != -1) {
        if (dentroDeDocumento) {
          // Exibe o valor do campo antes de iniciar um novo documento
          if (!fieldName.isEmpty() && !fieldValue.isEmpty()) {
            Serial.println(fieldName + ": " + fieldValue);
          }
          Serial.println("------------------------------------------");
        }
        dentroDeDocumento = true;
        fieldName = "";
        fieldValue = "";
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
      if (line.indexOf("}") != -1 && !fieldName.isEmpty()) {
        Serial.println(fieldName + ": " + fieldValue);
        fieldName = "";
        fieldValue = "";
      }
    }

    // Exibe o último documento capturado
    if (dentroDeDocumento) {
      if (!fieldName.isEmpty() && !fieldValue.isEmpty()) {
        Serial.println(fieldName + ": " + fieldValue);
      }
      Serial.println("------------------------------------------");
    }

    client.stop();
  } else {
    Serial.println("Falha na conexão com o Firestore.");
  }
}

void loop() {
  // O loop principal fica vazio
}
