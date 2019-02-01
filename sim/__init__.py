from sim import math
from sim import atom
import copy

ctx = {}

class Context:
	def __setattr__(self, name, value):
		global ctx
		ctx[name] = value

	def __getattr__(self, name):
		global ctx
		if name in ctx:
			return ctx[name]
		return None

context = Context()

atoms = []




def insert( atom ):
	if not type(atom).__name__ == "atom":
		raise TypeError("cannot insert non-atom entity")
	atoms.append(atom)

def calc_temperature():
	kinetic_energy = 0
	for i in atoms:
		kinetic_energy += i.speed**2 
	return (kinetic_energy / (3*(len(atoms)-1))), kinetic_energy

def scale_temperature( kinetic_energy ):
	for i in atoms:
		i.speed *= math.sqrt( (3 * ( len(atoms)-1 ) * context.k * context.T) / (kinetic_energy) )

def calc_momentum():
	momentum = copy.copy(math.v0)
	for i in atoms:
		momentum += i.v * i.speed * i.mass
	return momentum

def reduce_momentum( momentum ):
	for i in atoms:
		i.v = i.v * i.speed - momentum / (len(atoms) * i.mass )
		i.speed = i.v.length()
		i.v = i.v/i.v.length()
	

def control_temperature():
	temperature, kinetic_energy = calc_temperature()

	if abs(context.T - temperature) > 0.1:
		scale_temperature(kinetic_energy)

def control_momentum():
	momentum = calc_momentum()
	if momentum.length() > 0.1**15:
		reduce_momentum(momentum)

def dump():
	with open( context.output, "wt" ) as f:
		f.write( "%5i\n" % len(atoms))
		for i, k in enumerate(atoms):
			f.write( "%5i ATOM ATOM%5i%8.3f%8.3f%8.3f %8.4f %8.4f %8.4f\n" % ((i, i) + tuple(k.pos) + tuple(k.v * k.speed) ))

		f.write( "%f %f %f\n" % ( context.box_size.x, context.box_size.y, context.box_size.z ) )