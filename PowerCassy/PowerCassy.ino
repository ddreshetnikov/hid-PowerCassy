#include "HID-Project.h"

const int pinLed = LED_BUILTIN;
const int pinButton = 4;

// Buffer to hold RawHID data.
// If host tries to send more data than this,
// it will respond with an error.
// If the data is not read until the host sends the next data
// it will also respond with an error and the data will be lost.
uint8_t rawhidData[255];
uint8_t bttn = 0;
int voltDig = 0;
float voltV = 0;
int btV1 = 0, btV2 = 0;

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  Serial.begin(115200);

  // Set the RawHID OUT report array.
  // Feature reports are also (parallel) possible, see the other example for this.
  RawHID.begin(rawhidData, sizeof(rawhidData));
}

void loop() {
  if (!digitalRead(pinButton)){
    bttn = 1;
  }
  else {
    bttn = 0;
  }
//  Serial.print("bttn=");
//  Serial.println(bttn);
  // Check if there is new data from the RawHID device
  auto bytesAvailable = RawHID.available();
  if (bytesAvailable)
  {
    //digitalWrite(pinLed, HIGH);

    // Mirror data via Serial
    uint8_t buffIn[64];
    uint8_t buffOut[64];
    int i = 0;
    while (bytesAvailable--) {
      buffIn[i] = RawHID.read();
      Serial.print(buffIn[i]);
      Serial.print(" ");
      i++;
    }
    i = 0;
    Serial.println();
    if (buffIn[2] == 255){
       
     switch (buffIn[3]){
    case 1:
      //тип устройства. Отвечаем, что sensor cassy-2
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
    btV1 = buffIn[5];
    btV2 = buffIn[6];
      if (btV1 < 8){
        voltDig = btV1*256+btV2;
      }else{
        voltDig = btV1*256+btV2-65536;
      }
      voltV = 0.005*voltDig;
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;
    case 18:
      buffOut[0] = 3;
      buffOut[1] = 1;
      buffOut[2] = btV1;
      buffOut[3] = btV2;
      break;
    default:
      buffOut[0] = 1;
      buffOut[1] = 1;
      break;      
    }
    RawHID.write(buffOut, sizeof(buffOut));
      for (uint8_t i = 0; i < sizeof(buffOut); i++) {
      Serial.print(buffOut[i]);
      buffOut[i] = 0;
      Serial.print(" ");
    }
    }
    Serial.println();
    //Serial.print("Voltage is ");
    //Serial.println(voltV);
    //digitalWrite(pinLed, LOW);
  }
}
