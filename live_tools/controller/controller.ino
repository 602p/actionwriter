#include <Keyboard.h>
#include "defines.c"

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

#define USERKEY_MACRO1 1
#define USERKEY_MACRO2 2

#define LK_META LK_l_mar

const unsigned char DEFAULT_MAP[64] = {KEY_LEFT_GUI, ' ', KEY_END, 0 /*HWMeta*/, USERKEY_MACRO2, KEY_DELETE, KEY_LEFT_CTRL, KEY_LEFT_ALT, '\t', KEY_ESC, USERKEY_MACRO1, KEY_LEFT_SHIFT, 'z', 'q', '1', '`', 'a', 'x', 'w', '2', 's', 'c', 'e', '3', 'd', 'b', 'v', 't', 'r', '4', '5', 'f', 'g', 'n', 'm', 'y', 'u', '7', '6', 'j', 'h', ',', ']', 'i', '8', '+', 'k', '.', 'o', '9', 'l', '/', '[', 'p', '0', '-', ';', '\'', KEY_INSERT, KEY_RETURN, KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_BACKSPACE, KEY_HOME};
const unsigned char ROT13_MAP[64]   = {KEY_LEFT_GUI, ' ', KEY_END, 0 /*HWMeta*/, USERKEY_MACRO2, KEY_DELETE, KEY_LEFT_CTRL, KEY_LEFT_ALT, '\t', KEY_ESC, USERKEY_MACRO1, KEY_LEFT_SHIFT, 'm', 'd', '6', '`', 'n', 'k', 'j', '7', 'f', 'p', 'r', '8', 'q', 'o', 'i', 'g', 'e', '9', '0', 's', 't', 'a', 'z', 'l', 'h', '2', '1', 'w', 'u', ',', ']', 'v', '3', '+', 'x', '.', 'b', '4', 'y', '/', '[', 'c', '5', '-', ';', '\'', KEY_INSERT, KEY_RETURN, KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_BACKSPACE, KEY_HOME};

enum {
	M_DEFAULT,
	M_ROT13
} mode = M_DEFAULT;


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

	// Serial.begin(115200);
	// while (!Serial); //Initialize serial port and wait for connection
	Keyboard.begin();

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

	char leds = 0;
	bitWrite(leds, 0, state[LK_META]);
	bitWrite(leds, 1, mode==M_DEFAULT);
	bitWrite(leds, 2, state[LK_space]);
	writeAllLEDS(leds, 0);
	
	for(int i=0; i<64; i++){
		if(state[i] && !old_state[i]){
			keydown(i);
		}else if(!state[i] && old_state[i]){
			keyup(i);
		}
	}

	memcpy(old_state, state, sizeof state);
	delay(5);
}

void keydown(int code){
	if(mode==M_DEFAULT && state[LK_META] && code==LK_r){
		mode=M_ROT13;
		return;
	}

	if(mode==M_ROT13 && state[LK_META] && (code==LK_r || code==LK_mar_rel)){
		mode=M_DEFAULT;
		return;
	}

	if(code==LK_META) return;

	const unsigned char* active_map = mode==M_DEFAULT?DEFAULT_MAP:ROT13_MAP;
	Keyboard.press(active_map[code]);
}

void keyup(int code){
	if(code==LK_META) return;

	const unsigned char* active_map = mode==M_DEFAULT?DEFAULT_MAP:ROT13_MAP;
	Keyboard.release(active_map[code]);
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

