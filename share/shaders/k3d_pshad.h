/************************************************************************
 * pshad.h - define macros for easy use of reference meshes and texture
 *           spaces.
 *
 * This file is meant to be included in shaders which use solid
 * texturing.  It provides two macros:
 *
 * PSHAD_PARAMS(space,freq) - put this macro in the parameter list of 
 *                your shader to declare a shadingspace parameter (and 
 *                optionally Pref, if USE_PREF is nonzero).  Takes as
 *                arguments the default shading space name and frequency.
 *
 * DEFAULT_PSHAD_PARAMS - calls PSHAD_PARAMS with default space "shader"
 *                and default frequency 1.
 *
 * GET_PSHAD    - put this in the body of your shader, near the top.
 *                It sets Pshad to the shading coordinates of P (or Pref,
 *                if provided), and sets dPshad to the expected change
 *                in Pshad between adjacent shading samples.
 *
 * This file expects that the .sl file #define's the symbol USE_PREF
 * _prior_ to inclusion of this header file.  
 *
 * Author: Larry Gritz (gritzl@acm.org)
 *
 * Reference:
 *   _Advanced RenderMan: Creating CGI for Motion Picture_, 
 *   by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 *
 * $Revision: 1.1 $    $Date: 2004/05/19 18:15:20 $
 *
 ************************************************************************/


#ifndef PSHAD_H
#define PSHAD_H 1


#ifndef FILTERWIDTH_H
#include "k3d_filterwidth.h"    /* Needed for filterwidth and friends */
#endif



/* If USE_PREF is not defined prior to inclusion of this file, assume
 * that Pref will not be used.
 */
#ifndef USE_PREF
#define USE_PREF 0
#endif

#if (USE_PREF)

/* Pick an unlikely point value to let us recognize an uninitialized,
 * and therefore nonexistant, Pref.
 */
#define UNINITIALIZED_PREF point (-1e10, -1e10, -1e10)

#define PSHAD_PARAMS(spacedefault,freqdefault) \
                     varying point Pref = UNINITIALIZED_PREF;           \
                     string shadingspace = spacedefault;                \
                     float shadingfreq = freqdefault

#define GET_PSHAD    varying point Pshad;                               \
                     if (Pref != UNINITIALIZED_PREF)                    \
                          Pshad = transform (shadingspace, Pref);       \
                     else Pshad = transform (shadingspace, P);          \
                     Pshad *= shadingfreq;                              \
                     float dPshad = filterwidthp(Pshad)

#else /* if (USE_PREF) */

#define PSHAD_PARAMS(spacedefault,freqdefault) \
                     string shadingspace = spacedefault;                \
                     float shadingfreq = freqdefault

#define GET_PSHAD    varying point Pshad;                               \
                     Pshad = shadingfreq * transform (shadingspace, P); \
                     float dPshad = filterwidthp(Pshad)

#endif /* USE_PREF */

#define DEFAULT_PSHAD_PARAMS PSHAD_PARAMS("shader",1)

#endif /* defined(PSHAD_H) */
