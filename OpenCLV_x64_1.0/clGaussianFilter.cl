__kernel void gaussian_filter_3d_32f(__global float *Raw, 
						   __constant float *Kernel,
						    int kernelsize,
						   __global float *Processed){

	int k = get_global_id(0);  //z
	int j = get_global_id(1);  //y
	int i = get_global_id(2);  //x
		
	 int linear_coord = i + get_global_size(2)*j + get_global_size(1)*get_global_size(2)*k;

	int kernel_it = (kernelsize - 1)/2;
	int kernel_it_z = 0;

	if(depth < kernelsize){
		kernel_it_z = 0;
	}else{
		kernel_it_z = kernel_it;
	}

	float Processed_Value = 0;

	for(int z = -kernel_it_z; z <= kernel_it_z; z++){
		for(int y = -kernel_it; y <= kernel_it; y++){
			for(int x = -kernel_it; x <= kernel_it; x++){
				//test pixel to prevent memory error
				int tempx = 0;
				int tempy = 0;
				int tempz = 0;
							
				//clamps data to edge
				if(i + x < 0 || i + x >= width){
					tempx = i;
				}else{
					tempx = i + x;
				}

				if(j + y < 0 || j + y >= height){
					tempy = j;
				}else{
					tempy = j + y;
				}

				if(k + z < 0 || k + z >= depth){
					tempz = k;
				}else{
					tempz = k + z;
				}

				int raw_linear_coords = (tempx) + (tempy)*get_global_size(2) + (tempz)*get_global_size(1)*get_global_size(2);
				int kernel_linear_coords = (x + kernel_it) + (y + kernel_it)*kernelsize + (z + kernel_it_z)*kernelsize*kernelsize;
				float raw_value = Raw[raw_linear_coords];
				float kernel_value = Kernel[kernel_linear_coords];
				Processed_Value = Processed_Value + raw_value*kernel_value;
			}
		}
	}

	Processed[linear_coord] = Processed_Value;
}

