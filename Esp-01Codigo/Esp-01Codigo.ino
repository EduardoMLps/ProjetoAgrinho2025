/* o endereço: http://192.168.4.1/   */

#include "FS.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

#define Modelo DHT11

const char *nome_da_rede = "Agrinho2025";
const char *senha = "123456789";

ESP8266WebServer servidor(80);

uint8_t Pino_SensorUmidade = 2;
uint8_t Pino_SensorSolo = 3;
uint8_t Pino_SensorChuva = 0;
uint8_t Pino_Valvula = 1;


DHT dht(Pino_SensorUmidade, Modelo);

//Variaveis:
float Temp;
float Umid;
float SoloUmi;
bool Chuva;
void setup()
{
  Serial.begin(9600);
  Serial.println("Configurando circuito.");
  pinMode(Pino_SensorSolo, FUNCTION_3);

  pinMode(Pino_SensorUmidade, INPUT);
  pinMode(Pino_SensorSolo, INPUT);
  pinMode(Pino_SensorChuva, INPUT);
  
  Serial.println("Iniciando sensores.");
  dht.begin();
  
  Serial.println("Criando rede.");
  WiFi.softAP(nome_da_rede, senha);
  Serial.println(WiFi.localIP());
  //fazer a função pagina_requisitada quando somonelse colocar o endereço
  servidor.on("/", Pagina_Requisitada);
  //caso contrário:
  servidor.onNotFound(Pagina_Inexistente);
  
  //iniciar servidor:
  Serial.println("Iniciando servidor.");
  servidor.begin();
}
void loop()
{
  servidor.handleClient();
}

void Pagina_Requisitada() {
  Temp = dht.readTemperature();
  Umid = dht.readHumidity();
  SoloUmi = digitalRead(Pino_SensorSolo);
  Chuva = digitalRead(Pino_SensorChuva);
  
  servidor.send(200, "text/html",
                Monta_HTML(Temp, Umid, SoloUmi, Chuva));
}

void Pagina_Inexistente() {
  servidor.send(404, "text/html",
                "<H1><!DOCTYPE html>"
                "<html>P&aacute;gina n&atilde;o encontrada"
                "</H1></html>");
}

String Monta_HTML(float temp, float umid, float solo, bool chu) {
  String ptr = "";
  ptr += "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" "
         "content=\"width=device-width, initial-scale=1.0, "
         "user-scalable=no\">\n";
  ptr += "<title>ROB&Oacute;TICA PARAN&Aacute;</title>\n";
  ptr += "<style>html { font-family: Helvetica; "
         "display: inline-block; margin: 0px auto; "
         "text-align: center;}\n";
  ptr += "body{margin-top: 50px; background: #009541;} "
         "h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: "
         "#444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "<script>\n";
  ptr += "setInterval(loadDoc,200);\n";
  ptr += "function loadDoc() {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && "
         "this.status == 200) {\n";
  ptr += "document.getElementById(\"webpage\")"
         ".innerHTML =this.responseText}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>ROB&Oacute;TICA PARAN&Aacute;</h1>\n";
  ptr += "<h2>Aula 37 - M&oacute;dulo WI-FI IoT "
         "com Sensor</h2>\n";
  ptr += "<h2>&#127781; Esta&ccedil;&atilde;o "
         "Meteorol&oacute;gica &#127781;</h2>\n";
  ptr += "<p>&#127777; Temperatura: ";
  ptr += (int)Temp;
  ptr += "°C</p>";
  ptr += "<p>&#x1F4A7; Umidade: ";
  ptr += (int)Umid;
  ptr += "%</p>";
  ptr += "<p>&#x1F4A7; Solo: ";
  ptr += (int)solo;
  ptr += "</p>";
  ptr += "<p>&#x1F4A7; chuva: ";
  ptr += (bool)chu;
  ptr += "</p>";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
