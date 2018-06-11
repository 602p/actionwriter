import serial, pygame, json

SQUARE_SIZE=60
WIDTH=8
HEIGHT=(65//8)
# mapping={}
with open("mapping.json", 'r') as fd:
	mapping={eval(k):v for k,v in json.load(fd).items()}
alphabet = "qwertyuiopasdfghjklzxcvbnm1234567890"
cache = set()



with serial.Serial("COM13", 9600) as conn:
	conn.readline()
	conn.readline()

	pygame.init()
	font = pygame.font.SysFont("monospace", 14)
	screen = pygame.display.set_mode((WIDTH*SQUARE_SIZE, HEIGHT*SQUARE_SIZE))
	run=1
	while run:
		for e in pygame.event.get():
			if e.type==pygame.QUIT:
				run=False
		screen.fill((0,0,0))
		try:data = (list(map(lambda a:int(a), conn.readline().decode("ascii", "ignore").strip().split(" "))))
		except BaseException as e:
			print("WARN: RX Error: "+str(e))
			continue
		print(len(data))
		if len(data)!=64:
			print("WARN: Bad packet")
			continue
		print(data)

		for idx in range(len(data)):
			v=data[idx]
			j=(idx%8)
			i=(idx//8)
			pygame.draw.rect(screen, (0,0,0 if idx not in cache else 100) if not v else (0,150,0), (SQUARE_SIZE*j, SQUARE_SIZE*i, SQUARE_SIZE, SQUARE_SIZE))
			screen.blit(font.render(str(idx), False, (255,255,255)), (SQUARE_SIZE*j, SQUARE_SIZE*i))
			if idx in mapping:
				screen.blit(font.render(mapping[idx], False, (255,255,255)), (SQUARE_SIZE*j, SQUARE_SIZE*i+20))
			if v:
				cache&={idx}
				for letter in alphabet:
					if pygame.key.get_pressed()[getattr(pygame, "K_"+letter)]:
						mapping[idx]=letter
			i+=1
		pygame.display.flip()

mapping_o = {}
for i in sorted(mapping.keys()):
	mapping_o[i]=mapping[i]

# with open("mapping.json", 'w') as fd:
# 	json.dump(mapping_o, fd, indent=4)
# with open("all_outputs.json", 'w') as fd:
# 	json.dump(cache, fd, indent=4)