#include <Ethernet.h>
#include <SPI.h>

// MAC Address
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0xBA, 0x3F }; // Port 2
//byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0xBA, 0x5A }; // Port 1

byte ip[] = { 172,16,14,73 };//só se decidir setar 172.16.238.104:80
// Inicia o servidor na porta 80
byte server[] = {172,16,14,70 }; // Google

const int LM35 = A0; // Define o pino que lera a saída do LM35
float temperatura; // Variável que armazenará a temperatura medida



EthernetClient client;

void setup()
{
  pinMode(10,OUTPUT);
  
  Ethernet.begin(mac);
  Serial.begin(9600);

  Serial.println("connecting...");

  if (client.connect(server, 5000)) {
    Serial.println("connected");
  } else {
    Serial.println("connection failed");
  }
}

void loop()
{
  char buf[8];
  temperatura = (float(analogRead(LM35))*5/(1023))/0.01;
   Serial.print("Temperatura: ");
   Serial.println(temperatura);

   digitalWrite(10,LOW);
   
   byte *b = (byte*)&temperatura;
   client.write(b,4);
   
  if (client.available()) {
    String c = client.readString();//
    Serial.print(c);

    if(c.equals("ligar"))
    {  
       Serial.print("ip");
       digitalWrite(10,HIGH);
    }
    else
    {
      digitalWrite(10,LOW);
    }   
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}
