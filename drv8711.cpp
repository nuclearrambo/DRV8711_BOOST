#include<SPI.h>
#include<msp430f5529.h>
#include "usci_isr_handler.h"
#include "HardwareSerial.h"
#define CS BIT1
#define STEP BIT2
#define SLEEPn BIT6
#define DIR BIT1
#define RST BIT7
#define STALL BIT0
#define FAULT BIT2

int flag =0;
int i,j;
void Initialize(void);
void getCurrentRegisters();
unsigned int WriteSPI(unsigned char dataHi, unsigned char dataLo);
uint16_t ReadSPI(unsigned char address);
String inputString;
char in;

void setup()
{
  //output pins
  P8DIR |= CS;
  P4DIR |= (STEP|DIR);
  P2DIR |= (RST|STALL|FAULT);
  P6DIR |= SLEEPn;
  pinMode(P7_4, OUTPUT);
  digitalWrite(P2_2, HIGH); //nSLEEP = high
  digitalWrite(P2_0, HIGH);
  digitalWrite(P6_6, HIGH);
  digitalWrite(P4_1, HIGH); //dir = high
  digitalWrite(P2_7, LOW); //reset = low
  Serial.begin(115200);
  SPI.begin();
  SPI.setClockDivider(2);
  delay(1);
  Initialize();
  getCurrentRegisters(); 
  inputString.reserve(8);
  digitalWrite(P7_4, HIGH);
  delayMicroseconds(10);
  digitalWrite(P7_4, LOW);
}

void loop()
{
  //Rotate the motor 90 degrees CCW by pulsing
  //the STP pin
	for(i=0; i<90; i++){
		for(j=0; j<10275; j++){
		   digitalWrite(P4_2, LOW);
		   delayMicroseconds(55);
		   digitalWrite(P4_2, HIGH);
		   delayMicroseconds(2);
		}
	}
	
	//Modify the config to rotate CW
	WriteSPI(0x0C, 0x47);
	
	//Rotate the motor by -90 degrees in CW direction
	//by pulsing the STP pin
	for(i=0; i<90; i++){
		for(j=0; j<10275; j++){
		   digitalWrite(P4_2, LOW);
		   delayMicroseconds(55);
		   digitalWrite(P4_2, HIGH);
		   delayMicroseconds(2);
		}
	}
	
	//Halt the motor
	WriteSPI(0x0C, 0x44);
	while(1);
}


void Initialize()
{
  //CTRL Register defaults
  unsigned char CTRLdataHi, CTRLdataLo;
  CTRLdataHi = 0x0C;
  CTRLdataLo = 0x45;
  WriteSPI(CTRLdataHi, CTRLdataLo);

  //TORQUE defaults
  unsigned char TORQUEHi, TORQUELo;
  TORQUEHi = 0x17;
  TORQUELo = 0xFF;
  WriteSPI(TORQUEHi, TORQUELo);
  
  //OFF defaults
  unsigned char OFFHi, OFFLo;
  OFFHi = 0x20;
  OFFLo = 0xF0;
  WriteSPI(OFFHi, OFFLo);

  //BLANK defaults
  unsigned char BLNKHi, BLNKLo;
  BLNKHi = 0x31;
  BLNKLo = 0xF0;
  WriteSPI(BLNKHi, BLNKLo);
  
  //DECAY defaults
  unsigned char DECAYHi, DECAYLo;
  DECAYHi = 0x41;
  DECAYLo = 0x10;
  WriteSPI(DECAYHi, DECAYLo);

  //STALL defaults
  unsigned char STALLHi, STALLLo;
  STALLHi = 0x53;
  STALLLo = 0x40;
  WriteSPI(STALLHi, STALLLo);
   
  //DRIVE defaults
  unsigned char DRIVEHi, DRIVELo;
  DRIVEHi = 0x60;
  DRIVELo = 0x0F;
  WriteSPI(DRIVEHi, DRIVELo);
 
  //STATUS defaults
  unsigned char STATUSHi, STATUSLo;
  STATUSHi = 0x70;
  STATUSLo = 0x00;
  WriteSPI(STATUSHi, STATUSLo);
}

unsigned int WriteSPI(unsigned char dataHi, unsigned char dataLo)
{
//  unsigned int readData = 0;
//  SPI.setBitOrder(MSBFIRST);
//  digitalWrite(P8_1, HIGH);
//  SPI.transfer(dataHi);
//  
//  SPI.transfer(dataLo);
//  digitalWrite(P8_1, LOW);
	unsigned int readData = 0;

	digitalWrite(P8_1,HIGH);

	UCB0TXBUF = dataHi;
	while (UCB0STAT & BUSY);
	readData |= (UCB0RXBUF << 8);
        
	UCB0TXBUF = dataLo;
	while (UCB0STAT & BUSY);
        readData |= UCB0RXBUF;
        digitalWrite(P8_1, LOW);
        readData &= 0x7FFF;
        
    return readData;
}

void getCurrentRegisters()
{
 Serial.print("\n-----------------------------------------------\n");
 Serial.print("CTRL register ");
 Serial.print(WriteSPI(0x80, 0x00),HEX);
 Serial.print("\n");
 
 Serial.print("TORQUE register ");
 Serial.print(WriteSPI(0x90, 0x00),HEX);
 Serial.print("\n");
 
 Serial.print("OFF register ");
 Serial.print(WriteSPI(0xA0, 0x00),HEX);
 Serial.print("\n"); 
 
 Serial.print("BLANK register ");
 Serial.print(WriteSPI(0xB0, 0x00),HEX);
 Serial.print("\n");
 
 Serial.print("DECAY register ");
 Serial.print(WriteSPI(0xC0, 0x00),HEX);
 Serial.print("\n");
 
 Serial.print("STALL register ");
 Serial.print(WriteSPI(0xD0, 0x00),HEX);
 Serial.print("\n");
 
 Serial.print("DRIVE register ");
 Serial.print(WriteSPI(0xE0, 0x00),HEX);
 Serial.print("\n");
 
 Serial.print("STATUS register ");
 Serial.print(WriteSPI(0xF0, 0x00),HEX);
 Serial.print("\n-----------------------------------------------\n");
}

//Could be used to issue custom commands to DRV8711 and 
//modify the configuration accordingly. This was never
//developed further. 
void serialEvent() {
	while(Serial.available()){
		in = Serial.read();
		flag = 1;
	}
	if(flag){
		if(in==0){
			Serial.println("Blank command");
		}
		flag = 0;
	}

}


