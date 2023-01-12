#define MIN_FAN_SPEED 20

//------------------------Temps for mode 0 (default), multiplied to x10 to represent 34.7 like 347 to boost resolution net
#define TEMPMAX1 400
#define TEMPMIN1 300
//-------------------------
//------------------------Temps for mode 1 (cool)
#define TEMPMAX2 335
#define TEMPMIN2 300
//-------------------------
//------------------------Temps for mode 2 (quiet)
#define TEMPMAX3 450
#define TEMPMIN3 300
//-------------------------
 //Hysteresis temp overdrive

byte fan_target_speed(int temp, int temp_min, int temp_max, byte speed_min, byte speed_max){
  return map(constrain(temp, temp_min, temp_max), temp_min, temp_max, map(speed_min, 0, 100, 0, 255), map(speed_max, 0, 100, 0, 255));
}


bool allow_rpm_decrease = false;
int increasing_temp;
void FANS(){
  if (millis() - FAN_timer >= fan_delay) {
            FAN_timer = millis();
            DHT_real_temp_x10 = (int)(dht.readTemperature()*10);
            switch(FanMode){          
              case 0: //Default fan curve
              target_speed = fan_target_speed(DHT_real_temp_x10, TEMPMIN1, TEMPMAX1, MIN_FAN_SPEED, 100);
              break;
              case 1: //Cool fan curve
              target_speed = fan_target_speed(DHT_real_temp_x10, TEMPMIN2, TEMPMAX2, MIN_FAN_SPEED, 100);
              break;
              case 2: //Quiet fan curve
              target_speed = fan_target_speed(DHT_real_temp_x10, TEMPMIN3, TEMPMAX3, 0, 100);
              break;
              case 3: //Fan OFF
              target_speed = 0;
              allow_rpm_decrease = true;
              break;
              case 4: //Fan MANUAL
              target_speed = map(constrain(fan_manual, 0, 100), 0, 100, 0, 255);
              allow_rpm_decrease = true;
              break;
              case 5: //Fan 100%
              target_speed = 255;
              allow_rpm_decrease = true;
              break;
            }
            if(increasing_temp - DHT_real_temp_x10 >= HYSTERESIS)allow_rpm_decrease = true;
            }
  
  
   if (millis() - FAN_ADJUST_timer >= 50) {
            FAN_ADJUST_timer = millis();
            if(real_speed < target_speed){
              analogWrite(FANPIN, real_speed++);
              increasing_temp = DHT_real_temp_x10;
              allow_rpm_decrease = false;
            }
            else
            if(real_speed > target_speed && allow_rpm_decrease){
              analogWrite(FANPIN, real_speed--);
            }
   }
}

void updateFans(){
  allow_rpm_decrease = true;
}
