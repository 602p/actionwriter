import serial
import ctypes
import time
import random
import psutil
import time
hllDll = ctypes.WinDLL ("User32.dll")
get_caplock = lambda: hllDll.GetKeyState(0x14)

get_numlock = lambda: hllDll.GetKeyState(0x90)

get_scrlock = lambda: hllDll.GetKeyState(0x91)

get_islocked = lambda: hllDll.GetForegroundWindow() == 0

get_isbatt  = lambda: not psutil.sensors_battery().power_plugged

def get_hicpu():
		c= psutil.cpu_percent(0.05)
		# print(c)
		return c>75		

last_num = None

with serial.Serial("COM13", 115200) as conn:
	print("Opened")
	while 1:
		if conn.inWaiting():
			print("-->", conn.readline().decode("ascii", "ignore")[:-1])
		# i=int(input(">"))
		byte = (int(round(time.time() * 1000))%500>250, get_caplock(), get_numlock(), get_scrlock(), 0, get_islocked(), get_hicpu(), get_isbatt())
		num = sum(map(lambda t: 2**t[0] if t[1] else 0, enumerate(byte)))
		if num==last_num: continue
		last_num=num
		conn.write(bytes([num]))