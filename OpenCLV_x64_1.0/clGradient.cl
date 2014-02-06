#pragma OPENCL EXTENSION cl_khr_fp64: enable

__kernel void Gradient_32f(__global float *Raw, 
						   __global float *X,
						   __global float *Y,
						   __global float *Z){ 


	//Since we are using global offset, the actual width is (get_global_size(2) + xkernelsize)
	// and the height is (get_global_size(1) + ykernelsize)

	int k = get_global_id(0);  //z
	int j = get_global_id(1);  //y
	int i = get_global_id(2);  //x


	int linear_coords = i + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*k;	
	float Processed_Value = 0;

	if(get_global_id(2) > 0 && get_global_id(2) < get_global_size(2) - 1 && 
	   get_global_id(1) > 0 && get_global_id(1) < get_global_size(1) - 1 &&
	   get_global_id(0) > 0 && get_global_id(0) < get_global_size(0) - 1){

		int linear_coord_minus_one_x = i - 1 + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*k;
		int linear_coord_plus_one_x = i + 1 + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*k;
		Processed_Value = -Raw[linear_coord_minus_one_x] + Raw[linear_coord_plus_one_x];
		X[linear_coords] = Processed_Value;

		int linear_coord_minus_one_y = i + get_global_size(2)*(j - 1) + get_global_size(2)*get_global_size(1)*k;
		int linear_coord_plus_one_y = i + get_global_size(2)*(j + 1) + get_global_size(2)*get_global_size(1)*k;
		Processed_Value = -Raw[linear_coord_minus_one_y] + Raw[linear_coord_plus_one_y];
		Y[linear_coords] = Processed_Value;

		int linear_coord_minus_one_z = i + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*(k - 1);
		int linear_coord_plus_one_z = i + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*(k + 1);
		Processed_Value = -Raw[linear_coord_minus_one_z] + Raw[linear_coord_plus_one_z];
		Z[linear_coords] = Processed_Value;
	}else{
		X[linear_coords] = 0;
		Y[linear_coords] = 0;
		Z[linear_coords] = 0;
	}
}

__kernel void Gradient_64f(__global double *Raw, 
						   __global double *X,
						   __global double *Y,
						   __global double *Z){ 


	//Since we are using global offset, the actual width is (get_global_size(2) + xkernelsize)
	// and the height is (get_global_size(1) + ykernelsize)

	int k = get_global_id(0);  //z
	int j = get_global_id(1);  //y
	int i = get_global_id(2);  //x

	int linear_coords = i + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*k;		
	double Processed_Value = 0;
	
	if(get_global_id(2) > 0 && get_global_id(2) < get_global_size(2) - 1 && 
	   get_global_id(1) > 0 && get_global_id(1) < get_global_size(1) - 1 &&
	   get_global_id(0) > 0 && get_global_id(0) < get_global_size(0) - 1){

		int linear_coord_minus_one_x = i - 1 + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*k;
		int linear_coord_plus_one_x = i + 1 + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*k;
		Processed_Value = -Raw[linear_coord_minus_one_x] + Raw[linear_coord_plus_one_x];
		X[linear_coords] = Processed_Value;

		int linear_coord_minus_one_y = i + get_global_size(2)*(j - 1) + get_global_size(2)*get_global_size(1)*k;
		int linear_coord_plus_one_y = i + get_global_size(2)*(j + 1) + get_global_size(2)*get_global_size(1)*k;
		Processed_Value = -Raw[linear_coord_minus_one_y] + Raw[linear_coord_plus_one_y];
		Y[linear_coords] = Processed_Value;

		int linear_coord_minus_one_z = i + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*(k - 1);
		int linear_coord_plus_one_z = i + get_global_size(2)*j + get_global_size(2)*get_global_size(1)*(k + 1);
		Processed_Value = -Raw[linear_coord_minus_one_z] + Raw[linear_coord_plus_one_z];
		Z[linear_coords] = Processed_Value;
	}else{
		X[linear_coords] = 0;
		Y[linear_coords] = 0;
		Z[linear_coords] = 0;
	}
}



