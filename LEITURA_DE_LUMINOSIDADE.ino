/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// WORKING WITH ESP32 3.0.0 LIBRARY UPDATE - 04/06/2024 - LASTONE CODE VERSION
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Example sketch for the BH1750_WE library
* Mode selection / abbreviations:
* CHM:    Continuously H-Resolution Mode
* CHM_2:  Continuously H-Resolution Mode2
* CLM:    Continuously L-Resolution Mode
* OTH:    One Time H-Resolution Mode
* OTH_2:  One Time H-Resolution Mode2
* OTL:    One Time L-Resolution Mode
* Measuring time factor:
* 1.0 ==> Measuring Time Register = 69
* 0.45 ==> Measuring Time Register = 31
* 3.68 ==> Mesuring Time Register = 254
* Other implemented functions, not used in this example:
* resetDataReg() --> resets Data Register
* powerOn() --> Wake Up!
* powerDown() --> Sleep well, my BH1750
* If you change the measuring time factor for calibration purposes, 
* then you need to divide the light intensity by the measuring time factor.
* More information can be found on:
* https://wolles-elektronikkiste.de/en/bh1750fvi-gy-30-302-ambient-light-sensor
* or in German:
* https://wolles-elektronikkiste.de/bh1750fvi-lichtsensormodul
***************************************************************************/
/*Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
$servername = "localhost";
// REPLACE with your Database name
$dbname = "luminarias";
// REPLACE with Database user
$username = "pi";
// REPLACE with Database user password
$password = "raspberry";
// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "tPmAT5Ab3j7F9";
$api_key = $lux = $chipID = ""; */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#include    <WiFi.h>
#include    <WiFiClientSecure.h>
#include    <HTTPClient.h>
#include    <Wire.h>
#include    <BH1750_WE.h>
#define     BH1750_ADDRESS 0x23
BH1750_WE   myBH1750 = BH1750_WE(BH1750_ADDRESS); 
// You may also pass a TwoWire object: 
//BH1750_WE myBH1750 = BH1750_WE(&Wire, BH1750_ADDRESS);
// If you don't pass any parameter, Wire and 0x23 will be applied
// Replace with your network credentials
const char* ssid     = "raspiAP";
const char* password = "raspberry";
// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://10.0.0.220/lux/post_lux_data.php";
// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String      apiKeyValue = "tPmAT5Ab3j7F9ContiCelesc";
#define     SERIAL_BAUDRATE   115200
uint32_t    lastUpdate = 0;
uint32_t    chipID = 0;
// DO NOT FORGET TO CHANGE TIME HERE!!!!!!!!!!!
uint16_t    interval = 5000;  // 30000;   // 600000;  // PRODUCTION CHANGE ME!!!
// LUX GLOBAL VARIABLES
uint16_t    luxReadMax = 65535, luxReadMin = 0, luxRead;
String      luxReadStr = "NULL";
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void showChipID(void){
for(int i = 0; i < 17; i = i + 8) {
	  chipID |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;}
Serial.printf("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
Serial.printf("This chip has %d cores\n", ESP.getChipCores());
 Serial.print("Chip ID: "); Serial.println(chipID, HEX); }
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void setup(){
  Serial.begin(SERIAL_BAUDRATE);
  vTaskDelay(2500);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    vTaskDelay(500);
    Serial.print(".");}
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
// SHOW CHIP ID - EPS32
  showChipID();
  Wire.begin();
  if(!myBH1750.init()){ // sets default values: mode = CHM, measuring time factor = 1.0
    Serial.println("Connection to the BH1750 failed");
    Serial.println("Check wiring and I2C address");
    while(1){}}
  else{ Serial.println("BH1750 is connected");}
// myBH1750.setMeasuringTimeFactor(0.45); // uncomment for selection of value between 0.45 and 3.68
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void loop() {
// START SEND DATA TO SERVER - RPi
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (millis() - lastUpdate >= interval) { // NON BLOCKING CODE - 5000 mS
    lastUpdate += interval;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if(myBH1750.init()){
// NEW LUX READINGS
      myBH1750.setMode(OTH); // sets mode and starts measurement
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* An OTH and OTH_2 measurement takes ~120 ms. I suggest to wait 
     140 ms to be on the safe side. 
     An OTL measurement takes about 16 ms. I suggest to wait 20 ms
     to be on the safe side. */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  vTaskDelay(200); // wait for measurement to be completed, change for OTL
  luxRead = (uint16_t)myBH1750.getLux();
  Serial.print(F("Light intensity: "));
  Serial.print(luxRead);
  Serial.println(F(" Lux"));
  luxReadStr = String(luxRead);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
if(luxRead < luxReadMin || luxRead > luxReadMax){
    Serial.println("ERRO NA MEDIDA");
    luxReadStr = "NULL";}
    Serial.println(luxReadStr);}
    else{luxReadStr = "NULL";}
// REALLY SEND DATA TO SERVER  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    // WiFiClientSecure *client = new WiFiClientSecure;
    // client->setInsecure(); // don't use SSL certificate??????
    // HTTPClient https;
    WiFiClient client;
    HTTPClient http;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Your Domain name with URL path or IP address with path
    // https.begin(*client, serverName);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    http.begin(client, serverName);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // https.addHeader("Content-Type", "application/x-www-form-urlencoded");
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Prepare your HTTP POST request data
    String httpRequestData =  "api_key=" + apiKeyValue + 
                              "&lux=" + luxReadStr +
                              "&chipID=" + String(chipID, HEX) + "";
    Serial.print("httpRequestData: "); Serial.println(httpRequestData);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// You can comment the httpRequestData variable above
// then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
// String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Send HTTP POST request
//  int httpResponseCode = https.POST(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// If you need an HTTP request with a content type: text/plain
// https.addHeader("Content-Type", "text/plain");
// int httpResponseCode = https.POST("Hello, World!");
// If you need an HTTP request with a content type: application/json, use the following:
// https.addHeader("Content-Type", "application/json");
// int httpResponseCode = https.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode); }
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    else {  Serial.print("Error code: ");
            Serial.println(httpResponseCode);}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Free resources // https.end();}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    http.end();}
  else { Serial.println("WiFi Disconnected"); }
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
}   // END --> if (millis) - SEND DATA TO SERVER - RPi
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Send an HTTP POST request every 30 seconds
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
