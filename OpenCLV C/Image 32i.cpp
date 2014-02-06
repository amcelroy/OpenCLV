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

#include "clLabviewDevice.h"
#include "Image Header.h"
#include <exception>

void WriteImage_i32(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
				size_t XOffset, size_t YOffset, size_t ZOffset, int *Data, double *Time, int *Error){
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
		int test = Data[Width*Height*Depth];
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

void ReadImage_i32(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
				size_t XOffset, size_t YOffset, size_t ZOffset, int *Data, double *Time, int *Error){
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
		int test = Data[Width*Height*Depth];
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