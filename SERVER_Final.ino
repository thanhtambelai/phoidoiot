
// Thong so Wifi
//const char* ssid = "TEST";
//const char* password = "5uperM@n!@#$%";

const char* ssid = "Luffy";
const char* password = "111222333";

#include <ESP8266WiFi.h>
#include <Wire.h>

#include <DHT.h>
#define DHTPIN 0
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);

#include <BH1750.h>
BH1750 lightMeter;


char ra; //Trang thai nut Ra
char vao; //Trang thai nut Vao

// Tao server
WiFiServer server(80);

// Pin
int x; 
String tt ="Đang đóng";
String rain;

void setup() {

  // Khoi dong Serial
  Serial.begin(9600);
  dht.begin();
  Wire.begin();
  lightMeter.begin();
  Serial.println("Running...");
  delay(10);

  pinMode(16, INPUT_PULLUP); //vao
  pinMode(2, INPUT_PULLUP); //ra
  pinMode(15,INPUT); //rainss
  pinMode(14,OUTPUT); // Enable pin - chân khởi động - nối vào GND sẽ giúp ta bật động cơ bước, nối vô VCC động cơ bước được thả ra. Nôm na: GND = servo.attach, VCC = servo.detach
  pinMode(12,OUTPUT); // Step pin
  pinMode(13,OUTPUT); // Dir - pin
  digitalWrite(14,HIGH); // Set Enable off
  digitalWrite(13,LOW);

  // Ket noi toi mang WiFi 
  Serial.println();
  Serial.println();
  Serial.print("Dang ket noi toi mang ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Da ket noi WiFi");

  // Khoi dong server
  server.begin();
  Serial.println("Khoi dong Server");

  // In dia chi IP
  Serial.println(WiFi.localIP());
}

void loop() {

  ra = chong_doi_ra();
  if (ra == true && tt == "Đang đóng"){
     dcra();
  }

  vao = chong_doi_vao();
    if (vao == true){
  }

  // Kiem tra neu co client ket noi
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Doi client gui data
  Serial.println("Co client moi");
  while(!client.available()){
    delay(1);
  }

  
 // Reading temperature and humidity
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  //light
  uint16_t lux = lightMeter.readLightLevel();
  // read rainss
  int mua = digitalRead(15);
  if (mua == 1) rain = "Không mưa";
  else 
  {
    rain = "Có mưa";
    if (tt == "Đang mở") dcvao();
  }
  
  // Doc yeu cau tu client
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Kiem tra yeu cau la gi
  if (req.indexOf("/on") != -1){
    dcra();
  }
  else if (req.indexOf("/off") != -1) {
    dcvao();
  }
  client.flush();

  // Chuan bi thong tin phan hoi
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<head>";
  s += "<meta http-equiv=\"Refresh\" content=\"5; url=/update\" />";
  s += "<meta http-equiv=\"Content-Type\" content=\"application/vnd.wap.xhtml+xml; charset=utf-8\" />";
  s += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  s += "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
  s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
  s += "</head>";
  s += "<div class=\"container\">";
  s += "<h1><center>Đồ án tốt nghiệp - Cơ điện tử - HCMUTE</center></h1>";
  s += "<hr />";
  s += "<hr />";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-6\"><input class=\"btn btn-block btn-lg btn-success\" type=\"button\" value=\"Phơi ra\" onclick=\"on()\"></div>";
  s += "<div class=\"col-md-6\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"Thu vào\" onclick=\"off()\"></div>";
  s += "</div></div>";
  s += "<hr />";
  s += "<hr />";
  s += "<div class=\"col-md-3\">Nhiệt độ:</div><div class=\"col-md-3\">" + String(t) + " *C" + "</div>";
  s += "<div class=\"col-md-3\">Độ ẩm:</div><div class=\"col-md-3\">" + String(h) + " %" +"</div>";
  s += "<div class=\"col-md-3\">Ánh sáng:</div><div class=\"col-md-3\">" + String(lux) + " lux" + "</div>";
  s += "<div class=\"col-md-3\">Có mưa:</div><div class=\"col-md-3\">" + rain + "  " + "</div>";
  s += "<div class=\"col-md-3\">Trạng thái:</div><div class=\"col-md-3\">" + String(tt) + "</div>";
  s += "<br />";
  s += "<br>";
  s += "<hr />";
  s += "<hr />";
  //s += "<center><strike>Nguyễn Thành Tâm <3</strike></center>";
  s += "<center>Đại học Sư phạm Kỹ thuật TP HCM</center>";
  s += "<script>function on() {$.get(\"/on\");}</script>";
  s += "<script>function off() {$.get(\"/off\");}</script>";

  // Gui thong tin cho client
  client.print(s);
  delay(1);
  Serial.println("Ngat ket noi Client");
  
}

void dcvao() {
  digitalWrite(14,LOW); // Đặt Enable ở trạng thái LOW
  digitalWrite(13,HIGH); // Đặt Dir  ở trạng thái HIGH
  for(x = 0; x < 600; x++) // Cho chay 1 vong
  {
    digitalWrite(12,HIGH);
    delay(1);
    digitalWrite(12,LOW);
    delay(1);
  }
  digitalWrite(14,HIGH);
  tt = "Đang đóng";
}

void dcra() {
  digitalWrite(14,LOW); // Đặt Enable ở trạng thái LOW
  digitalWrite(13,LOW); // Đặt Dir  ở trạng thái HIGH
  for(x = 0; x < 600; x++) // Cho chay 1 vong
  {
    digitalWrite(12,HIGH);
    delay(1);
    digitalWrite(12,LOW);
    delay(1);
  }
  digitalWrite(14,HIGH);
  tt = "Đang mở";
}


boolean chong_doi_ra(){
int nhannutra =! digitalRead(2);
delay(50);
return nhannutra;
}

boolean chong_doi_vao(){
int nhannutvao =! digitalRead(16);
delay(50);
return nhannutvao;
}
