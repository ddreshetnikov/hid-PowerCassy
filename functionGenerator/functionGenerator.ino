#include "HID-Project.h"

#include <AD9833.h>     // Include the library

#define FNC_PIN 10       // Can be any digital IO pin

AD9833 gen(FNC_PIN);

const int pinLed = LED_BUILTIN;

// Buffer to hold RawHID data.
// If host tries to send more data than this,
// it will respond with an error.
// If the data is not read until the host sends the next data
// it will also respond with an error and the data will be lost.
uint8_t rawhidData[255];

float vIn = 0;
float vCalc = 0;

int voltDig = 0;
float voltV = 0;

int wave;

unsigned long freqDig;
float freq = 0;

int amplDig;
float ampl;

int offsetDig;
float offset;

int ratio;

bool com50 = 0;

int btV1 = 0, btV2 = 0;

void setup() {
  pinMode(pinLed, OUTPUT);
  Serial.begin(115200);
  pinMode(A0, INPUT);
  // Set the RawHID OUT report array.
  // Feature reports are also (parallel) possible, see the other example for this.
  RawHID.begin(rawhidData, sizeof(rawhidData));
  gen.Begin();
  //gen.ApplySignal(SINE_WAVE,REG1,200);
  //gen.EnableOutput(true);
}

void loop() {
  vIn = analogRead(A0);
  vCalc = vIn*5.0/1023.0*1680.0/(680.0);
  //Serial.println(vCalc);
//  Serial.print("bttn=");
//  Serial.println(bttn);
  // Check if there is new data from the RawHID device
  auto bytesAvailable = RawHID.available();
  if (bytesAvailable)
  {
    uint8_t buffIn[64];
    uint8_t buffOut[64];
    int i = 0;
    while (bytesAvailable--) {
      buffIn[i] = RawHID.read();
      //Serial.print(buffIn[i]);
      //Serial.print(" ");
      i++;
    }
    i = 0;
    //Serial.println();

    if (buffIn[2] == 255){
       
     switch (buffIn[3]){
    case 1:
      //тип устройства. Отвечаем, что power cassy
        buffOut[0] = 3;
        buffOut[1] = 1;
        buffOut[2] = 2;//12 - mobile cassy, 29-pocket cassy, 32 - ваттметр 
        buffOut[3] = 5; // версия D
      break;
    case 2:
        // версия по. текущая версия 1.11
        buffOut[0] = 3;
        buffOut[1] = 1;
        buffOut[2] = 0;
        buffOut[3] = 108;
      break;
    case 28:
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;
    case 16:
      buffOut[0] = 2;
      buffOut[1] = 1;
      buffOut[2] = 254;
      break;
    case 51:
      buffOut[0] = 2;
      buffOut[1] = 1;
      buffOut[2] = 0;
      break;
    case 49:
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;
    case 48:
      if (buffIn[5] < 8){
        voltDig = buffIn[5]*256+buffIn[6];
      }else{
        voltDig = buffIn[5]*256+buffIn[6]-65536;
      }
      voltV = 0.005*voltDig;
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;
    case 18:
      buffOut[0] = 3;
      buffOut[1] = 1;
      btV1 = int(vCalc/0.005/256);
      btV2 = int(vCalc/0.005)%256;
      buffOut[2] = btV1;
      buffOut[3] = btV2;
      break;

    case 50:
      wave = buffIn[4];
      
      freqDig = buffIn[5]*pow(256, 3)+buffIn[6]*pow(256, 2)+buffIn[7]*256.0+buffIn[8];
      freq = freqDig/1000.0;
      
      if (buffIn[9] < 8){
        amplDig = buffIn[9]*256+buffIn[10];
      }else{
        amplDig = buffIn[9]*256+buffIn[10]-65536;
      }

      ampl = amplDig*0.005;
      
      if (buffIn[11] < 8){
        offsetDig = buffIn[11]*256+buffIn[12];
      }else{
        offsetDig = buffIn[11]*256+buffIn[12]-65536;
      }

      offset = offsetDig*0.005;

      ratio = buffIn[13];

      buffOut[0] = 1;
      buffOut[1] = 1;
//    Serial.print("Wave is ");
//    Serial.println(wave);
//    Serial.print("Frequency is ");
//    Serial.println(freq);
//    Serial.print("Amplitude is ");
//    Serial.println(ampl);
//    Serial.print("Offset is ");
//    Serial.println(offset);
//    Serial.print("Ratio is ");
//    Serial.println(ratio);
      switch (wave){
        case 1: //sin
          //gen.EnableOutput(false);
          gen.ApplySignal(SINE_WAVE,REG1,freq);
          gen.EnableOutput(true);
          break;
        case 3: //square 0 to ampl
          //gen.EnableOutput(false);
          gen.ApplySignal(SQUARE_WAVE,REG1,freq);
          gen.EnableOutput(true);
          break;
        case 5: //triang 0 to ampl          
          //gen.EnableOutput(false);
          gen.ApplySignal(TRIANGLE_WAVE,REG1,freq);
          gen.EnableOutput(true);
        break;
        default:
          gen.EnableOutput(false);
          break;
      }
      break;
    default:
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;      
    }
    RawHID.write(buffOut, sizeof(buffOut));
      for (uint8_t i = 0; i < sizeof(buffOut); i++) {
      //Serial.print(buffOut[i]);
      buffOut[i] = 1;
      //Serial.print(" ");
    }
    }
    //Serial.println();
  }
}
