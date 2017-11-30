#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"            

#define FIREBASE_HOST "nhietdodoam-6beb8.firebaseio.com" //Thay bằng địa chỉ firebase của bạn
#define FIREBASE_AUTH "L2RXzvkid2JlAYyVo2UAzMW9KP9QpFdAMYEJpOC9"   //Không dùng xác thực nên không đổi
#define WIFI_SSID "WIFI"   //Thay wifi và mật khẩu
#define WIFI_PASSWORD "bat3gdiem"

const int DHTPIN = D1;       //Đọc dữ liệu từ DHT11 ở chân D1 trên mạch 
const int DHTTYPE = DHT11;  //Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22

int delTime = 2000;

int led[2] = {D4, D3} ;                 // LED connected to digital pin 16
int button[2] = {D5, D2};                  // BUTTON connected to digital pin 0
int ledStatus[2] = {0, 0};
boolean reading[2] = {} ;//Biến dùng để lưu trạng thái thời gian thực của các button 
unsigned long waitTime = 500;
boolean lastButtonStatus[2] = {0, 0}; //Biến dùng để lưu trạng thái của phím bấm. Mặc định là 0 <=> LOW <=> chưa nhấn
boolean buttonLongPress[2] = {0, 0};  // Mặc định là không có sự kiện nhấn giữ.
int buttonLongPressTimes[2] = {0,0}; //Biến dùng để lưu số lượng sự kiện nhấn giữ button của bạn.
unsigned long lastChangedTime[2];
unsigned long lastTimePushToServer; //Thoi gian cuoi push len server

int count = 0; 
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
  for (int i = 0;i < sizeof(led) / sizeof(int); i++) {//Xem hàm sizeof tại http://arduino.vn/reference/sizeof
    pinMode(led[i],OUTPUT);   // Đặt chân LED là OUTPUT - hiển nhiên rồi
    digitalWrite(led[i],ledStatus[i]); //Xuất trạng thái mặc định
  }
  for (int i = 0;i < sizeof(button) / sizeof(int); i++)
    pinMode(button[i],INPUT); // Đặt chân button là INPUT - hiển nhiên rồi
  
  dht.begin();         // Khởi động cảm biến

}

void readAndPushDataToServer() {
    if(millis() - lastTimePushToServer > delTime) {
    float h = dht.readHumidity();    //Đọc độ ẩm
    float t = dht.readTemperature(); //Đọc nhiệt độ

    Firebase.setFloat("do am", h);
    Firebase.setFloat("nhiet do", t);
    
    Serial.println(count);
    Serial.println(t);
    Serial.println(h);
    Serial.println("---------------");
    count++;

    lastTimePushToServer = millis();
  }
}

void changeLedStatus() {
  ledStatus[1] = 0;
}

void loop() {
  for (int i = 0;i < sizeof(button) / sizeof(int); i++) {
    reading[i] = digitalRead(button[i]); // đọc giá trị của button
    if (reading[i] != lastButtonStatus[i]) { // Nếu bạn đang nhấn button rồi thả ra giữa chừng hoặc đang thả button rồi nhấn lại 
                                       // thì ta cần phải cập nhập lại thời gian thay đổi cuối cùng
      if (reading[i] == LOW) buttonLongPress[i] = false;
      lastButtonStatus[i] = reading[i]; //Cập nhập trạng thái cuối cùng.
      lastChangedTime[i] = millis(); //Cập nhập thời gian
    } // Còn nếu bạn đang nhấn giữ button hoặc thả nó thời gian dài chỉ sẽ không ảnh hưởng đến việc này
  
    if (millis() - lastChangedTime[i] > waitTime) { // Nếu sự chênh lệch giữa thời điểm đang xét và thời điểm cuối cùng thay đổi trạng thái của button lớn hơn thời gian đợi để tạo sự kiện nhấn giữ THÌ nó là một sự kiến nhấn giữ
      if (buttonLongPress[i] != reading[i]) {
        buttonLongPressTimes[i] = 1;
      } else {
        buttonLongPressTimes[i]++;
      }
      buttonLongPress[i] = reading[i]; // Cập nhập trạng thái của button = với trạng thái của button
      lastChangedTime[i] = millis();
    }
  }

  for (int i = 0;i < sizeof(button) / sizeof(int); i++) {
    if (buttonLongPress[i] == true && buttonLongPressTimes[i] == 1) { // Nếu có sự kiện nhấn giữ và đó là lần đầu tiên, ta thay đổi trạng thái của đèn LED. Lúc này ta không cần phải loại bỏ sự kiện này nữa. Tạo điều kiện cho việc nhấn giữ tổ hợp phím
      ledStatus[i] = !ledStatus[i]; //Dấu ! là dấu phủ định, 0 thành 1, 1 thành 0, tắt thành sáng, sáng thành tắt
      buttonLongPressTimes[i]++; // Tránh trường hợp thay đổi 2 lần
    }
    digitalWrite(led[i],ledStatus[i]); // Xuất giá trị của đèn LED ra đèn LED
  }
  
  if (ledStatus[0] == HIGH && ledStatus[1] == LOW) {
    readAndPushDataToServer();
  }

  if (ledStatus[1] == HIGH && ledStatus[0] == LOW) {
    readAndPushDataToServer();
    changeLedStatus();
  }


 }
