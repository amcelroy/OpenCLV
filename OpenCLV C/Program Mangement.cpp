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
#include <exception>

/***Function***
*Deletes a kernel
***************/

void DeleteKernel(cl_kernel K, int *Error){
#ifndef NO_OPENCL
	try{
		*Error = clLabviewDevice::Error(clReleaseKernel(K));
	}
	catch(...){
		*Error = clLabviewDevice::Error(CL_INVALID_KERNEL);
	}

	return;
#endif
}

/***Function***
*Sets the arguments of a kernel.  Arg Num is the 
*argument postion in the OpenCL position call.
***************/

void SetKernelArguments(cl_kernel K, int ArgNum, void *Data, size_t SizeOfData, int Local, int *Error){
#ifndef NO_OPENCL
	if(Local == 0){
		*Error = clSetKernelArg(K, ArgNum, SizeOfData, Data);
		*Error = clLabviewDevice::Error(*Error);
	}else{
		*Error = clSetKernelArg(K, ArgNum, SizeOfData, NULL);
		*Error = clLabviewDevice::Error(*Error);
	}
	return;
#endif
}

/***Function***
*Loads a kernel by name from a program
***************/

cl_kernel LoadKernel(clLabviewDevice *d, cl_program P, char KernelName[], int *Error){

	cl_kernel Kernel = NULL;

#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return NULL;

	try{
		Kernel = clCreateKernel(P, KernelName, Error);
		*Error = clLabviewDevice::Error(*Error);
	}
	catch(...){
		*Error = clLabviewDevice::Error(CL_INVALID_KERNEL_NAME);
	}

#endif

	if(*Error == 0){
		return Kernel;
	}else{
		return NULL;
	}

}

/***Function***
*Loads Program from a file
***************/

cl_program LoadProgram(clLabviewDevice *d, const char ProgramSource[], size_t Size, int *Error){
	
	cl_program Program = 0;

#ifndef NO_OPENCL
	try{
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return NULL;
	
	Program = clCreateProgramWithSource(d->GetContext(), 1, &ProgramSource, &Size, Error);

	if(*Error != 0){
		*Error = clLabviewDevice::Error(*Error);
		return NULL;
	}

	*Error = clBuildProgram(Program, 1, d->GetDevicePtr(), NULL, NULL, NULL);
	*Error = clLabviewDevice::Error(*Error);
	d->AddProgram(Program);
	}catch(...){
		*Error = clLabviewDevice::Error(CL_BUILD_PROGRAM_FAILURE);
	}
#endif

	return Program;
}

/***Function***
*Deletes a program
***************/

void DeleteProgram(clLabviewDevice *d, cl_program P, int *Error){
#ifndef NO_OPENCL
	try{
		d->RemoveProgram(P);
		*Error = clLabviewDevice::Error(clReleaseProgram(P));
	}
	catch(...){
		*Error = clLabviewDevice::Error(CL_INVALID_PROGRAM);
	}
	return;
#endif
}

void UnloadCompiler(int Platform, int *Error){
#ifndef NO_OPENCL
	cl_uint NumberOfPlatforms;
	*Error = clLabviewDevice::Error(clGetPlatformIDs(NULL, NULL, &NumberOfPlatforms));	
	cl_platform_id *p = new cl_platform_id[NumberOfPlatforms];
	*Error = clLabviewDevice::Error(clGetPlatformIDs(NumberOfPlatforms, p, NULL));
#endif

#ifdef CL_VERSION_1_1
	try{
		//*Error = clLabviewDevice::Error(clUnloadCompiler());
	}
	catch(...){
		*Error = clLabviewDevice::Error(-502);
	}
#endif

#ifdef CL_VERSION_1_2
	try{
		//*Error = clLabviewDevice::Error(clUnloadCompiler(p[Platform]));
	}
	catch(...){
		*Error = clLabviewDevice::Error(-502);
	}
#endif
}

/***Function***
*Gets the compiler error size
***************/

void GetCompileErrorSize(clLabviewDevice *d, cl_program P, size_t *LogSize, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;
	
	*Error = clGetProgramBuildInfo(P, d->GetDevice(), CL_PROGRAM_BUILD_LOG, NULL, NULL, LogSize);
	*Error = clLabviewDevice::Error(*Error);
	return;
#endif
}

/***Function***
*Copies the compiler error log to Log[]
***************/

void GetCompileLog(clLabviewDevice *d, cl_program P, char Log[], size_t *LogSize, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;
	
	*Error = clGetProgramBuildInfo(P, d->GetDevice(), CL_PROGRAM_BUILD_LOG, *LogSize, Log, NULL);
	*Error = clLabviewDevice::Error(*Error);
	return;
#endif
}

/***Function***
*Deletes memory on the device
***************/

void DeleteMemory(clLabviewDevice *d, cl_mem DeviceMem, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;
	
	try{
		d->RemoveMemory(DeviceMem);
		*Error = clLabviewDevice::Error(clReleaseMemObject(DeviceMem));
	}
	catch(...){
		*Error = clLabviewDevice::Error(CL_INVALID_MEM_OBJECT);
	}
#endif
}

void GetKernelWorkGroupInfo(clLabviewDevice *d, cl_kernel K, 
							size_t *MaxWGSize, size_t *WGMultiple,
							size_t *LocalMem, size_t *PrivateMem,
							size_t *CompileWGSize, int *Error){
#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return;						
	
	try{
		*Error = clGetKernelWorkGroupInfo(K, d->GetDevice(), 
					CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), MaxWGSize, NULL);
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != 0){
			return; 
		}

		*Error = clGetKernelWorkGroupInfo(K, d->GetDevice(), 
					CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), WGMultiple, NULL);
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != 0){
			return; 
		}

		*Error = clGetKernelWorkGroupInfo(K, d->GetDevice(), 
					CL_KERNEL_LOCAL_MEM_SIZE, sizeof(size_t), LocalMem, NULL);
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != 0){
			return; 
		}

		*Error = clGetKernelWorkGroupInfo(K, d->GetDevice(), 
					CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(size_t), PrivateMem, NULL);
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != 0){
			return; 
		}

		*Error = clGetKernelWorkGroupInfo(K, d->GetDevice(), 
					CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(size_t)*3, CompileWGSize, NULL);
		*Error = clLabviewDevice::Error(*Error);
		if(*Error != 0){
			return; 
		}
	}catch(...){

	}
#endif
}