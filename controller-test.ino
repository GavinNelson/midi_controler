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
int count = 0;   //which y pin we are selecting
int val;
int vals[8];
int aPin = 21;
bool change = false;
int acount;

void setup(){  
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(aPin, INPUT);
}

void loop () {

  for (count=0; count<=7; count++) {

    // select the bit  
    r0 = bitRead(count,0);    // use this with arduino 0013 (and newer versions)     
    r1 = bitRead(count,1);    // use this with arduino 0013 (and newer versions)     
    r2 = bitRead(count,2);    // use this with arduino 0013 (and newer versions)     

    //r0 = count & 0x01;      // old version of setting the bits
    //r1 = (count>>1) & 0x01;      // old version of setting the bits
    //r2 = (count>>2) & 0x01;      // old version of setting the bits

    digitalWrite(s0, r0);
    digitalWrite(s1, r1);
    digitalWrite(s2, r2);

    //Either read or write the multiplexed pin here
    val = analogRead(aPin)/8;
    
    if (count == 0) {
      if (change == true) {
        printArray(vals);
      }
      change = false;
    }
    if (vals[count] != val) {
//    if ((vals[count] + 1) < val || (vals[count] - 1) > val) {
      change = true;
      vals[count] = val;
    }
  }  
}


void printArray(int arr[]) {
  for (acount = 0; acount < 7; acount++) {
    Serial.print(arr[acount]);
    Serial.print("\t");
  }
  Serial.print("\n");
}

