#ifndef MODULES_CLOTH_D_VECF_H
#define MODULES_CLOTH_D_VECF_H

#include <math.h>

namespace d_Math
{
	class d_Vecf
	{
	public:
		float *p;
		int vec_size;

		d_Vecf() : p(0), vec_size(0)
		{
		}
		
		d_Vecf(const d_Vecf& other) : p(0)
		{
			set_size(other.vec_size);
			for(int i = 0; i != vec_size; ++i)
				p[i] = other.p[i];
		}
		
		~d_Vecf()
		{
			if(p)
				delete[] p;
		}

		void set_size(int size)
		{
			if(!p)
			{
				p = new float[size];
				vec_size = size;
				return;
			}
			else
			{
				float *tmp = new float[size];
				int iter=0;
				if(size > vec_size)
					iter = vec_size;
				else
					iter = size;
				for(int i=0; i<iter; i++)
				{
					tmp[i] = p[i];
				}
				delete[] p;
				p = tmp;
				vec_size = size;
			}
		}

		float &operator [] (int index) {return p[index];}

		d_Vecf operator = (d_Vecf a) 
		{
			if(vec_size != a.vec_size)
				set_size(a.vec_size);
			for (int i=0; i<vec_size; i++) 
			{
				p[i] = a.p[i];
			}
			return *this;
		}

		d_Vecf operator + (d_Vecf a)
		{
			d_Vecf aux;
			aux.set_size(vec_size);
			for(int i=0; i<vec_size; i++)
				aux[i] = p[i] + a[i];
			return aux;
		}

		d_Vecf operator - (d_Vecf a)
		{
			d_Vecf aux;
			aux.set_size(vec_size);
			for(int i=0; i<vec_size; i++)
				aux[i] = p[i] - a[i];
			return aux;
		}

		d_Vecf operator * (float a)
		{
			d_Vecf aux;
			aux.set_size(vec_size);
			for(int i=0; i<vec_size; i++)
				aux[i] = p[i] * a;
			return aux;
		}

		d_Vecf operator / (float a)
		{
			d_Vecf aux;
			aux.set_size(vec_size);
			for(int i=0; i<vec_size; i++)
				aux[i] = p[i] / a;
			return aux;
		}

		d_Vecf operator + (void) const
		{
			return *this;
		}		

		d_Vecf operator - (void) const
		{
			d_Vecf aux;
			aux.set_size(vec_size);
			for(int i=0; i<vec_size; i++)
				aux[i] = -p[i];
			return aux;
		}
	};
}

#endif // !MODULES_CLOTH_D_VECF_H

