byte count;
void ARGB(){
  switch (this_mode) {
        case 0:
        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(LIGHT_COLOR, LIGHT_SAT, 255);
          break;
        case 1:
          if (millis() - color_timer > COLOR_SPEED) {
            color_timer = millis();
            if (++this_color > 255) this_color = 0;
          }
          for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(this_color, LIGHT_SAT, 255);
          break;
        case 2:
          if (millis() - rainbow_timer > 30) {
            rainbow_timer = millis();
            this_color += RAINBOW_PERIOD;
            if (this_color > 255) this_color = 0;
            if (this_color < 0) this_color = 255;
          }
          rainbow_steps = this_color;
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CHSV((int)floor(rainbow_steps), 255, 255);
            rainbow_steps += RAINBOW_STEP_2;
            if (rainbow_steps > 255) rainbow_steps = 0;
            if (rainbow_steps < 0) rainbow_steps = 255;
          }
          break;
          case 3: 
          if (millis() - color_timer > COLOR_SPEED_2) {
            color_timer = millis();
            if(breathebrt == BRIGHTNESS){
            BreatheTrigger = false;        
           }
           else
           if(breathebrt == 0){
            BreatheTrigger = true;
           } 
           //Brightness acurator. Да, костылями, знаю, но ведь работает же)
            if(BreatheTrigger == false){
              FastLED.setBrightness(breathebrt--);
              for (int i = 0; i < NUM_LEDS; i++){ 
            leds[i] = CHSV(LIGHT_COLOR, LIGHT_SAT, 255);
            }
           }
           else
           if(BreatheTrigger == true){
            FastLED.setBrightness(breathebrt++);
            for (int i = 0; i < NUM_LEDS; i++){ 
            leds[i] = CHSV(LIGHT_COLOR, LIGHT_SAT, 255);
            }
           } 
          }
         break;
         case 4:
         static uint32_t prevTime;
        if (millis() - prevTime > 20) {
        prevTime = millis();
        int thisPos = 0, lastPos = 0;
        FOR_i(0, NUM_LEDS) {
        leds[i] = getFireColor((inoise8(i * FIRE_STEP, counter)));
        }
        counter += 20;
        }
         break;
         case 5:
            for(int i = NUM_LEDS; i >= 0; i--){
            leds[i] = CHSV(HUE_BLUE, 0, 0);
            }
            count = 0;
            for (int i = 0 ; i <= map(constrain(real_speed, 60, 255), 60, 255, 0, TARGET_LEDS)  ; i++) {
            leds[i] = CHSV(this_color, LIGHT_SAT, 255);
            leds[i] = ColorFromPalette(myPal, (count * fan_index));   // заливка по палитре " от зелёного к красному"
            count++;
          }
         break;
         default:
          for(int i = NUM_LEDS; i >= 0; i--){
          leds[i] = CHSV(HUE_PURPLE, 255, 200);
          FastLED.show();
          }
         break;
  }

        FastLED.show();
}

byte MC = 0, MC_BRT = 0;
byte new_mode = 0;
bool LED_OFF = false;
unsigned long LEDchanger_timer;
void crossFader(byte crossMode){
      //----------------------------
      LED_OFF = false;
      LEDModeChanged = false;
      MC = BRIGHTNESS;
      MC_BRT = map(BRIGHTNESS, 0, 255, 1, 10);
      new_mode = crossMode;
      //----------------------------
}

void crossfader_helper(){
  if(millis() - LEDchanger_timer >= 15){
    LEDchanger_timer = millis();

           if(LEDStartup){
            LEDStartup = false;
            MC = 0;
           }
           if(MC == 0){
            LED_OFF = true;
            this_mode = new_mode;
           } 
            if(!LED_OFF){
              FastLED.setBrightness(MC = smartIncr(MC, -MC_BRT, 0, BRIGHTNESS));
              FastLED.show();
            }
           else
           if(LED_OFF){
            if(MC != BRIGHTNESS){
            FastLED.setBrightness(MC = smartIncr(MC, MC_BRT, 0, BRIGHTNESS));
            FastLED.show();
            }
            else
            if(MC == BRIGHTNESS){
            LEDModeChanged = true;
            updateEEPROM();
           }
           }           
  }
}



CHSV getFireColor(int val) { //For flame mode
  return CHSV(
           HUE_START + map(val, 0, 255, 0, HUE_GAP),                    // H
           constrain(map(val, 0, 255, MAX_SAT, MIN_SAT), 0, 255),       // S
           constrain(map(val, 0, 255, MIN_BRIGHT, MAX_BRIGHT), 0, 255)  // V
         );
}
