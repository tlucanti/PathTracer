
import ctypes
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

INT_MAX = 2 ** 32

class FLOAT_SEED(ctypes.Structure):
	_fields_ = [('number', ctypes.c_float),
		    ('seed', ctypes.c_uint)]

class FLOAT3_SEED(ctypes.Structure):
	_fields_ = [('x', ctypes.c_float),
		    ('y', ctypes.c_float),
		    ('z', ctypes.c_float),
		    ('seed', ctypes.c_uint)]

def distribution(name, array):
	plt.title(name)
	plt.grid()
	sns.kdeplot(array)
	plt.show()

def distribution3d(name, array):
	fig = plt.figure()
	plt.title(name)
	ax = fig.add_subplot(projection='3d')
	ax.scatter(array[:, 0], array[:, 1], array[:, 2])
	plt.show()

def nextRandomInt(times, seed):
	tr = ctypes.CDLL('./pathtracer.so')
	tr._call_nextRandomInt.argtypes = [ctypes.c_uint]
	tr._call_nextRandomInt.restype = ctypes.c_uint

	array = np.zeros(times, dtype=int)
	for i in range(times):
		seed = ctypes.c_uint(tr._call_nextRandomInt(seed)).value
		array[i] = seed

	distribution('nextRandomInt', array / INT_MAX)

def nextRandomFloat(times, seed):
	tr = ctypes.CDLL('./pathtracer.so')
	tr._call_nextRandomFloat.argtypes = [ctypes.c_uint]
	tr._call_nextRandomFloat.restype = FLOAT_SEED

	array = np.zeros(times)
	for i in range(times):
		res = tr._call_nextRandomFloat(seed)
		seed = res.seed
		array[i] = res.number

	distribution('nextRandomFloat', array)

def nextRandomFloatNeg(times, seed):
	tr = ctypes.CDLL('./pathtracer.so')
	tr._call_nextRandomFloatNeg.argtypes = [ctypes.c_uint]
	tr._call_nextRandomFloatNeg.restype = FLOAT_SEED

	array = np.zeros(times)
	for i in range(times):
		res = tr._call_nextRandomFloatNeg(seed)
		seed = res.seed
		array[i] = res.number

	distribution('nextRandomFloatNeg', array)

def randomDirection(times, seed):
	tr = ctypes.CDLL('./pathtracer.so')
	tr._call_randomDirection.argtypes = [ctypes.c_uint]
	tr._call_randomDirection.restype = FLOAT3_SEED

	array = np.zeros((times, 3))
	for i in range(times):
		res = tr._call_randomDirection(seed)
		seed = res.seed
		array[i][0] = res.x
		array[i][1] = res.y
		array[i][2] = res.z

	distribution3d('randomDirection', array)

def randomHemiSphere(times, seed):
	tr = ctypes.CDLL('./pathtracer.so')
	tr._call_randomHemiSphere.argtypes = [ctypes.c_uint]
	tr._call_randomHemiSphere.restype = FLOAT3_SEED

	array = np.zeros((times, 3))
	for i in range(times):
		res = tr._call_randomHemiSphere(seed, 0, 1, 0)
		seed = res.seed
		array[i][0] = res.x
		array[i][1] = res.y
		array[i][2] = res.z

	distribution3d('randomHemiSphere', array)

def main():
	#nextRandomInt(10000, 123)
	#nextRandomFloat(10000, 123)
	#nextRandomFloatNeg(10000, 123)
	randomDirection(5000, 123123)
	randomHemiSphere(5000, 121343)

if __name__ == '__main__':
	main()
