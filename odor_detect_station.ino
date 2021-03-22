
#include "rpcWiFi.h" //Wi-Fi library 
#include "DHT.h" //DHT library
#include "TFT_eSPI.h" //TFT LCD library 
#include "Free_Fonts.h" //free fonts library
#include <Multichannel_Gas_GMXXX.h>
#include <Wire.h>

GAS_GMXXX<TwoWire> gas;
TFT_eSPI tft; //initialize TFT LCD
TFT_eSprite spr = TFT_eSprite(&tft); //initialize sprite buffer

unsigned int no2, c2h5ch, voc, co;
#define DHTPIN 0 //define Signal Pin of DHT
#define DHTTYPE DHT11 //define DHT Sensor Type
DHT dht(DHTPIN, DHTTYPE); //initialize DHT sensor
 
const char* ssid = "xxxxx"; //Wi-Fi name    "xxxxx"
const char* password =  "xxxxx"; //Wi-Fi password    "xxxx."


const char* serverName = "io.adafruit.com"; //Domain Name for HTTP POST Request
String AIOKey = "aio_xxxxxxxxxxxxxxxxxxx"; //"Write API Key" 

// Use WiFiClient class to create TCP connections
WiFiClient client;
 
void setup() {
  Serial.begin(115200); //start serial communication
  //while (!Serial); //wait for Serial to be ready
  delay(100);
 
  WiFi.mode(WIFI_STA); //set WiFi to station mode 
  WiFi.disconnect(); //disconnect from an AP if it was previously connected
 
  Serial.println("Connecting to WiFi.."); //print string 
  WiFi.begin(ssid, password); //connect to Wi-Fi network
 
  // attempt to connect to Wi-Fi network:
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      // wait 1 second for re-trying
      delay(1000);
  }
    
  Serial.print("Connected to ");
  Serial.println(ssid); //print Wi-Fi name 
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); //print Wio Terminal's IP address
  Serial.println(""); //print empty line

  dht.begin(); //Start DHT sensor
  gas.begin(Wire, 0x08);

//  ----------------LCD------------------------

  tft.begin();
  tft.setRotation(3);
  //Head
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBoldOblique18pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Odor Station", 60, 10 , 1);
  //Line
  for (int8_t line_index = 0; line_index < 5 ; line_index++)
  {
    tft.drawLine(0, 50 + line_index, tft.width(), 50 + line_index, TFT_PINK);
  }
  //VCO & CO Rect
  tft.drawRoundRect(5, 60, (tft.width() / 2) - 20 , tft.height() - 65 , 10, TFT_WHITE); // L1
  //VCO Text
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("VOC", 7 , 65 , 1);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("ppm", 55, 108, 1);
  //CO Text
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("CO", 7 , 150 , 1);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("ppm", 55, 193, 1);
  // Temp rect
  tft.drawRoundRect((tft.width() / 2) - 10  , 60, (tft.width() / 2) / 2 , (tft.height() - 65) / 2 , 10, TFT_BLUE); // s1
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_RED) ;
  tft.drawString("Temp", (tft.width() / 2) - 1  , 70 , 1); // Print the test text in the custom font
  tft.setTextColor(TFT_GREEN);
  tft.drawString("o", (tft.width() / 2) + 30, 95, 1);
  tft.drawString("C", (tft.width() / 2) + 40, 100, 1);
  //No2 rect
  tft.drawRoundRect(((tft.width() / 2) + (tft.width() / 2) / 2) - 5  , 60, (tft.width() / 2) / 2 , (tft.height() - 65) / 2 , 10, TFT_BLUE); // s2
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("NO2", ((tft.width() / 2) + (tft.width() / 2) / 2)   , 70 , 1); // Print the test text in the custom font
  tft.setTextColor(TFT_GREEN);
  tft.drawString("ppm", ((tft.width() / 2) + (tft.width() / 2) / 2) + 30 , 120, 1);
  //Humi Rect
  tft.drawRoundRect((tft.width() / 2) - 10 , (tft.height() / 2) + 30, (tft.width() / 2) / 2 , (tft.height() - 65) / 2 , 10, TFT_BLUE); // s3
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_RED) ;
  tft.drawString("Humi", (tft.width() / 2) - 1 , (tft.height() / 2) + 40 , 1); // Print the test text in the custom font
  tft.setTextColor(TFT_GREEN);
  tft.drawString("%", (tft.width() / 2) + 30, (tft.height() / 2) + 70, 1);
  //c2h5ch Rect
  tft.drawRoundRect(((tft.width() / 2) + (tft.width() / 2) / 2) - 5  , (tft.height() / 2) + 30, (tft.width() / 2) / 2 , (tft.height() - 65) / 2 , 10, TFT_BLUE); // s4
  tft.setFreeFont(&FreeSansBoldOblique9pt7b);
  tft.setTextColor(TFT_RED) ;
  tft.drawString("Ethyl", ((tft.width() / 2) + (tft.width() / 2) / 2)   , (tft.height() / 2) + 40 , 1); // Print the test text in the custom font
  tft.setTextColor(TFT_GREEN);
  tft.drawString("ppm", ((tft.width() / 2) + (tft.width() / 2) / 2) + 30 , (tft.height() / 2) + 90, 1);
    
}
 
void loop() {
  Serial.print("Connecting to ");
  Serial.println(serverName); //print server IP address

  //check whether Wio Terminal is connected to server
  if (!client.connect(serverName, 80)) {  //80 is default port of HTTP
      Serial.println("Connection failed!");
  } else {
      Serial.println("Connected to server!");

  // VOC
  float voc = gas.getGM502B();
  if (voc > 999) voc = 999;
  Serial.print("VOC: ");
  Serial.print(voc);
  Serial.println(" ppm");
  spr.createSprite(40, 30);
  spr.fillSprite(TFT_BLACK);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE);
  spr.drawNumber(voc, 0, 0, 1);
  spr.pushSprite(15, 100);
  spr.deleteSprite();
  //CO
  float co = gas.getGM702B();
  if (co > 999) co = 999;
  Serial.print("CO: ");
  Serial.print(co);
  Serial.println(" ppm");
  spr.createSprite(40, 30);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE);
  spr.drawNumber(co, 0, 0, 1);
  spr.setTextColor(TFT_GREEN);
  spr.pushSprite(15, 185);
  spr.deleteSprite();

  float t = dht.readTemperature(); //Assign variable to store temperature
  float h = dht.readHumidity(); //Assign variable to store humidity

  

  //display sensor data on serial monitor 
  Serial.print("temperature: "); //print string
  Serial.println(t); //print data stored in t
  Serial.print("Humidity:");
  Serial.println(h);

  if (h > 99) h = 99;
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println( "%");
  spr.createSprite(30, 30);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE);
  spr.drawNumber(h, 0, 0, 1);
  spr.pushSprite((tft.width() / 2) - 1, (tft.height() / 2) + 67);
  spr.deleteSprite();
                             
  
  spr.createSprite(30, 30);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE);
  spr.drawNumber(t, 0, 0, 1);
  spr.setTextColor(TFT_GREEN);
  spr.pushSprite((tft.width() / 2) - 1, 100);
  spr.deleteSprite();
  //NO2
  float no2 = gas.getGM102B();
  if (no2 > 999) no2 = 999;
  Serial.print("NO2: ");
  Serial.print(no2);
  Serial.println(" ppm");
  spr.createSprite(45, 30);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE);
  spr.drawNumber(no2, 0, 0, 1);
  spr.pushSprite(((tft.width() / 2) + (tft.width() / 2) / 2), 97);
  spr.deleteSprite();

  //C2H5CH
  float c2h5ch = gas.getGM302B();
  if (c2h5ch > 999) c2h5ch = 999;
  Serial.print("C2H5CH: ");
  Serial.print(c2h5ch);
  Serial.println(" ppm");
  spr.createSprite(45, 30);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);
  spr.setTextColor(TFT_WHITE);
  spr.drawNumber(c2h5ch, 0 , 0, 1);
  spr.pushSprite(((tft.width() / 2) + (tft.width() / 2) / 2), (tft.height() / 2) + 67);
  spr.deleteSprite();



  //data needed to send along with API key
  String tempStr;
  tempStr +="value=";
  tempStr += String(t);
  
  String humiStr;
  humiStr +="value=";
  humiStr += String(h);
  
  String conmgStr;
  conmgStr +="value=";
  conmgStr += String(co);
  
  String nitrgStr;
  nitrgStr +="value=";
  nitrgStr += String(no2);

  String alchoStr;
  alchoStr +="value=";
  alchoStr += String(c2h5ch);
  
  String vocgStr;
  vocgStr +="value=";
  vocgStr += String(voc);

  //make an HTTP POST request
  client.println("POST /api/v2/Ubonagritechno/feeds/temperature/data HTTP/1.1");
  client.println("Host: xxxx.com");
  client.println("Connection: keep-alive");
  client.println("X-AIO-Key: aio_xxxxxxxxxxxxxxxxxxxxx");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(tempStr.length());
  client.println();
  client.println(tempStr);

  //make an HTTP POST request
  client.println("POST /api/v2/Ubonagritechno/feeds/humidity/data HTTP/1.1");
  client.println("Host: xxxx.com");
  client.println("Connection: keep-alive");
  client.println("X-AIO-Key: aio_xxxxxxxxxxxxxxxxxxxxx");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(humiStr.length());
  client.println();
  client.println(humiStr);

  //make an HTTP POST request
  client.println("POST /api/v2/Ubonagritechno/feeds/carbon/data HTTP/1.1");
  client.println("Host: xxxx.com");
  client.println("Connection: keep-alive");
  client.println("X-AIO-Key: aio_xxxxxxxxxxxxxxxxxxxxx");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(conmgStr.length());
  client.println();
  client.println(conmgStr);

  //make an HTTP POST request
  client.println("POST /api/v2/Ubonagritechno/feeds/nitro/data HTTP/1.1");
  client.println("Host: xxxx.com");
  client.println("Connection: keep-alive");
  client.println("X-AIO-Key: aio_xxxxxxxxxxxxxxxxxxxxx");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(nitrgStr.length());
  client.println();
  client.println(nitrgStr);

  //make an HTTP POST request
  client.println("POST /api/v2/Ubonagritechno/feeds/ethy/data HTTP/1.1");
  client.println("Host: xxxx.com");
  client.println("Connection: keep-alive");
  client.println("X-AIO-Key: aio_xxxxxxxxxxxxxxxxxxxxx");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(alchoStr.length());
  client.println();
  client.println(alchoStr);


  //make an HTTP POST request
  client.println("POST /api/v2/Ubonagritechno/feeds/vvocs/data HTTP/1.1");
  client.println("Host: xxxx.com");
  client.println("Connection: close");
  client.println("X-AIO-Key: aio_xxxxxxxxxxxxxxxxxxxxx");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(vocgStr.length());
  client.println();
  client.println(vocgStr);

  
  delay(2000);
  Serial.println("Sent to dashboard IO");
  
  }
  
  client.stop(); //disconnect from server
  //wait 10 seconds to send the next POST request
  Serial.println("Waiting...");
  delay(3000);
}
