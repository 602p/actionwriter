#undef USE_NICOHOOD_HID

#ifdef USE_NICOHOOD_HID
	#include "HID-Project.h"
	#define _KEYBOARD BootKeyboard
#else
	#include <Keyboard.h>
	#define _KEYBOARD Keyboard
#endif

#include "defines.c"

const int COL_ADDRESS_PINS[4] = {2, 3, 4, 5};
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

#define LK_META LK_l_mar

const unsigned char DEFAULT_MAP[64] = {KEY_LEFT_GUI, ' ', KEY_END, 0 /*HWMeta*/,
	0 /*unused*/, KEY_DELETE, KEY_LEFT_CTRL, KEY_LEFT_ALT, '\t', KEY_ESC,
	0 /*unused*/, KEY_LEFT_SHIFT, 'z', 'q', '1', '`', 'a', 'x', 'w', '2',
	's', 'c', 'e', '3', 'd', 'b', 'v', 't', 'r', '4', '5', 'f', 'g', 'n', 'm',
	'y', 'u', '7', '6', 'j', 'h', ',', ']', 'i', '8', '+', 'k', '.', 'o', '9',
	'l', '/', '[', 'p', '0', '-', ';', '\'', KEY_INSERT, KEY_RETURN,
	KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_BACKSPACE, KEY_HOME};


const unsigned char ROT13_MAP[64]   = {KEY_LEFT_GUI, ' ', KEY_END, 0 /*HWMeta*/,
	0 /*unused*/, KEY_DELETE, KEY_LEFT_CTRL, KEY_LEFT_ALT, '\t', KEY_ESC,
	0 /*unused*/, KEY_LEFT_SHIFT, 'm', 'd', '6', '`', 'n', 'k', 'j', '7', 'f',
	'p', 'r', '8', 'q', 'o', 'i', 'g', 'e', '9', '0', 's', 't', 'a', 'z', 'l',
	'h', '2', '1', 'w', 'u', ',', ']', 'v', '3', '+', 'x', '.', 'b', '4', 'y',
	'/', '[', 'c', '5', '-', ';', '\'', KEY_INSERT, KEY_RETURN, KEY_PAGE_DOWN,
	KEY_PAGE_UP, KEY_BACKSPACE, KEY_HOME};

enum {
	M_DEFAULT,
	M_ROT13,
	M_SERIAL
} mode = M_DEFAULT;

bool dip_config[8];
#define CFG_usbkbd_on dip_config[0]
#define CFG_serial_on dip_config[1]

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

	bool config = 0;
	setColumnHigh(13);
	for(int i=0; i<8; i++){
		bool b = readRow(i);
		dip_config[i] = b;
	}

	int dip_config_int=0;
	for(int i=0; i<8; i++) bitWrite(dip_config_int, i, dip_config[i]);
	
	while(!CFG_serial_on && !CFG_usbkbd_on){
		writeAllLEDS(255, 255);
		delay(200);
		writeAllLEDS(0, 0);
		delay(200);
	}

	if(CFG_serial_on){
		Serial.begin(115200);
		while (!Serial){ //Initialize serial port and wait for connection
			writeAllLEDS(0, (char)((millis()%600)<200));
		}
		Serial.println("controller online");
		Serial.print("config bits: ");
		Serial.println(dip_config_int, BIN);
		Serial.println("(CFG_serial_on = 1)");
		Serial.print("(CFG_usbkbd_on = ");
		Serial.print(CFG_usbkbd_on);
		Serial.println(")");
	}
	if(CFG_usbkbd_on){
		_KEYBOARD.begin();
	}

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
				delayMicroseconds(100);
			}
		}
	}
	
	for(int i=0; i<64; i++){
		if(state[i] && !old_state[i]){
			keydown(i);
		}else if(!state[i] && old_state[i]){
			keyup(i);
		}
	}

	char leds1 = 0;
	char leds2 = 0;
	bitWrite(leds1, 0, CFG_usbkbd_on);
	bitWrite(leds2, 0, CFG_serial_on);
	bitWrite(leds1, 1, mode==M_DEFAULT);
	bitWrite(leds1, 2, mode==M_ROT13);
	bitWrite(leds1, 3, mode==M_SERIAL);

	#ifdef USE_NICOHOOD_HID
	if(CFG_serial_on){
		bitWrite(leds1, 5, _KEYBOARD.getLeds() & LED_CAPS_LOCK);
		bitWrite(leds1, 6, _KEYBOARD.getLeds() & LED_NUM_LOCK);
	}
	#endif

	writeAllLEDS(leds1, leds2);

	memcpy(old_state, state, sizeof state);

	delay(5);
}

void keydown(int code){
	if(mode==M_DEFAULT && state[LK_META] && code==LK_r){
		mode=M_ROT13;
		return;
	}

	if(mode==M_DEFAULT && state[LK_META] && code==LK_s){
		mode=M_SERIAL;
		return;
	}

	if(mode!=M_DEFAULT && state[LK_META] && code==LK_mar_rel){
		mode=M_DEFAULT;
		return;
	}

	if(code==LK_META) return;

	unsigned char mapped_code = xf_mapped(code);
	
	if(CFG_usbkbd_on && mode!=M_SERIAL){
		_KEYBOARD.press(mapped_code);
	}
	if(CFG_serial_on && mode==M_SERIAL){
		Serial.print("P");
		Serial.println(code);
	}
}

void keyup(int code){
	if(code==LK_META) return;

	unsigned char mapped_code = xf_mapped(code);

	if(CFG_usbkbd_on && mode!=M_SERIAL){
		_KEYBOARD.release(mapped_code);
	}
	if(CFG_serial_on && mode==M_SERIAL){
		Serial.print("R");
		Serial.println(code);
	}
}

unsigned char xf_mapped(int code){
	const unsigned char* active_map = mode==M_ROT13?ROT13_MAP:DEFAULT_MAP;
	unsigned char mapped_code = active_map[code];

	if(state[LK_META]){
		if     (code==LK_bracket) 	mapped_code='\\';
		else if(code==LK_k) 		mapped_code=KEY_UP_ARROW;
		else if(code==LK_j) 		mapped_code=KEY_DOWN_ARROW;
		else if(code==LK_h) 		mapped_code=KEY_LEFT_ARROW;
		else if(code==LK_l) 		mapped_code=KEY_RIGHT_ARROW;
		else if(code==LK_one) 		mapped_code=KEY_F1;
		else if(code==LK_two) 		mapped_code=KEY_F2;
		else if(code==LK_three) 	mapped_code=KEY_F3;
		else if(code==LK_four) 		mapped_code=KEY_F4;
		else if(code==LK_five) 		mapped_code=KEY_F5;
		else if(code==LK_six) 		mapped_code=KEY_F6;
		else if(code==LK_seven) 	mapped_code=KEY_F7;
		else if(code==LK_eight) 	mapped_code=KEY_F8;
		else if(code==LK_nine) 		mapped_code=KEY_F9;
		else if(code==LK_zero) 		mapped_code=KEY_F10;
		else if(code==LK_minus) 	mapped_code=KEY_F11;
		else if(code==LK_plus) 		mapped_code=KEY_F12;
	}

	return mapped_code;
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

void writeAllLEDS(byte computerLeds, byte localLeds){
	digitalWrite(12, LOW);
	shiftOut(10, 11, MSBFIRST, localLeds);
	shiftOut(10, 11, MSBFIRST, computerLeds);
	digitalWrite(12, HIGH);
}
