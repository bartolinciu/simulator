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


context.R = 8.314459848

context.k = 1
context.Sigma = 1
context.Epsilon = 1
context.rc = 1.5



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
	if abs(context.T - temperature) > 0.1 * context.T:
		scale_temperature(kinetic_energy)
	temperature, kinetic_energy = calc_temperature()


def control_momentum():
	momentum = calc_momentum()
	if momentum.length() > 0.1**15:
		reduce_momentum(momentum)

def dump( fname ):
	with open( fname, "wt" ) as f:
		f.write("asdf\n")
		f.write( "%5i\n" % len(atoms))
		for i, k in enumerate(atoms):
			f.write( "%5i ATOM ATOM%5i%8.3f%8.3f%8.3f%8.4f%8.4f%8.4f\n" % ((i, i) + tuple(k.pos) + tuple(k.v * k.speed) ))

		f.write( "%f %f %f\n" % ( context.box_size.x, context.box_size.y, context.box_size.z ) )

def read( fname ):
	with open( fname, "rt" ) as f:
		data = f.read()
		data = data.splitlines()[1:]
		length = int(data[0])
		for i in range(length):
			atom_ = data[i+1][20:] 
			try:
				pos = math.point( float(atom_[:8]), float(atom_[8:16]), float(atom_[16:24]) )
				velocity = math.vector( float(atom_[24:32]), float(atom_[32:40]), float( atom_[40:48] ) )
			except ValueError:
				print("error")
				print(atom_[:8])
			else:
				atoms.append( atom.atom( pos = pos, velocity = velocity ) )
		box_size = data[length+1].split(" ")
		context.box_size = math.point( float(box_size[0]), float(box_size[1]), float(box_size[2]) )
		