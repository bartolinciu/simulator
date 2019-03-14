#!/usr/bin/python3
import sim
import sys
import random
import math

def cbrt(a):
	return a**(1/3)

if not len(sys.argv) in [ 3, 5 ]:
	print( "usage: python3 generate_fcc.py a [, b, c ] output_file" )
	exit(1)

if len(sys.argv) == 3:
	a = int(sys.argv[1])
	b = a
	c = b
else:
	a = int(sys.argv[1])
	b = int(sys.argv[2])
	c = int(sys.argv[3])

output_file = sys.argv[-1]

n = a*b*c*4


vs = [ 1, 1.22, 0.78, 1.13, 0.87 ]
nv = [ n-2*( int(math.ceil(0.1*n)) + int(math.ceil(0.2*n))) , int(math.ceil(0.2 * n)), int(math.ceil(0.2 * n)), int(math.ceil(0.1 * n)), int(math.ceil(0.1 * n)) ]


def get_random_speed():
	global nv
	i = random.randint(0, len(vs)-1)
	v = vs[i]
	nv[i] -= 1
	if nv[i] == 0:
		nv.remove(nv[i])
		vs.remove(vs[i])
	return v

sim.context.box_size = sim.math.point(a * cbrt(4), a * cbrt(4), a * cbrt(4))

for x in range(a):
	for y in range(b):
		for z in range(c):

			sim.insert( sim.atom.atom( pos = (x * cbrt(4), y * cbrt(4), z * cbrt(4)), velocity = sim.math.vector(Random = True), speed = get_random_speed() ) )
			sim.insert( sim.atom.atom( pos = ((x + 0.5)  * cbrt(4), (y + 0.5) * cbrt(4), z * cbrt(4) ), velocity = sim.math.vector(Random = True), speed = get_random_speed() ) )
			sim.insert( sim.atom.atom( pos = ((x + 0.5) * cbrt(4), y * cbrt(4), (z + 0.5) * cbrt(4) ), velocity = sim.math.vector(Random = True), speed = get_random_speed() ) )
			sim.insert( sim.atom.atom( pos = (x * cbrt(4), (y + 0.5) * cbrt(4), (z + 0.5) * cbrt(4) ), velocity = sim.math.vector(Random = True), speed = get_random_speed() ) )

sim.dump(output_file)