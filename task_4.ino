#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHTesp.h>
const char* ssid = "DESKTOP-1GQ0MNO 1520";
const char* password = "0v6863C*";

#define LCD_SDA 32
#define LCD_SCL 33
LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiServer server(80);
WiFiClient client;

#define DHTPIN 15
DHTesp dht;
#define LEDPIN 27

void setup() {
  Serial.begin(115200);
  Wire.begin(LCD_SDA, LCD_SCL);


  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting...");

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  server.begin();
  Serial.println("Server started");

  
  dht.setup(DHTPIN, DHTesp::DHT22);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
}

void loop() {
  client = server.available();
  if (!client) return;

  Serial.println("Client Connected");
  String request = "";

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      request += c;
      if (c == '\n') {
    
        if (request.indexOf("GET /temp") >= 0) {
          serveTemp();
        } else if (request.indexOf("GET /ledon") >= 0) {
          digitalWrite(LEDPIN, HIGH);
          serveLedOn();
        } else if (request.indexOf("GET /ledoff") >= 0) {
          digitalWrite(LEDPIN, LOW);
          serveLedOff();
        } else {
          serveInfo(); 
        }
        break;
      }
    }
  }

  delay(1);
  client.stop();
  Serial.println("Client Disconnected");
}

void serveInfo() {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html><head>");
  client.println("<title>ESP32 Control Panel</title>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; background-color: #f0f4c3; text-align: center; }");
  client.println("h1 { color: #33691e; }");
  client.println("p { font-size: 20px; margin: 10px; }");
  client.println("a { text-decoration: none; background-color: #8bc34a; color: white; padding: 10px 20px; border-radius: 5px; margin:5px; display:inline-block; }");
  client.println("a:hover { background-color: #558b2f; }");
  client.println("</style></head><body>");
  client.println("<h1>ESP32 Control Panel</h1>");
  client.println("<p><a href='/temp'>Show Temperature</a></p>");
  client.println("<p><a href='/ledon'>Turn LED ON</a></p>");
  client.println("<p><a href='/ledoff'>Turn LED OFF</a></p>");
  client.println("</body></html>");
}

void serveTemp() {
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html><head>");
  client.println("<title>DHT22 Temperature</title>");
  client.println("<meta http-equiv='refresh' content='5'>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; background-color: #e0f7fa; text-align: center; }");
  client.println("h1 { color: #00796b; }");
  client.println("p { font-size: 20px; }");
  client.println("a { text-decoration: none; background-color: #00acc1; color: white; padding: 10px 20px; border-radius: 5px; }");
  client.println("a:hover { background-color: #007c91; }");
  client.println("</style>");
  client.println("</head><body>");
  client.println("<h1>DHT22 Sensor Readings</h1>");
  client.print("<p>Temperature: "); client.print(temperature); client.println(" °C</p>");
  client.print("<p>Humidity: "); client.print(humidity); client.println(" %</p>");
  client.println("<p><a href='/'>Back</a></p>");
  client.println("</body></html>");
}

void serveLedOn() {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html><head>");
  client.println("<title>LED ON</title>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; background-color: #fff3e0; text-align: center; }");
  client.println("h1 { color: #f57c00; }");
  client.println("a { text-decoration: none; background-color: #fb8c00; color: white; padding: 10px 20px; border-radius: 5px; }");
  client.println("a:hover { background-color: #ef6c00; }");
  client.println("</style></head><body>");
  client.println("<h1>LED is NOW ON</h1>");
  client.println("<p><a href='/ledoff'>Turn OFF</a></p>");
  client.println("</body></html>");
}

void serveLedOff() {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html><head>");
  client.println("<title>LED OFF</title>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; background-color: #ffebee; text-align: center; }");
  client.println("h1 { color: #c62828; }");
  client.println("a { text-decoration: none; background-color: #e53935; color: white; padding: 10px 20px; border-radius: 5px; }");
  client.println("a:hover { background-color: #b71c1c; }");
  client.println("</style></head><body>");
  client.println("<h1>LED is NOW OFF</h1>");
  client.println("<p><a href='/ledon'>Turn ON</a></p>");
  client.println("</body></html>");
}
