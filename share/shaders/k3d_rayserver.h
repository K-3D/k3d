/* rayserver.h - Shading Language (PRMan side) include file for ray server.
 * 
 * These macros translate calls to trace(), visibility(), and
 * rayhittest() into calls to rayserver().  Each generates different
 * numbers of args, allowing rayserver() to be pathmorphic.
 *
 * It is assumed that the rayserver() function itself is implemented
 * as a DSO shadeop.  (See docs for PRMan 3.8 or later.)
 *
 * Note that the ray server expects its data in world space.
 *
 * Author: Larry Gritz (lg@bmrt.org)
 *
 * Reference:  
 *   Gritz, Larry, "Ray Tracing in PRMan (with a little help from
 *   BMRT)".  Appeared in: Apodaca and Gritz, eds., "Advanced
 *   RenderMan: Beyond the Companion", SIGGRAPH '98 course notes
 *   (Course #11), July 20, 1998.
 */

#ifndef RAYSERVER_H
#define RAYSERVER_H

#ifndef BMRT

/* PRMan side only -- BMRT already knows these functions */

#define worldp(p) transform("world",p)
#define worldv(v) vtransform("world",v)

#define trace(p,d) rayserver(worldp(p), worldv(d))

#define visibility(from,to) rayserver(worldp(from), worldp(to))

#define rayhittest(p,d,phit,nhit) \
        rayserver(worldp(p), worldv(d), phit,nhit)

#define fulltrace(p,d,chit,hitdist,phit,nhit,pmiss,rmiss) \
        rayserver(worldp(p),worldv(d), chit,hitdist,phit,nhit,pmiss,rmiss)

#define areashadow(Ps,nsamples) rayserver(worldp(Ps), nsamples, __lightname)

#define rs_indirect(Ps,Ns) \
        rayserver_indirect(worldp(Ps), normalize(worldv(Ns)))

float raylevel () { return 0; }
float isshadowray () { return 0; }

#endif

#endif
