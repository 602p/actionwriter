#define PIN_DATA  2
#define PIN_LATCH 3
#define PIN_CLOCK 4

void setup() {
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
}

int i=0;
void loop() {
  digitalWrite(PIN_LATCH, LOW);
  delay(1);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, (i>>8)&255);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, i&255);
  if(i==65535) i=0;
  else i++;
  digitalWrite(PIN_LATCH, HIGH);
  delay(10);
}
