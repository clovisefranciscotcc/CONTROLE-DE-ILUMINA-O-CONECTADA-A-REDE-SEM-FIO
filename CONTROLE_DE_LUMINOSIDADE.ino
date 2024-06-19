
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
//					CODIGO PARA CONTROLE DE LUMINOSIDADE
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include            <WiFi.h>
#include            <HTTPClient.h>
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// SERIAL COMMUNICATION BAUDRATE IN 'bps'
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
const uint32_t      SERIAL_BAUDRATE = 115200;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Replace with your network credentials
const char*         ssid     = "raspiAP";
const char*         password = "raspberry";
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// REPLACE with your Domain name and URL path or IP address with path
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
String              serverAddress   = "10.0.0.220";   // WLAN CATOLICA LAB MCU
String              serverFolder    = "lux";
String              serverFile      = "send_data_to_esp.php";
String              apiServerPath   = "";   
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
uint16_t            luxAmbientRecebido = 0;
uint16_t            ambienteLuxABNT = 0;
uint16_t            recLux = 0;
uint32_t            chipID = 0;
int8_t              ini;  // = 19; // CONTROL PWM INDEX ACCORDING LUX RECEIVED
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Variables used TO TIME
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
uint32_t            Actual_Millis, Previous_Millis;
uint16_t            refresh_time = 30000; // 60000;   // 1 MINUTO
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// Refresh rate of connection to RPi SERVER
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// AMBIENTE TERMINAIS DE ENTRADA DE SELECAO
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
const uint8_t       estoqueLux_PIN = 17; // estoqueLux
const uint8_t       linhaProducaoLux_PIN = 18; // linhaProducaoLux
const uint8_t       escritorioLux_PIN = 19; // escritorioLux
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// VEM DO INTERRUPTOR DE SELECAO NA LUMINARIA
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
const uint16_t      estoqueLux = 100;
const uint16_t      linhaProducaoLux = 300;
const uint16_t      escritorioLux = 500;
const String        ambientes[] = { "estoque Lux 100", 
                                    "linhaProducao Lux 300", 
                                    "escritorio Lux 500"};
uint8_t             amb_id = 250;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// PWM PINOUT AND SETTINGS
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
const uint8_t       PWM_PIN	= 14;
const uint16_t	    PWM_BASE_FREQ = 2000;		// MAX FREQ CONV PWM TO ANALOG
const uint8_t       PWM_TIMER_RES_BIT = 8;		// 8 ~ 16 bits
uint8_t             ajustaPWMpara = 19;              // VALOR A SER RECEBIDO
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// PWM RAZAO CICLICA TABELA CONVERSAO
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
uint8_t   setPWMto[] = {   13,   26,   38,   51,   64,
                           77,   89,  102,  115,  128,
                          140,  153,  166,  179,  191,
                          204,  217,  230,  242,  255};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
String serverAddFldrFile(void){
apiServerPath = "http://" + serverAddress + '/' + serverFolder + '/' + serverFile;
return (apiServerPath);}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// FUNCTION - void ajustaPWM(uint8_t dutyCycle)
// bool ledcWrite(uint8_t pin, uint32_t duty);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void ajustaPWM(uint8_t dutyCycle){
ledcWrite(PWM_PIN, dutyCycle);
Serial.printf("|>---<| AJUSTADO PWM com RAZAO CICLICA de: %d\n", dutyCycle);
vTaskDelay(500);}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// FUNCTION - void showChipID(void)
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void showChipID(void){
for(int i = 0; i < 17; i = i + 8) {
	  chipID |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;}
Serial.printf("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
Serial.printf("This chip has %d cores\n", ESP.getChipCores());
 Serial.print("Chip ID: "); Serial.println(chipID, HEX); }
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// FUNCTION - uint16_t buscaLuxRPi()
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
uint16_t buscaLuxRPi(void){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverAddFldrFile());    // serverAddress);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      recLux = payload.toInt();
      Serial.print("LUX Recebido PHP & RPi: ");
      Serial.println(payload);
    } else {Serial.println("Falha ao conectar ao servidor PHP.");}
    http.end();}
return recLux;}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// FUNCTION - void conectaWiFi(void)
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void conectaWiFi(void){
  Serial.println("Conectando ao WiFi...");
// CONEXAO WLAN - 10.0.0.220 - RPi
  WiFi.begin(ssid, password);             //Start wifi connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    vTaskDelay(750);
    Serial.print(". ");}
  Serial.print("\nWiFi Connected, my IP: ");
  Serial.println(WiFi.localIP());}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// FUNCTION - SELECAO selecaoInterruptor --> VERIFICA SELECAO DE AMBIENTE
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
uint16_t ambienteInterruptor(){
Serial.print("ESTADO estoqueLux_PIN = "); 
    Serial.println(digitalRead(estoqueLux_PIN));
Serial.print("ESTADO linhaProducaoLux_PIN = ");
    Serial.println(digitalRead(linhaProducaoLux_PIN));
Serial.print("ESTADO escritorioLux_PIN = ");
    Serial.println(digitalRead(escritorioLux_PIN));
  if (  (digitalRead(estoqueLux_PIN) == 1) &&
        (digitalRead(linhaProducaoLux_PIN) == 1) && 
        (digitalRead(escritorioLux_PIN) == 1)){
  Serial.println("SELECAO INCORRETA - AJUSTAR O AMBIENTE ADEQUADAMENTE E REINICIAR");
    ambienteLuxABNT = 0;
    return 0;}
  if (  (digitalRead(estoqueLux_PIN) == 0) && 
        (digitalRead(linhaProducaoLux_PIN) == 1) && 
        (digitalRead(escritorioLux_PIN) == 1)){
    ambienteLuxABNT = estoqueLux;            // 100;
    amb_id = 0;}
  if (  (digitalRead(estoqueLux_PIN) == 1) && 
        (digitalRead(linhaProducaoLux_PIN) == 0) && 
        (digitalRead(escritorioLux_PIN) == 1)){
    ambienteLuxABNT = linhaProducaoLux;      // 300;
    amb_id = 1;}
  if (  (digitalRead(estoqueLux_PIN) == 1) && 
        (digitalRead(linhaProducaoLux_PIN) == 1) && 
        (digitalRead(escritorioLux_PIN) == 0)){
    ambienteLuxABNT = escritorioLux;         // 500;
    amb_id = 2;}
    Serial.print("AMBIENTE SELECIONADO: ");
      Serial.println(ambientes[amb_id]);
    Serial.print("ambienteLuxABNT >>> "); 
      Serial.println(ambienteLuxABNT);
    return ambienteLuxABNT;}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// SEQUENCIA TIPO INCREMENTO DE DEGRAU PARA AJUSTE 
// DE LUMINOSIDADE NA LUMINARIA POR PWM
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void controleLuminaria(uint16_t  luxAmbientRecebido){
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
while(luxAmbientRecebido > (uint16_t)ambienteLuxABNT * 1.1){  // 10%
    ajustaPWM(setPWMto[ini--]);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
Serial.println("\n\n\n | >>> DESCENDO...");
Serial.print("¹ AMBIENTE SELECIONADO = ");      Serial.println(ambientes[amb_id]);
Serial.print("¹ luxAmbientRecebido = ");        Serial.println(luxAmbientRecebido);
Serial.print("¹ ambienteLuxABNT = ");           Serial.println(ambienteLuxABNT);
Serial.print("¹ setPWMto[ini] = ");             Serial.println(setPWMto[ini]);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    vTaskDelay(5000);
    luxAmbientRecebido = buscaLuxRPi();
    vTaskDelay(1000);
      if(ini < 0){ ini = 0; break;}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      if(luxAmbientRecebido < ambienteLuxABNT){break;}}     // END WHILE "SUBINDO"
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
while(luxAmbientRecebido < (uint16_t)ambienteLuxABNT){    // * 0.95){
    ajustaPWM(setPWMto[ini++]);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
Serial.println("\n\n\n | >>> SUBINDO...");
Serial.print("^ AMBIENTE SELECIONADO = ");      Serial.println(ambientes[amb_id]);
Serial.print("^ luxAmbientRecebido = ");        Serial.println(luxAmbientRecebido);
Serial.print("^ ambienteLuxABNT = ");           Serial.println(ambienteLuxABNT);
Serial.print("^ setPWMto[ini] = ");             Serial.println(setPWMto[ini]);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    vTaskDelay(5000);
    luxAmbientRecebido = buscaLuxRPi();
    vTaskDelay(1000);
    if(ini > 19){ini = 19; break;}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      if(luxAmbientRecebido > ambienteLuxABNT){break;}} // END WHILE "DESCENDO"


} // END FUNCTION
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// FUNCTION - void setup(void)
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void setup(void) {
  Serial.begin(SERIAL_BAUDRATE);
  vTaskDelay(2500);
  Serial.println("CONTROLE LUMINARIA - CLOVIS & FRANCISCO");
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ledcAttach(PWM_PIN, PWM_BASE_FREQ, PWM_TIMER_RES_BIT);
// INTERRUPTORES SELECAO AMBIENTE
  pinMode(estoqueLux_PIN,         INPUT_PULLUP);
  pinMode(linhaProducaoLux_PIN,   INPUT_PULLUP);
  pinMode(escritorioLux_PIN,      INPUT_PULLUP);
// VERIFICA SELECAO DE AMBIENTE ANTES DE MAIS NADA!!!
  uint16_t ambienteSelecionado = ambienteInterruptor();
  while (ambienteSelecionado == 0) {
    Serial.println("SELECAO INCORRETA do AMBIENTE");
    Serial.println("AJUSTAR O AMBIENTE ADEQUADAMENTE E REINICIAR");
    vTaskDelay(30000);}
  Serial.print("ambienteSelecionado = "); Serial.println(ambienteSelecionado);
// LEITURA CHIPID DO ESP32
  showChipID();
// CONECTA AO WIFI DO RPi
  conectaWiFi();
  buscaLuxRPi();}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// FUNCTION - void loop(void)
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void loop(void) {
  Actual_Millis = millis();
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(Actual_Millis - Previous_Millis > refresh_time){ // 30000 mS
    Previous_Millis = Actual_Millis;  
      if(WiFi.status()== WL_CONNECTED){       // Check WiFi connection status  
        Serial.println("\nREPEATED - Controle luminarias");
        Serial.print("AMBIENTE SELECIONADO: ");
        Serial.println(ambientes[amb_id]);
        controleLuminaria(buscaLuxRPi());
// LUMINOSIDADE ESTABILIZADA DE ACORDO COM ABNT
        // SEND DATA TO SERVER

// NO CODIGO DO LUX SOLICITAR INTERVALO ENTRE MEDIDAS PARA O SERVIDOR
// DEPOIS DE ESTABILIZADA A LUMINOSIDADE AUMENTA-SE O TEMPO ENTRE MEDIDAS E ENVIOS DE LUX
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   } else { Serial.println("WIFI connection error");
            Serial.println("WIFI REconnection");
            conectaWiFi();}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  } // END REFRESH TIME
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
} // END LOOP
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
