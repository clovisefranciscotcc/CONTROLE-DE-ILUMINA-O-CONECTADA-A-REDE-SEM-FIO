
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
//					CODIGO PARA LEITURA DE LUMINOSIDADE
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include    <WiFi.h>
#include    <WiFiClientSecure.h>
#include    <HTTPClient.h>
#include    <Wire.h>
#include    <BH1750_WE.h>
#define     BH1750_ADDRESS 0x23
BH1750_WE   myBH1750 = BH1750_WE(BH1750_ADDRESS); 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
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
uint16_t    interval = 5000;  // PRODUCTION CHANGE ME!!!
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
  vTaskDelay(200); // wait for measurement to be completed
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
    WiFiClient client;
    HTTPClient http;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Your Domain name with URL path or IP address with path
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    http.begin(client, serverName);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Prepare your HTTP POST request data
    String httpRequestData =  "api_key=" + apiKeyValue + 
                              "&lux=" + luxReadStr +
                              "&chipID=" + String(chipID, HEX) + "";
    Serial.print("httpRequestData: "); Serial.println(httpRequestData);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Send HTTP POST request
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    int httpResponseCode = http.POST(httpRequestData);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode); }
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    else {  Serial.print("Error code: ");
            Serial.println(httpResponseCode);}
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