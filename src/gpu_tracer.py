
import ctypes
import matplotlib.pyplot as plt
import numpy as np
from dotenv import load_dotenv
import os
from PIL import Image

def trace():
	width = int(os.getenv('SCREEN_WIDTH'))
	height = int(os.getenv('SCREEN_HEIGHT'))

	tr = ctypes.CDLL('./pathtracer.so')
	tr.run.argtypes = []
	tr.run.restype = ctypes.POINTER(ctypes.c_uint)

	res = ctypes.POINTER(ctypes.c_uint)
	res = tr.run()

	array = np.zeros((width, height, 3), dtype='uint8')
	i = 0
	for y in range(height):
		for x in range(width):
			color = res[i]
			i += 1
			array[y][x][0] = (color >> 16) & 0xFF
			array[y][x][1] = (color >> 8) & 0xFF
			array[y][x][2] = color & 0xFF
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
