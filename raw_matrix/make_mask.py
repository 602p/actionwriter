from mapping import mapping #Map of (col, row) to key name

columns=[[] for _ in range(13)]
for pos in mapping.keys():
	columns[pos[0]].append(pos[1])

masks=[]
for col in columns:
	masks.append([i in col for i in range(8)])

bitmasks=[]
for mask in masks:
	bitmasks.append("0b"+"".join(map(str, map(int, mask))))

print(bitmasks)