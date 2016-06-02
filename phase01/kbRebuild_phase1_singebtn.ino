/***
==============================================================
mkRebuild

Initial testing a single mechanical switch with arduino.
Code starts up a 1x1 matrix scan to determine the states
the switch currently has. It also performs debouncing during
the scan to remove any possible noise (there was during
development). Lastly, events was developed for possible 
future uses, Down, Up, Click, Double Click and Long Press.

Mechanical Keyboard Project from Sketchpunk Labs
Developer : Vor
==============================================================
***/

const int EVT_DOWN = 1;
const int EVT_UP = 0;
const int EVT_DBL = 2;
const int EVT_HOLD = 3;

const int DEBOUNCE = 50;
const int CLICK_THRESHOLD = 90;
const int DCLICK_THRESHOLD = 170;
const int HOLD_THRESHOLD = 500;

int colpin[] = {8};
int rowpin[] = {7};
const int ledPin = 6;
const int colLen = sizeof(colpin) / sizeof(int);
const int rowLen = sizeof(rowpin) / sizeof(int);
const int btnLen = colLen * rowLen;

bool ledState = false;
int btnState[btnLen];
int btnEvent[btnLen];
unsigned long btnStateTime[btnLen];
unsigned long btnDownTime[btnLen];

void setup() {
  //Test only code
  Serial.begin(9600);

  //set the state of all buttons
  memset(btnState,0,btnLen); 
  memset(btnEvent,0,btnLen);

  //setup pins
  pinMode(colpin[0],OUTPUT);
  pinMode(rowpin[0],INPUT);
  pinMode(ledPin,OUTPUT);
  
  //set pin defaults
  digitalWrite(colpin[0],LOW);
}

void loop(){
  int chk;
  bool isHigh = false;
  int pos;
  
  for(int c=0; c < colLen; c++){
    digitalWrite(colpin[c],HIGH);

    for(int r=0; r < rowLen; r++){      
      //isHigh = (digitalRead(rowpin[r]) == HIGH);
      //pos = (c+1) * r;
      //checkState(pos,isHigh);
      checkState((c+1) * r, digitalRead(rowpin[r]));
    }//for
    
    digitalWrite(colpin[c],LOW);   
  }//for
  //delay(100);
}//func

bool checkState(int pos, int state){
  if(btnState[pos] != state){
    btnState[pos] = state;
    btnStateTime[pos] = millis();
    return false;
  }//if

  unsigned long delta = millis() - btnStateTime[pos];
  if(delta < DEBOUNCE) return false;

  delta = millis() - btnDownTime[pos];

  if(btnEvent[pos] == EVT_UP && state == HIGH){
    Serial.print("Button Down - ");
    Serial.print(delta);

    if(delta <= DCLICK_THRESHOLD){
      Serial.print(" - DBL CLICK ");
      btnEvent[pos] = EVT_DBL;
    }else btnEvent[pos] = EVT_DOWN;
    
    Serial.print("\n");
    
    btnDownTime[pos] = millis();
    return true;
  }else if(btnEvent[pos] != EVT_UP && state == LOW){
    Serial.print("Button UP - ");
    Serial.print(delta);

    if(btnEvent[pos] == EVT_DOWN && delta >= CLICK_THRESHOLD) Serial.print(" - CLICK ");
    
    Serial.print("\n\n");
    
    btnEvent[pos] = EVT_UP;
    return true;
  }else if(btnEvent[pos] == EVT_DOWN && state == HIGH && delta >= HOLD_THRESHOLD){
    Serial.print("------------------- LONG HOLD - ");
    Serial.print(delta);
    Serial.print("\n");

    digitalWrite(ledPin, ( (ledState = !ledState) )?HIGH:LOW);
    btnEvent[pos] = EVT_HOLD;
    return true;
  }//if
  
  return false;
}//func

