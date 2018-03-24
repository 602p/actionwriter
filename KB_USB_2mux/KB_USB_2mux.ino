#include <Keyboard.h>

extern int kb_matrix_mask[13];
extern char kb_layout[64];

bool state[64];
bool old_state[64];

void setup() {
  // put your setup code here, to run once:
  int i=2;
  while(i<=8){
    pinMode(i, OUTPUT);
    i++;
  }
  pinMode(9, INPUT_PULLUP);

  pinMode(A1, INPUT_PULLUP);
  if(!digitalRead(A1)){while(1){
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
    delay(250);
   }}

  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  
  
  Serial.begin(115200);
  while(!Serial){}
  Serial.println("Ready_OK_v2");
  Keyboard.begin();

  memset(state, 0, 64);
  memset(old_state, 0, 64);
}

void writeLeftAddress(int address){
  if(!((address>=0)&&(address<=15))){
    address=0; 
  }

  digitalWrite(2,  bitRead(address, 0));
  digitalWrite(3,  bitRead(address, 1));
  digitalWrite(4,  bitRead(address, 2));
  digitalWrite(5,  bitRead(address, 3));
}

void writeRightAddress(int address){
  if(!((address>=0)&&(address<=7))){
    address=0; 
  }

  digitalWrite(6,  bitRead(address, 0));
  digitalWrite(7,  bitRead(address, 1));
  digitalWrite(8,  bitRead(address, 2));
}

void writeUnicode(String value){
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(224);
  delay(1);
  Keyboard.release(224);
  int i=0;
  int v;
  Serial.println("Typing "+value);
  while(i<value.length()){
    if(value[i]=='0') v=234;
    else if(value[i]>=49 && value[i]<=57) v=value[i]-49+224+1;
    else v=value[i];
    Keyboard.press(v);
    delay(1);
    Keyboard.release(v);
    Serial.println(v, DEC);
    i++;
  }
  Keyboard.release(KEY_LEFT_ALT);
}

int rot13(int c){
  if('a' <= c && c <= 'z'){
    return rot13b(c,'a');
  } else if ('A' <= c && c <= 'Z') {
    return rot13b(c, 'A');
  } else {
    return c;
  }
}

int rot13b(int c, int basis){
  c = (((c-basis)+13)%26)+basis;
  return c;
}

byte computerLeds = 0;
byte localLeds = 0;
bool fullWidthMode = false;
bool rot13Mode = false;

void loop() {
  int i=0;
  int z=0;
  while(i<13){
    writeLeftAddress(i);
    int j=0;
    while(j<8){
      writeRightAddress(j);
      if(bitRead(kb_matrix_mask[i], j)){
        state[z]=!digitalRead(9);
        z++;
      }
      j++;
    }
    i++;
  }

  int a=0;
  int mapped_key;
  while(a<64){
    if(a==62) mapped_key=state[3]?KEY_DELETE:KEY_BACKSPACE;
    else if(a==59) mapped_key=KEY_RETURN;
    else if(a==11) mapped_key=KEY_LEFT_SHIFT;
    else if(a==8) mapped_key=KEY_TAB;
    else if(a==0) mapped_key=KEY_LEFT_CTRL;
    else if(a==7) mapped_key=KEY_LEFT_GUI;
    else if(a==10) mapped_key=KEY_LEFT_ALT;
    else if(a==9) mapped_key=KEY_ESC;
    else if(a==6) mapped_key=KEY_CAPS_LOCK;
    else if(a==61) mapped_key=KEY_PAGE_UP;
    else if(a==60) mapped_key=KEY_PAGE_DOWN;
    else if(a==63) mapped_key=KEY_HOME;
    else if(a==2) mapped_key=KEY_END;
    else if(state[3]){ //L_MAR
      if(a==40) mapped_key=KEY_LEFT_ARROW;
      else if(a==50) mapped_key=KEY_RIGHT_ARROW;
      else if(a==46) mapped_key=KEY_UP_ARROW;
      else if(a==39) mapped_key=KEY_DOWN_ARROW;
      else{
        char keycap = kb_layout[a];
        if(keycap=='1') mapped_key=KEY_F1;
        else if(keycap=='2') mapped_key=KEY_F2;
        else if(keycap=='3') mapped_key=KEY_F3;
        else if(keycap=='4') mapped_key=KEY_F4;
        else if(keycap=='5') mapped_key=KEY_F5;
        else if(keycap=='6') mapped_key=KEY_F6;
        else if(keycap=='7') mapped_key=KEY_F7;
        else if(keycap=='8') mapped_key=KEY_F8;
        else if(keycap=='9') mapped_key=KEY_F9;
        else if(keycap=='0') mapped_key=KEY_F10;
        else if(keycap=='-') mapped_key=KEY_F11;
        else if(keycap=='=') mapped_key=KEY_F12;
        else if(keycap==']') mapped_key='\\';
        else mapped_key=0;
      }
    }
    else mapped_key = kb_layout[a];

    if(state[a] && !old_state[a]){
      if(state[3] && kb_layout[a]=='w'){
        fullWidthMode=!fullWidthMode;
        if(fullWidthMode) Serial.println("START FULLWIDTH");
        else Serial.println("STOP FULLWIDTH");
      }
      else if(fullWidthMode){
        Serial.print("PRESS ");
        Serial.println(a, DEC);
      }
      else if(state[3] && kb_layout[a]=='r'){
        rot13Mode = !rot13Mode;
      }
      else if(a==4) writeUnicode("00b2");
      else if(mapped_key!=0){
        if(rot13Mode){
          mapped_key=rot13(mapped_key);
        }
        Keyboard.press(mapped_key);
      }
    }else if(!state[a] && old_state[a]){
      if(fullWidthMode){
        Serial.print("RELEASE ");
        Serial.println(a, DEC);
      }
      else if(mapped_key!=0){
        if(rot13Mode){
          mapped_key=rot13(mapped_key);
        }
        Keyboard.release(mapped_key);
      }
    }

    a++;
  }
  memcpy(old_state, state, sizeof state);
  delay(10);

  if(Serial.available()){
    while(Serial.available()>1) Serial.read();
    computerLeds = Serial.read();
  }

  bitWrite(localLeds, 0, state[3]);
  bitWrite(localLeds, 1, fullWidthMode);
  bitWrite(localLeds, 2, rot13Mode);

  digitalWrite(12, LOW);
  shiftOut(10, 11, MSBFIRST, localLeds);
  shiftOut(10, 11, MSBFIRST, computerLeds);
  digitalWrite(12, HIGH);
}
