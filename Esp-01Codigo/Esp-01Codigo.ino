//Primeiro Commit: Mandado o código para o github.
//SPIFFS: Adicionado a biblioteca FS.h para usar SPIFFS
//WebSocket: Adicionado a biblioteca WebSocketsServer.h para se comunicar com a página.
//Design: Melhorado o design da página.
//Auto data: Esp mandará dados automáticamente para a página.
//mDNS: adicionado a livraria mDNS para link customizado.
//Graphs: criado a função para criar gráficos na página.
//Valvula: adicionado botão 'valvula' para abrir a valvula do circuito.
//Linhas: adicionado linhas para os gráficos na página.
//BugFix1: adicionado prints para depuração

/* o endereço: http://192.168.4.1/   */

#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DHT.h>
#include <WebSocketsServer.h>
#include <floatToString.h>

#define Modelo DHT11

String getContentType(String filename);
bool handleFileRead(String path);

const char *nome_da_rede = "Agrinho2025";
const char *senha = "123456789";

ESP8266WebServer servidor(80);
WebSocketsServer webSocket(81);

File fsUploadFile;

uint8_t Pino_SensorUmidade = 2;
uint8_t Pino_SensorSolo = 3;
uint8_t Pino_SensorChuva = 0;

DHT dht(Pino_SensorUmidade, Modelo);

//Variaveis:
float Temp;
float Umid;
String SoloUmi;
String Chuva;
void setup()
{
  Serial.begin(9600);
  Serial.println("Configurando circuito.");
  pinMode(Pino_SensorSolo, FUNCTION_3);

  pinMode(Pino_SensorUmidade, INPUT);
  pinMode(Pino_SensorSolo, INPUT);
  pinMode(Pino_SensorChuva, INPUT);

  iniciarWiFi();

  //iniciarmDNS();

  iniciarSPIFFS();

  iniciarWebSocket();
  
  iniciarSensores();

  //mandar a pagina:
  servidor.onNotFound([]() {
    if (!handleFileRead(servidor.uri()))
      servidor.send(404, "text/plain", "404: Nao encontrado.");
  });

  //iniciar servidor:
  Serial.println("Iniciando servidor.");
  servidor.begin();
}

unsigned long tickAntigo = 0;
const long tempoDeEnvio = 1000;
void loop()
{
  unsigned long currentTick = millis();
  
  webSocket.loop();
  servidor.handleClient();

  SoloUmi = digitalRead(Pino_SensorSolo);
  Chuva = digitalRead(Pino_SensorChuva);
  
  //Enviar dados a cada 1 segundos:
  if(currentTick - tickAntigo >= tempoDeEnvio) {

    Temp = dht.readTemperature();
    Umid = dht.readHumidity();
    
    if(isnan(Temp)) {
      Serial.println("error: no 'Temp'");
      Temp = 0;
    }
    if(isnan(Umid)) {
      Serial.println("error: no 'Umid'");
      Umid = 0;
    }
    enviarDados();
    
    tickAntigo = millis();
  }
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = servidor.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

void iniciarSensores() {
  Serial.println("Iniciando sensores.");
  dht.begin();
}

void iniciarWiFi() {
  Serial.println("Criando rede.");
  WiFi.softAP(nome_da_rede, senha);

  Serial.println(WiFi.localIP());
}

void iniciarmDNS() {
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
  }
  MDNS.addService("http", "tcp", 80);
  Serial.println("mDNS responder started");
}

void iniciarSPIFFS() {
  Serial.println("Começando SPIFFS.");
  SPIFFS.begin();
}

void iniciarWebSocket() {
  Serial.println("Começando WebSocket.");
  webSocket.begin();
  webSocket.onEvent(eventoWebSocket);
}

void eventoWebSocket(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      switch(payload[0]) {
        case 'V':
        Serial.println("Abrir a valvula");
        break;
      }
      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\n", num, lenght);
      hexdump(payload, lenght);

      // send message to client
      // webSocket.sendBIN(num, payload, lenght);
      break;
  }
}

void enviarDados() {
  char S[15];

  String TempC = String(floatToString(Temp, S, sizeof(S), 1));
  String UmidC = String(floatToString(Umid, S, sizeof(S), 1));
  String SoloUmiC = String(SoloUmi);
  String ChuvaC = String(Chuva);

  //Serial.println(TempC);
  //Serial.println(UmidC);
  //Serial.println(Temp);
  //Serial.println(Umid);

  // mandar mensagem:
  webSocket.broadcastTXT(".");
  webSocket.broadcastTXT(TempC);
  webSocket.broadcastTXT(UmidC);
  webSocket.broadcastTXT(SoloUmiC);
  webSocket.broadcastTXT(ChuvaC);
  webSocket.broadcastTXT(":");
}
