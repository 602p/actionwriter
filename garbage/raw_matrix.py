import serial, pygame, json

SQUARE_SIZE=60
WIDTH=8
HEIGHT=16
# mapping={}
with open("mapping.json", 'r') as fd:
	mapping={eval(k):v for k,v in json.load(fd).items()}
alphabet = "qwertyuiopasdfghjklzxcvbnm1234567890"
cache = []
zcache=[]



with serial.Serial("COM13", 115200) as conn:
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
		try:data = (list(map(int, conn.readline().decode("ascii", "ignore").strip().split(" "))))
		except BaseException as e:
			print("WARN: RX Error: "+str(e))
			continue
		# print(len(data))
		# if len(data)!=64:
		# 	print("WARN: Bad packet")
		# 	continue



		for idx, v in enumerate(data):
			# v=data[idx]
			j=(idx%8)
			i=(idx//8)
			pygame.draw.rect(screen, (0,0,0 if idx not in cache else 100) if not v else (0,150,0), (SQUARE_SIZE*j, SQUARE_SIZE*i, SQUARE_SIZE, SQUARE_SIZE))
			screen.blit(font.render(str(idx), False, (255,255,255)), (SQUARE_SIZE*j, SQUARE_SIZE*i))
			if idx in mapping:
				screen.blit(font.render(mapping[idx], False, (255,255,255)), (SQUARE_SIZE*j, SQUARE_SIZE*i+20))
			if v:
				cache.append(idx)
				zcache.append((i,j))
				for letter in alphabet:
					if pygame.key.get_pressed()[getattr(pygame, "K_"+letter)]:
						mapping[idx]=letter
			i+=1
		pygame.display.flip()

# with open("mapping.json", 'w') as fd:
# 	json.dump(mapping, fd, indent=4)
with open("all_outputs.json", 'w') as fd:
	json.dump(zcache, fd, indent=4)