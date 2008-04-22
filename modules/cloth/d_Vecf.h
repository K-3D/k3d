#include <math.h>

#ifndef D_VECF_H
#define D_VECF_H

namespace d_Math
{
	class d_Vecf
	{
	public:
		float *p;
		int vec_size;

		d_Vecf()
		{
			vec_size = -1;
		}

		void set_size(int size)
		{
			if(vec_size == -1)
			{
				p = (float *) malloc (size*sizeof(float));
				vec_size = size;
				return;
			}
			else
			{
				float *tmp = (float *) malloc (size*sizeof(float));
				int iter=0;
				if(size > vec_size)
					iter = vec_size;
				else
					iter = size;
				for(int i=0; i<iter; i++)
				{
					tmp[i] = p[i];
				}
				p = tmp;
				vec_size = size;
			}
		}

		float &operator [] (int index) {return p[index];}

		d_Vecf operator = (d_Vecf a) 
		{
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

#endif //D_VECF_H

