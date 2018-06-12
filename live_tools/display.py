import serial, pygame, json

import mapping

SQUARE_SIZE=60
WIDTH=16
HEIGHT=8

def chunks(l, n):
	"""Yield successive n-sized chunks from l."""
	for i in range(0, len(l), n):
		yield l[i:i + n]

seen=list(mapping.mapping.keys())

with serial.Serial("/dev/ttyACM0", 115200) as conn:
	conn.readline()
	conn.readline()

	pygame.init()
	fontb = pygame.font.SysFont("Arial", 12, True)
	font = pygame.font.SysFont("Arial", 14, False)
	screen = pygame.display.set_mode((13*SQUARE_SIZE, HEIGHT*SQUARE_SIZE))
	run=1
	while run:
		screen.fill((0,0,0))
		data=conn.readline().decode("ascii", "ignore").strip()
		if data[0]=="[" and data[-1]=="]":
			data=data[1:-1]
			if len(data)==WIDTH*HEIGHT:
				columns = list(chunks(list(map(int, data)),8))
				for col, lst in enumerate(columns):
					for row, v in enumerate(lst):
						if v:
							color = (0,200,0)
							if not (col, row) in seen:
								seen.append((col, row))
						elif (col, row) in seen:
							color = (100, 100, 100)
						else:
							color = (30, 30, 30)
						pygame.draw.rect(screen, color, (SQUARE_SIZE*col, SQUARE_SIZE*row, SQUARE_SIZE, SQUARE_SIZE))
						screen.blit(font.render(str((col, row)), False, (200,200,200)), (SQUARE_SIZE*col, SQUARE_SIZE*row))
						screen.blit(fontb.render(mapping.mapping.get((col, row), ""), False, (255,255,255)), (SQUARE_SIZE*col, SQUARE_SIZE*row+15))
			else: print('len err')
		else: print('fmt err')

		for e in pygame.event.get():
			if e.type==pygame.KEYDOWN and e.key==pygame.K_q:
				run=False
		pygame.display.flip()
		# pygame.image.save(screen, "map.png")
		# run=False
