void parsing() {

  if (onlineFlag && millis() - timeoutTimer > TIMEOUT) {
    onlineFlag = false;
    /*
    for(int i=0; i<20; i++){
    Serial.println(PCdata[i]);
  }
  */
  }
  
  while (Serial.available() > 0) {
    char aChar = Serial.read();
    if(aChar == 'G'){
      FillArray();
      SendData();
    }
    else
    if (aChar != 'E') {
      inData[index] = aChar;
      index++;
      inData[index] = '\0';
    } else {
      char *p = inData;
      char *str;
      index = 0;
      String value = "";
      while ((str = strtok_r(p, ";", &p)) != NULL) {
        string_convert = str;
        PCdata[index] = string_convert.toInt();
        index++;
      }
      index = 0;
      timeoutTimer = millis();
      onlineFlag = true;
      ApplyChanges();
      Serial.println("OK");
    }
  }

  //Debug()
 
  
}

void updateEEPROM() {
  EEPROM.updateByte(1, FanMode);
  EEPROM.updateByte(2, fan_manual);
  EEPROM.updateByte(3, this_mode);
  EEPROM.updateInt(4, BRIGHTNESS);
  EEPROM.updateByte(8, LIGHT_COLOR);
  EEPROM.updateInt(12, LIGHT_SAT);
  EEPROM.updateByte(16, COLOR_SPEED);
  EEPROM.updateByte(17, COLOR_SPEED_2);
  EEPROM.updateByte(18, HYSTERESIS);
}

void readEEPROM() {
  FanMode = EEPROM.readByte(1);
  fan_manual = EEPROM.readByte(2);
  this_mode = EEPROM.readByte(3);
  BRIGHTNESS = EEPROM.readInt(4); 
  LIGHT_COLOR = EEPROM.readByte(8);
  LIGHT_SAT = EEPROM.readInt(12);
  COLOR_SPEED = EEPROM.readByte(16);
  COLOR_SPEED_2 = EEPROM.readByte(17);
  HYSTERESIS = EEPROM.readByte(18);
}

void FillArray(){
  readEEPROM();
  PCdata[0] = FanMode; //Режим обдува
  PCdata[1] = fan_manual; //Ручная скорость вертушек
  PCdata[2] = this_mode; //режим подсветки
  PCdata[3] = BRIGHTNESS; 
  PCdata[4] = LIGHT_COLOR;
  PCdata[5] = LIGHT_SAT;
  PCdata[6] = COLOR_SPEED;
  PCdata[7] = COLOR_SPEED_2;
  PCdata[8] = HYSTERESIS;
  PCdata[9] = map(real_speed, 0, 255, 0, 100);
  PCdata[10] = (int)(dht.readTemperature()*10);
  PCdata[11] = (int)(dht.readHumidity()*10);
  
}

void SendData()
{
  for(int i=0; i<13; i++){
      Serial.print(PCdata[i]);
      Serial.print(";");
  }
  Serial.println();
  }


void ApplyChanges(){
  FanMode = PCdata[0];
  fan_manual = PCdata[1];
  crossFader((byte)PCdata[2]);
  BRIGHTNESS = PCdata[3];
  LIGHT_COLOR = PCdata[4];
  LIGHT_SAT = PCdata[5];
  COLOR_SPEED = PCdata[6];
  COLOR_SPEED_2 = PCdata[7];
  HYSTERESIS = PCdata[8];
  FastLED.setBrightness(BRIGHTNESS);
  updateFans();
  updateEEPROM();
}


//1;200;3;255;180;255;70;40;E Queue example
//0;200;4;255;180;255;70;40;67;34;10; reply example

// PCdata array contains
/* FanMode
    FanManual
    Backlight_color
    LightColor
    LightSat
    Color_Speed
    Color_speed_2 (дыхание)
    Fan_delay
    Brightness
 * 
 */
