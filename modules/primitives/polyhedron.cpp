// K-3D
// Copyright (c) 2002-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/node.h>

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <sstream>
#include <vector>

#ifndef MAXLONG
#define MAXLONG 0x7FFFFFFF
#endif

#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131e-16
#endif

#define BIG_EPSILON 3e-2

#define Err(x) { k3d::log() << "Kaleido error : " << x << std::endl; \
 k3d::log() << " line : " << __LINE__ << std::endl; return 0; }


#define AZ k3d::pi()/7 // axis azimuth
#define EL k3d::pi()/17 // axis elevation

#define xcompl(x)(frac(x),(double)frax.n/(frax.n-frax.d))

namespace libk3dprimitives
{

namespace kaleido
{

// Copyright note: Kaleido source code license changed in 2002.
//  This is an older version placed under the GNU General Public License.
//  The 2002 version only includes minor changes in data structures.

/**********************************************************
* kaleido
*
*	Kaleidoscopic construction of uniform polyhedra
*	Copyright © 1991-1997 Dr. Zvi Har'El <rl@math.technion.ac.il>
*
*	This program is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation; either version 2 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program; if not, write to the Free Software
*	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*	Author:
*		Dr. Zvi Har'El,
*		Technion, Israel Institute of Technology,
*		Haifa 32000, Israel.
*		E-Mail: rl@math.technion.ac.il
**********************************************************/

/** \file
		\author Dr. Zvi Har'El <rl@math.technion.ac.il>
*/

class Kvector
{
public:
	double x,y,z;

	Kvector()
	{
		x = y = z = 0;
	}

	Kvector(const double X, const double Y, const double Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	Kvector& operator = (const Kvector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}
};

Kvector operator + (const Kvector& a, const Kvector& b);
Kvector operator - (const Kvector& a, const Kvector& b);
Kvector operator * (const Kvector& a, const double d);
Kvector operator * (const double d, const Kvector& a);
Kvector operator ^ (const Kvector& a, const Kvector& b);

inline Kvector operator + (const Kvector& a, const Kvector& b)
{ return Kvector(a.x + b.x, a.y + b.y, a.z + b.z); }

inline Kvector operator - (const Kvector& a, const Kvector& b)
{ return Kvector(a.x - b.x, a.y - b.y, a.z - b.z); }

inline Kvector operator * (const Kvector& a, const double d)
{ return Kvector(d*a.x, d*a.y, d*a.z); }

inline Kvector operator * (const double d, const Kvector& a)
{ return a*d; }

inline Kvector operator * (const Kvector& a, const Kvector& b)
{ return Kvector(a.x * b.x, a.y * b.y, a.z * b.z); }

inline Kvector operator ^ (const Kvector& a, const Kvector& b) {
 return Kvector(a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x);
}


typedef struct { // See uniformnames.cpp for explanation of the fields
	char *Wythoff, *name, *dual;
	short Coxeter, Wenninger;
} Uniform;

typedef struct {long n,d;} Fraction;

Kvector  rotate( Kvector vertex, Kvector axis, double angle );

double  dot( Kvector a, Kvector b );

void  frac( double x );


extern Uniform uniform[];
extern Fraction frax;
extern int last_uniform;

long numerator(double x)
{
	frac(x);
	return frax.n;
}

long denominator(double x)
{
	frac(x);
	return frax.d;
}

int mod(int i, const int j);


class Polyhedron;

Polyhedron* kaleido( unsigned int nsym );


class Polyhedron
{
public:
	Polyhedron()
	{
		index = -1;
		N = M = V = E = F = D = chi = g = 0;
		K = 2;
		hemi = onesided = 0;
		even = -1;
		p[0] = p[1] = p[2] = p[3] = minr = gon = 0.0;
		polyform = "";
		config = "";
		name = "";
		dual_name = "";
	}

	~Polyhedron()
	{
	}

	/* NOTE: some of the int's can be replaced by short's, char's,
		or even bit fields, at the expense of readability!!!*/
	int index; /* index to the standard list, the array uniform[] */
	int N; /* number of faces types (atmost 5)*/
	int M; /* vertex valency  (may be big for dihedral polyhedra) */
	int V; /* vertex count */
	int E; /* edge count */
	int F; /* face count */
	int D; /* density */
	int chi; /* Euler characteristic */
	int g; /* order of symmetry group */
	int K; /* symmetry type: D=2, T=3, O=4, I=5 */
	int hemi;/* flag hemi polyhedron */
	int onesided;/* flag onesided polyhedron */
	int even; /* removed face in pqr| */
	std::vector<int> Fi; // face counts by type (array N)
	std::vector<int> rot; // vertex configuration (array M of 0..N-1)
	std::vector<int> snub; // snub triangle configuration (array M of 0..1)
	std::vector<int> firstrot; // temporary for vertex generation (array V)
	std::vector<int> ftype; // face types (array F)
	std::vector< std::vector<int> > incid; // vertex-face incidence (matrix M x V of 0..F-1)
	std::vector< std::vector<int> > adj; // vertex-vertex adjacency (matrix M x V of 0..V-1)
	double p[4]; /* p, q and r; |=0 */
	double minr; /* smallest nonzero inradius */
	double gon; /* basis type for dihedral polyhedra */
	std::vector<double> n_sides; // number of side of a face of each type (array N)
	std::vector<double> m_faces; // number of faces at a vertex of each type (array N)
	std::vector<double> gamma; // fundamental angles in radians (array N)
	std::string polyform; // printable Wythoff symbol
	std::string config; // printable vertex configuration
	std::string name; // name, standard or manufactured
	std::string dual_name; // dual name, standard or manufactured
	std::vector<Kvector> v; // vertex coordinates (array V)
	std::vector<Kvector> f; // face coordinates (array F)

	int UnpackSym(unsigned int nsym);
	int Moebius();
	void Decompose();
	void GuessName();
	int Newton();
	void Exceptions();
	void Count();
	void Configuration();
	int Faces();
	int Vertices();

private:
	int Dihedral(const std::string& newname, const std::string& newdual_name);

};


const std::string PrintN(int n)
{
	std::ostringstream stream;
	stream << n;

	return std::string(stream.str());
}

const std::string Sprintfrac(double x)
{
	frac(x);

	if(!frax.d)
		return "infinity";

	if(frax.d == 1)
		return PrintN(frax.n);

	return PrintN(frax.n) + "/" + PrintN(frax.d);
}

double Same(Kvector a, Kvector b, double epsilon)
{
	return fabs(a.x - b.x) < epsilon
		&& fabs(a.y - b.y) < epsilon
		&& fabs(a.z - b.z) < epsilon;
}

// Compute the polar reciprocal of the plane containing a, b and c:
//
// If this plane does not contain the origin, return p such that
//	dot(p,a) = dot(p,b) = dot(p,b) = r.
//
// Otherwise, return p such that
//	dot(p,a) = dot(p,b) = dot(p,c) = 0
// and
//	dot(p,p) = 1.
Kvector Pole(double r, Kvector a, Kvector b, Kvector c)
{
	Kvector p = (b - a) ^ (c - a);
	double k = dot(p, a);
	if(fabs(k) < 1e-6)
		return (1 / sqrt(dot(p, p)))*p;

	return (r/k)*p;
}

// Compute the mathematical modulus function
int mod(int i, const int j)
{
	return (i %= j) >= 0 ? i : (j < 0 ? i-j : i+j);
}

Polyhedron* kaleido( unsigned int nsym )
{
	// Allocate a Polyhedron structure P
	Polyhedron* P = new Polyhedron();
	if(!P)
		return 0;

	// Unpack input symbol into P
	if(!P->UnpackSym(nsym))
		return 0;

	// Find Mebius triangle, its density and Euler characteristic
	if(!P->Moebius())
		return 0;

	// Decompose Schwarz triangle
	P->Decompose();

	// Find the names of the polyhedron and its dual
	P->GuessName();

	// Solve Fundamental triangles, optionally printing approximations
	if(!P->Newton())
		return 0;

	// Deal with exceptional polyhedra
	P->Exceptions();

	// Count edges and faces, update density and characteristic if needed
	P->Count();

	// Generate printable vertex configuration
	P->Configuration();

	// Compute coordinates
	if(!P->Vertices()) return 0;
	if(!P->Faces()) return 0;

	return P;
}

int Polyhedron::UnpackSym(unsigned int nsym)
/*
* Unpack input symbol: Wythoff symbol from nsym, an index to uniform[].
* The symbol is a three fractions and a bar in some order.
* We allow no bar only if it results from the input symbol #80.
*/
{
	if(nsym < 1 || nsym > 80)
		Err("Bad number");

	char* sym = uniform[index = nsym - 1].Wythoff;

	int i = 0, bars = 0;
	char c;
	for (;;) {
		while((c = *sym++) && std::isspace(c));
		if(!c)
		{
			if (i == 4 && (bars || index == last_uniform - 1)) return 1;
			if (!bars) Err("no bars")
			Err("not enough fractions")
		}
		if (i == 4) Err("data exceeded")
		if (c == '|'){
			if (++bars > 1) Err("too many bars")
			p[i++] = 0;
			continue;
		}
		if (!std::isdigit(c)) Err("not a digit")
		int n = c - '0';
		while ((c = *sym++) && std::isdigit(c)) n = n * 10 + c - '0';
		if (c && std::isspace (c)) while ((c = *sym++) && std::isspace(c))
			;
		if (c != '/') {
			sym--;
			if ((p[i++] = n) <= 1) Err("fraction<=1")
			continue;
		}
		while( (c = *sym++) && std::isspace(c));

		if (!c || !std::isdigit (c)) return 0;
		int d = c - '0';
		while ((c = *sym++) && std::isdigit(c)) d = d * 10 + c - '0';
		if (!d) Err ("zero denominator")
		sym--;
		if ((p[i++] = (double) n / d) <= 1) Err("fraction<=1")
	}
}

int Polyhedron::Moebius()
/*
* Using Wythoff symbol (p|qr, pq|r, pqr| or |pqr), find the Moebius
* triangle (2 3 K) (or (2 2 n)) of the Schwarz triangle (pqr),
* the order g of its symmetry group, its Euler characteristic chi,
* and its covering density D.
* g is the number of copies of (2 3 K) covering the sphere, i.e.,
*
*		g * pi * (1/2 + 1/3 + 1/K - 1) = 4 * pi
*
* D is the number of times g copies of (pqr) cover the sphere, i.e.
*
*		D * 4 * pi = g * pi * (1/p + 1/q + 1/r - 1)
*
* chi is V - E + F, where F = g is the number of triangles, E = 3*g/2 is the
* number of triangle edges, and V = Vp+ Vq+ Vr, with Vp = g/(2*np) being the
* number of vertices with angle pi/p (np is the numerator of p).
*/
{
	int twos = 0, j;
/*
* Arrange Wythoff symbol in a presentable form. In the same time check
* the restrictions on the three fractions: They all have to be greater
* then one, and the numerators 4 or 5 cannot occur together.
* We count the ocurrences of 2 in `two', and save the largest numerator
* in `K', since they reflect on the symmetry group.
*/
	K = 2;
	if(index == last_uniform - 1)
		polyform = "|";

	for(j = 0; j < 4; j++) {
		if(p[j]) {
			std::string s = Sprintfrac(p[j]);
			if(j && p[j-1])
				polyform += " ";
			else
				polyform += s;

			if(p[j] != 2) {
				int k;
				if ((k = numerator (p[j])) > K) {
					if (K == 4) break;
					K = k;
				} else if (k < K && k == 4) break;
			} else twos++;
		} else
			polyform += "|";
	}

/*
* Find the symmetry group K (where 2, 3, 4, 5 represent the dihedral,
* tetrahedral, octahedral and icosahedral groups, respectively),
* and its order g.
*/
	if (twos >= 2) { // dihedral
		g = 4 * K;
		K = 2;
	} else {
		if (K > 5) Err ("numerator too large")
		g = 24 * K / (6 - K);
	}
/*
* Compute the nominal density D and Euler characteristic chi.
* In few exceptional cases, these values will be modified later.
*/
	if (index != last_uniform - 1) {
		int i;
		D = chi = - g;
		for(j = 0; j < 4; j++) if (p[j]) {
			chi +=  i = g / numerator (p[j]);
			D += i * denominator (p[j]);
		}
		chi /= 2;
		D /= 4;
		if(D <= 0) Err ("nonpositive density")
	}
	return 1;
}

void Polyhedron::Decompose()
/*
* Decompose Schwarz triangle into N right triangles and compute
* the vertex count V and the vertex valency M.
* V is computed from the number g of Schwarz triangles in the cover, divided
* by the number of triangles which share a vertex. It is halved for one-sided
* polyhedra, because the kaleidoscopic construction really produces a double
* orientable covering of such polyhedra. All q' q|r are of the "hemi" type,
* i.e. have equatorial {2r} faces, and therefore are (except 3/2 3|3 and the
* dihedra 2 2|r) one-sided. A well known example is 3/2 3|4, the "one-sided
* heptahedron". Also, all p q r| with one even denominator have a crossed
* parallelogram as a vertex figure, and thus are one-sided as well.
*/
{
	int j;

	if(!p[1]) { // p|q r
		N = 2;
		M = 2 * numerator (p[0]);
		V = g / M;
		n_sides.resize(N);
		m_faces.resize(N);

		for(j = 0; j < 2; j++) {
			n_sides[j] = p[j+2];
			m_faces[j] = p[0];
		}
		for (j = M / 2; j--;) {
			rot.push_back(0);
			rot.push_back(1);
		}
	} else if(!p[2]) { // p q|r
		N = 3;
		M = 4;
		V = g / 2;
		n_sides.resize(N);
		m_faces.resize(N);

		n_sides[0] = 2 * p[3];
		m_faces[0] = 2;
		for (j = 1; j < 3; j++) {
			n_sides[j] = p[j-1];
			m_faces[j] = 1;
			rot.push_back(0);
			rot.push_back(j);
		}
		if (fabs (p[0] - xcompl (p[1])) < DBL_EPSILON) {/* p = q' */
				/* p[0]==xcompl(p[1]) should work. */
				/* However, MSDOS yeilds a 7e-17 difference! */
				/* Reported by Jim Buddenhagen <jb1556@daditz.sbc.com> */
			hemi = 1;
			D = 0;
			if (p[0] != 2 && !(p[3] == 3 && (p[0] == 3 || p[1] == 3))) {
				onesided = 1;
				V /= 2;
				chi /= 2;
			}
		}
	} else if (!p[3]) { // p q r|
		M = N = 3;
		V = g;
		n_sides.resize(N);
		m_faces.resize(N);

		for (j = 0; j < 3; j++) {
			if (!(denominator (p[j]) % 2)) {
				/* what happens if there is more than one even denominator???*/
				if (p[(j+1)%3] != p[(j+2)%3]) { /*needs postprocessing*/
					even = j;/*memorize the removed face*/
					chi -= g / numerator (p[j]) / 2;
					onesided = 1;
					D = 0;
				} else {/*for p = q we get a double 2 2r|p*/
				/*noted by Roman Maeder <maeder@inf.ethz.ch> for 4 4 3/2|*/
				/*Euler characteristic is still wrong*/
					D /= 2;
				}
				V /= 2;
			}
			n_sides[j] = 2 * p[j];
			m_faces[j] = 1;
			rot.push_back(j);
		}
	} else { /* |p q r - snub polyhedron*/
		N = 4;
		M = 6;
		V = g / 2;/* Only "white" triangles carry a vertex */
		n_sides.resize(N);
		m_faces.resize(N);
		snub.resize(M);

		std::vector<int>::iterator t = snub.begin();
			m_faces[0] = n_sides[0] = 3;
			for (j = 1; j < 4; j++) {
				n_sides[j] = p[j];
				m_faces[j] = 1;
				rot.push_back(0);
				rot.push_back(j);
				*t++ = 1;
				*t++ = 0;
			}
		}

	// Sort the fundamental triangles (using bubble sort) according to
	// decreasing n[i], while pushing the trivial triangles (n[i] = 2)
	// to the end.
	int J = N - 1;
	while(J) {
		int last;
		last = J;
		J = 0;
		for (j = 0; j < last; j++) {
			if ((n_sides[j] < n_sides[j+1] || n_sides[j] == 2) && n_sides[j+1] != 2) {
				int i;
				double temp;
				temp = n_sides[j];
				n_sides[j] = n_sides[j+1];
				n_sides[j+1] = temp;
				temp = m_faces[j];
				m_faces[j] = m_faces[j+1];
				m_faces[j+1] = temp;
				for (i = 0; i < M; i++) {
					if(rot[i] == j) rot[i] = j+1;
					else if(rot[i] == j+1) rot[i] = j;
				}
				if (even != -1) {
					if (even == j) even = j+1;
					else if (even == j+1) even = j;
				}
				J = j;
			}
		}
	}

	// Get rid of repeated triangles
	for (J = 0; J < N && n_sides[J] != 2;J++) {
		int k, i;
		for (j = J+1; j < N && n_sides[j]==n_sides[J]; j++) m_faces[J] += m_faces[j];
		k = j - J - 1;
		if (k) {
			for (i = j; i < N; i++) {
				n_sides[i - k] = n_sides[i];
				m_faces[i - k] = m_faces[i];
			}
			N -= k;
			for (i = 0; i < M; i++) {
				if(rot[i] >= j) rot[i] -= k;
				else if(rot[i] > J) rot[i] = J;
			}
			if (even >= j) even -= k;
		}
	}

	// Get rid of trivial triangles
	if (!J) J = 1; /*hosohedron*/
	if (J < N) {
		int i;
		N = J;
		for (i = 0; i < M; i++) {
			if(rot[i] >= N) {
				for (j = i + 1; j < M; j++) {
					rot[j-1] = rot[j];
					if(snub.size()) snub[j-1] = snub[j];
				}
				M--;
			}
		}
	}

	// Truncate arrays
	n_sides.resize(N);
	m_faces.resize(N);
	if(snub.size()) snub.resize(M);
}

void Polyhedron::GuessName()
/*
* Get the polyhedron name, using standard list or guesswork.
* Ideally, we should try to locate the Wythoff symbol in the standard list
* (unless, of course, it is dihedral), after doing few normalizations,
* such as sorting angles and splitting isoceles triangles.
*/
{
	if(index != -1)
	{
		// tabulated
		name = uniform[index].name;
		dual_name = uniform[index].dual;
		return;
	}

	if(K == 2)
	{
		// dihedral nontabulated
		if(!p[0])
		{
			if(N == 1)
			{
				name = "octahedron";
				dual_name = "cube";
				return;
			}

			gon = n_sides[0] == 3 ? n_sides[1] : n_sides[0];
			if(gon >= 2)
				Dihedral("antiprism", "deltohedron");
			else
				Dihedral("crossed antiprism", "concave deltohedron");

			return;
		}

		if(!p[3] || !p[2] && p[3] == 2)
		{
			if(N == 1) {
				name = "cube";
				dual_name = "octahedron";
				return;
			}

			gon = n_sides[0] == 4 ? n_sides[1] : n_sides[0];

			Dihedral("prism", "dipyramid");
			return;
		}

		if(!p[1] && p[0] != 2)
		{
			gon = m_faces[0];
			Dihedral("hosohedron", "dihedron");
			return;
		}

		gon = n_sides[0];
		Dihedral("dihedron", "hosohedron");
		return;
	}

	// Other nontabulated
	static std::string pre[] = { "tetr", "oct", "icos" };
	name = pre[K - 3] + "ahedral ";
	if(onesided) name += "one-sided ";
	else if(D == 1) name += "convex ";
	else name += "nonconvex ";

	dual_name = name;
	name += "isogonal polyhedron";
	dual_name += "isohedral polyhedron";
}

int Polyhedron::Dihedral(const std::string& newname, const std::string& newdual_name )
{
	std::string s = Sprintfrac(gon < 2 ? xcompl(gon) : gon);

	name = s + "-gonal " + newname;
	dual_name = s + "-gonal " + newdual_name;

	return 1;
}

int Polyhedron::Newton()
// Solve the fundamental right spherical triangles
{
	// First, we find initial approximations
	int j;

	gamma.resize(N);
	if(N == 1)
	{
		gamma[0] = k3d::pi() / m_faces[0];
		return 1;
	}

	for(j = 0; j < N; j++)
		gamma[j] = k3d::pi() / 2 - k3d::pi() / n_sides[j];

	// Next, iteratively find closer approximations for gamma[0]
	// and compute other gamma[j]'s from Napier's equations.
	for(;;)
	{
		double delta = k3d::pi();
		for(j = 0; j < N; j++)
			delta -= m_faces[j] * gamma[j];

		if(fabs(delta) < 11 * DBL_EPSILON)
			return 1;

		// On a RS/6000, fabs(delta)/DBL_EPSILON may occilate between 8 and 10.
		// Reported by David W. Sanderson <dws@ssec.wisc.edu>
		double sigma = 0;
		for(j = 0; j < N; j++)
			sigma += m_faces[j] * tan(gamma[j]);

		gamma[0] += delta * tan (gamma[0]) / sigma;
		if(gamma[0] < 0 || gamma[0] > k3d::pi())
			Err("gamma out of bounds")

		double cosa = cos(k3d::pi() / n_sides[0]) / sin (gamma[0]);
		for(j = 1; j < N; j++)
			gamma[j] = asin(cos (k3d::pi() / n_sides[j]) / cosa);
	}
}

void Polyhedron::Exceptions()
{
	// Postprocess pqr| where r has an even denominator (cf. Coxeter &al. Sec.9).
	// Remove the {2r} and add a retrograde {2p} and retrograde {2q}.
	int j;
	if(even != -1)
	{
		M = N = 4;
		n_sides.resize(N);
		m_faces.resize(N);
		gamma.resize(N);
		rot.resize(M);

		for(j = even + 1; j < 3; j++)
		{
			n_sides[j-1] = n_sides[j];
			gamma[j-1] = gamma[j];
		}

		n_sides[2] = xcompl (n_sides[1]);
		gamma[2] = - gamma[1];
		n_sides[3] = xcompl (n_sides[0]);
		m_faces[3] = 1;
		gamma[3] = - gamma[0];
		rot[0] = 0;
		rot[1] = 1;
		rot[2] = 3;
		rot[3] = 2;
	}

	// Postprocess the last polyhedron |3/2 5/3 3 5/2 by taking a |5/3 3 5/2,
	// replacing the three snub triangles by four equatorial squares and adding
	// the missing {3/2} (retrograde triangle, cf. Coxeter &al. Sec. 11).
	if(index == last_uniform - 1)
	{
		N = 5;
		M = 8;
		n_sides.resize(N);
		m_faces.resize(N);
		gamma.resize(N);
		rot.resize(M);
		snub.resize(M);

		hemi = 1;
		D = 0;
		for(j = 3; j; j--)
		{
			m_faces[j] = 1;
			n_sides[j] = n_sides[j-1];
			gamma[j] = gamma[j-1];
		}

		m_faces[0] = n_sides[0] = 4;
		gamma[0] = k3d::pi() / 2;
		m_faces[4] = 1;
		n_sides[4] = xcompl (n_sides[1]);
		gamma[4] = - gamma[1];
		for(j = 1; j < 6; j += 2) rot[j]++;
		rot[6] = 0;
		rot[7] = 4;
		snub[6] = 1;
		snub[7] = 0;
	}
}

void Polyhedron::Count()
/*
* Compute edge and face counts, and update D and chi.
* Update D in the few cases the density of the polyhedron is meaningful but
* different than the density of the corresponding Schwarz triangle
* (cf. Coxeter &al., p. 418 and p. 425).
* In these cases, spherical faces of one type are concave (bigger than a
* hemisphere), and the actual density is the number of these faces less the
* computed density.
* Note that if j != 0, the assignment gamma[j] = asin(...)
* implies gamma[j] cannot be obtuse.
* Also, compute chi for the only non-Wythoffian polyhedron.
*/
{
	Fi.resize(N);
	for(int j = 0; j < N; j++)
	{
		int temp = V * numerator(m_faces[j]);
		E += temp;
		F += Fi[j] = temp / numerator(n_sides[j]);
	}

	E /= 2;

	if(D && gamma[0] > k3d::pi() / 2)
		D = Fi[0] - D;

	if(index == last_uniform - 1)
		chi = V - E + F;
}

// Generate a printable vertex configuration symbol
void Polyhedron::Configuration()
{
	int j;
	for(j = 0; j < M; j++)
	{
		std::string s = Sprintfrac(n_sides[rot[j]]);

		if(!j)
			config = "(";
		else
			config += ".";

		config += s;
	}

	config += ")";

	if((j = denominator(m_faces[0])) != 1)
		config += "/" + PrintN(j);
}

int Polyhedron::Vertices()
/*
* Compute polyhedron vertices and vertex adjecency lists.
* The vertices adjacent to v[i] are v[adj[0][i], v[adj[1][i], ... v[adj[M-1][i],
* ordered counterclockwise.
* The algorith is a BFS on the vertices, in such a way that the vetices
* adjacent to a givem vertex are obtained from its BFS parent by a cyclic
* sequence of rotations. firstrot[i] points to the first  rotaion in
* the sequence when applied to v[i]. Note that for non-snub polyhedra,
* the rotations at a child are opposite in sense when compared to the
* rotations at the parent. Thus, we fill adj[*][i] from the end to signify
* clockwise rotations. The firstrot[] array is not needed for display
* thus it is freed after being used for face computations below.
*/
{
	int newV = 2;

	v.resize(V);

	adj.resize(M);
	for(int row = 0; row < M; row++)
		(adj[row]).resize(V, 0);

	firstrot.resize(V); /* temporary , put in Polyhedron
			structure so that may be freed on error */
	double cosa = cos (k3d::pi() / n_sides[0]) / sin (gamma[0]);
	v[0].x = 0;
	v[0].y = 0;
	v[0].z = 1;
	firstrot[0] = 0;
	adj[0][0] = 1;
	v[1].x = 2 * cosa * sqrt (1 - cosa * cosa);
	v[1].y = 0;
	v[1].z = 2 * cosa * cosa - 1;
	if(!snub.size()) {
		firstrot[1] = 0;
		adj[0][1] = -1;/*start the other side*/
		adj[M-1][1] = 0;
	} else {
		firstrot[1] = snub[M-1] ? 0 : M-1;
		adj[0][1] = 0;
	}
	for(int i = 0; i < newV; i++) {
		int j, k;
		int last, one, start, limit;
		if (adj[0][i] == -1) {
			one = -1; start = M-2; limit = -1;
		} else {
			one = 1; start = 1; limit = M;
		}
		k = firstrot[i];
		for (j = start; j != limit; j += one) {
			Kvector temp;
			int J;
			temp = rotate (v[adj[j-one][i]], v[i],
				one * 2 * gamma[rot[k]]);
			for (J=0; J<newV && !Same(v[J], temp, BIG_EPSILON); J++) /*noop*/;
			adj[j][i] = J;
			last = k;
			if (++k == M) k = 0;
			if (J == newV) { /*new vertex*/
				if (newV == V) Err ("too many vertices")
				v[newV++] = temp;
				if(!snub.size()) {
					firstrot[J] = k;
					if (one > 0) {
						adj[0][J] = -1;
						adj[M-1][J] = i;
					} else {
						adj[0][J] = i;
					}
				} else {
					firstrot[J] = !snub[last] ? last :
						!snub[k] ? (k+1)%M : k ;
					adj[0][J] = i;
				}
			}
		}
	}
	return 1;
}

int Polyhedron::Faces()
/*
* Compute polyhedron faces (dual vertices) and incidence matrices.
* For orientable polyhedra, we can distinguish between the two faces meeting
* at a given directed edge and identify the face on the left and the face on
* the right, as seen from the outside.
* For one-sided polyhedra, the vertex figure is a papillon (in Coxeter &al.
* terminology, a crossed parallelogram) and the two faces meeting at an edge
* can be identified as the side face (n[1] or n[2]) and
* the diagonal face (n[0] or n[3]).
*/
{
	f.resize(F);
	ftype.resize(F);

	// Init incid matrix with -1
	incid.resize(M);
	for(int row = 0; row < M; row++)
		(incid[row]).resize(V, -1);

	minr = 1 / fabs (tan (k3d::pi() / n_sides[hemi]) * tan (gamma[hemi]));

	int newF = 0;
	for(int i = 0; i < V; i++) {
		for(int j = 0; j < M; j++) {
			int pap = 0; // papillon edge type

			if (incid[j][i] != -1) continue;
			incid[j][i] = newF;
			if (newF == F) Err ("too many faces")
			f[newF] = Pole(minr, v[i], v[adj[j][i]],
				v[adj[mod(j + 1, M)][i]]);
			ftype[newF] = rot [mod(
				firstrot[i] + (adj[0][i] < adj[M - 1][i] ?
				j : -j - 2), M)];
			if (onesided) pap = (firstrot[i] + j) % 2;
			int i0 = i;
			int J = j;
			for (;;) {
				int k;
				k = i0;
				if ((i0 = adj[J][k]) == i) break;
				for (J = 0; J < M && adj[J][i0] != k; J++) /*noop*/;
				if (J == M) Err ("too many faces")
				if (onesided && (J + firstrot[i0]) % 2 == pap) {
					incid [J][i0] = newF;
					if (++J >= M) J = 0;
				} else {
					if (--J < 0) J = M - 1;
					incid [J][i0] = newF;
				}
			}
		newF++;
		}
	}

	rot.clear();
	snub.clear();

	return 1;
}

Fraction frax;

void  frac( double x )
/*
* Find the numerator and the denominator using the Euclidean algorithm.
*/
{
	static Fraction zero = {0,1}, inf = {1,0};
	Fraction r0, r;
	long fl;
	double s = x;
	r = zero;
	frax = inf;
	for (;;) {
		if (fabs(s) > (double) MAXLONG) return;
		fl = (long) floor (s);
		r0 = r;
		r = frax;
		frax.n = frax.n * fl + r0.n;
		frax.d = frax.d * fl + r0.d;
		if (x == (double)frax.n/(double)frax.d) return;
		s = 1 / (s - fl);
	}
}

double  dot( Kvector a, Kvector b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Kvector  rotate( Kvector vertex, Kvector axis, double angle )
{
	Kvector p = dot(axis, vertex) * axis;
	return p + cos(angle)*(vertex - p) +
		sin(angle)*(axis ^ vertex);
}

/****************************************************************************
*	List of Uniform Polyhedra and Their Kaleidoscopic Formulae
*	==========================================================
*
*	Each entry contains the following items:
*
*	1)	Wythoff symbol.
*	2)	Polyhedron name.
*	3)	Dual name.
*	4)	Coxeter &al. reference figure.
*	5)	Wenninger reference figure.
*
*	Notes:
*
*	(1)	Cundy&Roulette's trapezohedron has been renamed to
*		deltohedron, as its faces are deltoids, not trapezoids.
*	(2)	The names of the non-dihedral polyhedra are those
*		which appear in Wenninger (1984). Some of them are
*		slightly modified versions of those in Wenninger (1971).
*
*	References:
*
*	Coxeter, H.S.M., Longuet-Higgins, M.S. & Miller, J.C.P.,
*		Uniform polyhedra, Phil. Trans. Royal Soc. London, Ser. A,
*		246 (1953), 401-409.
*	Cundy, H.M. & Rollett, A.P.,
*		"Mathematical Models", 3rd Ed., Tarquin, 1981.
*	Har'El, Z.
*		Unifom solution for uniform polyhedra, Geometriae Dedicata,
*		47 (1993), 57-110.
*	Wenninger, M.J.,
*		"Polyhedron Models", Cambridge University Press, 1971.
*		"Dual Models", Cambridge University Press, 1984.
*
****************************************************************************/

Uniform uniform[] = {

/****************************************************************************
*		Dihedral Schwarz Triangles (D5 only)
****************************************************************************/

/* (2 2 5) (D1/5) */

/*1*/	{"2 5|2","pentagonal prism",
			"pentagonal dipyramid",0,0},
/*2*/	{"|2 2 5","pentagonal antiprism",
			"pentagonal deltohedron",0,0},

/* (2 2 5/2) (D2/5) */

/*3*/	{"2 5/2|2","pentagrammic prism",
			"pentagrammic dipyramid",0,0},
/*4*/	{"|2 2 5/2","pentagrammic antiprism",
			"pentagrammic deltohedron",0,0},

/* (5/3 2 2) (D3/5) */

/*5*/	{"|2 2 5/3","pentagrammic crossed antiprism",
			"pentagrammic concave deltohedron",0,0},

/****************************************************************************
*		Tetrahedral Schwarz Triangles
****************************************************************************/

/* (2 3 3) (T1) */

/*6*/	{"3|2 3","tetrahedron",
			"tetrahedron",15,1},
/*7*/	{"2 3|3","truncated tetrahedron",
			"triakistetrahedron",16,6},

/* (3/2 3 3) (T2) */

/*8*/	{"3/2 3|3","octahemioctahedron",
			"octahemioctacron",37,68},

/* (3/2 2 3) (T3) */

/*9*/	{"3/2 3|2","tetrahemihexahedron",
			"tetrahemihexacron",36,67},

/****************************************************************************
*		Octahedral Schwarz Triangles
****************************************************************************/

/* (2 3 4) (O1) */

/*10*/	{"4|2 3","octahedron",
			"cube",17,2},
/*11*/	{"3|2 4","cube",
			"octahedron",18,3},
/*12*/	{"2|3 4","cuboctahedron",
			"rhombic dodecahedron",19,11},
/*13*/	{"2 4|3","truncated octahedron",
			"tetrakishexahedron",20,7},
/*14*/	{"2 3|4","truncated cube",
			"triakisoctahedron",21,8},
/*15*/	{"3 4|2","rhombicuboctahedron",
			"deltoidal icositetrahedron",22,13},
/*16*/	{"2 3 4|","truncated cuboctahedron",
			"disdyakisdodecahedron",23,15},
/*17*/	{"|2 3 4","snub cube",
			"pentagonal icositetrahedron",24,17},

/* (3/2 4 4) (O2b) */

/*18*/	{"3/2 4|4","small cubicuboctahedron",
			"small hexacronic icositetrahedron",38,69},

/* (4/3 3 4) (O4) */

/*19*/	{"3 4|4/3","great cubicuboctahedron",
			"great hexacronic icositetrahedron",50,77},
/*20*/	{"4/3 4|3","cubohemioctahedron",
			"hexahemioctacron",51,78},
/*21*/	{"4/3 3 4|","cubitruncated cuboctahedron",
			"tetradyakishexahedron",52,79},

/* (3/2 2 4) (O5) */

/*22*/	{"3/2 4|2","great rhombicuboctahedron",
			"great deltoidal icositetrahedron",59,85},
/*23*/	{"3/2 2 4|","small rhombihexahedron",
			"small rhombihexacron",60,86},

/* (4/3 2 3) (O7) */

/*24*/	{"2 3|4/3","stellated truncated hexahedron",
			"great triakisoctahedron",66,92},
/*25*/	{"4/3 2 3|","great truncated cuboctahedron",
			"great disdyakisdodecahedron",67,93},

/* (4/3 3/2 2) (O11) */

/*26*/	{"4/3 3/2 2|","great rhombihexahedron",
			"great rhombihexacron",82,103},

/****************************************************************************
*		Icosahedral Schwarz Triangles
****************************************************************************/

/* (2 3 5) (I1) */

/*27*/	{"5|2 3","icosahedron",
			"dodecahedron",25,4},
/*28*/	{"3|2 5","dodecahedron",
			"icosahedron",26,5},
/*29*/	{"2|3 5","icosidodecahedron",
			"rhombic triacontahedron",28,12},
/*30*/	{"2 5|3","truncated icosahedron",
			"pentakisdodecahedron",27,9},
/*31*/	{"2 3|5","truncated dodecahedron",
			"triakisicosahedron",29,10},
/*32*/	{"3 5|2","rhombicosidodecahedron",
			"deltoidal hexecontahedron",30,14},
/*33*/	{"2 3 5|","truncated icosidodechedon",
			"disdyakistriacontahedron",31,16},
/*34*/	{"|2 3 5","snub dodecahedron",
			"pentagonal hexecontahedron",32,18},

/* (5/2 3 3) (I2a) */

/*35*/	{"3|5/2 3","small ditrigonal icosidodecahedron",
			"small triambic icosahedron",39,70},
/*36*/	{"5/2 3|3","small icosicosidodecahedron",
			"small icosacronic hexecontahedron",40,71},
/*37*/	{"|5/2 3 3","small snub icosicosidodecahedron",
			"small hexagonal hexecontahedron",41,110},

/* (3/2 5 5) (I2b) */

/*38*/	{"3/2 5|5","small dodecicosidodecahedron",
			"small dodecacronic hexecontahedron",42,72},

/* (2 5/2 5) (I3) */

/*39*/	{"5|2 5/2","small stellated dodecahedron",
			"great dodecahedron",43,20},
/*40*/	{"5/2|2 5","great dodecahedron",
			"small stellated dodecahedron",44,21},
/*41*/	{"2|5/2 5","great dodecadodecahedron",
			"medial rhombic triacontahedron",45,73},
/*42*/	{"2 5/2|5","truncated great dodecahedron",
			"small stellapentakisdodecahedron",47,75},
/*43*/	{"5/2 5|2","rhombidodecadodecahedron",
			"medial deltoidal hexecontahedron",48,76},
/*44*/	{"2 5/2 5|","small rhombidodecahedron",
			"small rhombidodecacron",46,74},
/*45*/	{"|2 5/2 5","snub dodecadodecahedron",
			"medial pentagonal hexecontahedron",49,111},

/* (5/3 3 5) (I4) */

/*46*/	{"3|5/3 5","ditrigonal dodecadodecahedron",
			"medial triambic icosahedron",53,80},
/*47*/	{"3 5|5/3","great ditrigonal dodecicosidodecahedron",
			"great ditrigonal dodecacronic hexecontahedron",54,81},
/*48*/	{"5/3 3|5","small ditrigonal dodecicosidodecahedron",
			"small ditrigonal dodecacronic hexecontahedron",55,82},
/*49*/	{"5/3 5|3","icosidodecadodecahedron",
			"medial icosacronic hexecontahedron",56,83},
/*50*/	{"5/3 3 5|","icositruncated dodecadodecahedron",
			"tridyakisicosahedron",57,84},
/*51*/	{"|5/3 3 5","snub icosidodecadodecahedron",
			"medial hexagonal hexecontahedron",58,112},

/* (3/2 3 5) (I6b) */

/*52*/	{"3/2|3 5","great ditrigonal icosidodecahedron",
			"great triambic icosahedron",61,87},
/*53*/	{"3/2 5|3","great icosicosidodecahedron",
			"great icosacronic hexecontahedron",62,88},
/*54*/	{"3/2 3|5","small icosihemidodecahedron",
			"small icosihemidodecacron",63,89},
/*55*/	{"3/2 3 5|","small dodecicosahedron",
			"small dodecicosacron",64,90},

/* (5/4 5 5) (I6c) */

/*56*/	{"5/4 5|5","small dodecahemidodecahedron",
			"small dodecahemidodecacron",65,91},

/* (2 5/2 3) (I7) */

/*57*/	{"3|2 5/2","great stellated dodecahedron",
			"great icosahedron",68,22},
/*58*/	{"5/2|2 3","great icosahedron",
			"great stellated dodecahedron",69,41},
/*59*/	{"2|5/2 3","great icosidodecahedron",
			"great rhombic triacontahedron",70,94},
/*60*/	{"2 5/2|3","great truncated icosahedron",
			"great stellapentakisdodecahedron",71,95},
/*61*/	{"2 5/2 3|","rhombicosahedron",
			"rhombicosacron",72,96},
/*62*/	{"|2 5/2 3","great snub icosidodecahedron",
			"great pentagonal hexecontahedron",73,113},

/* (5/3 2 5) (I9) */

/*63*/	{"2 5|5/3","small stellated truncated dodecahedron",
			"great pentakisdodekahedron",74,97},
/*64*/	{"5/3 2 5|","truncated dodecadodecahedron",
			"medial disdyakistriacontahedron",75,98},
/*65*/	{"|5/3 2 5","inverted snub dodecadodecahedron",
			"medial inverted pentagonal hexecontahedron",76,114},

/* (5/3 5/2 3) (I10a) */

/*66*/	{"5/2 3|5/3","great dodecicosidodecahedron",
			"great dodecacronic hexecontahedron",77,99},
/*67*/	{"5/3 5/2|3","small dodecahemicosahedron",
			"small dodecahemicosacron",78,100},
/*68*/	{"5/3 5/2 3|","great dodecicosahedron",
			"great dodecicosacron",79,101},
/*69*/	{"|5/3 5/2 3","great snub dodecicosidodecahedron",
			"great hexagonal hexecontahedron",80,115},

/* (5/4 3 5) (I10b) */

/*70*/	{"5/4 5|3","great dodecahemicosahedron",
			"great dodecahemicosacron",81,102},

/* (5/3 2 3) (I13) */

/*71*/	{"2 3|5/3","great stellated truncated dodecahedron",
			"great triakisicosahedron",83,104},
/*72*/	{"5/3 3|2","great rhombicosidodecahedron",
			"great deltoidal hexecontahedron",84,105},
/*73*/	{"5/3 2 3|","great truncated icosidodecahedron",
			"great disdyakistriacontahedron",87,108},
/*74*/	{"|5/3 2 3","great inverted snub icosidodecahedron",
			"great inverted pentagonal hexecontahedron",88,116},

/* (5/3 5/3 5/2) (I18a) */

/*75*/	{"5/3 5/2|5/3","great dodecahemidodecahedron",
			"great dodecahemidodecacron",86,107},

/* (3/2 5/3 3) (I18b) */

/*76*/	{"3/2 3|5/3","great icosihemidodecahedron",
			"great icosihemidodecacron",85,106},

/* (3/2 3/2 5/3) (I22) */

/*77*/	{"|3/2 3/2 5/2","small retrosnub icosicosidodecahedron",
			"small hexagrammic hexecontahedron",91,118},

/* (3/2 5/3 2) (I23) */

/*78*/	{"3/2 5/3 2|","great rhombidodecahedron",
			"great rhombidodecacron",89,109},
/*79*/	{"|3/2 5/3 2","great retrosnub icosidodecahedron",
			"great pentagrammic hexecontahedron",90,117},

/****************************************************************************
*		Last But Not Least
****************************************************************************/

/*80*/	{"3/2 5/3 3 5/2","great dirhombicosidodecahedron",
			"great dirhombicosidodecacron",92,119}
};

int last_uniform = sizeof (uniform) / sizeof (uniform[0]);

} // namespace kaleido

namespace detail
{
	k3d::color colors[13] = {
		k3d::color(0, 0, 0), k3d::color(0, 0, 0), k3d::color(0, 0, 0),
		k3d::color(1, 0, 0), // triangles
		k3d::color(0, 1, 0), // quads
		k3d::color(0, 0, 1), // pentagons
		k3d::color(1, 1, 0), // hexagons
		k3d::color(0, 0, 0),
		k3d::color(1, 0, 1), // octagons
		k3d::color(0, 0, 0), k3d::color(0, 0, 0), k3d::color(0, 0, 0),
		k3d::color(0, 1, 1) // decagons
	};

	std::string names[13] = {
		"", "", "",
		"Triangles material",
		"Quadrilaterals material",
		"Pentagons material",
		"Hexagons material",
		"",
		"Octagons material",
		"", "", "",
		"Decagons material"
	};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// polyhedron_implementation

class polyhedron_implementation :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::node > > base;

public:
	polyhedron_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_document(Document),
		m_number(init_owner(*this) + init_name("number") + init_label(_("Number")) + init_description(_("Polyhedron Number")) + init_value(1) + init_constraint(constraint::minimum<k3d::int32_t>(1, constraint::maximum<k3d::int32_t>(80))) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_dual(init_owner(*this) + init_name("dual") + init_label(_("Dual")) + init_description(_("Polyhedron's dual")) + init_value(false)),
		m_size(init_owner(*this) + init_name("size") + init_label(_("Size")) + init_description(_("Polyhedron size (scale)")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_number.changed_signal().connect(make_reset_mesh_slot());
		m_dual.changed_signal().connect(make_reset_mesh_slot());
		m_size.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		k3d::imaterial* const material = m_material.pipeline_value();
		const unsigned long number = m_number.pipeline_value();
		const bool dual = m_dual.pipeline_value();
		const double size= m_size.pipeline_value();
		const std::string name = dual ? kaleido::uniform[number - 1].dual : kaleido::uniform[number - 1].name;

		LoadPolyhedron(number, dual);

		// Add geometry to the mesh ...
		for(unsigned long i = 0; i != m_PolyhedronPoints.size(); ++i)
			Mesh.points.push_back(new k3d::legacy::point(m_PolyhedronPoints[i] * m_PolyhedronScaling * size));

		Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
		k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

		unsigned long polygon_index = 0;
		for(unsigned long i = 0; i != m_PolyhedronPaths.size(); ++i)
		{
			if(!m_PolyhedronPaths[i].size())
				continue;

			std::vector<k3d::legacy::split_edge*> edges;
			for(unsigned long j = 0; j != m_PolyhedronPaths[i].size(); ++j)
				edges.push_back(new k3d::legacy::split_edge(Mesh.points[m_PolyhedronPaths[i][j]]));

			k3d::legacy::loop_edges(edges.begin(), edges.end());

			// Set material ...
			unsigned long color_index = *m_polygon_colors.begin();
			// Trick to handle special case #39 and #57
			if(polygon_index < m_polygon_colors.size())
				color_index = m_polygon_colors[polygon_index];

			k3d::legacy::face* new_face = new k3d::legacy::face(edges.front(), material);
			new_face->uniform_data["Cs"] = detail::colors[color_index];
			polyhedron.faces.push_back(new_face);

			polygon_index++;
		}
		k3d::legacy::set_companions(polyhedron);

		assert_warning(k3d::legacy::is_valid(polyhedron));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<polyhedron_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x916c98dd, 0x74f24362, 0x98364c94, 0x2a9f9ae7),
			"Polyhedron",
			_("Generates a uniform polyhedron"),
			"Polygons",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d::idocument& m_document;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_number;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_dual;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_size;

	//k3d_data_property(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint) m_previous_materials;

	/// Special variables and methods to generate polyhedra ...
	typedef std::vector<k3d::point3> PolyhedronPoints;
	PolyhedronPoints m_PolyhedronPoints;

	std::string m_PolyhedronProperties;
	double m_PolyhedronScaling;

	typedef std::vector<int> PolyhedronPath;
	typedef std::vector<PolyhedronPath> PolyhedronPaths;
	PolyhedronPaths m_PolyhedronPaths;

	// Triangulated self-intersecting polygons have to
	// use their real color: reate a triangulation list
	std::vector<unsigned long> m_polygon_colors;
	typedef std::map<unsigned long, k3d::imaterial*> material_map_t;
	material_map_t m_materials;

	void LoadPolyhedron(int PolyhedronIndex, bool Dual)
	{
		// Clear out old data ...
		m_PolyhedronPoints.clear();
		m_PolyhedronPaths.clear();
		m_PolyhedronScaling = 0.5;

		// Solve polyhedron
		kaleido::Polyhedron* polyhedron = kaleido::kaleido(PolyhedronIndex);
		return_if_fail(polyhedron);

		double freeze = 0;

		if(!Dual)
			vrmodel(polyhedron, polyhedron->v, polyhedron->V, polyhedron->f, polyhedron->F, false, AZ, EL, freeze);
		else
			vrmodel(polyhedron, polyhedron->f, polyhedron->F, polyhedron->v, polyhedron->V, true, AZ, EL, freeze);

		// Write the polyhedron geometric properties string
		m_PolyhedronProperties = "";
		if(Dual && (polyhedron->hemi || polyhedron->index == kaleido::last_uniform - 1))
		{
			m_PolyhedronProperties += "infinite dual";

			// Infinite duals have to scaled to a unit sphere
			double maxlength = 0.0;
			for(unsigned long i = 0; i < m_PolyhedronPoints.size(); i++)
			{
				double length = k3d::to_vector(m_PolyhedronPoints[i]).length();
				if(length > maxlength)
					maxlength = length;
			}

			m_PolyhedronScaling /= maxlength;
		}

		// Free the polyhedron data ...
		delete polyhedron;
	}

	void PolyhedronPoint(kaleido::Kvector v)
	{
		m_PolyhedronPoints.push_back(k3d::point3(v.x, v.y, v.z));
	}

	void PolyhedronFace()
	{
		m_PolyhedronPaths.push_back(PolyhedronPath());
	}

	void PolyhedronPathPoint(int PointIndex)
	{
		m_PolyhedronPaths.back().push_back(PointIndex);
	}

	void PolyhedronTriangle(int PointIndex1, int PointIndex2, int PointIndex3)
	{
		m_PolyhedronPaths.push_back(PolyhedronPath());

		m_PolyhedronPaths.back().push_back(PointIndex1);
		m_PolyhedronPaths.back().push_back(PointIndex2);
		m_PolyhedronPaths.back().push_back(PointIndex3);
	}

	void PolyhedronQuad(int PointIndex1, int PointIndex2, int PointIndex3, int PointIndex4)
	{
		m_PolyhedronPaths.push_back(PolyhedronPath());

		m_PolyhedronPaths.back().push_back(PointIndex1);
		m_PolyhedronPaths.back().push_back(PointIndex2);
		m_PolyhedronPaths.back().push_back(PointIndex3);
		m_PolyhedronPaths.back().push_back(PointIndex4);
	}

	void vrmodel(kaleido::Polyhedron *P, const std::vector<kaleido::Kvector>& v, int V, const std::vector<kaleido::Kvector>& f, int F, const bool dual, double azimuth, double elevation, double angle)
	{
		// Rotate frame
		static kaleido::Kvector X(1,0,0), Y(0,1,0), Z(0,0,1);
		kaleido::Kvector axis = kaleido::rotate (kaleido::rotate (X, Y, elevation), Z, azimuth);
		kaleido::Kvector rx = kaleido::rotate(X, axis, angle);
		kaleido::Kvector ry = kaleido::rotate(Y, axis, angle);
		kaleido::Kvector rz = kaleido::rotate(Z, axis, angle);

		// Rotate polyhedron
		std::vector<kaleido::Kvector> vb;
		for(int r = 0; r < V; r++)
		{
			kaleido::Kvector rotv = v[r].x * rx + v[r].y * ry + v[r].z * rz;
			vb.push_back(rotv);
			PolyhedronPoint(rotv);
		}

		std::vector<kaleido::Kvector> fb;
		for(int r = 0; r < F; r++)
			fb.push_back(f[r].x * rx + f[r].y * ry + f[r].z * rz);

	// Auxiliary vertices (needed because current VRML browsers cannot handle
	// non-simple paths, i.e., paths with self intersections):
	// Each non-simple face is assigned an auxiliary vertex. By connecting it to
	// the rest of the vertices the face is triangulated. The circum-center is used
	// for the regular star faces of uniform polyhedra. The in-center is used for
	// the pentagram (#79) and hexagram (#77) of the high-density snub duals, and
	// for the pentagrams (#40, #58) and hexagram (#52) of the stellated duals with
	// configuration (....)/2. Finally, the self-intersection of the crossed
	// parallelogram is used for duals with form p q r| with an even denominator.

	// This method do not work for the hemi-duals, whose faces are not star-shaped
	// and have two self-intersections each.
	// Thus, for each face we need six auxiliary vertices: The self intersections
	// and the terminal points of the truncations of the infinite edges. The ideal
	// vertices are listed, but are not used by the face-list.
	// Note that the face of the last dual (#80) is octagonal, and consists of two
	// quadrilaterals of the infinite type.

		int i, j, l, ll, ii;
		std::vector<int> hit;

		if(dual && P->even != -1)
			hit.resize(F);

		for(i = 0; i < F; i++)
			if(!dual && (kaleido::frac (P->n_sides[P->ftype[i]]),
				kaleido::frax.d != 1 && kaleido::frax.d != kaleido::frax.n - 1) ||
				dual && (P->K == 5 && P->D > 30 ||
				kaleido::denominator (P->m_faces[0]) != 1)) {
				// find the center of the face
				double h;
				if (!dual && P->hemi && !P->ftype[i])
					h = 0;
				else
					h = P->minr / dot (fb[i],fb[i]);
				PolyhedronPoint(h * fb[i]);
			} else if (dual && P->even != -1) {
				// find the self-intersection of a crossed parallelogram. hit is set if v0v1 intersects v2v3
				kaleido::Kvector v0, v1, v2, v3, c0, c1, p;
				double d0, d1;
				v0 = vb[P->incid[0][i]];
				v1 = vb[P->incid[1][i]];
				v2 = vb[P->incid[2][i]];
				v3 = vb[P->incid[3][i]];
				d0 = sqrt (dot (v0 - v2, v0 - v2));
				d1 = sqrt (dot (v1 - v3, v1 - v3));
				c0 = d1 *(v0 + v2);
				c1 = d0 *(v1 + v3);
				p = (0.5 / (d0 + d1))*(c0 + c1);
				PolyhedronPoint(p);
				p = (p - v2)^(p - v3);
				hit[i] = (dot (p, p) < 1e-6);
			} else if (dual && P->hemi && P->index != kaleido::last_uniform - 1) {
				// find the terminal points of the truncation and the
				// self-intersections.
				//  v23       v0       v21
				//  |  \     /  \     /  |
				//  |   v0123    v0321   |
				//  |  /     \  /     \  |
				//  v01       v2       v03

				kaleido::Kvector v0, v1, v2, v3, v01, v03, v21, v23, v0123, v0321 ;
				kaleido::Kvector u;
				double t = 1.5; //truncation adjustment factor
				j = !P->ftype[P->incid[0][i]];
				v0 = vb[P->incid[j][i]];//real vertex
				v1 = vb[P->incid[j+1][i]];//ideal vertex (unit kaleido::Kvector)
				v2 = vb[P->incid[j+2][i]];//real
				v3 = vb[P->incid[(j+3)%4][i]];//ideal
				//compute intersections
				//	this uses the following linear algebra:
				//	v0123 = v0 + a v1 = v2 + b v3
				//	v0 x v3 + a (v1 x v3) = v2 x v3
				//	a (v1 x v3) = (v2 - v0) x v3
				//	a (v1 x v3) . (v1 x v3) =
				//		(v2 - v0) x v3 . (v1 x v3)
				u = v1 ^ v3;
				v0123 = v0 + (dot((v2 - v0)^v3,
					u) / dot(u,u))*v1;
				v0321 = v0 + (dot((v0 - v2)^v1,
					u) / dot (u,u))*v3;
				//compute truncations
				v01 = v0 + t*(v0123 - v0);
				v23 = v2 + t*(v0123 - v2);
				v03 = v0 + t*(v0321 - v0);
				v21 = v2 + t*(v0321 - v2);
				PolyhedronPoint(v01);
				PolyhedronPoint(v23);
				PolyhedronPoint(v0123);
				PolyhedronPoint(v03);
				PolyhedronPoint(v21);
				PolyhedronPoint(v0321);
			} else if (dual && P->index == kaleido::last_uniform - 1) {
				// find the terminal points of the truncation and the
				// self-intersections.
				//  v23       v0       v21
				//  |  \     /  \     /  |
				//  |   v0123    v0721   |
				//  |  /     \  /     \  |
				//  v01       v2       v07
				//
				//  v65       v4       v67
				//  |  \     /  \     /  |
				//  |   v4365    v4567   |
				//  |  /     \  /     \  |
				//  v43       v6       v45

				kaleido::Kvector v0, v1, v2, v3, v4, v5, v6, v7, v01, v07, v21, v23;
				kaleido::Kvector v43, v45, v65, v67, v0123, v0721, v4365, v4567;
				double t = 1.5;//truncation adjustment factor
				kaleido::Kvector u;
				for (j = 0; j < 8; j++)
					if (P->ftype[P->incid[j][i]] == 3)
						break;
				v0 = vb[P->incid[j][i]];//real {5/3}
				v1 = vb[P->incid[(j+1)%8][i]];//ideal
				v2 = vb[P->incid[(j+2)%8][i]];//real {3}
				v3 = vb[P->incid[(j+3)%8][i]];//ideal
				v4 = vb[P->incid[(j+4)%8][i]];//real {5/2}
				v5 = vb[P->incid[(j+5)%8][i]];//ideal
				v6 = vb[P->incid[(j+6)%8][i]];//real {3/2}
				v7 = vb[P->incid[(j+7)%8][i]];//ideal
				//compute intersections
				u = v1 ^ v3;
				v0123 = v0 + (dot ((v2 - v0)^v3,
					u) / dot (u,u))*v1;
				u = v7 ^ v1;
				v0721 = v0 + (dot ((v2 - v0)^v1,
					u) / dot (u,u))*v7;
				u = v5 ^ v7;
				v4567 = v4 + (dot ((v6 - v4)^v7,
					u) / dot (u,u))*v5;
				u = v3 ^ v5;
				v4365 = v4 + (dot ((v6 - v4)^v5,
					u) / dot (u,u))*v3;
				//compute truncations
				v01 = v0 + t*(v0123 - v0);
				v23 = v2 + t*(v0123 - v2);
				v07 = v0 + t*(v0721 - v0);
				v21 = v2 + t*(v0721 - v2);
				v45 = v4 + t*(v4567 - v4);
				v67 = v6 + t*(v4567 - v6);
				v43 = v4 + t*(v4365 - v4);
				v65 = v6 + t*(v4365 - v6);
				PolyhedronPoint(v01);
				PolyhedronPoint(v23);
				PolyhedronPoint(v0123);
				PolyhedronPoint(v07);
				PolyhedronPoint(v21);
				PolyhedronPoint(v0721);
				PolyhedronPoint(v45);
				PolyhedronPoint(v67);
				PolyhedronPoint(v4567);
				PolyhedronPoint(v43);
				PolyhedronPoint(v65);
				PolyhedronPoint(v4365);
			}


		// Face list:
		// Each face is printed in a separate line, by listing the indices of its
		// vertices. In the non-simple case, the path is represented by the
		// triangulation, each triangle consists of two polyhedron vertices and one
		// auxiliary vertex.

		ii = V;
		for (i = 0; i < F; i++) {

			PolyhedronFace();

			if (dual) {
				if(P->K == 5 && P->D > 30 || kaleido::denominator (P->m_faces[0]) != 1) {
					for (j = 0; j < P->M - 1; j++)
						PolyhedronTriangle(P->incid[j][i], P->incid[j+1][i], ii);

					PolyhedronTriangle(P->incid[j][i], P->incid[0][i], ii++);

				} else if (P->even != -1) {
					if(hit[i])
					{
						PolyhedronTriangle(P->incid[3][i], P->incid[0][i], ii);
						PolyhedronTriangle(P->incid[1][i], P->incid[2][i], ii);
					}
					else
					{
						PolyhedronTriangle(P->incid[0][i], P->incid[1][i], ii);
						PolyhedronTriangle(P->incid[2][i], P->incid[3][i], ii);
					}
					ii++;
				} else if (P->hemi && P->index != kaleido::last_uniform - 1) {
					j = !P->ftype[P->incid[0][i]];
					PolyhedronTriangle(ii, ii+1, ii+2);
					PolyhedronQuad(P->incid[j][i], ii+2, P->incid[j+2][i], ii+5);
					PolyhedronTriangle(ii+3, ii+4, ii+5);
					ii += 6;
				} else if (P->index == kaleido::last_uniform - 1) {
					for (j = 0; j < 8; j++)
						if (P->ftype[P->incid[j][i]] == 3)
							break;
					PolyhedronTriangle(ii, ii+1, ii+2);
					PolyhedronQuad(P->incid[j][i], ii+2, P->incid[(j+2)%8][i], ii+5);
					PolyhedronTriangle(ii+3, ii+4, ii+5);

					PolyhedronTriangle(ii+6, ii+7, ii+8);
					PolyhedronQuad(P->incid[(j+4)%8][i], ii+8, P->incid[(j+6)%8][i], ii+11);
					PolyhedronTriangle(ii+9, ii+10, ii+11);
					ii += 12;
				} else {
					for (j = 0; j < P->M; j++)
						PolyhedronPathPoint(P->incid[j][i]);
				}
			} else {
				int split = (kaleido::frac (P->n_sides[P->ftype[i]]),
					 kaleido::frax.d != 1 && kaleido::frax.d != kaleido::frax.n - 1);
				int k = 0;
				for (j = 0; j < V; j++) {
					for (k = 0; k < P->M; k++)
						if (P->incid[k][j] == i)
							break;
					if (k != P->M)
						break;
				}
				if (!split)
					PolyhedronPathPoint(j);
				ll = j;
				for (l = P->adj[k][j]; l != j; l = P->adj[k][l]) {
					for (k = 0; k < P->M; k++)
						if (P->incid[k][l] == i)
							break;
					if (P->adj[k][l] == ll) k = kaleido::mod (k + 1 , P->M);
					if (!split)
						PolyhedronPathPoint(l);
					else
						PolyhedronTriangle(ll, l, ii);
					ll = l;
				}

				if (split)
					PolyhedronTriangle(ll, j, ii++);
			}
		}

		// Face color indices - for polyhedra with multiple face types
		// For non-simple faces, the index is repeated as many times
		// as needed by the triangulation
		m_polygon_colors.clear();

		if(dual)
		{
			m_polygon_colors.push_back(P->M);
		}
		else if(P->N == 1)
		{
			for(i = 0; i < F; i++)
				m_polygon_colors.push_back(kaleido::numerator(P->n_sides.front()));
		}
		else
		{
			for(i = 0; i < F; i++)
				if(kaleido::frac(P->n_sides[P->ftype[i]]), kaleido::frax.d == 1 || kaleido::frax.d == kaleido::frax.n - 1)
					m_polygon_colors.push_back(kaleido::numerator(P->n_sides[P->ftype[i]]));
				else
					for(j = 0; j < kaleido::frax.n; j++)
						m_polygon_colors.push_back(kaleido::numerator(P->n_sides[P->ftype[i]]));
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// polyhedron_factory

k3d::iplugin_factory& polyhedron_factory()
{
	return polyhedron_implementation::get_factory();
}

} // namespace libk3dprimitives

