#include <DHT.h>
#include <EEPROMex.h> //Библиотека EEPROM

#define NUM_LEDS 20           // количество светодиодов (данная версия поддерживает до 150 штук)
#define TARGET_LEDS 16
#include <FastLED.h>
CRGB leds[NUM_LEDS];


// Uncomment the type of sensor in use:
#define DHTTYPE DHT11     // DHT 11 
#define LED_PIN 12              // пин DI светодиодной ленты 
#define DHTPIN 4         // Pin which is connected to the DHT sensor.
#define FANPIN 10

DHT dht(DHTPIN, DHTTYPE);
#define TIMEOUT 5000    // таймаут соединения, мс

DEFINE_GRADIENT_PALETTE(fanlevel_gp) {
  0,    0,    255,  0,  // green
  120,  255,  255,  0,  // yellow
  125,  255,  50,   0,  // red
  255,  255,  0,    0   // red
};
CRGBPalette32 myPal = fanlevel_gp;

//Other variables
byte FanMode = 1;
int DHT_real_temp_x10; //real temp, multiplied x10 to store double value in integer variable. 34.7 stored like 347
byte real_speed = 0;
byte target_speed;
byte fan_manual;
byte LIGHT_COLOR = 230;             // начальный цвет подсветки
byte LIGHT_SAT = 255;             // начальная насыщенность подсветки
byte COLOR_SPEED = 50;
byte COLOR_SPEED_2 = 50;
int RAINBOW_PERIOD = 1;
float RAINBOW_STEP_2 = 5;
int fan_delay = 1000;
byte breathebrt = 255;
short BRIGHTNESS = 200;        // яркость по умолчанию (0 - 255)
#define HUE_GAP 30      // заброс по hue
#define FIRE_STEP 15    // шаг огня
#define HUE_START 0     // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
#define MIN_BRIGHT 20   // мин. яркость огня
#define MAX_BRIGHT 255  // макс. яркость огня
#define MIN_SAT 240     // мин. насыщенность
#define MAX_SAT 255     // макс. насыщенность
int counter = 0;
byte BreatheTrigger = false;
byte this_mode = 2;
float rainbow_steps;
int this_color;
float fan_index = (float)255/TARGET_LEDS;
byte HYSTERESIS = 15;
bool LEDModeChanged,LEDStartup;
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
unsigned long color_timer; 
unsigned long rainbow_timer; 
unsigned long FAN_timer; 
unsigned long FAN_ADJUST_timer; 
// ленивая жопа
#define FOR_i(from, to) for(int i = (from); i < (to); i++)
// парсинг
boolean onlineFlag;
uint32_t timeoutTimer;
char inData[82];       // массив входных значений (СИМВОЛЫ)
int PCdata[20];        // массив численных значений показаний с компьютера
byte index = 0;
String string_convert;

int smartIncr(int value, int incr_step, int mininmum, int maximum) {
  int val_buf = value + incr_step;
  val_buf = constrain(val_buf, mininmum, maximum);
  return val_buf;
}

void setup() {
  //Hardware tweaks
  TCCR1A = 0b00000001;  // 8bit
  TCCR1B = 0b00001101;  // x8 fast pwm
  Serial.begin(9600);
  dht.begin();
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  Serial.println("ProjectCool");
  Serial.println  ("by SonikPr");
  Serial.println  ("Driver Ver: 1.0"); 
  Serial.println  ("Software ver: 4.0");
  
  //Software tweaks and testing
  Startup(0);
  TestFans();
  readEEPROM();
  Startup(TestDHT());
  crossFader(this_mode);
  updateFans();
  LEDStartup = true;
}

int TestDHT(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    FanMode = 5;
    return 1;
  }
  else
  return 2;
}

void TestFans(){
  analogWrite(FANPIN, 255); //Set it to full-speed
  delay(50);
  for(int i = 255; i >= 0 ; i--){
    analogWrite(FANPIN, i);
    delay(10);
  }
}

int STARTLEDS;
void Startup(int displayment){
  CHSV feedback_color;
  byte delay_time;
  byte repeats = 1;
  switch(displayment){
    case 0:
    default:
    feedback_color = CHSV(HUE_PURPLE, 255, 200);
    delay_time = 30;
    break;
    case 1:
    feedback_color = CHSV(HUE_RED, 255, 200);
    delay_time = 50;
    repeats = 5;
    break;
    case 2:
    feedback_color = CHSV(HUE_GREEN, 255, 200);
    delay_time = 10;
    break;
    
  }
  for(byte i = 0; i< repeats; i++){
    for(int i = 0; i <= NUM_LEDS; i++){
      leds[i] = feedback_color;
      FastLED.show();
      delay(delay_time);
    }
    delay(delay_time);
    for(int i = 0; i <= NUM_LEDS; i++){
      leds[i] = CHSV(HUE_BLUE, 255, 0);
      FastLED.show();
      delay(delay_time);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  FANS();
  ARGB();
  parsing();
  if(!LEDModeChanged){
         crossfader_helper();
         }
}
