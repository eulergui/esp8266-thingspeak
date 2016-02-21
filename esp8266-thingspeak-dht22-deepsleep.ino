/* Guilherme Euler
*
* Programa para enviar dados do sensor DHT22 para o ThingSpeak.com
* Baseado no código do Pedro Minatel (http://pedrominatel.com.br/arduino/integrando-o-sensor-dht11-no-thingspeak-com-o-esp8266/)
* Data: 15 de Fevereiro de 2016
* Author: Guilherme Euler
* Website: http://www.infodicas.com.br
* 
* Leitura do sensor DHT22 a cada 5 minutos entrando em modo deep sleep para economia de energia. Não cheguei a testar o consumo em modo deep sleep. Caso alguém faça o teste, favor informar :)
* Para alterar o tempo de deepsleep, localizar no código o parâmetro "300000000" (sem aspas) e alterar para o valor desejado
* Neste caso, "300000000" equivale a 5 minutos, sendo que cada minuto = 60000000
*/
 
#include <ESP8266WiFi.h>
#include "DHT.h" //Utilizei a Lib da Adafruit sem maiores problemas
 
//Define do pino a ser utilizado no ESP para o sensor = GPIO4
#define DHT_DATA_PIN 4 //Pino a ser utilizado para a leitura do sensor, neste caso, pino 4
#define DHTTYPE DHT22 //Modelo do sensor, neste caso DHT22

const char* ssid = "------------"; //SSID da rede WiFi
const char* password = "---------"; //Senha da rede WiFi
 
String apiKey = "----------"; //API Key do canal no ThingSpeak
const char* server = "api.thingspeak.com";

DHT dht(DHT_DATA_PIN, DHTTYPE, 15);
 
void setup() {
  
  delay(10);
  dht.begin();

}
 
void loop() {

  //Inicia o WiFi
  WiFi.begin(ssid, password);
  delay(10);
  int counter = 0;
  //Espera a conexão no router
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    counter += 1;
      if (counter > 20) {
        ESP.deepSleep(300000000, WAKE_RF_DEFAULT);
      }
  }
    
  //Espera 1 segundo para fazer a leitura
  delay(1000);
  //Leitura de umidade
  float umidade = dht.readHumidity();
  //Leitura de temperatura
  float temperatura = dht.readTemperature();
 
  //Se não for um numero retorna erro de leitura
  if (isnan(umidade) || isnan(temperatura)) {
    return;
  }

  delay(10);
  WiFiClient client;
 
  //Inicia um client TCP para o envio dos dados
  if (client.connect(server,80)) {
    String postStr = apiKey;
           postStr +="&amp;field1=";
           postStr += String(temperatura);
           postStr +="&amp;field2=";
           postStr += String(umidade);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
  }
  client.stop();
  delay(10);
  WiFi.disconnect();
  delay(10);

  ESP.deepSleep(300000000, WAKE_RF_DEFAULT);
}
