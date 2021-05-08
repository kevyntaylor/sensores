
#include "PMS.h"
#include <Wire.h>  
#include <SoftwareSerial.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//CONEXION PARA AM2302
#define PIN_CONEXION D8// A cuál pin está conectado el lector
#define TIPO_SENSOR DHT22 // Puede ser DHT11 también
DHT sensor(PIN_CONEXION, TIPO_SENSOR);

//CONEXION PARA ESP8266MOD
const String SSID = "KevynT";
const String PASSWORD = "MATHIAS0108";
const String SERVER_ADDRESS = "http://unortecapms7003dht22.com.co/";

//Variables del sensor de humedad y temp
float humedad, temperaturaEnGradosCelsius = 0;
int ultimaVezLeido = 0;
long milisegundosDeEsperaParaLeer = 2000; // 2 segundos

//Librerias para pms7003
PMS pms(Serial);
PMS::DATA data;
int seg = 0;
void setup()
{
    Serial.begin(9600);  // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board) 
    Serial.setTimeout(2000);
    // Muy importante la siguiente línea, pues configura el pin del sensor como INPUT_PULLUP
    sensor.begin();
    while(!Serial) { }// Esperar que haya conexión serial

    Serial.println("Iniciando el dispositivo");
    Serial.println("Kevyn Taylor (Developer software)");
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    Serial.print("Conectando a:\t");
    Serial.println(SSID); 
    // Esperar a que nos conectemos
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(200);
     Serial.print('.');
    }
    // Mostrar mensaje de exito y dirección IP asignada
    Serial.println();
    Serial.print("Conectado a:\t");
    Serial.println(WiFi.SSID()); 
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
}

void loop()
{
   if (pms.read(data)){ 
      Serial.print("PM 2.5 (ug/m3): ");
      Serial.println(data.PM_AE_UG_2_5);
      Serial.print("PM 10.0 (ug/m3): ");
      Serial.println(data.PM_AE_UG_10_0); 
      Serial.println();   
      humedad = sensor.readHumidity();
      temperaturaEnGradosCelsius = sensor.readTemperature();
     if(!isnan(temperaturaEnGradosCelsius) && !isnan(humedad)) {
        Serial.print("Humedad: ");
        Serial.print(humedad);
        Serial.print(" %\t");
        Serial.print("Temperatura: ");
        Serial.print(temperaturaEnGradosCelsius);
        Serial.println(" *C");
      }
    
    HTTPClient http;
    String full_url;
    int httpCode;
    full_url = SERVER_ADDRESS + "am2302.php?temperature=" + temperaturaEnGradosCelsius + "&humidity=" + humedad;
    Serial.println(full_url);
    http.begin(full_url);
    httpCode = http.GET();
    http.end();

    full_url = SERVER_ADDRESS + "pms7003.php?pm2=" + data.PM_AE_UG_2_5 + "&pm10=" + data.PM_AE_UG_10_0;
    Serial.println(full_url);
    http.begin(full_url);
    httpCode = http.GET();
    http.end();        
    }
}
