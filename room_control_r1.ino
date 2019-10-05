/************************************************* ************************************************** ******
* OPEN-SMART Rich Shield Lesson 7: Temperature measurement and display
* Through the NTC temperature measurement module to obtain the temperature, and display on the digital tube. 
*
* The following functions are available:
* temper.getTemperature(); // get the temperature value, and return float-point number 
*
* displayTemperature(int8_t temperature); // display temperature, at the end of the value of the word added C is the degree of Celsius
*
* disp.init(); // initialization
* disp.display(int8_t DispData []); // display the numbers in the array
* // For example, the array is {1,2,3,4}, then show 1234
************************************************** ************************************************** *****/

#include <Wire.h>
#include "RichShieldLED.h"
#include "RichShieldTM1637.h"
#include "RichShieldNTC.h"
#include "RichShieldKnob.h"
#include "RichShieldKEY.h"

#define KNOB_PIN A0//SIG pin of Rotary Angle Sensor module connect to A0 of IO Shield, that is pin A0 of OPEN-SMART UNO R3
Knob knob(KNOB_PIN);

#define NTC_PIN A1 //SIG pin of NTC module connect to A1 of IO Shield, that is pin A1 of OPEN-SMART UNO R3
NTC temper(NTC_PIN);  

#define CLK 10//CLK of the TM1637 IC connect to D10 of OPEN-SMART UNO R3
#define DIO 11//DIO of the TM1637 IC connect to D11 of OPEN-SMART UNO R3
TM1637 disp(CLK,DIO);

#define LED1 7
#define LED2 6
#define LED3 5
#define LED4 4
LED led(LED1,LED2,LED3,LED4);

int angle; //on angle, off 20 minute
unsigned long old_tick;
int relay_sts = 0;
int max_temp = 30;

#define KEY1_PIN 9//
#define KEY2_PIN 8//
Key key(KEY1_PIN,KEY2_PIN);

unsigned long key_tick;

void relay_on(){
  led.on(3);
  relay_sts = 1;
  old_tick = millis();
}
void relay_off(){
  led.off(3);
  relay_sts = 0;
  old_tick = millis();
}
void setup()
{
  Serial.begin(9600);  
  disp.init();//The initialization of the display
//  old_tick = millis();
  relay_off();
  key_tick = 0;
  delay(100);//
}

void loop()
{
  float celsius;
  unsigned long this_tick;
  int keynum;
  int sec;
  static int old_sec = 0;
  static int limit_loop = 0;
  
  this_tick = millis();
  sec = (this_tick-old_tick)/1000;
  celsius = temper.getTemperature();//get temperature
  angle = knob.getAngle()/4;
  angle = min(60, max(1, angle));
  delay(20);//delay 1000ms
  
#if 1  
  keynum = key.get();
  if (keynum){
    led.on(1);
    delay(20);//delay 1000ms
    if (this_tick < key_tick){
      if(keynum == 2) //if you press K1
      {
        max_temp++;
      }
      else if(keynum == 1)//if you press K2
      {
        max_temp--;
      }
      max_temp = min(32, max(27, max_temp));  
    }
    displayTemperature(max_temp, angle);
    while(key.get());
    key_tick = this_tick + 5 * 1000;
  }
#endif
  if (sec > old_sec){
    if (this_tick > key_tick){
      led.off(1);
      displayTemperature((int8_t)celsius, angle);//
    }
    Serial.print("status : ");
    Serial.print(relay_sts);
    Serial.print(", time : ");
    Serial.println(sec);
    if (relay_sts == 0){
      if (sec > (angle  * 60)){ // * 60
        relay_on();
      }
    }
    else{
      if (sec > (20 * 60)){ // test, org 20  // * 60
        relay_off();
      }
    }
    if ((int8_t)celsius >= max_temp){
      if (limit_loop > 10){
        led.on(4);
        limit_loop = 0;
      }
      led.on(3);
    }
    else{
      led.off(4);
    }
    limit_loop++;
    old_sec = sec;
  }
}
/************************************************* *********************/
/* Function: Display temperature on 4-digit digital tube */
/* Parameter: -int8_t temperature, temperature range is -40 ~ 125 degrees celsius */
/* Return Value: void */

void displayTemperature(int8_t temperature, int angle)
{
  int8_t temp[4];
  temp[2] = angle / 10;  
  temp[3] = angle % 10;  
  temp[0] = temperature / 10;
  temp[1] = temperature % 10;
  disp.display(temp);
}
/*********************************************************************************************************
The end of file
*********************************************************************************************************/
