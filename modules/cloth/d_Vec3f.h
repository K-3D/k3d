#ifndef D_VEC3F_H
#define D_VEC3F_H

namespace d_Math
{
	class d_Vec3f
	{
	public:
		float p[3];

		d_Vec3f()  //Initialize to zero everything
		{
			p[0] = p[1] = p[2] = 0;
		}

		d_Vec3f(float x, float y, float z) //Put values
		{
			p[0]=x;p[1]=y;p[2]=z;
		}

		d_Vec3f(const d_Vec3f &v) //Copy constructor
		{
			p[0] = v[0];
			p[1] = v[1];
			p[2] = v[2];
		}

		float &operator [] (int i)
		{
			if(i>3)
			{
				std::cout << "Bad index for vector!" << std::endl;
				return p[0];
			}
			return p[i];
		}

		float operator [] (int i) const
		{
			if(i>3)
			{
				std::cout << "Bad index for vector!" << std::endl;
				return p[0];
			}
			return p[i];
		}

		d_Vec3f operator + (const d_Vec3f v) const
		{
			d_Vec3f res(p[0]+v[0], p[1]+v[1], p[2]+v[2]);
			return res;
		}

		d_Vec3f operator - (const d_Vec3f v) const
		{
			d_Vec3f res(p[0]-v[0], p[1]-v[1], p[2]-v[2]);
			return res;
		}

		d_Vec3f operator * (const float a) const
		{
			d_Vec3f res(p[0]*a, p[1]*a, p[2]*a);
			return res;
		}

		d_Vec3f operator / (const float a) const
		{
			d_Vec3f res(p[0]/a, p[1]/a, p[2]/a);
			return res;
		}

	};

	float len(const d_Vec3f a)
	{
		return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
	}
}



#endif

