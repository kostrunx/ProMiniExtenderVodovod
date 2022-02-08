/*
 * verzija 1.0.0
 * osnovna verzija zaceta 20.10.2021
 * 
 */
//ProMiniExtender
#include <Wire.h>                 //knjiznica za I2C protokol

#define I2C_MSG_IN_SIZE    4
#define I2C_MSG_OUT_SIZE   4

#define CMD_DIGITAL_WRITE  1
#define CMD_DIGITAL_READ   2
#define CMD_ANALOG_WRITE   3
#define CMD_ANALOG_READ    4

volatile uint8_t sendBuffer[I2C_MSG_OUT_SIZE];
//ProMiniExtender

//moja koda
boolean PowerUp = true;				    //zastavica za powerup cikel 
boolean OutEnable = true;        	//izhodi so omogoceni
boolean Countms = false;          //zastavica za casovnik za 1ms zakasnitev
boolean Odpiranje = false;        //zastavica za lokalno zahtevo za odpiranje ventila
boolean Zapiranje = false;        //zastavica za lokalno zahtevo za zapiranje ventila
boolean OdpiranjeRemote = false;  //zastavica za remote zahtevo za odpiranje ventila
boolean ZapiranjeRemote = false;  //zastavica za remote zahtevo za zapiranje ventila
boolean OdpiranjeErr = false;     //zastavica za napako pri odpiranju
boolean ZapiranjeErr = false;     //zastavica za napako pri zapiranju
boolean OdpiranjeEnable = false;  //zastavica za možnost odpiranja
boolean ZapiranjeEnable = false;  //zastavica za možnost zapiranja

const int INPinOdprt = 4;         //DIO4 je IN za odprto stanje ventila
const int INPinZaprt = 5;         //DIO5 je IN za zaprto stanje ventila
const int INPinErrOdpiranje = 3;  //DIO3 je IN za napako pri odpiranju
const int INPinErrZapiranje = 2;  //DIO2 je IN za napako pri zapiranju

const int AINPT0 = A0             //AIO je PT0 signal
const int AINPT1 = A1             //AI1 je PT1 signal

const int LedPin = 13;            //DIO13 je OUT za vgrajeno LED
const int OUTPinOdpiranje = 8;    //DIO8 je OUT za odpiranje ventila
const int OUTPinZapiranje = 9;    //DIO9 je OUT za zapiranje ventila
const int OUTPinOdpiranjeLED = 7; //DIO7 je OUT za ventil odpiranje LED
const int OUTPinZapiranjeLED = 6; //DIO6 je OUT za ventil zapiranje LED
const int OUTPinOdprtLED = 16;    //DIO16 je OUT za ventil odprt LED
const int OUTPinZaprtLED = 17;    //DIO17 je OUT za ventil zaprt LED
const int OUTPinErrLED = 10;      //DIO10 je OUT za Error LED

boolean VentilOdpiranjeZeljenaVrednost = LOW;
boolean VentilZapiranjeZeljenaVrednost = LOW;
boolean VentilOdpiranjeDejanskaVrednost = LOW;
boolean VentilZapiranjeDejanskaVrednost = LOW;
boolean OdpiranjeLEDStanje = LOW;
boolean ZapiranjeLEDStanje = LOW;
boolean OdprtLEDStanje = LOW;
boolean ZaprtLEDStanje = LOW;
boolean ErrLEDStanje = LOW;

unsigned long Timer0 = 0;

int StartupDelay = 0;             //startup delay za preprecitev napacnega polozaja ventila ob vklopu napajanja

//moja koda

void setup() {

  //moja koda
  //delay(10000);
  //moja koda
  
  //ProMiniExtender
  Wire.begin(0x7f);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  //ProMiniExtender

  //moja koda
  pinMode(INPinOdprt, INPUT);                 //definicija vhodnega pina
  pinMode(INPinZaprt, INPUT);                 //definicija vhodnega pina
  pinMode(INPinErrOdpiranje, INPUT_PULLUP);   //definicija vhodnega pina
  pinMode(INPinErrZapiranje, INPUT_PULLUP);   //definicija vhodnega pina
  pinMode(AINPT0, INPUT);                     //definicija vhodnega pina
  pinMode(AINPT1, INPUT);                     //definicija vhodnega pina
  pinMode(LedPin, OUTPUT);                    //definicija izhodnega pina
  pinMode(OUTPinOdpiranje, OUTPUT);           //definicija izhodnega pina
  pinMode(OUTPinZapiranje, OUTPUT);           //definicija izhodnega pina
  pinMode(OUTPinOdpiranjeLED, OUTPUT);        //definicija izhodnega pina
  pinMode(OUTPinZapiranjeLED, OUTPUT);        //definicija izhodnega pina
  pinMode(OUTPinOdprtLED, OUTPUT);            //definicija izhodnega pina
  pinMode(OUTPinZaprtLED, OUTPUT);            //definicija izhodnega pina
  pinMode(OUTPinErrLED, OUTPUT);              //definicija vhodnega pina
  //moja koda

}

void loop() {

  //moja koda
  if (PowerUp == true) {			        //ce gre za powerup cikel
	  if (INPinOdprt == LOW) {	        //ce ventil ni odprt
		  Odpiranje = true;				        //postavi zahtevo za odpiranje
	  }
	  PowerUp = false;					        //pobrisi zastavico za powerup cikel
  }
  if (millis() < Timer0) {
    Timer0 = 0;                       //reset stevca ob prelivu funkcije millis()
  }
  if (millis() > (Timer0 + 9)) {      //vsakih 10ms poveca stevec
    Timer0 = millis();
    Countms = true;                   //zastavica, da je potrebno pogledati stanje vhodov
  }
  if (Countms == true) {              //ce je zastavica postavljena
	BeriVhode;							            //preberi vhode
  Countms = false;
  }
  
  DolociStanja;
  PostaviIzhode;
  

}
  
  
void BeriVhode() {
	VentilOdprt = INPinOdprt;			//priredi stanje zastavice stanju vhoda
	VentilZaprt = INPinZaprt;			//priredi stanje zastavice stanju vhoda
  if OdpiranjeErr == true {           //ce je prislo do napake pri odpiranju
    OdpiranjeEnable = false;          //potem prepreci nadaljnje odpiranje
    OutEnable = false;                //in pobrisi zastavico za izhode
  }
  if ZapiranjeErr == true {           //ce je prislo do napake pri zapiranju
    ZapiranjeEnable = false;          //potem prepreci nadaljnje zapiranje
    OutEnable = false;                //in pobrisi zastavico za izhode
  }
}

void KontrolaPolozajaVentila() {
	if (VentilOdprt == 1) {				//ce je ventil odprt
		OdpiranjeLEDStanje = HIGH;		//postavi LED na ON
		OdpiranjeEnable = false;		//onemogoci nadaljnje odpiranje
		
		
	
	
	
	
		
 if (Countms == true) {              //ce je zastavica postavljena
    IN1Vrednost = digitalRead(IN1Pin);  //preberi stanje vhoda
    if (IN1Vrednost == LOW) {           //in tipka pritisnjena
      if (IN1DebounceValue < 1000) {      //in debounce cas se ni prevelik
        IN1DebounceValue = IN1DebounceValue + 1;  //potem povecaj debounce stevec
      }
    }
    else {                              //ce pa tipka ni pritisnjena, ali pa se se odbija
      IN1DebounceValue = 0;             //potem pobrisi debounce stevec
    }
    if (IN1DebounceValue == IN1DebouncePreset) {  //ce je debounce cas potekel
      Ry1DejanskaVrednost = digitalRead(Ry1Pin);  //potem najprej preberi dejansko stanje releja
      Ry1ZeljenaVrednost = !Ry1DejanskaVrednost;  //zeljeno stanje je negirano dejansko stanje
      if (OutEnable == true) digitalWrite(Ry1Pin,Ry1ZeljenaVrednost);    //postavi Ry1 na zeljeno vrednost
    }
    
    IN2Vrednost = digitalRead(IN2Pin);  //preberi stanje vhoda
    if (IN2Vrednost == LOW) {           //ce je zastavica postavljena in tipka pritisnjena
      if (IN2DebounceValue < 1000) {      //in debounce cas se ni prevelik
        IN2DebounceValue = IN2DebounceValue + 1;  //potem povecaj debounce stevec
      }
    }
    else {                              //ce pa tipka ni pritisnjena, ali pa se se odbija
      IN2DebounceValue = 0;             //potem pobrisi debounce stevec
    }
    if (IN2DebounceValue == IN2DebouncePreset) {  //ce je debounce cas potekel
      Ry2DejanskaVrednost = digitalRead(Ry2Pin);  //potem najprej preberi dejansko stanje releja
      Ry2ZeljenaVrednost = !Ry2DejanskaVrednost;  //zeljeno stanje je negirano dejansko stanje
      if (OutEnable == true) digitalWrite(Ry2Pin,Ry2ZeljenaVrednost);    //postavi Ry2 na zeljeno vrednost
    }
    
    IN3Vrednost = digitalRead(IN3Pin);  //preberi stanje vhoda
    if (IN3Vrednost == LOW) {           //ce je zastavica postavljena in tipka pritisnjena
      if (IN3DebounceValue < 1000) {      //in debounce cas se ni prevelik
        IN3DebounceValue = IN3DebounceValue + 1;  //potem povecaj debounce stevec
      }
    }
    else {                              //ce pa tipka ni pritisnjena, ali pa se se odbija
      IN3DebounceValue = 0;             //potem pobrisi debounce stevec
    }
    if (IN3DebounceValue == IN3DebouncePreset) {  //ce je debounce cas potekel
      Ry3DejanskaVrednost = digitalRead(Ry3Pin);  //potem najprej preberi dejansko stanje releja
      Ry3ZeljenaVrednost = !Ry3DejanskaVrednost;  //zeljeno stanje je negirano dejansko stanje
      if (OutEnable == true) digitalWrite(Ry3Pin,Ry3ZeljenaVrednost);    //postavi Ry3 na zeljeno vrednost
    }
    
    IN4Vrednost = digitalRead(IN4Pin);  //preberi stanje vhoda
    if (IN4Vrednost == LOW) {           //ce je zastavica postavljena in tipka pritisnjena
      if (IN4DebounceValue < 1000) {      //in debounce cas se ni prevelik
        IN4DebounceValue = IN4DebounceValue + 1;  //potem povecaj debounce stevec
      }
    }
    else {                              //ce pa tipka ni pritisnjena, ali pa se se odbija
      IN4DebounceValue = 0;             //potem pobrisi debounce stevec
    }
    if (IN4DebounceValue == IN4DebouncePreset) {  //ce je debounce cas potekel
      Ry4DejanskaVrednost = digitalRead(Ry4Pin);  //potem najprej preberi dejansko stanje releja
      Ry4ZeljenaVrednost = !Ry4DejanskaVrednost;  //zeljeno stanje je negirano dejansko stanje
      if (OutEnable == true) digitalWrite(Ry4Pin,Ry4ZeljenaVrednost);    //postavi Ry4 na zeljeno vrednost
    }
    ACInput = analogRead(ACDetectPin);          //preberi vrednost analognega vhoda
    Skupaj = Skupaj - Odcitki[BeriIndeks];      //odstej najstarejsi odcitek
    Odcitki[BeriIndeks] = ACInput;              //spremenljivka privzame trenutno vrednost analognega vhoda
    Skupaj = Skupaj + Odcitki[BeriIndeks];      //pristej nanjnovejsi odcitek
    BeriIndeks = BeriIndeks + 1;                //povecaj stevec branja za 1
    if (BeriIndeks >= NumOdcitki) {             //ce je stevilo odcitkov za povprecje dosezeno
      BeriIndeks = 0;                           //potem pobrisi stevec odcitkov
    }
    Povprecje = Skupaj / NumOdcitki;            //izracunaj povprecno vrednost odcitkov
    Countms = false;  
    if(Povprecje <= 200) {                       //ce je povprecje manjse od 1/3 maksimuma
      ACPrisotna = false;                       //potem napajanje ni prisotno
    }
    if(Povprecje > 200) {                       //ce je povprecje vecje od 1/3 maksimuma
      ACPrisotna = true;                        //potem je napajanje prisotno
      if(Povprecje > 600) {                     //ce pa je povprecje preko 2/3 maksimuma
        ACPrisotna = false;                     //potem je nekaj narobe z napajanjem
      }
    }
    if (ACPrisotna == true) {
      if (OutEnable == false) StartupDelay = StartupDelay + 1;
      if (StartupDelay > 1000) {
        StartupDelay = 1000;
        OutEnable = true;
      }
    }
    if(ACPrisotna == false){                    //ce je prislo do izpada 220 napajalne napetosti
      Ry1ZeljenaVrednost = LOW;                 //potem izklopi vse izhode
      digitalWrite(Ry1Pin,Ry1ZeljenaVrednost);
      Ry2ZeljenaVrednost = LOW;
      digitalWrite(Ry2Pin,Ry2ZeljenaVrednost);
      Ry3ZeljenaVrednost = LOW;
      digitalWrite(Ry3Pin,Ry3ZeljenaVrednost);
      Ry4ZeljenaVrednost = LOW;
      digitalWrite(Ry4Pin,Ry4ZeljenaVrednost);
      StartupDelay = 0;                         //in pobrisi stevec za startup delay
      OutEnable = false;                        //in onemogoci vse izhode
    }
  }
}

void BeriVhode() {
  if OdpiranjeErr == true {           //ce je prislo do napake pri odpiranju
    OdpiranjeEnable = false;          //potem prepreci nadaljnje odpiranje
    OutEnable = false;                //in pobrisi zastavico za izhode
  }
  if ZapiranjeErr == true {           //ce je prislo do napake pri zapiranju
    ZapiranjeEnable = false;          //potem prepreci nadaljnje zapiranje
    OutEnable = false;                //in pobrisi zastavico za izhode
  }
}
//moja koda

//ProMiniExtender
void receiveEvent(int count)
{
  if (count == I2C_MSG_IN_SIZE)
  {
    byte cmd = Wire.read();
    byte port = Wire.read();
    
    int value = Wire.read();
    value += Wire.read()*256;
    switch(cmd)
      {
        case CMD_DIGITAL_WRITE:
          //pinMode(port,OUTPUT);         //spremenjeno, da ne spreminjam funkcije I/O pinov
          if(OutEnable == true) digitalWrite(port,value);
          break;
        case CMD_DIGITAL_READ:
          //pinMode(port,INPUT_PULLUP);   //spremenjeno, da ne spreminjam funkcije I/O pinov
          clearSendBuffer();
          sendBuffer[0] = digitalRead(port);
          break;
        case CMD_ANALOG_WRITE:
          if(OutEnable ==  true) analogWrite(port,value);
          break;
        case CMD_ANALOG_READ:
          clearSendBuffer();
          if (port == 6) {
          valueRead = Povprecje;
          }
          else {
          valueRead = analogRead(port);
          }
          sendBuffer[0] = valueRead & 0xff;
          sendBuffer[1] = valueRead >> 8;
          break;
      }
  }
}

void clearSendBuffer()
{
  for(byte x=0; x < sizeof(sendBuffer); x++)
    sendBuffer[x]=0;
}

void requestEvent()
{
  Wire.write((const uint8_t*)sendBuffer,sizeof(sendBuffer));
}
//ProMiniExtender
