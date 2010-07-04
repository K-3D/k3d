import k3d

class control_point:
	def __init__(self, position, weight):
		self.position = position
		self.weight = weight

class nucurve:
	def __init__(self, order):
		self.order = order
		self.control_points=[]
		self.knots = []

class arc_length:
	def __init__(self, T, Point, Length):
		self.t = T
		self.point = Point
		self.length = Length

def basis(i, k, t, knots):
	ret = 0
	if k > 0:
		n1 = (t - knots[i]) * basis(i, k - 1, t, knots)
		d1 = knots[i + k] - knots[i]
		n2 = (knots[i + k + 1] - t) * basis(i + 1, k - 1, t, knots)
		d2 = knots[i + k + 1] - knots[i + 1]
		if d1 > 0.0001 or d1 < -0.0001:
			a = n1 / d1
		else:
			a = 0
		
		if d2 > 0.0001 or d2 < -0.0001:
			b = n2 / d2
		else:
			b = 0

		ret = a + b
	else:
		if knots[i] <= t and t <= knots[i + 1]:
			ret = 1
		else:
			ret = 0
	return ret


def evaluate(Curve, T):
	c = k3d.vector3(0, 0, 0)
	for i in range(len(Curve.control_points)):
		control_point = Curve.control_points[i]
		w = control_point.weight * basis(i, Curve.order-1, T, Curve.knots)
		b = control_point.position * w
		c = c + b

	return c

def mix(x, y, alpha):
	return x * (1 - alpha) + y * (alpha)

def start_t(Curve):
	return Curve.knots[0]

def end_t(Curve):
	return Curve.knots[len(Curve.knots)-1]

def get_arc_length_detail(Curve, SegmentCount):
	min_t = start_t(Curve)
	max_t = end_t(Curve)

	arc_lengths = []
	arc_lengths.append(arc_length(min_t, evaluate(Curve, min_t), 0))

	for i in range(1, SegmentCount + 1):
		t = mix(min_t, max_t, float(i) / float(SegmentCount))
		point = evaluate(Curve, t)
		last_arc = arc_lengths[len(arc_lengths)-1]
		arc_lengths.append(arc_length(t, point, last_arc.length + k3d.length(point - last_arc.point)))
		
	return arc_lengths
		
def get_arc_lengths(Curve, MaxError):
	segment_count = 4

	last_arcs = get_arc_length_detail(Curve, segment_count)
	last_length = last_arcs[len(last_arcs)-1].length
	last_error = last_length

	while last_error / last_length > MaxError:
		segment_count *= 2
		
		arcs = get_arc_length_detail(Curve, segment_count)
		length = arcs[len(arcs)-1].length
		error = length - last_length

		last_arcs = arcs
		last_length = length
		last_error = error

	return last_arcs

def get_uniform_arc_length_position(Arcs, Length):
	for i in range(1, len(Arcs)):
		if Length <= Arcs[i].length:
			u = (Length - Arcs[i-1].length) / (Arcs[i].length - Arcs[i-1].length)
			return mix(Arcs[i-1].point, Arcs[i].point, u)

	return Arcs[len(Arcs)-1].point


