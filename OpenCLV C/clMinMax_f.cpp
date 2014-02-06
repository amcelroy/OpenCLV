//#include "clMinMax_f.h"
//
//clMinMax::clMinMax(){}
//
//clMinMax::~clMinMax(){}
//
//void clMinMax::SetData(float *p_Data, int Width, int Height, int Depth){
//	int size = Width*Height*Depth;
//	int i = 0;
//
//	//find the first divisible chunk size
//	for(i = 512; i > 0; i++){
//		if(size % i == 0){
//			m_Chunk = i;
//			break;
//		}	
//	}	
//
//	size_t size_bytes = sizeof(float)*size;
//
//	clLoadProgram("clMinMax.cl", "vectormin");
//	cl_mem InputMin = clCreateBuffer(m_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size_bytes, p_Data, &m_error);
//	__RecursiveSearchMin(InputMin, size);
//
//	clLoadKernel("vectormax");
//	//reset the chunk size
//	m_Chunk = i;
//	cl_mem InputMax = clCreateBuffer(m_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, size_bytes, p_Data, &m_error);
//	__RecursiveSearchMax(InputMax, size);
//}
//
//void clMinMax::__RecursiveSearchMin(cl_mem Input, int ItemsToSearch){
//	int NewItemsToSearch = ItemsToSearch / m_Chunk;
//	float Test = (float)ItemsToSearch / (float)m_Chunk;
//	
//	//Test if items to search is < the Chunk size
//	if(Test < 1){
//		//Items is less then chunk size, compute a new chunk size
//		m_Chunk = ItemsToSearch;
//	}
//
//	//Compute the size of the output array
//	size_t size_bytes = sizeof(float)*ItemsToSearch/m_Chunk;
//
//	cl_mem Output = clCreateBuffer(m_context, CL_MEM_READ_WRITE, size_bytes, NULL, &m_error);	
//	
//	m_error = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), &Input);
//	m_error = clSetKernelArg(m_kernel, 1, sizeof(int), &m_Chunk);
//	m_error = clSetKernelArg(m_kernel, 2, sizeof(cl_mem), &Output);
//	checkErr(m_error, "Couldn't set args");
//
//	size_t work_size = ItemsToSearch / m_Chunk;
//
//	m_error = clEnqueueNDRangeKernel(m_device_queue, m_kernel, 1,
//									NULL, &work_size, NULL, 0, NULL, NULL);
//	checkErr(m_error, "Divergence Kernel not Enqueued Properly");
//
//	//After this iteration is finished, delete the old input
//	clReleaseMemObject(Input);
//
//	if(Test < 1){
//		//we've reached the final iteration, read the final value and clean up the final mem object
//
//		//retrieve the min and max from the final subset
//		m_error = clEnqueueReadBuffer(m_device_queue, Output, CL_TRUE, 0, sizeof(float), 
//											&m_Min, 0, NULL, NULL);
//		checkErr(m_error, "Couldn't read the Min Buffer");
//
//		clReleaseMemObject(Output);
//	}else{
//		__RecursiveSearchMin(Output, NewItemsToSearch);
//	}
//
//	return;
//}
//
//void clMinMax::__RecursiveSearchMax(cl_mem Input, int ItemsToSearch){
//	int NewItemsToSearch = ItemsToSearch / m_Chunk;
//	float Test = (float)ItemsToSearch / (float)m_Chunk;
//	
//	//Test if items to search is < the Chunk size
//	if(Test < 1){
//		//Items is less then chunk size, compute a new chunk size
//		m_Chunk = ItemsToSearch;
//	}
//
//	//Compute the size of the output array
//	size_t size_bytes = sizeof(float)*ItemsToSearch/m_Chunk;
//
//	cl_mem Output = clCreateBuffer(m_context, CL_MEM_READ_WRITE, size_bytes, NULL, &m_error);	
//	
//	m_error = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), &Input);
//	m_error = clSetKernelArg(m_kernel, 1, sizeof(int), &m_Chunk);
//	m_error = clSetKernelArg(m_kernel, 2, sizeof(cl_mem), &Output);
//	checkErr(m_error, "Couldn't set args");
//
//	size_t work_size = ItemsToSearch / m_Chunk;
//
//	m_error = clEnqueueNDRangeKernel(m_device_queue, m_kernel, 1,
//									NULL, &work_size, NULL, 0, NULL, NULL);
//	checkErr(m_error, "Divergence Kernel not Enqueued Properly");
//
//	//After this iteration is finished, delete the old input
//	clReleaseMemObject(Input);
//
//	if(Test < 1){
//		//we've reached the final iteration, read the final value and clean up the final mem object
//		//retrieve the min or max from the final subset
//		m_error = clEnqueueReadBuffer(m_device_queue, Output, CL_TRUE, 0, sizeof(float), 
//											&m_Max, 0, NULL, NULL);
//		checkErr(m_error, "Couldn't read the Min Buffer");
//
//		clReleaseMemObject(Output);
//	}else{
//		__RecursiveSearchMax(Output, NewItemsToSearch);
//	}
//
//	return;
//}
//
//void clMinMax::TestMinMax(float *p_Data, int Width, int Height, int Depth){
//	int size = Width*Height*Depth;
//
//	float tempMin = 10000000;
//	float tempMax = -10000000;
//
//	for(int i = 0; i < size; i++){
//		float local = p_Data[i];
//
//		if(local < tempMin){
//			tempMin = local;
//		}
//
//		if(local > tempMax){
//			tempMax = local;
//		}
//	}
//
//	m_Min = tempMin;
//	m_Max = tempMax;
//}