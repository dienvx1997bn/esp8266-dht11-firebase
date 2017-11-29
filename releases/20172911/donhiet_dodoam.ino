#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"            

#define FIREBASE_HOST "nhietdodoam-6beb8.firebaseio.com" //Thay bằng địa chỉ firebase của bạn
#define FIREBASE_AUTH "L2RXzvkid2JlAYyVo2UAzMW9KP9QpFdAMYEJpOC9"   //Không dùng xác thực nên không đổi
#define WIFI_SSID "WIFI"   //Thay wifi và mật khẩu
#define WIFI_PASSWORD "bat3gdiem"

const int DHTPIN = 5;       //Đọc dữ liệu từ DHT11 ở chân 2 trên mạch Arduino
const int DHTTYPE = DHT11;  //Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22
 
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

  dht.begin();         // Khởi động cảm biến

}
void loop() {
  
  float h = dht.readHumidity();    //Đọc độ ẩm
  float t = dht.readTemperature(); //Đọc nhiệt độ
  
  Firebase.setFloat("do am", h);
  Firebase.setFloat("nhiet do", t);
  
  Serial.println(t);
  Serial.println(h);
  
  delay(2000);
}
