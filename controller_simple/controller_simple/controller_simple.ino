const int COL_ADDRESS_PINS[4] = {0, 1, 4, 5};
const int ROW_ADDRESS_PINS[3] = {6, 7, 8};
const int ROW_READ_PIN = A0;

const unsigned char MASK[13] = {
	0b00000001,
	0b10001011,
	0b01101111,
	0b00000001,
	0b01111010,
	0b01011010,
	0b01011010,
	0b11111111,
	0b11111111,
	0b01111110,
	0b01011010,
	0b11111101,
	0b01111011
};

bool state[64];
bool old_state[64];

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

	memset(old_state, 0, sizeof state);
}

void loop(){
	memset(state, 0, sizeof state);
	int state_pos = 0;

	for(int col=0; col<13; col++){
		setColumnHigh(col); //Have to do this because we have to scan all columns
		for(int row=0; row<8; row++){
			if(bitRead(MASK[col], row)){
				state[state_pos] = readRow(row);
				state_pos++;
			}
		}
	}
	
	for(int i=0; i<64; i++){
		Serial.print(state[i]);
	}
	Serial.println("");
}

void setColumnHigh(int address){
	for(int i=0; i<4; i++)
		digitalWrite(COL_ADDRESS_PINS[i], bitRead(address, i));
}

bool readRow(int address){
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

