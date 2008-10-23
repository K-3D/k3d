#ifndef MODULES_CLOTH_PARTICLE_H
#define MODULES_CLOTH_PARTICLE_H

#include "d_Vec3f.h"

using namespace d_Math;

class Particle
{
public:
	Particle(int ind)
	{
		x = v = f = n =  d_Vec3f(0,0,0);
		m = .018;
		index = ind;
	}

	Particle(int ind, d_Vec3f pos, d_Vec3f vel, float ms)
	{
		x = pos;
		v = vel;
		f = d_Math::d_Vec3f(0,0,0);
		m = ms;
		index = ind;
	}

	d_Vec3f x;
	d_Vec3f v;
	d_Vec3f n;
	d_Vec3f f;
	float m;
	float t;
	int index;
};

d_Vec3f add_gravity(float g, float m)
{
	return d_Vec3f(0, 0, g*m);
}

d_Vec3f add_spring(d_Vec3f p1, d_Vec3f p2, float k, float r)
{
	//d_Vec3f d = p1-p2;
	//return d*(-k)*(len(d)-r)/len(d);
	d_Vec3f d = p1-p2;
	return d*((-k*(len(d)-r))/len(d));
}

d_Vec3f add_damping(d_Vec3f x, float k)
{
	return x*(-k);
}

#endif // !MODULES_CLOTH_PARTICLE_H

