#include "SPIFFS.h"
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <DHT.h>                

const char* ssid = "Ste";
const char* password = "20042020"; 

const char* mqtt_server = ""; //insira seu servidor MQTT
const int mqtt_port = 8883;

String Read_rootca;
String Read_cert;
String Read_privatekey;

#define BUFFER_LEN  256
long lastMsg = 0;
char msg[BUFFER_LEN];
int value = 0;
byte mac[6];
char mac_Id[18];
int count = 1;
WiFiClientSecure espClient;
PubSubClient client(espClient);


#define DHTPIN 12 
DHT dht(DHTPIN, DHT11);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando.. ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem Recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void reconnect() {
  
  
  while (!client.connected()) {
    Serial.print("Inicianco conexão MQTT...");
    
    
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    
    
    if (client.connect(clientId.c_str())) {
      Serial.println("conectada");
      
    
      client.publish("ei_out", "hello world");
    
   
      client.subscribe("ei_in");
    } else {
      Serial.print("erro, rc=");
      Serial.print(client.state());
      Serial.println(" Esperando 5 segundos");
      
      
      delay(5000);
    }
  }
}

void setup() {
  
  Serial.begin(115200);
  dht.begin();
  Serial.setDebugOutput(true);
  
  
  pinMode(2, OUTPUT);
  setup_wifi();
  delay(1000);
  
 
  if (!SPIFFS.begin(true)) {
    Serial.println("Erro no SPIFFS");
    return;
  }
  
  File file2 = SPIFFS.open("/AmazonRootCA1.pem", "r");
  if (!file2) {
    Serial.println("Erro na leitura");
    return;
  }
  Serial.println("Root CA:");
  while (file2.available()) {
    Read_rootca = file2.readString();
    Serial.println(Read_rootca);
  }
 
  File file4 = SPIFFS.open("/4b12bc-certificate.pem.crt", "r");
  if (!file4) {
    Serial.println("Erro na leitura");
    return;
  }
  Serial.println("Certificado:");
  while (file4.available()) {
    Read_cert = file4.readString();
    Serial.println(Read_cert);
  }
 
  File file6 = SPIFFS.open("/4b12bc-private.pem.key", "r");
  if (!file6) {
    Serial.println("Erro na leitura");
    return;
  }
  Serial.println("Private Key:");
  while (file6.available()) {
    Read_privatekey = file6.readString();
    Serial.println(Read_privatekey);
  }
  
  char* pRead_rootca;
  pRead_rootca = (char *)malloc(sizeof(char) * (Read_rootca.length() + 1));
  strcpy(pRead_rootca, Read_rootca.c_str());

  char* pRead_cert;
  pRead_cert = (char *)malloc(sizeof(char) * (Read_cert.length() + 1));
  strcpy(pRead_cert, Read_cert.c_str());

  char* pRead_privatekey;
  pRead_privatekey = (char *)malloc(sizeof(char) * (Read_privatekey.length() + 1));
  strcpy(pRead_privatekey, Read_privatekey.c_str());

  Serial.println("================================================================================================");
  Serial.println("Passando certificados anexados ao espClient");
  Serial.println();

  espClient.setCACert(pRead_rootca);
  espClient.setCertificate(pRead_cert);
  espClient.setPrivateKey(pRead_privatekey);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

 
  WiFi.macAddress(mac);
  snprintf(mac_Id, sizeof(mac_Id), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(mac_Id);
  
  delay(2000);
 
}


void loop() {
  float h = dht.readHumidity();        
  float t = dht.readTemperature();      
  if (isnan(h) || isnan(t))
  {
    Serial.println("Erro na leitura do sensor DHT11");
    return;
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
   
    String macIdStr = mac_Id;
    String Temprature = String(t);
    String Humidity = String(h);
    snprintf (msg, BUFFER_LEN, "{\"Id\" : \"%d\", \"Temperatura\" : %s, \"Umidade\" : %s}", count, Temprature.c_str(), Humidity.c_str());
    Serial.print("Publicando mensagem: ");
    Serial.print(count);
    Serial.println(msg);
    client.publish("dht11", msg);
    count = count + 1;
  }          
}
