const int COL_ADDRESS_PINS[4] = {0, 1, 4, 5};
const int ROW_ADDRESS_PINS[3] = {6, 7, 8};
const int ROW_READ_PIN = A0;

void setup(){
	//Configure the pins. Switch circuit is closed when it's low, so set the
	// input pin to be high by default
	pinMode(ROW_READ_PIN, INPUT_PULLUP);
	for(int i=0; i<4; i++) pinMode(COL_ADDRESS_PINS[i], OUTPUT);
	for(int i=0; i<3; i++) pinMode(ROW_ADDRESS_PINS[i], OUTPUT);

	//Set up shift register. Nasal demons attack without this
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
	writeAllLEDS(0, 0);

	Serial.begin(115200);
	while (!Serial); //Initialize serial port and wait for connection
}

void loop(){
	Serial.print("[");
	for(int col=0; col<=15; col++){
		setColumnHigh(col);
		for(int row=0; row<=7; row++){
			Serial.print(readRow(row));
		}
	}
	Serial.println("]");
	
	delay(10); //Randomly determined
}

void setColumnHigh(int address){
	if(!((address>=0)&&(address<=15))){
		address=0; 
	}

	for(int i=0; i<4; i++)
		digitalWrite(COL_ADDRESS_PINS[i], bitRead(address, i));
}

bool readRow(int address){
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

