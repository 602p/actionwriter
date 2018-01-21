import json

with open("mapping.json", 'r') as infd:
	mapping = json.load(infd)

map_list = [None]*64
for k, v in mapping.items():
	map_list[int(k)]=v

with open("map_list.json", 'w') as outfd:
	json.dump({"default":map_list}, outfd, indent=4)