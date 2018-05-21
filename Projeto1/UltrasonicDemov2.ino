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

#define TRIGGER_PIN  5
#define ECHO_PIN     7

#define TRIGGER_PIN2  12
#define ECHO_PIN2     13

#define NUMSTART 15
#define DESVIO  2


Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
Ultrasonic ultrasonic2(TRIGGER_PIN2, ECHO_PIN2);
float s1 =0, s2=0;
float S1MAX=0,S2MAX=0;
int entrou=0,statusEntrada =0;
int saiu=0, saiuConf=1,statusSaida=0;
int entrouConf = 0;
int ordem[1];

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
        
        if(S1MAX < s1)
        {
          S1MAX=s1;
        }

        if(S2MAX < s2)
        { 
          S2MAX=s2;
        }
        
    

       /* Serial.print("S1:");
        Serial.print(s1);

        Serial.print("S2:");
        Serial.println(s2);*/

    }

  
        
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


  

  //Serial.print(statusEntrada);
  //Serial.print("||");
  //Serial.print(statusSaida);
    
  /*ordem[S1][S2]*/
  /*ENTRADA*/
  if(abs(s1 - S1MAX) > DESVIO && abs(s2 - S2MAX) < DESVIO && statusEntrada == 0)
  {
    Serial.print("Passou S1");
    entrou++;
    statusEntrada++;
    ordem[0]= 1;
  } 
  

   /*SAIDA*/
  if(abs(s1 - S1MAX) < DESVIO && abs(s2 - S2MAX) > DESVIO  && statusSaida == 0)
  {
    Serial.print("Passou S2");
    saiu++;
    statusSaida++;
    ordem[0]= 2;
  }  
 /*ENTRADA 2*/
  if(abs(s1 - S1MAX) < DESVIO && abs(s2 - S2MAX) > DESVIO && statusEntrada == 1)
  {
       Serial.print("Passou S2");
        entrouConf++; 
        statusEntrada++;
        ordem[1]= 2;
        //statusSaida=1;
          
  }

 

  /*SAIDA 2*/
  if(abs(s1 - S1MAX )> DESVIO && abs(s2 - S2MAX) < DESVIO && statusSaida == 1)
  {
        Serial.print("Passou S1");
        saiuConf++; 
        statusSaida++;
        ordem[1]= 1;
        //statusEntrada=0;
        
        
  }

  Serial.print(ordem[0]);
  Serial.print(":");
  Serial.print(ordem[0]);
  
  if(statusEntrada && ordem[0]== 1 && ordem[1]== 2)
        {
          Serial.print("ENTROU");
          statusEntrada =0;
          statusSaida = 0;
         
   
         // delay(2000);
        }
        //Serial.print(passou2);
  if(statusSaida == 2 && ordem[0]== 2 && ordem[1]== 1)
        {
          Serial.print("SAIU");
          statusEntrada =0;
          statusSaida = 0;
          
          
          //delay(2000);
        }
        //Serial.print(passou2); 
  
  
 /*SAIDA*/


   
       
        //Serial.print(passou2);
 
  
 
  
  
  

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
