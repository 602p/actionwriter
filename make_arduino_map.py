import json

with open("full_map_list.json", 'r') as fd:
	mapping=json.load(fd)

def write_map(mapping, fd):
	fd.write("{")
	fd.write(",".join(map(lambda x:"'"+(str(x).replace("'","\\'") if len(x)==1 else "\\0")+"'", mapping)))
	fd.write("}")

with open("KB_USB/kb_layout.ino", 'w') as fd:
	fd.write("char kb_layout[64]=")
	write_map(mapping["default"], fd)
	# fd.write(",")
	# write_map(mapping["shifted"], fd)
	fd.write(";")