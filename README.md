Python: Versão 3.12.3

Bibliotecas utilizadas: pyrebase/datetime/pytz

Para instalação das bibliotecas, é necessário abrir o  terminal VsCode e digitar os seguintes comandos(um por vez):
pip install pyrebase4/pip install pytz

Para credencial do pytohn copiar o caminho do arquivo json do seu computador, e colar no código.




ESP8266: Para acessar o banco de dados 

Biblotecas:
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h> // Inclua a biblioteca padrão de tempo

Wi-Fi:
#define WIFI_SSID "nome da rede"
#define WIFI_PASSWORD "senha da rede"

Configurações do Firestore:
FIREBASE_API_KEY "AIzaSyAEeuYXv8-meyp1ACslr_jALYSksZmRcZ0"
FIREBASE_PROJECT_ID "carereminder-10bab"

URL do Firestore (REST API): "https://firestore.googleapis.com/v1/projects/" FIREBASE_PROJECT_ID "/databases/(default)/documents/TabelaRemedios"


