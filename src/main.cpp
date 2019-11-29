#include <Arduino.h>
#include <dht.h>
#include <Servo.h>
#include <Wire.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

float temperature=0;
float humidity=0;
int dhtPin=16;
int greenPin=5;
int redPin=4;
int buzzerPin=14;
int servoPin=12;
int pos=0;
Servo servo;
int k=0;
int j=5;
String data;
int buzz=0;
int led=0;
int door=0;

dht DHT;
void setup(){
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(buzzerPin,OUTPUT);
  servo.attach(servoPin);
  Serial.begin(115200);
  WiFi.begin("tatoon", "ciaociao32");   //WiFi connection
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    delay(500);
    Serial.println("Waiting for connection");
  }
  Serial.begin(9600);
}

void getDhtDta(float& temperature, float& humidity){
  int chk=DHT.read11(dhtPin);
   switch (chk)
  {
    case DHTLIB_OK:  
		  Serial.print("OK,\t"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		  Serial.print("Checksum error,\t"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		  Serial.print("Time out error,\t"); 
		break;
    default: 
		  Serial.print("Unknown error,\t"); 
		break;
  }
  temperature=DHT.temperature;
  humidity=DHT.humidity;
}

void allarm(){  
  if(temperature>=26 || humidity >=80)
  {
    digitalWrite(greenPin,LOW);
    digitalWrite(redPin,HIGH);
    tone(buzzerPin,110,1000);
    tone(buzzerPin,2000,1000);
    if(pos!=90){
      for(pos = 0; pos < 90; pos += 1)
      {
        servo.write(pos);
        delay(15);
      }
    }    
    led=1;
    buzz=1;
    door=1;
  }
  else
  {
    digitalWrite(greenPin,HIGH);
    digitalWrite(redPin,LOW);
    if(pos!=0){
      for(pos = 90; pos>=1; pos-=1)
      {
        servo.write(pos);
        delay(15);
      }
    }    
    led=0;
    buzz=0;
    door=0;
  }
}

void loop(){
  getDhtDta(temperature, humidity);
  allarm();
  
  Serial.print(humidity, 1);
  Serial.print(",\t");
  Serial.println(temperature, 1);

  if(k<=0)
  {
    k=j;
    //sprintf(temp,"%f F", temperature);
    data=String(temperature)+";"+String(humidity)+";"+buzz+";"+led+";"+door;
    if(WiFi.status()== WL_CONNECTED)
    {   //Check WiFi connection status 
      HTTPClient http;    //Declare object of class HTTPClient
    
      http.begin("http://192.168.43.106/post.php");      //Specify request destination
      http.addHeader("Content-Type", "text/plain");  //Specify content-type header
    
      int httpCode = http.POST(data);   //Send the request
      String payload = http.getString();                  //Get the response payload
    
      /*Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload */
    
      http.end();  //Close connection
    }
    else
    { 
      Serial.println("Error in WiFi connection"); 
    }
  }
  k--;
  delay(2000);  //Send a request every 30 seconds
}