import sim
import copy

class atom:
	def __init__( self, pos = sim.math.O, mass = 1, velocity = sim.math.v0, speed = None ):
		if not type(pos).__name__ in ["tuple", "point"] or not type(velocity).__name__ in ["list", "vector"]:
			raise TypeError("unsupported arguments(s) for constuctor: '%s' and '%s'" % (type(pos).__name__, type(velocity).__name__))
		if type(pos).__name__ == "tuple":
			pos = sim.math.point(pos)
		if type(velocity).__name__ == "list":
			v = sim.math.vector(velocity)
		self.pos = pos
		self.mass = mass
		self.resultant = copy.copy(sim.math.v0)
		if speed == None:
			self.speed = velocity.length()
			self.v = velocity / self.speed
		else:
			self.speed = speed
			self.v = round(velocity, 1)

	def update(self, dt):
		a = self.resultant / self.mass
		self.pos += self.v * self.speed * dt +  a * dt**2 * 0.5 
		if self.pos.x >= sim.context.box_size.x:
			self.pos.x %= sim.context.box_size.x
		elif self.pos.x < 0:
			self.pos.x += (abs(self.pos.x//sim.context.box_size.x) ) * sim.context.box_size.x

		if self.pos.y >= sim.context.box_size.y:
			self.pos.y %= sim.context.box_size.y
		elif self.pos.y < 0:
			self.pos.y += (abs(self.pos.y//sim.context.box_size.y) ) * sim.context.box_size.y

		if self.pos.z >= sim.context.box_size.z:
			self.pos.z %= sim.context.box_size.z
		elif self.pos.z < 0:
			self.pos.z += (abs(self.pos.z//sim.context.box_size.z) ) * sim.context.box_size.z

		self.tmp_v = self.v * self.speed +  a * dt*0.5
		self.resultant = copy.copy(sim.math.v0)

	def update_speed( self, dt ):
		a = self.resultant / self.mass
		self.tmp_v = self.tmp_v + a * 0.5 * dt
		self.v = self.tmp_v/self.tmp_v.length()
		self.speed = self.tmp_v.length() 

	def apply_force( self, force ):
		self.resultant += force

	def __str__( self ):
		return "atom: pos: %s, v: %s" % (str(self.pos), str(self.v))


