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
#include <Average.h>

#define TRIGGER_PIN  12
#define ECHO_PIN     13

#define TRIGGER_PIN2  5
#define ECHO_PIN2     7

#define NUMSTART 10
#define DESVIO  2


Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
Ultrasonic ultrasonic2(TRIGGER_PIN2, ECHO_PIN2);
float s1, s2;
float S1MOD,S2MOD;
int ordem[2];
int cont =0;

Average<float> aveS1(NUMSTART);
Average<float> aveS2(NUMSTART);
float cmMsec;
  long microsec = ultrasonic.timing();
  long microsec2 = ultrasonic2.timing();

void sensor(float *s1,float *s2)
{
 
 microsec = ultrasonic.timing();
 microsec2 = ultrasonic2.timing();

  //cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  *s1 = ultrasonic.convert(microsec, Ultrasonic::IN);
  *s2 = ultrasonic.convert(microsec2, Ultrasonic::IN);

  
}

void setup()
  {
  Serial.begin(9600);
 

    ordem[0]= 0;
    ordem[1]= 0;
    
    S1MOD = 0;
    S2MOD = 0;   

    s1 = 0;
    s2 = 0;

    for(int i=0; i < NUMSTART ; i++)
    {
        sensor(&s1,&s2);

        aveS1.push(s1);
        aveS2.push(s2);   
    }  
     
    S1MOD = aveS1.mode();
    S2MOD = aveS2.mode();  
      
        
    Serial.print("MOD S1:");
    Serial.println(S1MOD);

    Serial.print("MOD S2:");
    Serial.println(S2MOD);

  
  }



void loop()
  {
  
 
  
  
  sensor(&s1,&s2);
  
  
  Serial.print(" 1 IN: ");
  Serial.print(s1);

  Serial.print("||| 2 IN: ");
  Serial.println(s2);


  

  //Serial.print(statusEntrada);
  //Serial.print("||");
  //Serial.print(statusSaida);
    
  /*ordem[S1][S2]*/
  /*ENTRADA*/
  if(abs(s1 - S1MOD) > DESVIO && ordem[0] == 0)
  {
    ordem[0]= 1;
  } 
 /*ENTRADA 2*/
  if(abs(s1 - S1MOD) < DESVIO && abs(s2 - S2MOD) > DESVIO && ordem[0] == 1)
  {
        ordem[1]= 2;        
  }



  if(abs(s2 - S1MOD) > DESVIO && ordem[0] == 0)
  {
    ordem[0]= 2;
     
  } 
  if(abs(s1 - S1MOD) > DESVIO && abs(s2 - S2MOD) < DESVIO && ordem[0] == 2)
  {
        ordem[1]= 1;
                
  }
  
 
  Serial.print(ordem[0]);
  Serial.print(":");
  Serial.println(ordem[1]);
  
  if(ordem[0]== 1 && ordem[1]== 2)
  {
    Serial.print("ENTROU");
    ordem[0]= 0;
    ordem[1]= 0;
    delay(1000);
  }
    
  if(ordem[0]== 2 && ordem[1]== 1)
  {
    Serial.print("SAIU");
    ordem[0]= 0;
    ordem[1]= 0;
    delay(1000);
  }
  /*Zera caso tiver repetição*/ 
  if(cont > 10 && (ordem[0] == 2 || ordem[0] == 1))
  {
    ordem[0]= 0;
    ordem[1]= 0;
    cont = 0;
  }
  cont++; 
       

  //delay(100);
  }
