//Librerias
/*
Libreria CapacitiveSensor: permite utilizar dos o mas pin del Arduino en un sensor capacitivo
*/
#include <CapacitiveSensor.h>
/*
Libreria Adafruit_NeoPixel: permite manipular los leds (Leds utilizados: sk6812)
*/
#include <Adafruit_NeoPixel.h>
//seleccion del canal para MIDI del 0 al 15, el canal escogido ha sido el 1
#define  midichannel 1;                             
//NEOPIXEL: variables necesarias para inicializar los leds (o el arreglo de leds)
#define PIN 8 //pin de salida 
#define NUM_LEDS 93 //cantidad de Leds
int delayValue = 500; //tiempo de espera
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
//strip: nombre del arreglo de leds
//Nota: usamos el mismo pin transmisor para todos los leds
CapacitiveSensor   cs_3_2 = CapacitiveSensor(2,3);       //sensor 1: base pin 2, salida pin 3   
CapacitiveSensor   cs_4_2 = CapacitiveSensor(2,4);       //sensor 2: base pin 2, salida pin 4
CapacitiveSensor   cs_5_2 = CapacitiveSensor(2,5);       //sensor 3: base pin 2, salida pin 5 
CapacitiveSensor   cs_6_2 = CapacitiveSensor(2,6);       //sensor 4: base pin 2, salida pin 6 
CapacitiveSensor   cs_7_2 = CapacitiveSensor(2,7);       //sensor 5: base pin 2, salida pin 7 
/*
Threshold: umbral de resistencia del material, esta variable depende de cada material.
*/
int tresh1 = 1500; //Threshold sensor 1
int tresh2 = 800;   //Threshold sensor 2
int tresh3 = 1500;  //Threshold sensor 3
int tresh4 = 4000;  //Threshold sensor 4
int tresh5 = 2000;  //Threshold sensor 5


// Note: change array size according to the number of capacitive sensor that you want to manage.

// MIDI notes associated with each sensor. Change these values to send other notes.
byte Notes[5] = {67, 71 , 64, 66, 73};
//int Threshold[2] = {2000, 2000};      // Value that each sensor has to overcome to send a note 
int Threshold[5] = {tresh1, tresh2, tresh3, tresh4, tresh5};                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
boolean VelocityFlag  = false;                    // Velocity ON (true) or OFF (false)

//boolean activePad[2] = {0,0};                   // Array of flags indicating wich sensor is currently playing
boolean activePad[5] = {0,0,0,0,0};
//int PinPlayTime[2] = {0,0};                     // Counter since a note started to play
int PinPlayTime[5] = {0,0,0,0,0};
//int MaxTime[2] = {0,0};                      // Cycles before a 2nd hit is allowed
int MaxTime[5] = {0,0,0,0,0};

byte status1;                                     // byte that we will use to compose a part of the MIDI message

int pin = 0;     
int velocity = 0;

bool debug = true; // Set this flag to true if you want to read cap sense values in Serila Monitor to check if everything is working properly


void setup() 
{
  // Initialize serial port (we will use serial to debug our code and then to send midi commands)
  //prueba calibracion
  cs_3_2.reset_CS_AutoCal();
  cs_4_2.reset_CS_AutoCal();
  cs_5_2.reset_CS_AutoCal();
  cs_6_2.reset_CS_AutoCal();
  cs_7_2.reset_CS_AutoCal();
  
  //-----------------------------
  
  strip.begin();
  strip.show();
  //strip.setBrightness(25);
  Serial.begin(115200);
}

void loop() {
  if(debug){
    Serial.println("\t");
  }
    
  for(int pin=0; pin < 5; pin++){
     float capValue = readValue(pin); // Read value of the capacitive sensor
     if(debug){
      Serial.print(" pin: "); 
      Serial.print(pin);
      Serial.print(" - "); 
      Serial.print(capValue); 
     }

    // Check if cap. sensor value is above the threshold
    if((capValue > Threshold[pin])){
      if((activePad[pin] == false)){
        if(VelocityFlag == true){
          velocity = (capValue / 8) -1 ;                                              
        }
        else {
          velocity = 127;
          //NEOPIXEL
          //Serial.println("pin tocado: ");
          //Serial.print(pin); 
          led(pin);      
        }
     
        MIDI_TX(144, Notes[pin], velocity); //note on (144 is signal for note on)
 
        PinPlayTime[pin] = 0;
        activePad[pin] = true;
      }
      else{
        PinPlayTime[pin] = PinPlayTime[pin] + 1;
      }
    }
    else if(activePad[pin] == true){
      PinPlayTime[pin] = PinPlayTime[pin] + 1;
      if(PinPlayTime[pin] > MaxTime[pin]){
        activePad[pin] = false;
        MIDI_TX(128, Notes[pin],0); //note off (128 is signal for note on)
      }
    }
  }
  borrar();
 }

// This function read the value of the capacitive sensor we want to read
// pin input variable is the index of the sensor
// With the switch/case we select the correct sensor to read.
float readValue(int pin){
  float value = 0;
  switch(pin){
  case 0:
     value =  cs_3_2.capacitiveSensor(50);
      return value;
      break;
  case 1:
      value =  cs_4_2.capacitiveSensor(50);
      return value;
      break;
  case 2:
      value =  cs_5_2.capacitiveSensor(50);
      return value;
      break;
  case 3:
      value =  cs_6_2.capacitiveSensor(50);
      return value;
      break;
  case 4:
      value =  cs_7_2.capacitiveSensor(50);
      return value;
      break;
   default:
      break;
  }
}

//*******************************************************************************************************************
// Transmit MIDI Message
//*******************************************************************************************************************
void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY) {
  if(!debug){
    status1 = MESSAGE + midichannel;    // The first byte we have to send contains information about the command (note on ,note off, etc..) and the channel were we want to send the message
    Serial.write(status1);
    Serial.write(PITCH);                // The second byte of the message contains the pitch
    Serial.write(VELOCITY);             // The last byte of the message contatins the velocity of the note
  }

}
//******************************************
//  NEOPIXEL
//******************************************
void led(int pin){

  switch(pin){
  case 0:
          //Serial.println("encender serie 1");
          serie_1();
      break;
  case 1:
          //Serial.println("encender serie 2");
          serie_2();
      break;
  case 2:
          //Serial.println("encender serie 3");
          serie_3();
      break;
  case 3:
          //Serial.println("encender serie 4");
          serie_4();
      break;
  case 4:
          //Serial.println("encender serie 5");
          serie_5();
      break;
   default:
      break;
  }
  
  }
