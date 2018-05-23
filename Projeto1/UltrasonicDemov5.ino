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
#include <Ethernet.h>
#include <SPI.h>

#define TRIGGER_PIN  4
#define ECHO_PIN     5

#define TRIGGER_PIN2  2
#define ECHO_PIN2     3

#define NUMSTART 5
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

  const int LM35 = A0; // Define o pino que lera a saída do LM35
float temperatura; // Variável que armazenará a temperatura medida

EthernetClient client;
// MAC Address
//byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0xBA, 0xAF }; // Port 0
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0xBA, 0x8A}; // Port 1

//byte ip[] = { 172,16,14,73 }; //só se decidir setar 172.16.238.104:80
// Inicia o servidor na porta 80
byte server[] = {172,16,13,141}; // Google




void sensor(float *s1,float *s2)
{
 
 microsec = ultrasonic.timing();
 microsec2 = ultrasonic2.timing();

  //cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  *s1 = ultrasonic.convert(microsec, Ultrasonic::IN);
  *s2 = ultrasonic.convert(microsec2, Ultrasonic::IN);
  
   
  
}

void conect()
{
  Ethernet.begin(mac);
  

  Serial.println("connecting...");

  if (client.connect(server, 5000)) {
    Serial.println("connected");
    delay(1000);
  } else {
    Serial.println("connection failed");
  }
  
}
void setup()
  {
  //Ethernet.begin(mac);
  Serial.begin(9600);

   ordem[0]= 0;
    ordem[1]= 0;
    
    S1MOD = 0;
    S2MOD = 0;   

    s1 = 0;
    s2 = 0;
             
    conect();
    
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
      temperatura = (float(analogRead(LM35))*5/(1023))/0.01;
      Serial.print("Temp:");
      Serial.print(temperatura);
      //digitalWrite(2,HIGH);
      Serial.println("  ENTROU");
      
      byte *b = (byte*)&temperatura;
      float valoraux = 1;
      byte *valor = (byte*)&valoraux;
      client.write(b,sizeof(float));
      client.write(valor,sizeof(float));
      
      
      ordem[0]= 0;
      ordem[1]= 0;
      
      
      delay(10000);
      //digitalWrite(2,LOW);
    }
      
    if(ordem[0]== 2 && ordem[1]== 1)
    {
      temperatura = (float(analogRead(LM35))*5/(1023))/0.01;
      Serial.print("Temp:");
      Serial.print(temperatura);
      //digitalWrite(3,HIGH);
      Serial.print("SAIU");
      
      byte *b = (byte*)&temperatura;
      float valoraux = 0;
      byte *valor = (byte*)&valoraux;
      client.write(b,sizeof(float));
      client.write(valor,sizeof(float));
      
      ordem[0]= 0;
      ordem[1]= 0;
      
      delay(10000);
      //digitalWrite(3,LOW);
    }
  /*Zera caso tiver repetição*/ 
  if(cont > 10 && (ordem[0] == 2 || ordem[0] == 1))
  {
    ordem[0]= 0;
    ordem[1]= 0;
    cont = 0;
  }
  cont++; 
       

  //delay(1000);
  }

