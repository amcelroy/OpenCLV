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


#ifdef _WINDOWS
#define EXPORT __declspec(dllexport)
#include <Windows.h>
#else
#define EXPORT 
#endif

#ifndef __CL_LABVIEW_DEVICE_H__
#define __CL_LABVIEW_DEVICE_H__

#include <utility>
#include <CL/cl.h>
#include <iostream>
#include <vector>
#include "cintools/extcode.h"

//#include "clMath_f.h"

#define OPENCLV_DEVICE_NOT_FOUND				-500
#define OPENCLV_DEVICE_NOT_CONFIGURED			-501
#define OPENCLV_HOST_DEVICE_MEMORY_MISMATCH		-502
#define OPENCLV_ARRAY_DEFINITION_IS_ZERO		-503

#define OPENCLV_CANT_INIT_FFT_FRAMEWORK			-504
#define OPENCLV_CANT_DESTROY_FFT_FRAMEWORK		-505
#define OPENCLV_FFT_FAILED						-506
#define OPENCLV_FFT_BUFFER_FAILED				-507
#define OPENCLV_FFT_GET_BUFF_SIZE_FAILED		-508
#define OPENCLV_FFT_GET_LAYOUT_FAILED			-509
#define OPENCLV_FFT_PLAN_DESTROY_FAILED			-510
#define OPENCLV_FFT_DEFAULT_PLAN_FAILED			-511
#define OPENCLV_FFT_SET_RESULT_FAILED			-512
#define OPENCLV_FFT_SET_LAYOUT_FAILED			-513
#define OPENCLV_FFT_SET_BATCHSIZE_FAILED		-514
#define OPENCLV_FFT_SET_STRIDES_IN_FAILED		-515
#define OPENCLV_FFT_SET_STRIDES_OUT_FAILED		-516
#define OPENCLV_FFT_SET_PLAN_DIST_FAILED		-517
#define OPENCLV_FFT_BAKE_PLAN_FAILED			-518
#define OPENCLV_FFT_SET_PRECISION_FAILED		-519
#define OPENCLV_FFT_INVALID_PLAN				-520

#define OPENCLV_MAPPED_MEMORY_ERROR				-550
#define OPENCLV_MEMORY_NOT_MAPPED				-551
#define OPENCLV_HOST_DEVICE_MEMORY_TYPE			-552
#define OPENCLV_KERNEL_ALREADY_EXISTS			-553

#define OPENCLV_MATRIX_NOT_SQUARE				-600
#define OPENCLV_KERNEL_NOT_ODD					-601
#define OPENCLV_MEMORY_TYPES_DIFFERENT			-602
#define OPENCLV_SET_COPY_PARAMETERS				-603


#define OPENCLV_KERNEL_RUNTIME_ERROR			-700


#define __CL_ENABLE_EXCEPTIONS //Enables try, catch coding

struct KernelEvent{
	LVUserEventRef *lv_event;
	cl_kernel kernel;
};

struct MemoryEvent{
	LVUserEventRef *lv_event;
	cl_mem mem;
};


class clLabviewDevice
{
public:
	EXPORT clLabviewDevice(void);
	EXPORT ~clLabviewDevice(void);

	EXPORT int CreateContext(int Platform, int Device);
	EXPORT int GetError() {return m_error;}

	//Error code function that converts OpenCL errors to
	//Labview errors by subtracting 5000
	static int Error(int ErrorCode);

	//Suite of functions to return method variables
	cl_context GetContext() {return m_context;}	
	cl_device_id GetDevice() {return m_device;}
	cl_device_id *GetDevicePtr() {return &m_device;}
	cl_command_queue GetQueue() {return m_queue;}	
	cl_command_queue *GetQueuePtr() {return &m_queue;}
	cl_event *GetEvent() {return &m_time;}

	void AddKernelEvents(KernelEvent KE) {m_kernel_events.push_back(KE);}
	void AddMemoryEvents(MemoryEvent ME) {m_memory_events.push_back(ME);}

	int AddProgram(cl_program P);
	int RemoveProgram(cl_program P);
	std::vector<cl_program> GetProgramBuffer() {return m_ProgramBuffer;}

	int AddMemory(cl_mem M);
	int RemoveMemory(cl_mem M);
	std::vector<cl_mem> GetMemoryBuffer() {return m_MemoryBuffer;}

	int AddKernel(cl_kernel K);
	int RemoveKernel(cl_kernel K);
	std::vector<cl_kernel> GetKernelBuffer() {return m_KernelBuffer;}

	void ThrowMemEvent(cl_mem M);
	void ThrowKernelEvent(cl_kernel K);

	bool KernelRegistered(cl_kernel K);
	bool MemoryRegistered(cl_mem M);

	LVUserEventRef *GetLVEvent(cl_kernel K);
	LVUserEventRef *GetLVEvent(cl_mem M);

	//Sanitizes the inputs and checks for valid objects.  Returns 0 if all clear, error codes otherwise
	static int SanitizeDevice(clLabviewDevice *d);

	//Gets profiling info and
	//returns execution time in ns
	double GetTime() {
		__ProfileQueue();
		return m_ExecTime;
	}

	//Spawns threads for event handling
	static DWORD WINAPI AsynchronousKernelThread( LPVOID lpParam );
	static DWORD WINAPI AsynchronousMemoryThread( LPVOID lpParam );
	void CreateEventWaitThread(clLabviewDevice *d, cl_kernel K, LVUserEventRef *lv_event);
	void CreateEventWaitThread(clLabviewDevice *d, cl_mem M, LVUserEventRef *lv_event);

protected:
	//Group of variabels and functions used for profiling
	cl_event m_time;
	double m_ExecTime;
	void __ProfileQueue();

	//Error code
	cl_int m_error;

	//Selected device from the above options (CPU, GPU, Accelerator)
	cl_device_id m_device;

	//Context from the selected device
	cl_context m_context;

	//Device Queue
	cl_command_queue m_queue;

	void LVCallback(cl_event e, cl_int status, void *data);

	std::vector<KernelEvent> m_kernel_events;
	std::vector<MemoryEvent> m_memory_events;

	std::vector<cl_program> m_ProgramBuffer;
	std::vector<cl_kernel> m_KernelBuffer;
	std::vector<cl_mem> m_MemoryBuffer;


};

//Event Event Handling Stuff
struct KernelThreadInformation{
	clLabviewDevice *d;
	LVUserEventRef *lv_event;
	cl_command_queue Queue;
	cl_kernel Kernel;
};

//Event Handling Stuff
struct MemoryThreadInformation{
	clLabviewDevice *d;
	LVUserEventRef *lv_event;
	cl_command_queue Queue;
	cl_mem Memory;
};

extern "C" {

EXPORT int ArrayCheck(int Width, int Height, int Depth);

EXPORT void DeleteDevice(clLabviewDevice *d, int *Error);
EXPORT void DeleteQueue(clLabviewDevice *d, int *Error);

//Allocates memory on disk of Size in bytes
EXPORT cl_mem AllocateMemory(clLabviewDevice *d, size_t Size, int *Error);

//Allows user to copy one buffer to another
EXPORT void CopyBuffer(clLabviewDevice *d, cl_mem Source, int SourceType, 
				cl_mem Dest, int DestType,
				size_t SrcWidth, size_t SrcHeight, size_t SrcDepth,
				size_t SrcXOffset, size_t SrcYOffset, size_t SrcZOffset, 
				size_t DstWidth, size_t DstHeight, size_t DstDepth,
				size_t DstXOffset, size_t DstYOffset, size_t DstZOffset, 
				size_t XRegion, size_t YRegion, size_t ZRegion, double *Time, int *Error);

//32f Memory Functions
EXPORT void ReadMemory_32f(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				float *Data, double *Time, int *Error);
EXPORT void WriteMemory_32f(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				float *Data, double *Time, int *Error);

//64f Memory Functions
EXPORT void ReadMemory_64f(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				double *Data, double *Time, int *Error);
EXPORT void WriteMemory_64f(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				double *Data, double *Time, int *Error);

//u16 Memory Functions
EXPORT void ReadMemory_u16(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				unsigned short *Data, double *Time, int *Error);
EXPORT void WriteMemory_u16(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				unsigned short *Data, double *Time, int *Error);

//i16 Memory Functions
EXPORT void ReadMemory_i16(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				short *Data, double *Time, int *Error);
EXPORT void WriteMemory_i16(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				short *Data, double *Time, int *Error);

//u8 Memory Functions
EXPORT void ReadMemory_u8(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				unsigned char *Data, double *Time, int *Error);
EXPORT void WriteMemory_u8(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				unsigned char *Data, double *Time, int *Error);

//i8 Memory Functions
EXPORT void ReadMemory_i8(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				char *Data, double *Time, int *Error);
EXPORT void WriteMemory_i8(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				char *Data, double *Time, int *Error);

//u32 Memory Functions
EXPORT void ReadMemory_u32(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				unsigned int *Data, double *Time, int *Error);
EXPORT void WriteMemory_u32(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				unsigned int *Data, double *Time, int *Error);

//i32 Memory Functions
EXPORT void ReadMemory_i32(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				int *Data, double *Time, int *Error);
EXPORT void WriteMemory_i32(clLabviewDevice *d, cl_mem DeviceMem, size_t DevWidth, size_t DevHeight, size_t DevDepth,
				size_t HostWidth, size_t HostHeight, size_t HostDepth, size_t XOffset, size_t YOffset, size_t ZOffset, 
				int *Data, double *Time, int *Error);

EXPORT void ExecuteKernel(clLabviewDevice *d, cl_kernel K,
					size_t GWO, size_t GHO, size_t GDO,  //Global Width, height, depth Offset
					size_t GW, size_t GH, size_t GD,	   //Global width, height, depth thread number
					size_t LW, size_t LH, size_t LD,     //Local width, height, depth
					double *Time, int *Error);

//Delete memory on the device, regardless of format
EXPORT void DeleteMemory(clLabviewDevice *d, cl_mem DeviceMem, int *Error);

//Kernel and Program tools
EXPORT void UnloadCompiler(int Platform, int *Error);
EXPORT cl_program LoadProgram(clLabviewDevice *d, const char ProgramSource[], size_t Size, int *Error);
EXPORT void DeleteProgram(clLabviewDevice *d, cl_program P, int *Error);
EXPORT void GetCompileErrorSize(clLabviewDevice *d, cl_program P, size_t *LogSize, int *Error);
EXPORT void GetCompileLog(clLabviewDevice *d, cl_program P, char Log[], size_t *LogSize, int *Error);
EXPORT cl_kernel LoadKernel(clLabviewDevice *d, cl_program P, char KernelName[], int *Error);
EXPORT void SetKernelArguments(cl_kernel K, int ArgNum, void *Data, size_t SizeOfData, int Local, int *Error);

EXPORT void GetKernelWorkGroupInfo(clLabviewDevice *d, cl_kernel K, 
							size_t *MaxWGSize, size_t *WGMultiple,
							size_t *LocalMem, size_t *PrivateMem,
							size_t *CompileWGSize, int *Error);
EXPORT void DeleteKernel(cl_kernel K, int *Error);

EXPORT void RegisterKernelEvent(clLabviewDevice *d, cl_kernel K, LVUserEventRef *rwer);
EXPORT void RegisterMemoryEvent(clLabviewDevice *d, cl_mem M, LVUserEventRef *rwer);
EXPORT void UnloadDLL();
}

#endif