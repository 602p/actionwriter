import serial
import ctypes
import time
import random
import psutil
hllDll = ctypes.WinDLL ("User32.dll")
get_caplock = lambda: hllDll.GetKeyState(0x14)

get_numlock = lambda: hllDll.GetKeyState(0x90)

get_scrlock = lambda: hllDll.GetKeyState(0x91)

get_islocked = lambda: hllDll.GetForegroundWindow() == 0

get_isbatt  = lambda: not psutil.sensors_battery().power_plugged

get_hicpu = lambda: psutil.cpu_percent(percpu=False)>75

with serial.Serial("COM13", 115200) as conn:
	print("Opened")
	i=0
	while 1:
		if conn.inWaiting():
			print("-->", conn.readline().decode("ascii", "ignore")[:-1])
		# i=int(input(">"))
		byte = (i>=2, get_caplock(), get_numlock(), get_scrlock(), 0, get_islocked(), get_hicpu(), get_isbatt())
		num = sum(map(lambda t: 2**t[0] if t[1] else 0, enumerate(byte)))
		# print(bin(i))
		conn.write(bytes([num]))
		time.sleep(0.1)
		i+=1
		if i==4:i=0