#include <ShiftRegister74HC595.h>

// LEDs through shift register
// 74HC595

// Buttons through shift register
// CD4021BE

// Faders through analog multiplex
// chip 74hc4051
// lib

uint8_t lbuttons[2];
uint8_t buttons[2];
unsigned long lastDebounceTime[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long debounceDelay = 10;    // the debounce time; increase if the output flickers
int faders[16];
int fadersUnstep[16];
int cc[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
int channel = 1;

uint8_t leds[2];

int LEDserialDataPin = 12; // DS
int LEDclockPin = 2; // SHCP
int LEDlatchPin = 3; // STCP
ShiftRegister74HC595<2> LEDsr(LEDserialDataPin, LEDclockPin, LEDlatchPin);

int SWdata = 21;
int SWclock = 22;
int SWlatch = 23;
//ShiftRegister4021BP SWsr(2, SWdata, SWclock, SWlatch); //attributes (number of shift registers, datapin, clockpin, latchpin)

int FAa = 4;
int FAb = 5;
int FAc = 6;
int FAin1 = 0;
int FAin2 = 1;

int r0;
int r1;
int r2;

void setup() {
  LEDsr.setAllHigh();

  //define pins as i/o
  pinMode(SWclock, OUTPUT);
  pinMode(SWdata, INPUT);
  pinMode(SWlatch, OUTPUT);
  pinMode(13, OUTPUT);

  pinMode(FAa, OUTPUT);
  pinMode(FAb, OUTPUT);
  pinMode(FAc, OUTPUT);

  //set pins low
  digitalWrite(SWclock, LOW);
  digitalWrite(SWdata, LOW);
  digitalWrite(SWlatch, LOW);


  Serial.begin(9600);
  // put your setup code here, to run once:

  // create arrays to store
  //   fader,
  //   led
  //   switch state
  //   fader ccs
  //   button notes


// ideas
// step sequencer mode - turn each step on or off with the button

// banks of different ccs/notes

// mute mode so toggling led sends 0 and stops responding to changes

}

void pulseLED() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
}

bool getBit(int index, uint8_t bytes[2]) {
  uint8_t b;
  if (index < 8) {
    b = bytes[0];
  } else {
    b = bytes[1];
    index = index - 8;
  }
  return (b >> index) & 1;
}

int diff(int a, int b) {
  if (a == b) {
    return 0;
  }
  if (a > b) {
    return a - b;
  } else {
    return b - a;
  }
}

void loop() {
  int byte;

  uint8_t cbuttons[2];

  digitalWrite(SWlatch, HIGH);
  delayMicroseconds(20);
  digitalWrite(SWlatch, LOW);
  
  for (byte = 1; byte >= 0; byte--) {
    cbuttons[byte] = shiftIn(SWdata, SWclock, MSBFIRST);
    for(int i = 0, mask = 1; i < 8; i++, mask = mask << 1) {
      int index = i + (byte * 8);
      if ((cbuttons[byte] & mask) != (lbuttons[byte] & mask)) {
        lastDebounceTime[index] = millis();
      }
      if ((millis() - lastDebounceTime[index]) > debounceDelay) {
        if ((cbuttons[byte] & mask) != (buttons[byte] & mask)) {
          
          buttons[byte] ^= (-(cbuttons[byte] & mask) ^ buttons[byte]) & (1UL << i);
          if (buttons[byte] & mask) {
            // toggle LED
            leds[byte] ^= 1UL << i;
            int bindex = 15 - index;
            int value = (leds[byte] & mask) ? fadersUnstep[bindex]/8 : 0;
            usbMIDI.sendControlChange(cc[bindex], value, channel);
          }
        }
      }
    }
    lbuttons[byte] = cbuttons[byte];
  }

  LEDsr.setAll(leds);

  // get all fader values
  for (int count = 0; count <= 7; count++) {
    int cfader;

    r0 = bitRead(count, 0);
    r1 = bitRead(count, 1);
    r2 = bitRead(count, 2);

    digitalWrite(FAa, r0);
    digitalWrite(FAb, r1);
    digitalWrite(FAc, r2);

    //TODO - reduce to 0-127 range first
    for (int i = 0; i < 2; i++) {

      int readPin = i == 0 ? FAin1 : FAin2;
      cfader = analogRead(readPin);
      int bindex = count + (i*8);
      int index = 15 - bindex;
      if (faders[index] != cfader) {
        if (faders[index] / 8 != fadersUnstep[index] / 8) {
          
          if (diff(fadersUnstep[index], cfader) > 1) {
            fadersUnstep[index] = cfader;
            if (getBit(bindex, leds)) {
              usbMIDI.sendControlChange(cc[index], cfader/8, channel);
            }
          }
        }

        faders[index] = cfader;
      }
    }
  }

  for (int i = 0; i < 16; i++) {
    Serial.print(fadersUnstep[i]);
    Serial.print("\t");
  }
  Serial.println();

  // if different from stored value, send midi and update stored value
  
  // get all button values
  // if changed update stored value
  // if changed from 0 to 1 toggle led and send midi note
  
  // send all led values
  while (usbMIDI.read()) {
  }
}
