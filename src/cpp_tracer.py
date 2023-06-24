
import ctypes
import numpy as np
from dotenv import load_dotenv
import os
import time
from PIL import Image

class timeit():
	_begin = 0

	@staticmethod
	def start():
		timeit._begin = time.time()

	@staticmethod
	def end():
		print(time.time() - timeit._begin)

def trace():
	width = int(os.getenv('SCREEN_WIDTH'))
	height = int(os.getenv('SCREEN_HEIGHT'))
	print(width, height)

	tr = ctypes.CDLL('./pathtracer.so')
	tr.run.argtypes = []
	tr.run.restype = ctypes.POINTER(ctypes.c_uint)
	tr.end.argtypes = []

	res = ctypes.POINTER(ctypes.c_uint)
	timeit.start()
	res = tr.run()
	print(end='render: ')
	timeit.end()

	array = np.zeros((height, width, 3), dtype='uint8')
	i = 0

	timeit.start()
	for y in range(height):
		for x in range(width):
			color = res[i]
			i += 1
			array[y][x][0] = (color >> 16) & 0xFF
			array[y][x][1] = (color >> 8) & 0xFF
			array[y][x][2] = color & 0xFF
	tr.end()
	print(end='transfer: ')
	timeit.end()
	return array

def render(array):
	timeit.start()
	im = Image.fromarray(array)
	im.save('image.jpg')
	print(end='save: ')
	timeit.end()
	return

	plt.imshow(array)
	plt.show()

def main():
	array = trace()
	render(array)

if __name__ == '__main__':
	load_dotenv('.config')
	main()
