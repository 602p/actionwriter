import json

with open("all_outputs.json", 'r') as fd:
	outputs = set(tuple(i) for i in json.load(fd))

print(len(outputs))
print(outputs)

WIDTH=8
HEIGHT=13

masks=[]
for row in range(HEIGHT):
	items_in_row = list(filter(lambda i:i[0]==row, outputs))
	print(row, items_in_row)
	v=0
	for col in range(WIDTH):
		if (row, col) in items_in_row:
			v+=2**col
	masks.append(v)

# masks[0]-=1
# print(len(masks))

with open("KB_New/kb_interest_mask.ino", 'w') as fd:
	fd.write("int kb_matrix_mask[13]={\n")
	fd.write(",\n".join(map(str, masks)))
	fd.write("};\n")