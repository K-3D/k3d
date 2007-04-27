/* srf_vol_cube - Brian Steiner - Sony Pictures Imageworks

   This shader raytraces a box that is one unit in size,
   and then ray marches through the volume
   StepSize            - distance between sample points.
   StepJitter          - 0-1 jitter the sample position.
   Density             - volume thickes per unit.
   Epsilon             - offset for calculating gradient normal. 
   Vol_Mult, Vol_Offset - animation controls.
   Do_Shading          - if 1, shading will be calculated.
   SurfNormalDepth     - the mixing depth from surface
                         normal to volume normal.
   Additive            - if 1 add samples, if 0 over samples . 
   ShowActiveVol       - if 1 show the active volume instead of density.
   RunShadowPass       - set to 1 if running a shadow pass.
*/
 
 
 
/*-----------------------------------------------------*/
/* fnc_traceBox returns an intersection point on a box */
point
fnc_traceBox (float XMin;
	      float XMax;
	      float YMin;
	      float YMax;
	      float ZMin;
	      float ZMax;
	      float idx;
	      string refractSpace;)
{
    extern point P;
    extern vector I;
    extern normal N;
    vector Rd,Ro;
    point Ri;
    vector Pn;
    float D,T;
    float TMin = 1000000;
    vector IN;
    normal NN;
 
    IN = normalize(I);
    NN = normalize(N);
    Rd = vtransform(refractSpace,IN);
    Ro = transform(refractSpace,P);
 
    /*plane_z_min*/
    Pn = (0,0,1);
    D = ZMin * -1;
    T = -(Pn . Ro + D) / (Pn . Rd);
    if(T > 0){
	TMin = T;
	Ri = Ro + T*Rd;
    }
    /*plane_z_max*/
    Pn = (0,0,-1);
    D = ZMax;
    T = -(Pn . Ro + D) / (Pn . Rd);
    if(T > 0 && T < TMin){
	TMin = T;
	Ri = Ro + T*Rd;
    }
    /*plane_x_min*/
    Pn = (1,0,0);
    D = XMin * -1;
    T = -(Pn . Ro + D) / (Pn . Rd);
    if(T > 0 && T < TMin){
	TMin = T;
	Ri = Ro + T*Rd;
    }
    /*plane_x_max*/
    Pn = (-1,0,0);
    D = XMax;
    T = -(Pn . Ro + D) / (Pn . Rd);
    if(T > 0 && T < TMin){
	TMin = T;
	Ri = Ro + T*Rd;
    }
    /*plane_y_min*/
    Pn = (0,1,0);
    D = YMin *-1;
    T = -(Pn . Ro + D) / (Pn . Rd);
    if(T > 0 && T < TMin){
	TMin = T;
	Ri = Ro + T*Rd;
    }
    /*plane_y_max*/
    Pn = (0,-1,0);
    D = YMax;
    T = -(Pn . Ro + D) / (Pn . Rd);
    if(T > 0 && T < TMin){
	TMin = T;
	Ri = Ro + T*Rd;
    }
    return Ri;
}
 
 
 
/*--------------------------------------------------*/
/* active_volume - controls animation in the volume */
float
active_volume(point Pos; float vol_mult, vol_offset;)
{
    return (noise((Pos+30.445)*2)-.5+vol_offset)*vol_mult;
}
 
 
 
/*-------------------------------------------------------*/
/* density function will return the final volume density */
float
get_density(point Pos; float vol_mult, vol_offset;)
{
    float dens = 0;
    float activeVol = 0;
    float offset_active = .1;
    float mult_active = 20;
    activeVol = active_volume(Pos,vol_mult,vol_offset);
    dens = pow(1-abs(noise(Pos*7)*2-1),3);
    dens += pow(1-abs(noise((Pos+24.72)*7)*2-1),3);
    return activeVol + (dens-2);
}
 

/*--------------------------------------*/
/* normal calculation inside the volume */
normal calcGradeNorm(point Pos; float vol_mult, vol_offset, dens, epsilon;)
{
    normal Nd;
    Nd = normal (get_density(point (xcomp(Pos) - epsilon, ycomp(Pos),
				    zcomp(Pos)),vol_mult,vol_offset) - dens,
		 get_density(point (xcomp(Pos),
				    ycomp(Pos) - epsilon, zcomp(Pos)),vol_mult,vol_offset) - dens,
		 get_density(point (xcomp(Pos),
				    ycomp(Pos), zcomp(Pos) - epsilon),vol_mult,vol_offset) - dens);
    Nd = ntransform("object","current",Nd);
    return Nd;
}
 
 
 
 
/*-----------------------------------------------*/
/* shading function returns diffuse ans specular */
void get_shading (point Pos;
		  normal Nf;
		  vector V;
		  float Roughness;
		  output color diff;
		  output color spec;)
{
    extern vector L;
    extern color Cl;
    diff = 0;
    spec = 0;
    illuminance (Pos, Nf, radians(90)){
	diff += Cl * max(0,normalize(L).Nf);
	spec += Cl * specularbrdf(L, Nf, V, Roughness);
    }
}
 
 
 
/*-------------*/
/* nomal mixer */
normal 
fnc_normalMix (normal N1; normal N2; float mixer)
{
    float N1_mag = 1;
    float N2_mag = 1;
    normal NN1 = normalize(N1);
    normal NN2 = normalize(N2);
    normal result; 
    N1_mag *= 1-mixer;
    N2_mag *= mixer;
    result = normalize(NN1 * N1_mag + NN2 * N2_mag);
    return result;
}
 
 
 
/*---------------------------*/
/* main ray marching shader  */
surface
k3d_volcube(float  StepSize       = 1;
	     float  StepJitter     = 0;
	     float  Density        = 1;
	     float  Epsilon        = .001;
	     float  Vol_Mult       = 1;
	     float  Vol_Offset     = 0;
	     float  Do_Shading     = 1;
	     float  SurfNormalDepth = .05;
	     float  Additive       = 1;
	     float  ShowActiveVol  = 0;
	     float  RunShadowPass  = 0;
    )
{
    point  inPoint_obj  = transform("object",P);
    point  outPoint_obj = fnc_traceBox(-.501,.501,-.501,.501,-.501,.501,1,"object");
    vector V  = normalize(-I);
    normal Nf = normalize(N);
    float  Roughness    = .21;
    color  diff	= 1;
    color  spec = 0;
    float  vol_length   = length(outPoint_obj-inPoint_obj);
    float  numOfSteps   = vol_length/StepSize;
    vector step_obj     = (outPoint_obj-inPoint_obj)/numOfSteps;
    vector step_cur     = vtransform("object","current",step_obj);
    float  curStep      = 0;
    float  density_sum  = 0;
    color  color_sum    = 0;
    float  shad_sum     = 0;
    float  remainder    = 100;
    float  cur_density  = 0;
    color  cur_color    = 0;
    float  density      = StepSize * Density;
    float  jitter       = (random() - .5) * StepJitter;
    float  cur_depth    = 0;
    point  Pcur_obj     = inPoint_obj + jitter * step_obj;
    point  Pcur        = P  + jitter * step_cur;
 
    Oi = 0;
    Ci = 0;
 
    /*----------------------------*/
    /*     step loop              */
    while(curStep < numOfSteps && density_sum < 1){
	cur_density = 0;
	cur_color = 0;
 
	/*--- Run Density Function ---*/
	if(ShowActiveVol == 1)
	    cur_density = active_volume(Pcur_obj,Vol_Mult,Vol_Offset);
	else
	    cur_density = get_density(Pcur_obj,Vol_Mult,Vol_Offset);
 
	/*--- If Density > 0 Run The Rest Of The Loop ---*/
	if(cur_density > 0 && RunShadowPass == 0){
	    cur_color = cur_density;
	    cur_color = 1;
	    if(Do_Shading > 0){
		if(cur_depth > 0){
		    normal Vol_Nf = calcGradeNorm(Pcur_obj,Vol_Mult,Vol_Offset,
						  cur_density,Epsilon);
		    Vol_Nf = normalize(Vol_Nf);
		    Nf = fnc_normalMix(Nf,Vol_Nf,clamp(cur_depth/SurfNormalDepth,0,1));
		}
		get_shading(Pcur,Nf,V,Roughness,diff,spec);
	    }
	    cur_color = (cur_color * diff) + spec*(1,.8,.2);
	    
	    /*---- if sample is not a full step ----*/
	    remainder = numOfSteps - curStep;
	    if(remainder < 1){
		cur_density *= remainder;
	    } 
 
	    cur_density *= density;
	    cur_color *= clamp(cur_density,0,1);
 
	    /*---- Composite Sample ----*/
	    if(Additive > 0){      
                /* Just Add Up Density */
		density_sum += max(0,cur_density);
		color_sum += clamp(cur_color,color 0, color 1);
	    }
	    else{
                /* Do Over Instead of Add */
		cur_color = clamp(cur_color,color 0,color 1);
		cur_density = clamp(cur_density,0,1);
		color_sum = ( cur_color) * (1-density_sum) + color_sum;
		density_sum = (cur_density) * (1-density_sum) + density_sum;
	    }
	}
	else {                   
            /* if Shadow Pass */
	    if(Additive > 0){
		shad_sum += max(0,cur_density);
	    }
	    else{ 
		cur_density = clamp(cur_density,0,1);
		shad_sum = (cur_density) * (1-shad_sum) + shad_sum;
	    }
	    if(shad_sum >= .5){
		density_sum = 1;
		color_sum = 1;
	    }
	    P = Pcur;           
            /* Displace Point To Current Sample */
	}
	
	/* jump to the next sample point */
	curStep += 1;
	Pcur_obj += step_obj;
	Pcur += step_cur;
	cur_depth += StepSize;
    }
 
    Ci = color_sum;
    Oi = density_sum;
}
