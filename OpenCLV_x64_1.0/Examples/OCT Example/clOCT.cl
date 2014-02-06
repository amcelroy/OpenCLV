__kernel void PreFFT_32f(__global unsigned short *Raw,
                           float Gain,
                           float Bias,
		           __global float *Windowed){
	
	int k = get_global_id(0); //depth
	int j = get_global_id(1);  //height
	int i = get_global_id(2);  //width
		
	int linear_coord = i + get_global_size(2)*j + get_global_size(1)*get_global_size(2)*k;

 	float in = ((float)Raw[linear_coord])*Gain - Bias;
	float t_WindowSize = get_global_size(2);	    //Set the window size				
	float t = (float)i;
		                                                               
	t = 2*M_PI_F*t / (t_WindowSize - 1);		    //Compute Hanning Window
	Windowed[linear_coord] = in*(.5 - .5*cos(t));       //Finish Hanning Window and output
}

__kernel void PreFFT_Global_32f(__global unsigned short *Raw,
                           float Gain,
                           float Bias,
						   __global float *HanningWindow,
		           __global float *Windowed){
	
	int k = get_global_id(0); //depth
	int j = get_global_id(1);  //height
	int i = get_global_id(2);  //width
		
	int linear_coord = i + get_global_size(2)*j + get_global_size(1)*get_global_size(2)*k;

 	float in = ((float)Raw[linear_coord])*Gain - Bias;
	
	Windowed[linear_coord] = in*HanningWindow[i];      //Finish Hanning Window and output
}

__kernel void PreFFT_Local_32f(__global unsigned short *Raw,
                           	float Gain,
                           	float Bias,
							         __global float *HanningWindow,
				            __local float *WindowMultiple,
		           	  __global float *Windowed){
	
	//The local_id is usually a base 2 for example 256.  Therefore,
	//we can't buffer the entire hanning window into local memory
	//Note: Since the since the size of the Hanning Window is 
	//      tied to the global thread count, we never have to 
	//		worry about accessing memory outside of the array
	WindowMultiple[get_local_id(2)] = HanningWindow[get_global_id(2)];

	//This forces all the memory reads in the workgroup to 
	//execute in a batch instead of individually
	read_mem_fence(CLK_LOCAL_MEM_FENCE); 

	int k = get_global_id(0); //depth
	int j = get_global_id(1);  //height
	int i = get_global_id(2);  //width	
	
	int linear_coord = i + get_global_size(2)*j + get_global_size(1)*get_global_size(2)*k;
 	
	float in = ((float)Raw[linear_coord])*Gain - Bias;

	WindowMultiple[get_local_id(2)] = in*WindowMultiple[get_local_id(2)];       //Finish Hanning Window and output

	Windowed[linear_coord] = WindowMultiple[get_local_id(2)];
	
	write_mem_fence(CLK_GLOBAL_MEM_FENCE);
}

__kernel void PostFFT_32f(__global float *Real,
			    __global float *Imag,
			    __global float *Mag,
    			    __global float *Phase){
	int k = get_global_id(0);  //z
	int j = get_global_id(1);  //y
	int i = get_global_id(2);  //x
		
	int linear_coord = i + get_global_size(2)*j + get_global_size(1)*get_global_size(2)*k;
        
    float o = hypot(Real[linear_coord], Imag[linear_coord]);
	Mag[linear_coord] = 20*log10(o);
	
	Phase[linear_coord] = atan2(Imag[linear_coord], Real[linear_coord]);

} 




