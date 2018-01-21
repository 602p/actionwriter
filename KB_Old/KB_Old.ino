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
  
  Serial.begin(115200);
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

String inString = "";
int v = 0;

void loop() {
//  digitalWrite(2, HIGH);
//  writeAddress(v);
//  while (Serial.available() > 0) {
//    int inChar = Serial.read();
//    inString += (char)inChar;
//    if (inChar == '\n') {
//      v=inString.toInt();
//      inString="";
//    }
//  }

  int i=0;
  while(i<13){
    writeAddress(i);
    int j=6;
    while(j<=12){
      Serial.print(digitalRead(j));
      Serial.print(" ");
        
      j++;
    }
    Serial.print(digitalRead(A0));
      Serial.print(" ");
//    delay(25);
    i++;
  }
  Serial.println("");
}
