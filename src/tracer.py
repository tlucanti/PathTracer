
import ctypes
import matplotlib.pyplot as plt
import numpy as np
from dotenv import load_dotenv
import os

def trace():
	width = int(os.getenv('SCREEN_WIDTH'))
	height = int(os.getenv('SCREEN_HEIGHT'))

	tr = ctypes.CDLL('./pathtracer.so')

	tr.run()
	array = np.zeros((width, height, 3), dtype=int)
	for x in range(width):
		for y in range(height):
			color = tr.get(x, y)
			array[y][x][0] = (color >> 16) & 0xFF
			array[y][x][1] = (color >> 8) & 0xFF
			array[y][x][2] = color & 0xFF
	tr.end()
	return array

def render(array):
	plt.imshow(array)
	plt.show()

def main():
	array = trace()
	render(array)

if __name__ == '__main__':
	load_dotenv('.config')
	main()
