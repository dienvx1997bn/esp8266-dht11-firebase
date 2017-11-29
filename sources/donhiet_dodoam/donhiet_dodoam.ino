#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"            

#define FIREBASE_HOST "nhietdodoam-6beb8.firebaseio.com" //Thay bằng địa chỉ firebase của bạn
#define FIREBASE_AUTH "L2RXzvkid2JlAYyVo2UAzMW9KP9QpFdAMYEJpOC9"   //Không dùng xác thực nên không đổi
#define WIFI_SSID "WIFI"   //Thay wifi và mật khẩu
#define WIFI_PASSWORD "bat3gdiem"

const int DHTPIN = D1;       //Đọc dữ liệu từ DHT11 ở chân D1 trên mạch 
const int DHTTYPE = DHT11;  //Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22

int delTime = 5000;

int ledPin = D4;                 // LED connected to digital pin 16
int btnPin = D2;                  // BUTTON connected to digital pin 0
int ledState = LOW;

 
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  pinMode(ledPin, OUTPUT);      // sets the digital pin as output
  pinMode(btnPin, INPUT);       // sets the digital pin as input
 
  dht.begin();         // Khởi động cảm biến

}

void readAndPushDataToServer() {
  float h = dht.readHumidity();    //Đọc độ ẩm
  float t = dht.readTemperature(); //Đọc nhiệt độ
  
  Firebase.setFloat("do am", h);
  Firebase.setFloat("nhiet do", t);
  
  Serial.println(t);
  Serial.println(h);
  Serial.println("---------------");
}

void loop() {
  int buttonStatus = digitalRead(btnPin);    //Đọc trạng thái button
//  if (buttonStatus == HIGH) { // Nếu mà button bị nhấn
//    if(ledState == HIGH) {
//      ledState = LOW;
//    } else if (ledState == LOW) {
//      ledState = HIGH;
//    }
//  }

  if (buttonStatus == HIGH ) ledState = HIGH;
  else if(buttonStatus == LOW ) ledState = LOW;
  Serial.println(buttonStatus);
  digitalWrite(ledPin,ledState); // Đèn led sáng

  if (ledState == HIGH) {
    readAndPushDataToServer();
    delay(delTime);
  }


  
 }
