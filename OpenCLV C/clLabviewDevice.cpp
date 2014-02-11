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
#include <exception>

clLabviewDevice::clLabviewDevice(){
	m_time = NULL;
	m_ExecTime = NULL;
	m_error = NULL;
	m_device = NULL;
	m_queue = NULL;
	m_context = NULL;
	
	//SetUnhandledExceptionFilter(OurCrashHandler);
}

clLabviewDevice::~clLabviewDevice(){}

bool clLabviewDevice::KernelRegistered(cl_kernel K){
	std::vector<KernelEvent>::iterator it;

	it = this->m_kernel_events.begin();

	for(it; it < this->m_kernel_events.end(); it++){
		if((*it).kernel == K){
			return true;
		}
	}

	return false;
}

LVUserEventRef *clLabviewDevice::GetLVEvent(cl_kernel K){
	std::vector<KernelEvent>::iterator it;

	it = this->m_kernel_events.begin();

	for(it; it < this->m_kernel_events.end(); it++){
		if((*it).kernel == K){
			return (*it).lv_event;
		}
	}

	return NULL;
}

bool clLabviewDevice::MemoryRegistered(cl_mem M){
	std::vector<MemoryEvent>::iterator it;

	it = this->m_memory_events.begin();

	for(it; it < this->m_memory_events.end(); it++){
		if((*it).mem == M){
			return true;
		}
	}

	return NULL;
}


LVUserEventRef *clLabviewDevice::GetLVEvent(cl_mem M){
	std::vector<MemoryEvent>::iterator it;

	it = this->m_memory_events.begin();

	for(it; it < this->m_memory_events.end(); it++){
		if((*it).mem == M){
			return (*it).lv_event;
		}
	}

	return NULL;
}

DWORD WINAPI clLabviewDevice::AsynchronousKernelThread( LPVOID lpParam ){
	//Run a while loop until the KernelCall return
	bool Finished = false;

	KernelThreadInformation *Event = (KernelThreadInformation*)lpParam;

	cl_command_queue Queue = Event->Queue;
	LVUserEventRef *lv_event = Event->lv_event;
	cl_kernel Kernel = Event->Kernel;
	clLabviewDevice *d = Event->d;

	clFinish(Queue);

	d->ThrowKernelEvent(Kernel);

	return 0;
}

DWORD WINAPI clLabviewDevice::AsynchronousMemoryThread( LPVOID lpParam ){
	//Run a while loop until the KernelCall return
	bool Finished = false;

	MemoryThreadInformation *Event = (MemoryThreadInformation*)lpParam;

	cl_command_queue Queue = Event->Queue;
	LVUserEventRef *lv_event = Event->lv_event;
	cl_mem Memory = Event->Memory;
	clLabviewDevice *d = Event->d;

	clFinish(Queue);

	d->ThrowMemEvent(Memory);

	return 0;
}

void clLabviewDevice::CreateEventWaitThread(clLabviewDevice *d, cl_kernel K, LVUserEventRef *lv_event){
	static KernelThreadInformation Event;

	Event.d = this;
	Event.lv_event = lv_event;
	Event.Queue = this->GetQueue();
	Event.Kernel = K;

	CreateThread(NULL, 0, AsynchronousKernelThread, &Event, 0, NULL);
}

void clLabviewDevice::CreateEventWaitThread(clLabviewDevice *d, cl_mem M, LVUserEventRef *lv_event){
	static MemoryThreadInformation Event;

	Event.d = this;
	Event.lv_event = lv_event;
	Event.Queue = this->GetQueue();
	Event.Memory = M;

	CreateThread(NULL, 0, AsynchronousMemoryThread, &Event, 0, NULL);
}

/***Function***
*Executes a kernel with the allowing the user to set 
* Global offsets, size, and local size.

* To simplify use, dimensions will be hardcoded to 3
***************/

void ExecuteKernel(clLabviewDevice *d, cl_kernel K,
					size_t GWO, size_t GHO, size_t GDO,  //Global Width, height, depth Offset
					size_t GW, size_t GH, size_t GD,	   //Global width, height, depth thread number
					size_t LW, size_t LH, size_t LD,     //Local width, height, depth
					double *Time, int *Error){
#ifndef NO_OPENCL	
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;
	
	try{
		*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
		if(*Error != 0)
			return;

		size_t global_offeset[3] = {GDO, GHO, GWO};
		size_t global_size[3] = {GD, GH, GW};
		size_t local_size[3] = {LD, LH, LW};
		unsigned int dim = 3;

		cl_event *NewEvent = new cl_event;

		bool KernelRegistered = d->KernelRegistered(K);

		if(KernelRegistered == true){
			//The kernel is registered 
			//keep the new event
		}else{
			//Kernel isn't regisitered,
			//just use the main event
			NewEvent = d->GetEvent();
		}

		if(LW == 0 || LH == 0 || LD == 0){
			*Error = clEnqueueNDRangeKernel(d->GetQueue(), K, dim, global_offeset, global_size, 
				NULL, 0, NULL, NewEvent);
		}else{
			*Error = clEnqueueNDRangeKernel(d->GetQueue(), K, dim, global_offeset, global_size, 
				local_size, 0, NULL, NewEvent);
		}

		if(KernelRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, K, d->GetLVEvent(K));
		}else{
			clFinish(d->GetQueue());
			d->ThrowKernelEvent(K);
		}

		*Error = clLabviewDevice::Error(*Error);
		*Time = d->GetTime();
	}catch(...){
		*Error = clLabviewDevice::Error(*Error = clLabviewDevice::Error(OPENCLV_KERNEL_RUNTIME_ERROR));
	}
#endif
}


void clLabviewDevice::ThrowMemEvent(cl_mem M){
	std::vector<MemoryEvent>::iterator it;

	it = this->m_memory_events.begin();

	for(it; it < this->m_memory_events.end(); it++){
		if((*it).mem == M){
			bool data = true;
			PostLVUserEvent(*(*it).lv_event, (void*)&data);
			break;
		}
	}
}

void clLabviewDevice::ThrowKernelEvent(cl_kernel K){
	std::vector<KernelEvent>::iterator it;

	it = this->m_kernel_events.begin();

	for(it; it < this->m_kernel_events.end(); it++){
		if((*it).kernel == K){
			bool data = true;
			PostLVUserEvent(*(*it).lv_event, (void*)&data);
			break;
		}
	}
}

int clLabviewDevice::AddKernel(cl_kernel K){
	std::vector<cl_kernel>::iterator it = m_KernelBuffer.begin();

	for(it; it < m_KernelBuffer.end(); it++){
		if(*it == K){
			return OPENCLV_KERNEL_ALREADY_EXISTS; 
		}
	}

	m_KernelBuffer.push_back(K);
	return 0;
}

int clLabviewDevice::RemoveKernel(cl_kernel K){
	std::vector<cl_kernel>::iterator it = m_KernelBuffer.begin();

	for(it; it < m_KernelBuffer.end(); it++){
		if(*it == K){
			m_KernelBuffer.erase(it);
			return 0;
			break;
		}
	}

	return clLabviewDevice::Error(CL_INVALID_KERNEL);
}

int clLabviewDevice::AddProgram(cl_program P){
	std::vector<cl_program>::iterator it = m_ProgramBuffer.begin();

	for(it; it < m_ProgramBuffer.end(); it++){
		if(*it == P){
			return OPENCLV_KERNEL_ALREADY_EXISTS; 
		}
	}

	m_ProgramBuffer.push_back(P);
	return 0;
}

int clLabviewDevice::RemoveProgram(cl_program P){
	std::vector<cl_program>::iterator it = m_ProgramBuffer.begin();

	for(it; it < m_ProgramBuffer.end(); it++){
		if(*it == P){
			m_ProgramBuffer.erase(it);
			return 0;
			break;
		}
	}

	return clLabviewDevice::Error(CL_INVALID_PROGRAM);
}

int clLabviewDevice::AddMemory(cl_mem M){
	std::vector<cl_mem>::iterator it = m_MemoryBuffer.begin();

	for(it; it < m_MemoryBuffer.end(); it++){
		if(*it == M){
			return OPENCLV_KERNEL_ALREADY_EXISTS; 
		}
	}

	m_MemoryBuffer.push_back(M);
	return 0;
}

int clLabviewDevice::RemoveMemory(cl_mem M){
	std::vector<cl_mem>::iterator it = m_MemoryBuffer.begin();

	for(it; it < m_MemoryBuffer.end(); it++){
		if(*it == M){
			m_MemoryBuffer.erase(it);
			return 0;
			break;
		}
	}	

	return clLabviewDevice::Error(CL_INVALID_MEM_OBJECT);
}

int clLabviewDevice::CreateContext(int Platform, int Device){
#ifdef NO_OPENCL
	return 0;
#else NO_OPENCL	
	cl_uint NumberOfPlatforms;
	m_error = clGetPlatformIDs(NULL, NULL, &NumberOfPlatforms);	
	cl_platform_id *p = new cl_platform_id[NumberOfPlatforms];	
	m_error = clGetPlatformIDs(NumberOfPlatforms, p, NULL);

	if(Platform >= NumberOfPlatforms){
		return clLabviewDevice::Error(CL_INVALID_PLATFORM);
	}

	cl_uint NumberOfDevices;
	m_error = clGetDeviceIDs(p[Platform], CL_DEVICE_TYPE_ALL, NULL, NULL, &NumberOfDevices);
	cl_device_id *q = new cl_device_id[NumberOfDevices];
	m_error = clGetDeviceIDs(p[Platform], CL_DEVICE_TYPE_ALL, NumberOfDevices, q, NULL);

	if(Device >= NumberOfDevices){
		return clLabviewDevice::Error(CL_DEVICE_NOT_FOUND);
	}

	m_device = q[Device];

	cl_device_type type;
	clGetDeviceInfo(q[Device], CL_DEVICE_TYPE, sizeof(type), &type, NULL);

	cl_context_properties m_context_properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)p[Platform], 0};
	m_context = clCreateContextFromType(m_context_properties, type, NULL, NULL, &m_error);

	if(m_error != 0){
		return clLabviewDevice::Error(m_error);
	}

	m_queue = clCreateCommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE, &m_error);

	if(m_error != 0){
		clReleaseContext(m_context);
		return clLabviewDevice::Error(m_error);
	}

	return clLabviewDevice::Error(m_error);
#endif
}

void clLabviewDevice::__ProfileQueue(){
#ifndef NO_OPENCL
	cl_ulong start;
	cl_ulong end;
	size_t cycle;
	
	m_error = clGetEventProfilingInfo(m_time, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
	m_error = clGetEventProfilingInfo(m_time, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);

	//Gets the cycle time in nano-seconds
	clGetDeviceInfo(m_device, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(size_t), &cycle, NULL);

	//cycle is in ns, so convert to ms
	//cycle = 1000000 / cycle;

	m_ExecTime = ((double)end - (double)start)/1000000;
#endif
}

//Performs a runtime check to make sure the device is valid
int clLabviewDevice::SanitizeDevice(clLabviewDevice *d){
#ifndef NO_OPENCL
	try{
		d->GetContext();
	}
	catch(...){
		return OPENCLV_DEVICE_NOT_FOUND;
	}
#endif
	return 0;
}


//Check an Array for 0 values
int ArrayCheck(int Width, int Height, int Depth){
	if(Width == 0 || Height == 0 || Depth == 0){
		return OPENCLV_ARRAY_DEFINITION_IS_ZERO;
	}else{
		return 0;
	}
}

int clLabviewDevice::Error(int ErrorCode){
	if(ErrorCode == 0){
		return 0;
	}else{
		return (ErrorCode - 5000);
	}
}

cl_mem AllocateMemory(clLabviewDevice *d, size_t Size, int *Error){
#ifdef NO_OPENCL
	return 0;
#else NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return NULL;
	try{
		cl_mem temp = clCreateBuffer(d->GetContext(), CL_MEM_READ_WRITE, Size, NULL, Error);	
		if(*Error != 0){
			*Error = clLabviewDevice::Error(*Error);
			return 0;
		}
		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		d->AddMemory(temp);
		return temp;
	}catch(...){
		if(*Error != 0){
			*Error = clLabviewDevice::Error(CL_INVALID_DEVICE);
			return NULL;
		}
	}
#endif
}

/***Function***
*Copies one buffer to another
***************/
EXPORT void CopyBuffer(clLabviewDevice *d, cl_mem Source, int SourceType, 
				cl_mem Dest, int DestType,
				size_t SrcWidth, size_t SrcHeight, size_t SrcDepth,
				size_t SrcXOffset, size_t SrcYOffset, size_t SrcZOffset, 
				size_t DstWidth, size_t DstHeight, size_t DstDepth,
				size_t DstXOffset, size_t DstYOffset, size_t DstZOffset, 
				size_t XRegion, size_t YRegion, size_t ZRegion, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;

	size_t SrcOrigin[3] = {SrcXOffset, SrcYOffset, SrcZOffset};
	size_t DstOrigin[3] = {DstXOffset, DstYOffset, DstZOffset};

	int size = 4;

	if(SourceType != DestType){
		*Error = clLabviewDevice::Error(OPENCLV_MEMORY_TYPES_DIFFERENT);
		return;
	}

	switch(SourceType){
		case 0:
		size = sizeof(float);
		break;

		case 1:
		size = sizeof(unsigned char);
		break;

		case 2:
		size = sizeof(char);
		break;

		case 3:
		size = sizeof(unsigned short);
		break;		

		case 4:
		size = sizeof(short);
		break;

		case 5:
		size = sizeof(unsigned int);
		break;

		case 6:
		size = sizeof(int);
		break;
	}

	size_t Region[3] = {size*XRegion, YRegion, ZRegion};

	if(XRegion == 0 && YRegion == 0 && ZRegion == 0){
		//Check if the arrays are the same size, if not, throw error
		if(SrcWidth == DstWidth && SrcHeight == DstHeight && SrcDepth == DstDepth){		
			Region[0] = size*DstWidth;
			Region[1] = DstHeight;
			Region[2] = DstDepth;
		}else{
			*Error = clLabviewDevice::Error(OPENCLV_SET_COPY_PARAMETERS);			
		}
	}

	try{
		size_t SrcRowPitch = SrcWidth*size;
		size_t SrcSlicePitch = SrcWidth*SrcHeight*size;
		size_t DstRowPitch = DstWidth*size;
		size_t DstSlicePitch = DstWidth*DstHeight*size;


		*Error = clEnqueueCopyBufferRect(d->GetQueue(), Source, Dest, 
										SrcOrigin, DstOrigin, Region, 
										SrcRowPitch, SrcSlicePitch, DstRowPitch, DstSlicePitch,
										NULL, NULL, d->GetEvent());
		if(*Error != CL_SUCCESS){
			*Error = clLabviewDevice::Error(*Error);
			return;
		}
		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();
		d->ThrowMemEvent(Source);
		d->ThrowMemEvent(Dest);
	}catch(...){
		if(*Error != 0){
			*Error = clLabviewDevice::Error(CL_INVALID_DEVICE);
			return;
		}
	}
#endif
}


/***Function***
*Suite of 32f memory functions
***************/

void ReadMemory_32f(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				float *Data, double *Time, int *Error){
	
#ifndef NO_OPENCL					
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(float)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(float);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(float);
		size_t DevRowPitch = DevWidth*sizeof(float);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(float);

		*Error = clEnqueueReadBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, HostRowPitch, HostSlicePitch, DevRowPitch, DevSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();
	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

void WriteMemory_32f(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				float *Data, double *Time, int *Error){

#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(float)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(float);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(float);
		size_t DevRowPitch = DevWidth*sizeof(float);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(float);

		*Error = clEnqueueWriteBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, DevRowPitch, DevSlicePitch, HostRowPitch, HostSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();
	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}



/***Function***
*Suite of 64f memory functions
***************/

void ReadMemory_64f(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
double *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(double)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(double);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(double);
		size_t DevRowPitch = DevWidth*sizeof(double);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(double);

		*Error = clEnqueueReadBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, HostRowPitch, HostSlicePitch, DevRowPitch, DevSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

void WriteMemory_64f(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				double *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(double)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(double);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(double);
		size_t DevRowPitch = DevWidth*sizeof(double);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(double);

		*Error = clEnqueueWriteBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, DevRowPitch, DevSlicePitch, HostRowPitch, HostSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

/***Function***
*Suite of u16 memory functions
***************/

void ReadMemory_u16(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				unsigned short *Data, double *Time, int *Error){

#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(unsigned short)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(unsigned short);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(unsigned short);
		size_t DevRowPitch = DevWidth*sizeof(unsigned short);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(unsigned short);

		*Error = clEnqueueReadBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, HostRowPitch, HostSlicePitch, DevRowPitch, DevSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

void WriteMemory_u16(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				unsigned short *Data, double *Time, int *Error){

#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(unsigned short)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(unsigned short);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(unsigned short);
		size_t DevRowPitch = DevWidth*sizeof(unsigned short);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(unsigned short);

		*Error = clEnqueueWriteBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, DevRowPitch, DevSlicePitch, HostRowPitch, HostSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

/***Function***
*Suite of i16 memory functions
***************/

void ReadMemory_i16(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				short *Data, double *Time, int *Error){

#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(short)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(short);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(short);
		size_t DevRowPitch = DevWidth*sizeof(short);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(short);

		*Error = clEnqueueReadBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, HostRowPitch, HostSlicePitch, DevRowPitch, DevSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

void WriteMemory_i16(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				short *Data, double *Time, int *Error){

#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(short)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(short);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(short);
		size_t DevRowPitch = DevWidth*sizeof(short);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(short);

		*Error = clEnqueueWriteBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, DevRowPitch, DevSlicePitch, HostRowPitch, HostSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

/***Function***
*Suite of u32 memory functions
***************/

void ReadMemory_u32(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				unsigned int *Data, double *Time, int *Error){
	
#ifndef NO_OPENCL	
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(unsigned int)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(unsigned int);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(unsigned int);
		size_t DevRowPitch = DevWidth*sizeof(unsigned int);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(unsigned int);

		*Error = clEnqueueReadBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, HostRowPitch, HostSlicePitch, DevRowPitch, DevSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

void WriteMemory_u32(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				unsigned int *Data, double *Time, int *Error){

#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(unsigned int)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}
	
	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(unsigned int);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(unsigned int);
		size_t DevRowPitch = DevWidth*sizeof(unsigned int);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(unsigned int);

		*Error = clEnqueueWriteBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, DevRowPitch, DevSlicePitch, HostRowPitch, HostSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

/***Function***
*Suite of i32 memory functions
***************/

void ReadMemory_i32(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				int *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(unsigned int)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(int);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(int);
		size_t DevRowPitch = DevWidth*sizeof(int);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(int);

		*Error = clEnqueueReadBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, HostRowPitch, HostSlicePitch, DevRowPitch, DevSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

void WriteMemory_i32(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				int *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(int)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(int);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(int);
		size_t DevRowPitch = DevWidth*sizeof(int);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(int);

		*Error = clEnqueueWriteBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, DevRowPitch, DevSlicePitch, HostRowPitch, HostSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();
	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

/***Function***
*Suite of u8 memory functions
***************/

void ReadMemory_u8(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				unsigned char *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(unsigned char)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(unsigned char);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(unsigned char);
		size_t DevRowPitch = DevWidth*sizeof(unsigned char);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(unsigned char);

		*Error = clEnqueueReadBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, HostRowPitch, HostSlicePitch, DevRowPitch, DevSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

	return;
#endif
}

void WriteMemory_u8(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				unsigned char *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(unsigned char)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(unsigned char);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(unsigned char);
		size_t DevRowPitch = DevWidth*sizeof(unsigned char);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(unsigned char);

		*Error = clEnqueueWriteBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, DevRowPitch, DevSlicePitch, HostRowPitch, HostSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}

#endif
}


/***Function***
*Suite of i8 memory functions
***************/

void ReadMemory_i8(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				char *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(char)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(char);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(char);
		size_t DevRowPitch = DevWidth*sizeof(char);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(char);

		*Error = clEnqueueReadBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, HostRowPitch, HostSlicePitch, DevRowPitch, DevSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();		

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}
	return;
#endif
}

void WriteMemory_i8(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset,
				char *Data, double *Time, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;	

	size_t Origin[3] = {XOffset, YOffset, ZOffset};
	size_t HostOrigin[3] = {0,0,0};
	size_t Region[3] = {sizeof(char)*HostWidth, HostHeight, HostDepth};

	cl_event *NewEvent = new cl_event;

	bool MemoryRegistered = d->MemoryRegistered(DeviceMem);

	if(MemoryRegistered == true){
		//The kernel is registered 
		//keep the new event
	}else{
		//Kernel isn't regisitered,
		//just use the main event
		NewEvent = d->GetEvent();
	}

	try{
		if(XOffset + HostWidth > DevWidth || YOffset + HostHeight > DevHeight || ZOffset + HostDepth > DevDepth){
			*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
			return;
		}

		size_t HostRowPitch = HostWidth*sizeof(char);
		size_t HostSlicePitch = HostWidth*HostHeight*sizeof(char);
		size_t DevRowPitch = DevWidth*sizeof(char);
		size_t DevSlicePitch = DevWidth*DevHeight*sizeof(char);

		*Error = clEnqueueWriteBufferRect(d->GetQueue(), DeviceMem, true, Origin, HostOrigin, Region, DevRowPitch, DevSlicePitch, HostRowPitch, HostSlicePitch, Data, NULL, NULL, d->GetEvent());
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != CL_SUCCESS){
			return;
		}

		if(MemoryRegistered == true){
			//The kernel is registered with a user event,
			//create a new thread to wait for event to finish
			d->CreateEventWaitThread(d, DeviceMem, d->GetLVEvent(DeviceMem));
		}else{
			clFinish(d->GetQueue());
			d->ThrowMemEvent(DeviceMem);
		}

		*Error = clLabviewDevice::Error(clFinish(d->GetQueue()));
		*Time = d->GetTime();

	}catch(...){
		*Error = clLabviewDevice::Error(OPENCLV_HOST_DEVICE_MEMORY_MISMATCH);
	}
#endif NO_OPENCL
}


/***Function***
*Deleted command queue
***************/

void DeleteQueue(clLabviewDevice *d, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;

	try{
		*Error = clFlush(d->GetQueue());
		if(*Error != 0){
			return; 
		}
		*Error = clFinish(d->GetQueue());
		if(*Error != 0){
			return; 
		}
		*Error = clLabviewDevice::Error(clReleaseCommandQueue(d->GetQueue()));
		if(*Error != 0){
			return; 
		}
	}
	catch(...){
		*Error = clLabviewDevice::Error(CL_INVALID_COMMAND_QUEUE);
	}

	try{
		*Error = clLabviewDevice::Error(clReleaseEvent(*d->GetEvent()));
		if(*Error != 0){
			return; 
		}
	}
	catch(...){
		*Error = clLabviewDevice::Error(CL_INVALID_EVENT);
	}
#endif
}


/***Function***
*Deletes a device
***************/

void DeleteDevice(clLabviewDevice *d, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;

	try{
		for(int i = 0; i < d->GetProgramBuffer().size(); i++){
			clReleaseProgram(d->GetProgramBuffer()[i]);
		}

		for(int i = 0; i < d->GetMemoryBuffer().size(); i++){
			clReleaseMemObject(d->GetMemoryBuffer()[i]);
		}

		for(int i = 0; i < d->GetKernelBuffer().size(); i++){
			clReleaseKernel(d->GetKernelBuffer()[i]);
		}
	
		DeleteQueue(d, Error);

		*Error = clLabviewDevice::Error(clReleaseContext(d->GetContext()));

		delete d;

		if(*Error != 0){
			return; 
		}
	}
	catch(...){
		*Error = clLabviewDevice::Error(CL_INVALID_DEVICE);
	}

//#ifdef CL_VERSION_1_2
//	try{
//		*Error = clLabviewDevice::Error(clReleaseDevice(d->GetDevice()));
//	}
//	catch(...){
//		*Error = clLabviewDevice::Error(CL_INVALID_DEVICE);
//	}
//#endif
#endif
}

void RegisterKernelEvent(clLabviewDevice *d, cl_kernel K, LVUserEventRef *rwer){
	KernelEvent Event;

	Event.kernel = K;
	Event.lv_event = rwer;
	
	d->AddKernelEvents(Event);
}

void RegisterMemoryEvent(clLabviewDevice *d, cl_mem Memory, LVUserEventRef *rwer){
	MemoryEvent Event;

	Event.mem = Memory;
	Event.lv_event = rwer;
	
	d->AddMemoryEvents(Event);
}

void UnloadDLL(){
	return;
}