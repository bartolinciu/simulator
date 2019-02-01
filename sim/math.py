from math import sqrt
import random
import sys

class vector:
	def __init__(self, x = 0, y = 0, z = 0, Random = False):
		if Random:
			self.x = random.random() - 0.5
			self.y = random.random() - 0.5
			self.z = random.random() - 0.5
			self.__itruediv__(self.length())
			return

		if type(x).__name__ == "list":
			self.x = x[0]
			self.y = x[1]
			self.z = x[2]
			return
		if type(x).__name__ == "vector":
			self.x = x.x
			self.y = x.y
			self.z = x.z
			return

		if type(x).__name__ == "tuple":
			x = point(x)
		if type(y).__name__ == "tuple":
			y = point(y)

		if type(x).__name__ == "point"  and type(y).__name__ == "point":
			self.x = y.x-x.x
			self.y = y.y-x.y
			self.z = y.z-x.z
			return


		self.x = x
		self.y = y
		self.z = z

	def __imul__(self, alpha):
		if not type(alpha).__name__ in [ "int", "float" ]:
			raise TypeError("unsupported operand type(s) for *=: 'vector' and '%s'" % type(alpha).__name__)
		self.x *= alpha
		self.y *= alpha
		self.z *= alpha
		return self

	def __idiv__(self, alpha):
		if not type(alpha).__name__ in [ "int", "float" ]:
			raise TypeError("unsupported operand type(s) for /=: 'vector' and '%s'" % type(alpha).__name__)
		self.x /= alpha
		self.y /= alpha
		self.z /= alpha
		return self

	def __itruediv__(self, alpha):
		if not type(alpha).__name__ in [ "int", "float" ]:
			raise TypeError("unsupported operand type(s) for /=: 'vector' and '%s'" % type(alpha).__name__)
		self.x /= alpha
		self.y /= alpha
		self.z /= alpha
		return self

	def __iadd__(self, v):
		if not type(v).__name__ in [ "list", "vector" ]:
			raise TypeError("unsupported operand type(s) for +=: 'vector' and '%s'" % type(alpha).__name__)
		if type(v).__name__ == "list":
			self.x += v[0]
			self.y += v[1]
			self.z += v[2]
		else:
			self.x += v.x
			self.y += v.y
			self.z += v.z
		return self

	def __isub__(self, v):
		if not type(v).__name__ in [ "list", "vector" ]:
			raise TypeError("unsupported operand type(s) for -=: 'vector' and '%s'" % type(alpha).__name__)
		if type(v).__name__ == "list":
			self.x -= v[0]
			self.y -= v[1]
			self.z -= v[2]
		else:
			self.x -= v.x
			self.y -= v.y
			self.z -= v.z
		return self

	def __mul__(self, alpha):
		if not type(alpha).__name__ in [ "int", "float" ]:
			raise TypeError("unsupported operand type(s) for *: 'vector' and '%s'" % type(alpha).__name__)
		return vector( self.x * alpha, self.y * alpha, self.z * alpha )

	def __div__(self, alpha):
		if not type(alpha).__name__ in [ "int", "float" ]:
			raise TypeError("unsupported operand type(s) for /: 'vector' and '%s'" % type(alpha).__name__)
		return vector( self.x / alpha, self.y / alpha, self.z / alpha )

	def __truediv__(self, alpha):
		if not type(alpha).__name__ in [ "int", "float" ]:
			raise TypeError("unsupported operand type(s) for /: 'vector' and '%s'" % type(alpha).__name__)
		return vector( self.x / alpha, self.y / alpha, self.z / alpha )


	def __add__(self, v):
		if not type(v).__name__ in ["list", "vector"]:
			raise TypeError("unsupported operand type(s) for +: 'vector' and '%s'" % type(alpha).__name__)
		if type(v).__name__ == "list":
			return vector( self.x + v[0], self.y + v[1], self.z + v[2] )
		else:
			return vector(self.x + v.x, self.y + v.y, self.z + v.z)

	def __sub__(self, v):
		if not type(v).__name__ in ["list", "vector"]:
			raise TypeError("unsupported operand type(s) for -: 'vector' and '%s'" % type(alpha).__name__)
		if type(v).__name__ == "list":
			return vector( self.x - v[0], self.y - v[1], self.z - v[2] )
		else:
			return vector(self.x - v.x, self.y - v.y, self.z - v.z)

	def __pos__(self):
		return self

	def __neg__(self):
		return self * -1

	def __round__(self, n):
		return self.__div__(self.length())
		
	def length(self):
		return sqrt( self.x**2 + self.y**2 + self.z**2 )

	def __str__(self):
		return str([self.x, self.y, self.z])

	def __iter__(self):
		yield self.x
		yield self.y
		yield self.z

class point:
	def __init__(self, x = 0, y = 0, z = 0):
		if type(x).__name__ == "tuple":
			self.x = x[0]
			self.y = x[1]
			self.z = x[2]
			return
		self.x = x
		self.y = y
		self.z = z

	def __iadd__( self, v ):
		if not type(v).__name__ in [ "list", "vector" ]:
			raise TypeError("unsupported operand type(s) for +=: 'point' and '%s'" % type(alpha).__name__)
		if type(v).__name__ == "vector":
			self.x += v.x
			self.y += v.y
			self.z += v.z
		else:
			self.x += v[0]
			self.y += v[1]
			self.z += v[2]
		return self

	def __isub__( self, v ):
		if not type(v).__name__ in [ "list", "vector" ]:
			raise TypeError("unsupported operand type(s) for -=: 'point' and '%s'" % type(alpha).__name__)
		if type(v).__name__ == "vector":
			self.x -= v.x
			self.y -= v.y
			self.z -= v.z
		else:
			self.x -= v[0]
			self.y -= v[1]
			self.z -= v[2]
		return self

	def __add__(self, v):
		if not type(v).__name__ in [ "list", "vector" ]:
			raise TypeError("unsupported operand type(s) for +: 'point' and '%s'" % type(alpha).__name__)
		if type(v).__name__ == "vector":
			return point( self.x + v.x, self.y + v.y, self.z + v.z )
		else:
			return point( self.x + v[0], self.y + v[1], self.z + v[2] )

	def __sub__(self, v):
		if not type(v).__name__ in [ "list", "vector" ]:
			raise TypeError("unsupported operand type(s) for +: 'point' and '%s'" % type(alpha).__name__)
		if type(v).__name__ == "vector":
			return point( self.x - v.x, self.y - v.y, self.z - v.z )
		else:
			return point( self.x - v[0], self.y - v[1], self.z - v[2] )

	def __str__(self):
		return str((self.x, self.y, self.z))

	def __iter__(self):
		yield self.x
		yield self.y
		yield self.z

v0 = vector(0,0,0)
O = point(0,0,0)

class segment:
	def __init__( self, a, b ):
		if type(a).__name__ == "tuple":
			a = point(a)

		if type(b).__name__ == "tuple":
			b = point(b)

		self.a = a
		self.b = b

	def length(self):
		return math.sqrt( (self.a.x-self.b.x)**2 + (self.a.y - self.b.y)**2 + (self.a.z - self.b.z)**2 )

if __name__ == "__main__":
	print( vector(1,1,1)/2 )
