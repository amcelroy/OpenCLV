#include "clDeviceManagement.h"
#include "clAmdFft.h"
#include <complex>
#include <assert.h>

//***********NEED TO HAVE LABVIEW ALLOCATE OUTPUT BUFFER!!!!!******************

void main(){
	clAmdFftStatus status;
	int Error = 0;
	double Time = 0;

	double *input;

	int Width = 4096;
	int Height = 1024;
	int Depth = 1;
	

	char *buff1 = new char[0];
	char *buff2 = new char[0];
	char *buff3 = new char[0];

	clLabviewDevice *d = CreateDevice(0, 0, &Error);
	cl_mem in = AllocateMemory(d, Width*Height*Depth*sizeof(float), &Error);


	float *labview = new float[Width*Height*Depth];
	float *out = new float[Width*Height*Depth];

	for(int i = 0; i < Width*Height*Depth; i++){
		labview[i] = i;
	}

	float *output = NULL;

	WriteMemory_32f(d, in, Width, Height, Depth, 0, 0, 0, labview, &Time, &Error);
	ReadMemory_32f(d, in, Width, Height, Depth, 0, 0, 0, out, &Time, &Error);
	
	cl_mem image = AllocateImage(d, Width, Height, Depth, 0, 0, &Error);
	cl_mem memory = AllocateMemory(d, Width*Height*Depth*sizeof(float)/2, &Error);
	
	WriteImage_32f(d, image, Width, Height, Depth, 0, 0, 0, labview, &Time, &Error);
	ImageToMemoryObject(d, image, memory, Width, Height, Depth, 0, 0, 0, &Time, &Error);

	AllocateFFTFramework(&Error);

	//	FFT state
	size_t OutputWidth;
	clAmdFftPlanHandle	plHandle = CreateFFTPlan(d, 0, 0, Width, Height, Depth, 1, 1, 1, 1, 0, 0, 0, &OutputWidth, 0, &Error); 
	
	float *R = new float[OutputWidth*Height*Depth];
	float *C = new float[OutputWidth*Height*Depth];
	
	cl_mem OutR = AllocateMemory(d, OutputWidth*Height*Depth*sizeof(float), &Error);
	cl_mem OutC = AllocateMemory(d, OutputWidth*Height*Depth*sizeof(float), &Error);

	
	ComputeFFT(plHandle, d, memory, NULL, OutR, OutC, 0, &Error);


	Error = clEnqueueReadBuffer( d->GetQueue(), OutR, CL_TRUE, 0, OutputWidth*Height*Depth*sizeof(float), &R[ 0 ], 
					0, NULL, NULL );

	Error = clEnqueueReadBuffer( d->GetQueue(), OutC, CL_TRUE, 0, OutputWidth*Height*Depth*sizeof(float), &C[ 0 ], 
					0, NULL, NULL );


	int TransformedWidth = OutputWidth;

	clAmdFftPlanHandle plHandle2 = CreateFFTPlan(d, 1, 1, TransformedWidth, Height, Depth, 1, 1, 1, 1, 0, 0, 0, &OutputWidth, 1, &Error);

	double *Orig = new double[OutputWidth*Height*Depth];
	cl_mem OrigD = AllocateMemory(d, OutputWidth*Height*Depth*sizeof(double), &Error);

	ComputeFFT(plHandle2, d, OutR, OutC, OrigD, NULL, 1, &Error);

	Error = clEnqueueReadBuffer( d->GetQueue(), OrigD, CL_TRUE, 0, OutputWidth*Height*Depth*sizeof(double), &Orig[ 0 ], 
					0, NULL, NULL );

	DestroyFFTFramework(&Error);

	DestroyFFTPlan(plHandle, &Error);
	DestroyFFTPlan(plHandle2, &Error);

	DeleteDevice(d, &Error);

}