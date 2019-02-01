import sim
import math
import sys
import random
import copy
import timeit
import threading

if not len(sys.argv) == 6:
	print("usage: python3 sim.py molar_mass temperature time_step step_count output_file")
	exit()

sim.context.output = sys.argv[5]

sim.context.M = float(sys.argv[1]) * 1000
sim.context.T = float(sys.argv[2])
sim.context.dt = float(sys.argv[3]) / 1000000000000
sim.context.n = int(sys.argv[4])
sim.context.R = 8.314459848

sim.context.k = 1
sim.context.Sigma = 1
sim.context.Epsilon = 1
sim.context.rc = 1.5


sim.context.scale = {}
sim.context.scale["Length"] = 0.375
sim.context.scale["Energy"] = 72.953E-23 
sim.context.scale["Mass"] = 1.66053892173E-27 * (sim.context.M/1000)
sim.context.scale["k"] = 1.3806485279E-23
sim.context.scale["Speed"] = math.sqrt( (sim.context.Epsilon * sim.context.scale["Energy"])/(sim.context.scale["Mass"] * 1) )
sim.context.scale["Temperature"] = (sim.context.Epsilon*sim.context.scale["Energy"])/(sim.context.k * sim.context.scale["k"])
sim.context.scale["Time"] = math.sqrt( ( sim.context.scale["Mass"] * ( sim.context.Sigma * sim.context.scale["Length"] ) ** 2 ) / ( sim.context.Epsilon * sim.context.scale["Energy"] ) )
sim.context.box_size = sim.math.point((4,4,4))

sim.context.units = {}
sim.context.units["Length"] = "nm"
sim.context.units["Energy"] = "J"
sim.context.units["Speed"] = "m/s"
sim.context.units["Mass"] = "kg"
sim.context.units["Temperature"] = "K"

vp = math.sqrt( (2 * sim.context.R * sim.context.T) / sim.context.M )

sim.context.T /= sim.context.scale["Temperature"]
sim.context.dt /= sim.context.scale["Time"]

vp /= sim.context.scale["Speed"]

vs = [vp, 1.22 * vp, 0.78 * vp, 1.13 * vp, 0.87 * vp]
nv = [ 102, 51, 51, 26, 26 ]

def get_random_speed():
	global nv
	i = random.randint(0, len(vs)-1)
	v = vs[i]
	nv[i] -= 1
	if nv[i] == 0:
		nv.remove(nv[i])
		vs.remove(vs[i])
	return v
 
for x in range(4):
	for y in range(4):
		for z in range(4):
			sim.insert( sim.atom.atom( pos = (x, y, z), velocity = sim.math.vector(Random = True), speed = get_random_speed() ) )
			sim.insert( sim.atom.atom( pos = (x + 0.5, y + 0.5, z), velocity = sim.math.vector(Random = True), speed = get_random_speed() ) )
			sim.insert( sim.atom.atom( pos = (x + 0.5, y, z + 0.5), velocity = sim.math.vector(Random = True), speed = get_random_speed() ) )
			sim.insert( sim.atom.atom( pos = (x, y + 0.5, z + 0.5), velocity = sim.math.vector(Random = True), speed = get_random_speed() ) )

print("atoms[0] primal speed:", sim.atoms[0].speed)

momentum = sim.calc_momentum()
print("momentum0:", momentum)

sim.reduce_momentum(momentum)

momentum = sim.calc_momentum()
print("momentum1:", momentum)

sim.control_temperature()

momentum = sim.calc_momentum()
print("momentum2:", momentum)


def calculate_force( r ):
	length = r.length()
	if length < sim.context.rc:
		f = ( r * 24*( 2 * length**(-12) - length**(-6) ))/(length**2)
		return f

class Updater( threading.Thread ):
	def __init__( self, dt, start, end ):
		threading.Thread.__init__(self)
		self.dt = dt
		self.start_ = start
		self.end = end

	def run(self):
		for i in sim.atoms[self.start_:self.end]:
			i.update(self.dt)



class ForceUpdater( threading.Thread ):
	def __init__(self, start, end):
		threading.Thread.__init__(self)
		self.start_ = start
		self.end = end
	def run(self):
		for k, i in enumerate(sim.atoms[self.start_:self.end]):
			k+= self.start_
			for j in sim.atoms[k+1:]:

				a, b = copy.copy(j.pos), copy.copy(i.pos)

				r = sim.math.vector( a, b )

				if abs(r.x) > sim.context.rc:
					a += [ sim.context.box_size.x * (r.x/abs(r.x)), 0, 0 ]

				if abs(r.y) > sim.context.rc:
					a += [ 0, sim.context.box_size.y * (r.y/abs(r.y)), 0 ]

				if abs(r.z) > sim.context.rc:
					a += [ 0, 0, sim.context.box_size.z * (r.z/abs(r.z)) ]

				r = sim.math.vector( a, b )

				f = calculate_force(r)
				if not f == None:
					i.apply_force(f)
					j.apply_force(-f)

updater_threads = []
force_updaters = []


for c in range(sim.context.n):
	dt = sim.context.dt
	if c % 20 == 0:
		print( "\nTemperature in step %i:"%c, sim.calc_temperature()[0] * sim.context.scale["Temperature"] )
	sys.stdout.write(".")
	sys.stdout.flush()
	
	for i in range(4):
		updater_thread = Updater(dt, i*64, i*64+64 )
		updater_threads.append(updater_thread)
		updater_thread.start()

	for i in updater_threads:
		i.join()

	for i in range(4):
		force_updater = ForceUpdater( i*64, i*64+64 )
		force_updaters.append(force_updater)
		force_updater.start()

	for i in force_updaters:
		i.join()

	for i in sim.atoms:
		i.update_speed(dt)

	
print( "\nTemperature in step %i:"%sim.context.n, sim.calc_temperature()[0] * sim.context.scale["Temperature"] )		



