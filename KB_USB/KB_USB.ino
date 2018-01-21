#include <Keyboard.h>

extern int kb_matrix_mask[13];
extern char kb_layout[64];

String inString = "";
int v = 0;
bool state[64];
bool old_state[64];

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  
  int i=6;
  while(i<=12){
    pinMode(i, INPUT_PULLUP);
    i++;
  }

  pinMode(A0, INPUT_PULLUP);

  pinMode(A1, INPUT_PULLUP);
  if(!digitalRead(A1)){while(1){}}
  
  Keyboard.begin();
  Serial.begin(9600);
  while(!Serial){}
  Serial.println("Ready");

  memset(state, 0, 64);
  memset(old_state, 0, 64);
}

void writeAddress(int address){
  if(!((address>=0)&&(address<=15))){
    address=0; 
  }

  digitalWrite(2, bitRead(address, 0));
  digitalWrite(3,  bitRead(address, 1));
  digitalWrite(4,  bitRead(address, 2));
  digitalWrite(5,  bitRead(address, 3));

  
}



void loop() {
  int i=0;
  int z=0;
  while(i<13){
    writeAddress(i);
    int j=6;
    while(j<=12){
      if(bitRead(kb_matrix_mask[i], j-6)){
        state[z]=!digitalRead(j);
        z++;
      }
        
      j++;
    }
     if(bitRead(kb_matrix_mask[i], 13-6)){
      state[z]=!digitalRead(A0);
      z++;
    }
    i++;
  }

  int a=0;
  int mapped_key;
  while(a<64){
    if(a==62) mapped_key=KEY_BACKSPACE;
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

    
    if(mapped_key!=0){
      if(state[a] && !old_state[a]){
        Keyboard.press(mapped_key);
        Serial.print("Press: ");
        Serial.println(mapped_key);
      }else if(!state[a] && old_state[a]){
        Keyboard.release(mapped_key);
        Serial.print("Release: ");
        Serial.println(mapped_key);
      }
    }
    a++;
  }
  memcpy(old_state, state, sizeof state);
  delay(10);

//  int b=0;
//  while(b<64){
//    Serial.print(state[b], DEC);
//    Serial.print(" ");
//    b++;
//  }
//  Serial.println("");
  
}
