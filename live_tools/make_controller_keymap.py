from mapping import mapping
import json, sys

columns=[[] for _ in range(13)]
for pos in mapping.keys():
	columns[pos[0]].append(pos[1])

keymap=[]
for col, entries in enumerate(columns):
	for row in sorted(entries):
		keymap.append(mapping[(col, row)])

print(keymap)
print()

with open("defines.c", 'w') as fd:
	for idx, item in enumerate(keymap):
		fd.write("\n#define LK_"+item+" "+str(idx))

with open(sys.argv[1], 'r') as fd:
	layout=json.load(fd)

for_arduino=[]
for item in keymap:
	for_arduino.append(layout[item])

print(", ".join(for_arduino))