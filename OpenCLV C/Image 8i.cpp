/*************************************************************************
    OpenCLV - OpenCL for Labview	
	Copyright (c) <2014>, <Austin McElroy - mcelroy.austin@gmail.com>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
		* Redistributions in binary form must reproduce the above copyright
		  notice, this list of conditions and the following disclaimer in the
		  documentation and/or other materials provided with the distribution.
		* Neither the name of the <organization> nor the
		  names of its contributors may be used to endorse or promote products
		  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#include "clLabviewDevice.h"
#include "Image Header.h"
#include <exception>

void WriteImage_i8(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
				size_t XOffset, size_t YOffset, size_t ZOffset, char *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};

	size_t Region[3] = {Width, Height, Depth};

	if(Depth == 1){
		Region[2] = 1;
	}

	if(Height == 1){
		Region[1] = 1;
	}

	if(Width == 1){
		Region[0] = 1;
	}

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(ImageObject);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset >= Width || YOffset >= Height || ZOffset >= Depth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}
		char test = Data[Width*Height*Depth];
		bool Block = !d->MemoryRegistered(ImageObject);
		*Error = clEnqueueWriteImage(d->GetQueue(), ImageObject, Block, Origin, Region, 0, 0, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, ImageObject, d->GetLVEvent(ImageObject));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(ImageObject);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

void ReadImage_i8(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
				size_t XOffset, size_t YOffset, size_t ZOffset, char *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t Region[3] = {Width, Height, Depth};

	if(Depth == 1){
		Region[2] = 1;
	}

	if(Height == 1){
		Region[1] = 1;
	}

	if(Width == 1){
		Region[0] = 1;
	}

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(ImageObject);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset >= Width || YOffset >= Height || ZOffset >= Depth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}
		char test = Data[Width*Height*Depth];
		bool Block = !d->MemoryRegistered(ImageObject);
		*Error = clEnqueueReadImage(d->GetQueue(), ImageObject, Block, Origin, Region, 0, 0, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, ImageObject, d->GetLVEvent(ImageObject));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(ImageObject);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}