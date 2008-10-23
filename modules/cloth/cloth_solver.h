#ifndef MODULES_CLOTH_CLOTH_SOLVER_H
#define MODULES_CLOTH_CLOTH_SOLVER_H

#include <vector>
#include <iostream>
#include "particle.h"
#include "d_Vecf.h"

/*helper functions to locate particle positions and velocities
  within the X and f vectors*/
unsigned P(unsigned p, unsigned c) { return 3*p*2+c; }
unsigned V(unsigned p, unsigned c) { return 3*p*2+3+c; }

using namespace std;

class cloth_solver
{
public:
	cloth_solver(unsigned num)
	{
		Particle tmp(0);
		for (int i=0; i<num; i++) 
			particles.push_back(tmp);

		for (int i=0; i<num; i++)
			particles[i].m = 2/(float)num;
		
		f.set_size(6*num);
		X.set_size(6*num);
		pnum = num;
		t=0;
		ks = 20;
		vd = 2;
		gravity = -9.81;
		damping = 2;
		stiffness = 20;
	}
	
	void set_constants(double g, double d, double s)
	{
		gravity = g;
		damping = d;
		stiffness = s;
	}
	
	void generate_particles(k3d::mesh::points_t Points)
	{
		for(int i=0; i<Points.size(); i++)
			for(int j=0; j<3; j++)
			{
				particles[i].x[j] = Points[i][j];
				particles[i].v[j] = 0;
			}
	}

	void update_vertices(k3d::mesh::points_t& Points)
	{
		for(int i=0; i<Points.size(); i++)
			for(int j=0; j<3; j++)
				Points[i][j] = particles[i].x[j];

	}

	void compute_accelerations(d_Vecf Y, d_Vecf *out, const k3d::mesh::selection_t& PointSelection)
	{
		static d_Vecf W;
		d_Vec3f tmp = d_Vec3f(0,0,0);
		d_Vec3f p1, p2;
		W.set_size(6*pnum);
		float eightrt = sqrt(8);
		ks = stiffness;
		for(int i=0; i<pnum; i++)
		{
			p1 = d_Vec3f(Y[P(i,0)], Y[P(i,1)], Y[P(i,2)]);
			//if(i!=0 && i!=5)
			//	tmp = add_gravity(particles[i].m, -9.81);
			//else
			tmp = add_gravity(particles[i].m, gravity);

			//if(i>0&&i<5)
			{
				//p2 = d_Vec3f(Y[P(i-1,0)], Y[P(i-1,1)], Y[P(i-1,2)]);
				//tmp = tmp + add_spring(p1, p2, ks, 2);
				//p2 = d_Vec3f(Y[P(i+1,0)], Y[P(i+1,1)], Y[P(i+1,2)]);
				//tmp = tmp + add_spring(p1, p2, ks, 2);
				//std::cout << "BLA: " << abs(p2[0]-p1[0]) << std::endl;
			}

/////////////////////////////////////

			if(i>5)
			{
				p2 = d_Vec3f(Y[P(i-6,0)], Y[P(i-6,1)], Y[P(i-6,2)]);
				tmp = tmp + add_spring(p1, p2, ks, 2);
			}

			if(i<30)
			{
				p2 = d_Vec3f(Y[P(i+6,0)], Y[P(i+6,1)], Y[P(i+6,2)]);
				tmp = tmp + add_spring(p1, p2, ks, 2);
			}

			if(i%6 != 0)
			{
				p2 = d_Vec3f(Y[P(i-1,0)], Y[P(i-1,1)], Y[P(i-1,2)]);
				tmp = tmp + add_spring(p1, p2, ks, 2);
			}

			if((i-5)%6 !=0)
			{
				p2 = d_Vec3f(Y[P(i+1,0)], Y[P(i+1,1)], Y[P(i+1,2)]);
				tmp = tmp + add_spring(p1, p2, ks, 2);
			}

/////////////////////////////////////////

			if(i>5 && (i-5)%6 !=0)
			{
				p2 = d_Vec3f(Y[P(i-5,0)], Y[P(i-5,1)], Y[P(i-5,2)]);
				tmp = tmp + add_spring(p1, p2, ks, eightrt);
			}

			if(i>5 && i%6 !=0)
			{
				p2 = d_Vec3f(Y[P(i-7,0)], Y[P(i-7,1)], Y[P(i-7,2)]);
				tmp = tmp + add_spring(p1, p2, ks, eightrt);
			}
				
			if(i<30 && (i-5)%6 !=0)
			{
				p2 = d_Vec3f(Y[P(i+7,0)], Y[P(i+7,1)], Y[P(i+7,2)]);
				tmp = tmp + add_spring(p1, p2, ks, eightrt);
			}

			if(i<5 && i%6 !=0)
			{
				p2 = d_Vec3f(Y[P(i+5,0)], Y[P(i+5,1)], Y[P(i+5,2)]);
				tmp = tmp + add_spring(p1, p2, ks, eightrt);
			}

/////////////////////////////////////////
			if(i<24)
			{
				p2 = d_Vec3f(Y[P(i+12,0)], Y[P(i+12,1)], Y[P(i+12,2)]);
				tmp = tmp + add_spring(p1, p2, ks, 4);
			}
			
			if(i>11)
			{
				p2 = d_Vec3f(Y[P(i-12,0)], Y[P(i-12,1)], Y[P(i-12,2)]);
				tmp = tmp + add_spring(p1, p2, ks, 4);
			}

			if((i-5)%6 !=0 && (i-4)%6 !=0)
			{
				p2 = d_Vec3f(Y[P(i+2,0)], Y[P(i+2,1)], Y[P(i+2,2)]);
				tmp = tmp + add_spring(p1, p2, ks, 4);
			}

			if(i%6 !=0 && (i-1)%6 !=0)
			{
				p2 = d_Vec3f(Y[P(i-2,0)], Y[P(i-2,1)], Y[P(i-2,2)]);
				tmp = tmp + add_spring(p1, p2, ks, 4);
			}
///////////////////////////////////////////
			
			tmp = tmp + add_damping(d_Vec3f(Y[V(i,0)], Y[V(i,1)], Y[V(i,2)]), damping);


			if(PointSelection.at(i)==1)
				tmp = d_Vec3f(0,0,0);
			//if(i==25)
			//	tmp = d_Vec3f(0,0,0);

			//if(i < 30)
			//{
			//	p2 = d_Vec3f(Y[P(i-6,0)], Y[P(i-6,1)], Y[P(i-6,2)]);
			//	tmp = add_spring(p1,p2,ks,len(particles[i].x-particles[i-6].x));
			//}
	
			W[P(i, 0)]= Y[V(i,0)];
			W[P(i, 1)]= Y[V(i,1)];
			W[P(i, 2)]= Y[V(i,2)];
			W[V(i, 0)]= tmp[0]/particles[i].m;
			W[V(i, 1)]= tmp[1]/particles[i].m;
			W[V(i, 2)]= tmp[2]/particles[i].m;
		}
		*out= W;
	}

	void get_state()
	{
		for (int i=0; i<particles.size(); i++) 
		{
			X[P(i,0)]= particles[i].x[0];
			X[P(i,1)]= particles[i].x[1];
			X[P(i,2)]= particles[i].x[2];
			X[V(i,0)]= particles[i].v[0];
			X[V(i,1)]= particles[i].v[1];
			X[V(i,2)]= particles[i].v[2];
		}
	}

	void set_state()
	{
		for (int i=0; i<particles.size(); i++) 
		{
			particles[i].t += t;
			particles[i].x[0]= X[P(i,0)];
			particles[i].x[1]= X[P(i,1)];
			particles[i].x[2]= X[P(i,2)];
			particles[i].v[0]= X[V(i,0)];
			particles[i].v[1]= X[V(i,1)];
			particles[i].v[2]= X[V(i,2)];
		}
	}

	void rk_step(float dt, const k3d::mesh::selection_t& PointSelection)
	{
		//for(int i=0; i<pnum; i++)
		//	particles[i].x = particles[i].x + d_Vec3f(0,0,dt);
		static d_Vecf k1, k2, k3, k4;
		k1.set_size(6*pnum);
		k2.set_size(6*pnum);
		k3.set_size(6*pnum);
		k4.set_size(6*pnum);
		get_state();
		compute_accelerations(X, &k1, PointSelection);
		k1 = k1*dt;
		compute_accelerations(X+k2/2, &k2, PointSelection);
		k2 = k2*dt;
		compute_accelerations(X+k2/2, &k3, PointSelection);
		k3 = k3*dt;
		compute_accelerations(X+k3, &k4, PointSelection);
		k4 = k4*dt;
		X = X + k1/6 + k2/3 + k3/3 + k4/6;
		t=dt;
		set_state();
	}

	unsigned S(unsigned r, unsigned c){ return r*pnum/(int)sqrt(pnum)+c; }

	vector<Particle> particles;
	d_Math::d_Vec3f generator;
	int pnum;
	d_Vecf X;
	d_Vecf f;
	int w, h;
	float r;
	float t;
	double ks;
	float vd;
	double gravity;
	double damping;
	double stiffness;
};

#endif // !MODULES_CLOTH_CLOTH_SOLVER_H

