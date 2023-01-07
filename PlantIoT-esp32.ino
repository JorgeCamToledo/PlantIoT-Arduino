#include <LiquidCrystal.h>
#include "DHT.h"
#include <HTTPClient.h>
#include <WiFi.h>
#define echoPin 33
#define trigPin 32
#define bomba 27
#define DHTPIN 4
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
int sensorPinSuelo = 34;
// vo = potenciometro, vdd = 5V, k = ground, a = 5V,rw = ground, vss = ground
const int rs = 5, en = 18, d4 = 19, d5 = 21, d6 = 22, d7 = 23;  
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const char*ssid = "software";
const char*password =  "software22";

void setup() {
Serial.begin(115200);
 WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(500);
    Serial.print(".");
  }
  Serial.print("Conectado con éxito, mi IP es: ");
  Serial.println(WiFi.localIP());
dht.begin();
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
pinMode(bomba,OUTPUT);
Serial.println(F("DHTxx test!"));
lcd.begin(20, 4);
}
void loop() {
  int suelo = analogRead(sensorPinSuelo);
 
  int distance1 = getDistance();
  String status_bomba = "Apagada";
  int capacidadTanque = porcentajeTanque(distance1);
  if (distance1 > 25){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Nivel de agua bajo");
    lcd.setCursor(0,2);
    lcd.print("Llenar tanque de");
    lcd.setCursor(0,3);
    lcd.print("de agua");
    delay(1000);
    lcd.clear();
    }
    else{
      //Es un sensor lento, por lo que hay que darle tiempo.
float h = dht.readHumidity();
float t = dht.readTemperature();

if (isnan(h) || isnan(t)) {
Serial.println(F("Failed to read from DHT sensor!"));
return;
}

if(suelo >= 3600){
lcd.setCursor(0,2);
lcd.print("Suelo: regando");
digitalWrite(bomba,HIGH);
status_bomba = "Encendida";
delay(2000);
digitalWrite(bomba,LOW);
 }
  else{
lcd.setCursor(0,2);
status_bomba= "Apagada";
lcd.print("Suelo: humedo");
    }

Serial.print(F("Humedad: "));
Serial.print(h);
Serial.print(F("% |Temperatura: "));
Serial.print(t);
Serial.print(F("°C "));
Serial.print(" |Suelo: ");
Serial.println(suelo);
Serial.print(" |Distancia: ");
Serial.print(distance1);
Serial.print(" |Porcentaje del tanque: ");
Serial.print(capacidadTanque);
Serial.print(" |Estado de la bomba:");
Serial.println(status_bomba);
delay(150000);
Serial.println("Mandar datos a la paginaa");
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status

    HTTPClient http;
    String datos_a_enviar = "humedad=" + String(h) + "&temperatura=" + String(t)+ "&capacidadTanque=" + String(capacidadTanque) + "&humSuelo=" + String(suelo) + "&statusBomba=" + String(status_bomba);

    http.begin("http://192.168.0.37:3002/api/planta/");        //Indicamos el destino
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Preparamos el header text/plain si solo vamos a enviar texto plano sin un paradigma llave:valor.

    int codigo_respuesta = http.POST(datos_a_enviar);   //Enviamos el post pasándole, los datos que queremos enviar. (esta función nos devuelve un código que guardamos en un int)

    if(codigo_respuesta>0){
      Serial.println("Código HTTP ► " + String(codigo_respuesta));   //Print return code

      if(codigo_respuesta == 200){
        String cuerpo_respuesta = http.getString();
        Serial.println("El servidor respondió ▼ ");
        Serial.println(cuerpo_respuesta);
  
      }

    }else{

     Serial.print("Error enviando POST, código: ");
     Serial.println(codigo_respuesta);

    }

    http.end();  //libero recursos

  }else{

     Serial.println("Error en la conexión WIFI");

  }
      }
}
int getDistance(){
  int duration, distance;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2)/29.1; // CM
  return distance;
}

int porcentajeTanque(int distancia){
  int centimetros;
  double porcentaje;
  centimetros = distancia -3 ;
  porcentaje = 100 - (centimetros*14.28);
  return porcentaje;
  
  }
