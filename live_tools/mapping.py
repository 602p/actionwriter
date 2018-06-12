
with open("map.txt", 'r') as fd:
	contents = fd.readlines()

mapping = {}
for line in contents:
	row, col, name = line.strip().split("\t")
	pos = (int(row), int(col))
	if pos in mapping:
		print("Collision!", pos)
	mapping[pos] = name