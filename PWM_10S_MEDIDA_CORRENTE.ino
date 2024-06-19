// CODIGO PARA TESTE DE CORRENTE EM FUNCAO DO PWM A CADA 10 SEGUNDOS

const uint8_t     PWM_PIN		= 14;
const uint16_t	  LEDC_BASE_FREQ = 2000;		// MAX FREQ CONV PWM TO ANALOG
const uint8_t     LEDC_TIMER_RES_BIT = 8;		// 8 ~ 16 bits

void setup() {
  Serial.begin(115200);
  while (!Serial) { vTaskDelay(1000); }
  ledcAttach(PWM_PIN, LEDC_BASE_FREQ, LEDC_TIMER_RES_BIT);
  Serial.println("AJUSTA setPWMto[dutyCycle] PARA MEDIR CORRENTE");}
uint8_t   setPWMto[] = {   13,   26,   38,   51,   64,
                           77,   89,  102,  115,  128,
                          140,  153,  166,  179,  191,
                          204,  217,  230,  242,  255};
void loop() {
for (int dutyCycle = 0; dutyCycle < 20; dutyCycle++){
  ledcWrite(PWM_PIN, setPWMto[dutyCycle]);
  Serial.println("|> AJUSTA setPWMto[dutyCycle] PARA MEDIR CORRENTE");
    Serial.print("|> setPWMto[dutyCycle]: "); Serial.println(setPWMto[dutyCycle]);
  vTaskDelay(10000);}}