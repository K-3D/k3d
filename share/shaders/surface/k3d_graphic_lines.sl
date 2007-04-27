/*
 * 2006-03-13 dan@3-e.net
 * need to clean up and rewrite this code to be faster and more compact.
 */
 
#include "k3d_rmannotes.h"

surface k3d_graphic_lines (

        float Ka=1.0;
        float Kd=1.0;
        float Ks=0.6;
        float roughness=1.0;
        float paint_spec=0.6;
        float paint_trans=0.0;
        float paint_fuzz=0.0;
        float ink_thresh=0.10;
        float ink_fuzz=0.0;
				float line_scale_master=1.0;

        string up_highlight="z";
        float line_scale_highlight=16.0;
        float contrast_highlight=1.0;
        float brightness_highlight=0.5;
        float randomness_highlight=0.0;
        float noise_size_highlight=1.0;
        color color_curve_highlight_color1=(0.0,0.0,0.0);
        color color_curve_highlight_color2=(0.1,0.05,0.0);
        color color_curve_highlight_color3=(0.8,0.6,0.4);
        color color_curve_highlight_color4=(1.0,1.0,0.8);
        color color_curve_highlight_color5=(1.0,1.0,1.0);
        color color_curve_highlight_color6=(1.0,1.0,1.0);
        float surface_opac_highlight=1.0;
        float surface_trans_highlight=1.0;
        float fuzz_highlight=0.3;
				float tfreq_highlight=50;
				color illumcolor_highlight=(0.0,0.0,0.0);
				float illumination_highlight=0.0;
				float Ka_highlight=0.10;
				float Kd_highlight=0.70;
				float Ks_highlight=0.20;
				float roughness_highlight=0.2;
				
        string up_paint="y";
        float line_scale_paint=16.0;
        float contrast_paint=1.0;
        float brightness_paint=0.3;
        float randomness_paint=0.0;
        float noise_size_paint=1.0;
        color color_curve_paint_color1=(0.0,0.0,0.0);
        color color_curve_paint_color2=(0.1,0.05,0.0);
        color color_curve_paint_color3=(0.8,0.6,0.4);
        color color_curve_paint_color4=(1.0,1.0,0.8);
        color color_curve_paint_color5=(1.0,1.0,1.0);
        color color_curve_paint_color6=(1.0,1.0,1.0);
        float surface_opac_paint=1.0;
        float surface_trans_paint=1.0;
				float fuzz_paint=0.3;
				float tfreq_paint=50;
				color illumcolor_paint=(0.0,0.0,0.0);
				float illumination_paint=0.0;
				float Ka_paint=0.10;
				float Kd_paint=0.70;
				float Ks_paint=0.20;
				float roughness_paint=0.2;
					
        string up_ink="x";
        float line_scale_ink=16.0;
        float contrast_ink=1.0;
        float brightness_ink=0.5;
        float randomness_ink=0.0;
        float noise_size_ink=1.0;
        color color_curve_ink_color1=(0.0,0.0,0.0);
        color color_curve_ink_color2=(0.1,0.05,0.0);
        color color_curve_ink_color3=(0.8,0.6,0.4);
        color color_curve_ink_color4=(1.0,1.0,0.8);
        color color_curve_ink_color5=(1.0,1.0,1.0);
        color color_curve_ink_color6=(1.0,1.0,1.0);
        float surface_opac_ink=1.0;
        float surface_trans_ink=1.0; 
				float fuzz_ink=0.3;
				float tfreq_ink=50;
				color illumcolor_ink=(0.0,0.0,0.0);
				float illumination_ink=0.0;
				float Ka_ink=0.10;
				float Kd_ink=0.70;
				float Ks_ink=0.20;
				float roughness_ink=0.2;
)
{
	/** Surface main-code start **/

	/** hightlights **/
	
	point P_highlight = P;
	normal N_highlight = N;
	color Ci_highlight = Ci;
	color Oi_highlight = Oi;

	color surface_color_highlight;
	color layer_color_highlight;
	color color_curve_highlight;
	color layer_opac_highlight;
	float stripemin_highlight;
	float stripemax_highlight;
	float tt_highlight;
	float x_highlight;
	float y_highlight;
	float z_highlight;
	float i_highlight;
	float n_highlight;
	float ns_highlight;
	vector Nf_highlight;
	vector V_highlight;
	point Psh_highlight;
	Psh_highlight=transform("shader",P_highlight);
	Nf_highlight=faceforward(normalize(N_highlight),I);
	V_highlight=-normalize(I);

/** setup tt_highlight with chosen axis **/
	if (up_highlight == "z")
	{z_highlight=zcomp(Psh_highlight);
		z_highlight*=line_scale_highlight*line_scale_master;
		tt_highlight=mod(z_highlight,1);
	}
	else if (up_highlight == "y")
	{y_highlight=ycomp(Psh_highlight);
		y_highlight*=line_scale_highlight*line_scale_master;
		tt_highlight=mod(y_highlight,1);
	}
	else {x_highlight=xcomp(Psh_highlight);
		x_highlight*=line_scale_highlight*line_scale_master;
		tt_highlight=mod(x_highlight,1);
	}
	illumcolor_highlight=(Ka_highlight*ambient()+Kd_highlight*diffuse(Nf_highlight)+Ks_highlight*specular(Nf_highlight,V_highlight,roughness_highlight));
	illumination_highlight=max(max(comp(illumcolor_highlight,0),comp(illumcolor_highlight,1)),comp(illumcolor_highlight,2));
	n_highlight=0;
	ns_highlight=noise_size_highlight;

	for (i_highlight=0; i_highlight<6.0 ;i_highlight+=1.0){
		n_highlight+=(2*noise(Psh_highlight*ns_highlight)-1)/ns_highlight;
		ns_highlight*=2.17;
	}
	illumination_highlight=(illumination_highlight*contrast_highlight)+(1.0-contrast_highlight)/2.0+(brightness_highlight-1.0)+n_highlight*randomness_highlight;
	if (illumination_highlight<0.01)
	{surface_color_highlight=color(0.0,0.0,0.0);
		layer_color_highlight=0.0;
	}
	else if (illumination_highlight>0.99)
	{surface_color_highlight=color(1.0,1.0,1.0);
		layer_color_highlight=1.0;
	}
	else {surface_color_highlight=color(0.0,0.0,0.0);
		layer_color_highlight=1.0;
	}
	stripemin_highlight=0.5-smoothstep(0.0,1.0,illumination_highlight)/2.0;
	stripemax_highlight=0.5+smoothstep(0.0,1.0,illumination_highlight)/2.0;
	float val_highlight=(smoothstep((stripemin_highlight)-(fuzz_highlight),(stripemin_highlight),(tt_highlight))-smoothstep((stripemax_highlight)-(fuzz_highlight),(stripemax_highlight),(tt_highlight)));
	layer_opac_highlight=color(val_highlight);
	surface_color_highlight=((surface_color_highlight)*(1-(layer_opac_highlight))+(layer_color_highlight)*(layer_opac_highlight));
	float color_val_highlight=mix(0.0,1.0,val_highlight);
	color_curve_highlight=spline(color_val_highlight,color_curve_highlight_color1,color_curve_highlight_color2,color_curve_highlight_color3,color_curve_highlight_color4,color_curve_highlight_color5,color_curve_highlight_color6);
	float spacescale_highlight=length(vtransform("shader",normalize(N_highlight)));
	vector Ndisp_highlight=normalize(N_highlight)*(0/max(spacescale_highlight,1e-6));
	
	P_highlight+=0*Ndisp_highlight;
	N_highlight=normalize(calculatenormal(P_highlight+(1-0)*Ndisp_highlight));
	Ci_highlight=surface_opac_highlight*color_curve_highlight;
	Oi_highlight=surface_trans_highlight;
	
	/** paint **/
	
	point P_paint = P;
	normal N_paint = N;
	color Ci_paint = Ci;
	color Oi_paint = Oi;

	color surface_color_paint;
	color layer_color_paint;
	color color_curve_paint;
	color layer_opac_paint;
	float stripemin_paint;
	float stripemax_paint;
	float tt_paint;
	float x_paint;
	float y_paint;
	float z_paint;
	float i_paint;
	float n_paint;
	float ns_paint;
	vector Nf_paint;
	vector V_paint;
	point Psh_paint;
	Psh_paint=transform("shader",P_paint);
	Nf_paint=faceforward(normalize(N_paint),I);
	V_paint=-normalize(I);
	if (up_paint == "z")
	{z_paint=zcomp(Psh_paint);
		z_paint*=line_scale_paint*line_scale_master;
		tt_paint=mod(z_paint,1);
	}
	else if (up_paint == "y")
	{y_paint=ycomp(Psh_paint);
		y_paint*=line_scale_paint*line_scale_master;
		tt_paint=mod(y_paint,1);
	}
	else {x_paint=xcomp(Psh_paint);
		x_paint*=line_scale_paint*line_scale_master;
		tt_paint=mod(x_paint,1);
	}illumcolor_paint=(Ka_paint*ambient()+Kd_paint*diffuse(Nf_paint)+Ks_paint*specular(Nf_paint,V_paint,roughness_paint));
	illumination_paint=max(max(comp(illumcolor_paint,0),comp(illumcolor_paint,1)),comp(illumcolor_paint,2));
	n_paint=0;
	ns_paint=noise_size_paint;
	for ( i_paint=0; i_paint<6.0 ; i_paint+=1.0 ){
		n_paint+=(2*noise(Psh_paint*ns_paint)-1)/ns_paint;
		ns_paint*=2.17;
	}
	illumination_paint=(illumination_paint*contrast_paint)+(1.0-contrast_paint)/2.0+(brightness_paint-1.0)+n_paint*randomness_paint;
	if (illumination_paint<0.01)
	{surface_color_paint=color(0.0,0.0,0.0);
		layer_color_paint=0.0;
	}
	else if (illumination_paint>0.99)
	{surface_color_paint=color(1.0,1.0,1.0);
		layer_color_paint=1.0;
	}
	else {surface_color_paint=color(0.0,0.0,0.0);
		layer_color_paint=1.0;
	}stripemin_paint=0.5-smoothstep(0.0,1.0,illumination_paint)/2.0;
	stripemax_paint=0.5+smoothstep(0.0,1.0,illumination_paint)/2.0;
	float val1=(smoothstep((stripemin_paint)-(fuzz_paint),(stripemin_paint),(tt_paint))-smoothstep((stripemax_paint)-(fuzz_paint),(stripemax_paint),(tt_paint)));
	layer_opac_paint=color(val1);
	surface_color_paint=((surface_color_paint)*(1-(layer_opac_paint))+(layer_color_paint)*(layer_opac_paint));
	float color_val1=mix(0.0,1.0,val1);
	color_curve_paint=spline(color_val1,color_curve_paint_color1,color_curve_paint_color2,color_curve_paint_color3,color_curve_paint_color4,color_curve_paint_color5,color_curve_paint_color6);
	float spacescale1=length(vtransform("shader",normalize(N_paint)));
	vector Ndisp1=normalize(N_paint)*(0/max(spacescale1,1e-6));
	P_paint+=0*Ndisp1;
	N_paint=normalize(calculatenormal(P_paint+(1-0)*Ndisp1));
	Ci_paint=surface_opac_paint*color_curve_paint;
	Oi_paint=surface_trans_paint;
	
	/** ink **/
	
	point P_ink = P;
	normal N_ink = N;
	color Ci_ink = Ci;
	color Oi_ink = Oi;

	color surface_color_ink;
	color layer_color_ink;
	color color_curve_ink;
	color layer_opac_ink;
	float stripemin_ink;
	float stripemax_ink;
	float tt_ink;
	float x_ink;
	float y_ink;
	float z_ink;
	float i_ink;
	float n_ink;
	float ns_ink;
	vector Nf_ink;
	vector V_ink;
	point Psh_ink;
	Psh_ink=transform("shader",P_ink);
	Nf_ink=faceforward(normalize(N_ink),I);
	V_ink=-normalize(I);
	if (up_ink == "z")
	{z_ink=zcomp(Psh_ink);
		z_ink*=line_scale_ink*line_scale_master;
		tt_ink=mod(z_ink,1);
	}
	else if (up_ink == "y")
	{y_ink=ycomp(Psh_ink);
		y_ink*=line_scale_ink*line_scale_master;
		tt_ink=mod(y_ink,1);
	}
	else {x_ink=xcomp(Psh_ink);
		x_ink*=line_scale_ink*line_scale_master;
		tt_ink=mod(x_ink,1);
	}illumcolor_ink=(Ka_ink*ambient()+Kd_ink*diffuse(Nf_ink)+Ks_ink*specular(Nf_ink,V_ink,roughness_ink));
	illumination_ink=max(max(comp(illumcolor_ink,0),comp(illumcolor_ink,1)),comp(illumcolor_ink,2));
	n_ink=0;
	ns_ink=noise_size_ink;
	for ( i_ink=0; i_ink<6.0; i_ink+=1.0 ){
		n_ink+=(2*noise(Psh_ink*ns_ink)-1)/ns_ink;
		ns_ink*=2.17;
	}
	illumination_ink=(illumination_ink*contrast_ink)+(1.0-contrast_ink)/2.0+(brightness_ink-1.0)+n_ink*randomness_ink;
	if (illumination_ink<0.01)
	{surface_color_ink=color(0.0,0.0,0.0);
		layer_color_ink=0.0;
	}
	else if (illumination_ink>0.99)
	{surface_color_ink=color(1.0,1.0,1.0);
		layer_color_ink=1.0;
	}
	else {surface_color_ink=color(0.0,0.0,0.0);
		layer_color_ink=1.0;
	}stripemin_ink=0.5-smoothstep(0.0,1.0,illumination_ink)/2.0;
	stripemax_ink=0.5+smoothstep(0.0,1.0,illumination_ink)/2.0;
	float val2=(smoothstep((stripemin_ink)-(fuzz_ink),(stripemin_ink),(tt_ink))-smoothstep((stripemax_ink)-(fuzz_ink),(stripemax_ink),(tt_ink)));
	layer_opac_ink=color(val2);
	surface_color_ink=((surface_color_ink)*(1-(layer_opac_ink))+(layer_color_ink)*(layer_opac_ink));
	float color_val2=mix(0.0,1.0,val2);
	color_curve_ink=spline(color_val2,color_curve_ink_color1,color_curve_ink_color2,color_curve_ink_color3,color_curve_ink_color4,color_curve_ink_color5,color_curve_ink_color6);
	float spacescale2=length(vtransform("shader",normalize(N_ink)));
	vector Ndisp2=normalize(N_ink)*(0/max(spacescale2,1e-6));
	P_ink+=0*Ndisp2;
	N_ink=normalize(calculatenormal(P_ink+(1-0)*Ndisp2));
	Ci_ink=surface_opac_ink*color_curve_ink;
	Oi_ink=surface_trans_ink;

	/** blend 3 layers **/
	
	float
	glinespec(vector N, V; float roughness)
	{
		float C = 0;
		vector H;

		illuminance(P, N, PI/2) {
			H = normalize(normalize(L)+V);
			C += pow(N.H, 1/roughness);
		}
		return C;
	}

	float cos_here;
	float diff, spec;
	normal Nf;
	vector normI, half;

	color gline_color, layer_color;
	color gline_opac, layer_opac;

	Nf = faceforward(normalize(N), I);
	normI = normalize(I);

	/* background layer  */
	gline_color = color (0, 0, 0);
	gline_opac = Os;

	/* gline paint layer ) */

	/* only care about percentage illumination */
	diff = 0;
	illuminance(P, Nf, PI/2)
	diff += normalize(L).Nf;

	/* antialias the transition */
	diff = smoothstep(paint_trans - paint_fuzz/2, paint_trans + paint_fuzz/2, diff);

	/* specular -- transition also antialiased */
	spec = glinespec(Nf, -normalize(I), roughness);
	spec = smoothstep(paint_spec - paint_fuzz/2, paint_spec + paint_fuzz/2, spec);

	layer_color = Cs*(Kd*diff*Ci_paint + Ka*ambient()) + Ci_highlight*Ks*spec;
	layer_opac = Oi_paint;

	gline_color = blend(gline_color, layer_color, layer_opac);
	gline_opac = union(gline_opac, layer_opac);

	/* gline ink layer ) */

	cos_here = normalize(Nf).normI;

	/* antialias ink lines */
	layer_opac = 1 - smoothstep(ink_thresh - ink_fuzz/2, ink_thresh + ink_fuzz/2, abs(cos_here));

	if (abs(cos_here) < ink_thresh)
		layer_color = Ci_ink;
	else
		layer_color = 0;

	gline_color = blend(gline_color, layer_color, layer_opac);
	gline_opac = union(gline_opac, layer_opac);

	color glineOi = gline_opac;
	color glineCi = gline_opac * gline_color;

	/** Surface main-code end **/

	Ci = glineCi;
	Oi = gline_opac;
}
