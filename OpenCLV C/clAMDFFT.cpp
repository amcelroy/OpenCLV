/*************************************************************************
    OpenCLV - OpenCL for Labview
    Copyright (C) 2014 - Austin McElroy - mcelroy.austin@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include <complex>
#include "clAmdFft.h"
#include "clLabviewDevice.h"

clAmdFftPlanHandle CreateFFTPlan(clLabviewDevice *d, 
					int FFTType, int Dimension,  
					int Width, int Height, int Depth,
					int StrideW, int StrideH, int StrideD, int StrideT, 
					int PaddingW, int PaddingH, int PaddingD, 
					size_t *OutputWidthFloat, int SingleOrDouble, int *Error){
	
	clAmdFftPlanHandle	plHandle = 0;

#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return NULL;					
						
	clAmdFftResultLocation	place = CLFFT_OUTOFPLACE;       //INPLACE NOT SUPPORTED
	size_t clStridesIn[ 4 ];
	size_t clStridesOut[ 4 ];
	clAmdFftLayout inLayout;
	clAmdFftLayout outLayout; 
	size_t batchSize;
	clAmdFftDim	dim;
	size_t clLengths[ 3 ];

	clAmdFftStatus status;

	int DistanceIn, DistanceOut;
	int OutputWidth;

	int FFTSize;

	if(FFTType == 1){
		//Needed to the FFT does the proper size on the inverse
		FFTSize = (Width - 1)*2;
	}else{
		FFTSize = Width;
	}

	switch(Dimension){
		case 0:
			//1D FFT
			batchSize = Depth*Height;
			dim = CLFFT_1D;
			clLengths[0] = FFTSize;
			clLengths[1] = 1;
			clLengths[2] = 1;
		break;

		case 1:
			//2D FFT
			batchSize = Depth;	
			dim = CLFFT_2D;
			clLengths[0] = FFTSize;
			clLengths[1] = Height;
			clLengths[2] = 1;
		break;
	
		case 2:
			//3D FFT
			batchSize = 1;
			dim = CLFFT_3D;
			clLengths[0] = FFTSize;
			clLengths[1] = Height;
			clLengths[2] = Depth;
		break;
	}

	switch(FFTType){
		//These cases are defined by the labview Type Def
		
		case 0:
			inLayout = CLFFT_REAL;
			outLayout = CLFFT_HERMITIAN_PLANAR;
			//*BufferSize = clLengths[2]*clLengths[1]*(clLengths[0]/2 + 1)*batchSize*sizeof(std::complex<float>)/sizeof(float);
			OutputWidth = FFTSize/2 + 1;
			*OutputWidthFloat = OutputWidth;
			DistanceIn = Width*clLengths[1]*clLengths[2];
			DistanceOut = OutputWidth*clLengths[1]*clLengths[2];
		break;

		case 1:
			inLayout = CLFFT_HERMITIAN_PLANAR;
			outLayout = CLFFT_REAL;
			//*BufferSize = clLengths[2]*clLengths[1]*(clLengths[0] - 1)*2*batchSize;
			OutputWidth = FFTSize;
			*OutputWidthFloat = OutputWidth;
			DistanceIn = Width*clLengths[1]*clLengths[2];
			DistanceOut = OutputWidth*clLengths[1]*clLengths[2];
		break;			
		
		case 2:
			inLayout = CLFFT_COMPLEX_PLANAR;
			outLayout = CLFFT_COMPLEX_PLANAR;
			//*BufferSize = clLengths[2]*clLengths[1]*(clLengths[0])*batchSize*sizeof(std::complex<float>)/sizeof(float);
			OutputWidth = FFTSize;
			*OutputWidthFloat = OutputWidth;
			DistanceIn = Width*clLengths[1]*clLengths[2];
			DistanceOut = OutputWidth*clLengths[1]*clLengths[2];
			place = CLFFT_INPLACE;
		break;
	}

	size_t fftVectorSize= 0, fftVectorSizePadded = 0, fftBatchSize = 0;
	
	clStridesIn[0] = 1;
	clStridesIn[ 1 ] = (clStridesIn[ 0 ])*(Width + PaddingW);
	clStridesIn[ 2 ] = (clStridesIn[ 1 ])*(clLengths[ 1 ] + PaddingH);
	clStridesIn[ 3 ] = (clStridesIn[ 2 ])*(clLengths[ 2 ] + PaddingD);

	clStridesOut[0] = 1;
	clStridesOut[ 1 ] = (clStridesOut[ 0 ])*(OutputWidth + PaddingW);
	clStridesOut[ 2 ] = (clStridesOut[ 1 ])*(clLengths[ 1 ] + PaddingH);
	clStridesOut[ 3 ] = (clStridesOut[ 2 ])*(clLengths[ 2 ] + PaddingD);

	fftVectorSize	= clLengths[ 0 ] * clLengths[ 1 ] * clLengths[ 2 ];
	fftVectorSizePadded = clStridesIn[ 3 ];
	fftBatchSize	= fftVectorSizePadded * batchSize;

	status = clAmdFftCreateDefaultPlan( &plHandle, d->GetContext(), dim, clLengths );

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_DEFAULT_PLAN_FAILED);
		return NULL;
	}

	status = clAmdFftSetResultLocation( plHandle, place );

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_SET_RESULT_FAILED);
		return NULL;
	}

	status = clAmdFftSetLayout( plHandle, inLayout, outLayout );

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_SET_LAYOUT_FAILED);
		return NULL;
	}

	if(SingleOrDouble == 0){
		status = clAmdFftSetPlanPrecision(plHandle, CLFFT_SINGLE);
	}else{
		status = clAmdFftSetPlanPrecision(plHandle, CLFFT_DOUBLE);
	}

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_SET_PRECISION_FAILED);
		return NULL;
	}

	status = clAmdFftSetPlanBatchSize( plHandle, batchSize );

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_SET_BATCHSIZE_FAILED);
		return NULL;
	}
	
	status = clAmdFftSetPlanInStride  ( plHandle, dim, clStridesIn );

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_SET_STRIDES_IN_FAILED);
		return NULL;
	}

	status = clAmdFftSetPlanOutStride ( plHandle, dim, clStridesOut );

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_SET_STRIDES_OUT_FAILED);
		return NULL;
	}

	status = clAmdFftSetPlanDistance  ( plHandle, DistanceIn, DistanceOut);

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_SET_PLAN_DIST_FAILED);
		return NULL;
	}

	status = clAmdFftBakePlan( plHandle, 0, d->GetQueuePtr(), NULL, NULL );

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_BAKE_PLAN_FAILED);
		return NULL;
	}

#endif
	
	return plHandle;


}

void DestroyFFTPlan(clAmdFftPlanHandle Handle, int *Error){
#ifndef NO_OPENCL
	if(Handle == NULL){
		*Error = *Error = clLabviewDevice::Error(OPENCLV_FFT_INVALID_PLAN);
	}
	
	clAmdFftStatus status;
	status = clAmdFftDestroyPlan( &Handle );

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_PLAN_DESTROY_FAILED);
		return;
	}
#endif
}

void ComputeFFT(clAmdFftPlanHandle Handle, clLabviewDevice *d, cl_mem InR, cl_mem InC, 
							cl_mem OutR, cl_mem OutC, int Direction, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;
								
	clAmdFftStatus status;

	if(Handle == NULL){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_INVALID_PLAN);
	}
	
	//get the buffersize
	size_t tmpbuf;
	status = clAmdFftGetTmpBufSize(Handle, &tmpbuf );

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_GET_BUFF_SIZE_FAILED);
		return;
	}
		
	//allocate the intermediate buffer	
	cl_mem clMedBuffer=NULL;
		
	if (tmpbuf)
	{
		clAmdFftPrecision Prec;
		clAmdFftGetPlanPrecision(Handle, &Prec);

		size_t BuffSize;
		if(Prec == CLFFT_SINGLE){
			BuffSize = sizeof(float)*tmpbuf;
		}else{
			BuffSize = sizeof(double)*tmpbuf;
		}

		cl_int medstatus;
		clMedBuffer = AllocateMemory(d, BuffSize, Error);	

		if(*Error != 0){
			*Error = clLabviewDevice::Error(OPENCLV_FFT_BUFFER_FAILED);
			return;
		}
	}

	cl_mem clMemBuffersIn[2] = {InR, InC};
	cl_mem clMemBuffersOut[2] = {OutR, OutC};

	clAmdFftDirection FFT_Direction;
	clAmdFftLayout inLayout, outLayout;

	status = clAmdFftGetLayout(Handle, &inLayout, &outLayout);

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_GET_LAYOUT_FAILED);
		return;
	}

	if(Direction == 1){
		FFT_Direction = CLFFT_BACKWARD;
	}else{
		FFT_Direction = CLFFT_FORWARD;
	}

	//Out C and Out R will only be 0 for a C -> C FFT
	cl_mem * BuffersOut;
	if( OutR == 0 && OutC == 0 ){
		BuffersOut = NULL;
	}else{
		BuffersOut = &clMemBuffersOut[ 0 ];
	}

	status = clAmdFftEnqueueTransform(Handle, FFT_Direction, 0, d->GetQueuePtr(), 0, NULL, NULL, 
			&clMemBuffersIn[0], BuffersOut, clMedBuffer);

	if(status != 0){
		*Error = clLabviewDevice::Error(OPENCLV_FFT_FAILED);
		return;
	}

	clFinish( d->GetQueue() );
#endif
}


void AllocateFFTFramework(int *Error){
#ifndef NO_OPENCL
	std::auto_ptr< clAmdFftSetupData > setupData( new clAmdFftSetupData );
	*Error = clAmdFftInitSetupData( setupData.get( ));

	if(*Error != 0){
		*Error = clLabviewDevice::Error(OPENCLV_CANT_INIT_FFT_FRAMEWORK);
		return;
	}

	*Error = clAmdFftSetup( setupData.get( ) );

	if(*Error != 0){
		*Error = clLabviewDevice::Error(OPENCLV_CANT_INIT_FFT_FRAMEWORK);
		return;
	}
#endif
}

void DestroyFFTFramework(int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clAmdFftTeardown( ));

	if(*Error != 0){
		*Error = clLabviewDevice::Error(OPENCLV_CANT_DESTROY_FFT_FRAMEWORK);
		return;
	}
#endif
}