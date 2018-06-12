// #include <Keyboard.h>

const int COL_ADDRESS_PINS[4] = {0, 1, 4, 5};
const int ROW_ADDRESS_PINS[3] = {6, 7, 8};
const int ROW_READ_PIN = A0;

void setup(){
	//Configure the pins. Switch circuit is closed when it's low, so set the
	// input pin to be high by default
	// int i;
	for(int i=0; i<4; i++) pinMode(COL_ADDRESS_PINS[i], OUTPUT);
	for(int i=0; i<3; i++) pinMode(ROW_ADDRESS_PINS[i], OUTPUT);
	pinMode(ROW_READ_PIN, INPUT_PULLUP);

	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);

	Serial.begin(115200);
	while (!Serial); //Initialize serial port and wait for connection
	// Keyboard.begin();
	writeAllLEDS(0, 0);
}

void loop(){
	Serial.print("[");
	for(int col=0; col<=15; col++){
		writeLeftAddress(col);
		for(int row=0; row<=7; row++){
			Serial.print(writeRightAddress(row));
		}
	}
	Serial.println("]");
	
	delay(10); //Randomly determined
}

void writeLeftAddress(int address){
	if(!((address>=0)&&(address<=15))){
		address=0; 
	}

	for(int i=0; i<4; i++)
		digitalWrite(COL_ADDRESS_PINS[i], bitRead(address, i));
}

bool writeRightAddress(int address){
	if(!((address>=0)&&(address<=7))){
		address=0; 
	}

	for(int i=0; i<3; i++)
		digitalWrite(ROW_ADDRESS_PINS[i], bitRead(address, i));

	return !digitalRead(ROW_READ_PIN);
}

void writeAllLEDS(byte localLeds, byte computerLeds){
	digitalWrite(12, LOW);
	shiftOut(10, 11, MSBFIRST, localLeds);
	shiftOut(10, 11, MSBFIRST, computerLeds);
	digitalWrite(12, HIGH);
}

