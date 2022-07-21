#include <ArduinoJson.h>    //https://github.com/bblanchon/ArduinoJson
#include <WiFi.h>

const char* ssid     = ""; //Wifi name
const char* password = ""; //WiFi password
String CityID = "3447212"; //Suzano/SP
String APIKEY = ""; //log into openweather and follow documentation
#define ALTITUDE 749.0 

#define I2C_SDA 27
#define I2C_SCL 26
#define LED_PIN 2

float temperature = 0;
float humidity = 0;
float pressure = 0;

WiFiClient client;
char* servername ="api.openweathermap.org";
String result;

int  iterations = 1800;
float Temperature;

void setup() 
{
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(9600);

  connectToWifi();
}

void loop() {
 
 delay(2000);

 if(iterations == 900)
 {
   getWeatherData();
   iterations = 0;   
 }
  iterations++;
 
 blinkLED();
}

void connectToWifi()
{
  WiFi.enableSTA(true);
  
  delay(2000);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}

void resetNextion()
{
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
}

void blinkLED()
{
  digitalWrite(LED_PIN,HIGH);
  delay(100);
  digitalWrite(LED_PIN,LOW);
}

void getWeatherData() 
{
  String result ="";
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(servername, httpPort)) {
        return;
    }
    String url = "/data/2.5/forecast?id="+CityID+"&units=metric&cnt=1&APPID="+APIKEY;

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + servername + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            client.stop();
            return;
        }
    }
    while(client.available()) {
        result = client.readStringUntil('\r');
    }

result.replace('[', ' ');
result.replace(']', ' ');

char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';

StaticJsonBuffer<1024> json_buf;
JsonObject &root = json_buf.parseObject(jsonArray);
if (!root.success())
{
  Serial.println("parseObject() failed");
}

String temperature = root["list"]["main"]["temp"];
String humidity = root ["list"]["main"]["humidity"];
String pressure = root ["list"]["main"]["pressure"];

resetNextion(); 

String temp = "temperature.txt=\""+temperature+"\"";
Serial.print(temp);
resetNextion();

  String humidi = "humidity.txt=\""+humidity+"\"";
  Serial.print(humidi);
  resetNextion();

String pressu = "pressure.txt=\""+pressure+"\"";
Serial.print(pressu);
resetNextion();

}
