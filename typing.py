import serial, pygame, json, os

with open("full_map_list.json", 'r') as fd:
	mapping=json.load(fd)

pressed = []
shift_key = mapping["default"].index("shift")
code_key = mapping["default"].index("code")

text=""

with serial.Serial("COM10", 2000000) as conn:
	conn.readline()
	conn.readline()

	while 1:
		try:data = (list(map(lambda a:not int(a), conn.readline().decode("ascii", "ignore").strip().split(" "))))
		except ValueError as e:
			print("WARN: RX Error: "+str(e))
			continue

		if len(data)!=64:
			print("WARN: Bad packet")
			continue
			
		matrix = []
		for i in range(64):
			lc_key = mapping["default"][i]
			if data[i] and lc_key not in pressed:
				if lc_key=="backspace":
					text = text[:-1]
				elif lc_key=="return":
					text+="\n"
				elif len(lc_key)==1:
					text+=mapping["shifted" if "shift" in pressed else "default"][i]
				os.system("cls")
				print(text)
				# print(pressed)
				pressed.append(lc_key)
			elif not data[i] and lc_key in pressed:
				pressed.remove(lc_key)

