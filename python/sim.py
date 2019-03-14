import sim
import math
import sys
import random
import copy
import timeit
import threading

if not len(sys.argv) == 7:
	print("usage: python3 sim.py molar_mass temperature time_step step_count ouput_file input_file")
	exit()

output = sys.argv[5]
input_file = sys.argv[6]

sim.context.M = float(sys.argv[1]) * 1000
sim.context.T = float(sys.argv[2])
sim.context.dt = float(sys.argv[3]) * (0.1**12)
sim.context.n = int(sys.argv[4])

sim.context.scale = {}
sim.context.scale["Length"] = 0.375e-9
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


sim.context.T /= sim.context.scale["Temperature"]
sim.context.dt /= sim.context.scale["Time"]

print( sim.context.dt )
exit()

sim.read(input_file)


def calculate_force( r ):
	length = r.length()
	if length < sim.context.rc:
		f = ( r * 24*( 2 * length**(-12) - length**(-6) ))/(length**2)
		return f
	else:
		return sim.math.vector(0,0,0)


class Updater( threading.Thread ):
	def __init__( self, dt, start, end ):
		threading.Thread.__init__(self)
		self.dt = dt
		self.start_ = start
		self.end = end

	def run(self):
		for i in range(self.start_,self.end):
			#print( i, "before:", sim.atoms[i].pos )
			#print( i, "speed: ", sim.atoms[i].v * sim.atoms[i].speed, sim.atoms[i].speed )
			sim.atoms[i].update(self.dt)
			#print( i, "after:", sim.atoms[i].pos )

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
					a.x += sim.context.box_size.x * (r.x/abs(r.x))

				if abs(r.y) > sim.context.rc:
					a.y += sim.context.box_size.y * (r.y/abs(r.y))

				if abs(r.z) > sim.context.rc:
					a.z += sim.context.box_size.z * (r.z/abs(r.z))

				r = sim.math.vector( a, b )
				#print( r )

				f = calculate_force(r)
				#print( f, f.length() )
				i.apply_force(f)
				j.apply_force(-f)
				#print( sim.atoms[0].resultant, sim.atoms[0].resultant.length() )

class SpeedUpdater( threading.Thread ):
	def __init__( self, dt, start, end ):
		threading.Thread.__init__(self)
		self.dt = dt
		self.start_ = start
		self.end = end

	def run(self):
		for i in range(self.start_,self.end):
			sim.atoms[i].update_speed(self.dt)


thread_count = 4

atoms_per_thread = []

n = len(sim.atoms)
t = thread_count
s = n - 1
operations_per_thread = (n * (n-1))/(2*t)



for i in range(t - 1):
	x = round( s - 0.5 + math.sqrt( 4*s*(s+1) + 1 - operations_per_thread*8 )/2 )
	s -= x
	atoms_per_thread.append( x )

atoms_per_thread.append( s )


for c in range(sim.context.n):
	#print( sim.atoms[0].pos, sim.atoms[0].resultant, sim.atoms[0].v * sim.atoms[0].speed )
	updater_threads = []
	force_updaters = []
	speed_updaters = []

	dt = sim.context.dt

	if  c % 10 == 0:
		sim.control_momentum()
		sim.control_temperature()

	for i in range(thread_count):
		updater_thread = Updater(dt, int(i*(len(sim.atoms)/thread_count)), int((i+1) * (len(sim.atoms)/thread_count)) )
		updater_threads.append(updater_thread)
		updater_thread.start()

	for i in updater_threads:
		i.join()

	start = 0

	for i in range(thread_count):
		force_updater = ForceUpdater( start, start + atoms_per_thread[i] )
		start += atoms_per_thread[i]
		force_updaters.append(force_updater)
		force_updater.start()

	for i in force_updaters:
		i.join()

	for i in range(thread_count):
		updater_thread = SpeedUpdater(dt, int(i*(len(sim.atoms)/thread_count)), int((i+1) * (len(sim.atoms)/thread_count)) )
		updater_threads.append(updater_thread)
		updater_thread.start()

	for i in speed_updaters:
		i.join()

	
	#print( "(%.20f, %.20f, %.20f); [%.20f, %.20f, %.20f]" % (tuple(sim.atoms[0].pos) + tuple( (sim.atoms[0].v * sim.atoms[0].speed) )) )

	
	if not c == 0 and c % 40 == 0:
		sys.stdout.write("\r%i%%" % (100 * c/sim.context.n))
		sys.stdout.flush()
	sys.stdout.write(".")
	sys.stdout.flush()

sim.dump(output)


