from mapping import mapping

columns=[[] for _ in range(13)]
for pos in mapping.keys():
	columns[pos[0]].append(pos[1])

keymap=[]
for col, entries in enumerate(columns):
	for row in sorted(entries):
		keymap.append(mapping[(col, row)])

print(keymap)

for_arduino=[]
for item in keymap:
	if len(item)==1 and item in "abcdefghijklmnopqrstuvwxyz":
		for_arduino.append("'%s'"%item)
	else:
		for_arduino.append({
			"code": 	"KEY_LEFT_GUI",
			"mar_rel": 	"KEY_ESC",
			"l_mar": 	"'?'",
			"r_mar": 	"KEY_LEFT_ALT",
			"lock": 	"KEY_LEFT_CTRL",
			"t_set":	"'?'",
			"t_clr":	"'?'",
			"space":	"' '",
			"shift":	"KEY_LEFT_SHIFT",
			"backspace":"KEY_BACKSPACE",
			"return":	"KEY_RETURN",
			"comma":	"','",
			"period":	"'.'",
			"slash":	"'/'",
			"colon":	"':'",
			"quote":	"'\\''",
			"fraction":	"'['",
			"bracket":	"']'",
			"minus":	"'-'",
			"plus":		"'+'",
			"one":		"'1'",
			"two":		"'2'",
			"three":	"'3'",
			"four":		"'4'",
			"five":		"'5'",
			"six":		"'6'",
			"seven":	"'7'",
			"eight":	"'8'",
			"nine":		"'9'",
			"zero":		"'0'",
			"paper_up":	"'?'",
			"paper_down":"'?'",
			"margin_return":"'?'",
			"repeat":	"'?'",
			"line_space":"'?'",
			"delete":	"'?'",
			"tab":		"'\\t'",
			"topleft":	"'~'",
			"semicolon":"';'"
		}[item])

print(", ".join(for_arduino))