
import ctypes
#import matplotlib.pyplot as plt
import numpy as np
from dotenv import load_dotenv
import os
from PIL import Image

def trace():
	width = int(os.getenv('SCREEN_WIDTH'))
	height = int(os.getenv('SCREEN_HEIGHT'))
	print(width, height)

	tr = ctypes.CDLL('./pathtracer.so')
	tr.run.argtypes = []
	tr.get.argtypes = [ctypes.c_uint, ctypes.c_uint]
	tr.get.restype = ctypes.c_uint
	tr.end.argtypes = []

	tr.run()
	array = np.zeros((height, width, 3), dtype='uint8')
	for x in range(width):
		for y in range(height):
			color = tr.get(x, y)
			array[y][x][0] = (color >> 16) & 0xFF
			array[y][x][1] = (color >> 8) & 0xFF
			array[y][x][2] = color & 0xFF
	tr.end()
	return array

def render(array):
	im = Image.fromarray(array)
	im.save('image.jpg')

	plt.imshow(array)
	plt.show()

def main():
	array = trace()
	render(array)

if __name__ == '__main__':
	load_dotenv('.config')
	main()
