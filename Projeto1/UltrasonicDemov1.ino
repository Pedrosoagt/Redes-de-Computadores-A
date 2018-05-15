/*
 * HCSR04Ultrasonic/examples/UltrasonicDemo/UltrasonicDemo.pde
 *
 * SVN Keywords
 * ----------------------------------
 * $Author: cnobile $
 * $Date: 2011-09-17 02:43:12 -0400 (Sat, 17 Sep 2011) $
 * $Revision: 29 $
 * ----------------------------------
 */

#include <Ultrasonic.h>

#define TRIGGER_PIN  6
#define ECHO_PIN     7

#define TRIGGER_PIN2  12
#define ECHO_PIN2     13

#define DESVIO 2
#define NUMSTART 10


Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
Ultrasonic ultrasonic2(TRIGGER_PIN2, ECHO_PIN2);
float s1 =0, s2=0;
float S1MAX=0,S2MAX=0;
int passou1=0, passou2=1;

void sensor(float *s1,float *s2)
{
  float cmMsec;
  long microsec = ultrasonic.timing();
  long microsec2 = ultrasonic2.timing();

  //cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  *s1 = ultrasonic.convert(microsec, Ultrasonic::IN);
  *s2 = ultrasonic.convert(microsec2, Ultrasonic::IN);

  
}

void setup()
  {
  Serial.begin(9600);

    for(int i=0; i < NUMSTART ; i++)
    {
      
        sensor(&s1,&s2);
        S1MAX=S1MAX+s1;
        S2MAX=S2MAX+s2;

       /* Serial.print("S1:");
        Serial.print(s1);

        Serial.print("S2:");
        Serial.println(s2);*/

    }

        S1MAX=S1MAX/(NUMSTART);
        S2MAX=S2MAX/(NUMSTART);
        
        Serial.print("MAX S1:");
        Serial.print(S1MAX);

        Serial.print("MAX S2:");
        Serial.println(S2MAX);

  
  }



void loop()
  {
  
 
  
  
  sensor(&s1,&s2);
  Serial.print(" 1 IN: ");
  Serial.print(s1);

  Serial.print("||| 2 IN: ");
  Serial.println(s2);
  
  if(s1 - S1MAX > DESVIO && s2 - S2MAX < DESVIO && passou1 < passou2)
  {
    Serial.print("Passou S1");
    passou1++;
  } 
  if(s1 - S1MAX < DESVIO && s2 - S2MAX > DESVIO && passou2 <= passou1)
  {
        Serial.print("Passou S2");
        passou2++; 
        //Serial.print(passou2);
  }
 


  //Serial.print("SENSOR 1MS: ");
  //Serial.print(microsec);
  //Serial.print(", CM: ");
  //Serial.print(cmMsec);
  //Serial.print(" 1 IN: ");
  ///Serial.print(s1);

  //Serial.print("||| 2 IN: ");
  //Serial.println(s2);
  delay(500);
  }
