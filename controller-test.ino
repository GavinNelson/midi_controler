1#include <MIDI.h>

 /*
 * codeexample for useing a 4051 * analog multiplexer / demultiplexer
 * by david c. and tomek n.* for k3 / malmï¿½ hï¿½gskola
 *
 * edited by Ross R.
 * edited by Igor de Oliveira SÃ¡.
 */  

int r0 = 0;      //value of select pin at the 4051 (s0)
int r1 = 0;      //value of select pin at the 4051 (s1)
int r2 = 0;      //value of select pin at the 4051 (s2)

int s0 = 10;//purple
int s1 = 11;//orange
int s2 = 8;//blue
int ctrlcount = 0;  //which pot
int val;
int vals[32];
int aPins[] = {21, 20, 19, 18};
bool change = false;

void setup(){  
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  for (int i = 0; i < sizeof(aPins) - 1; i ++) {
    pinMode(aPins[i], INPUT);
  }
}

void loop () {

  for (int muxcount = 0; muxcount <= 7; muxcount++) {

    setMuxChannel(muxcount);
    
    if (muxcount == 0) {
      if (change == true) {
        printVals();
      }
      change = false;
      ctrlcount = 0;
    }
    for (int incount = 0; incount <= sizeof(aPins) - 1; incount++) {
      val = analogRead(aPins[incount]);
      if (vals[ctrlcount] != val) {
  //    if (vals[count] + 1 < val || vals[count) - 1 > val || val = 1023) {
        change = true;
        vals[ctrlcount] = val;
        ctrlcount++;
      }
    }
  }  
}


void printVals() {
  for (int i = 0; i < sizeof(vals) - 1; i++) {
    Serial.print(vals[i]);//bitsi
    Serial.print("\t");
  }
  Serial.print("\n");
}

void setMuxChannel(int ch) {
      // select the bit
    r0 = bitRead(ch,0);
    r1 = bitRead(ch,1);
    r2 = bitRead(ch,2);

    digitalWrite(s0, r0);
    digitalWrite(s1, r1);
    digitalWrite(s2, r2);
}

