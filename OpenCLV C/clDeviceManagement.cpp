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

#include "clDeviceManagement.h"


clDeviceManagement::clDeviceManagement(void)
{
}


clDeviceManagement::~clDeviceManagement(void)
{

}

int GetPlatformNumber(){
#ifndef NO_OPENCL
	cl_uint NumberOfPlatforms;
	try{
		clGetPlatformIDs(1, NULL, &NumberOfPlatforms);
		return NumberOfPlatforms;
	}catch(...){
		return -1;
	}
#endif

	return 0;

}

int GetPlatformInfo(int Platform, char PlatformNames[], char PlatformProfile[], char PlatformVersion[], 
	char PlatformVendor[], char PlatformExtensions[]){
	int error = 0;

#ifndef NO_OPENCL

	try{
		if(Platform != -1){
			cl_uint NumberOfPlatforms;
			error = clGetPlatformIDs(NULL, NULL, &NumberOfPlatforms);	
			cl_platform_id *p = new cl_platform_id[NumberOfPlatforms];
			error = clGetPlatformIDs(NumberOfPlatforms, p, NULL);

			char *buff = new char[4096];

			size_t SizeOfString;
			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_NAME, NULL, NULL, &SizeOfString);
			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_NAME, SizeOfString, buff, NULL);
			memcpy(PlatformNames, buff, SizeOfString);

			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_PROFILE, NULL, NULL, &SizeOfString);
			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_PROFILE, SizeOfString, buff, NULL);
			memcpy(PlatformProfile, buff, SizeOfString);

			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_VERSION, NULL, NULL, &SizeOfString);
			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_VERSION, SizeOfString, buff, NULL);
			memcpy(PlatformVersion, buff, SizeOfString);

			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_VENDOR, NULL, NULL, &SizeOfString);
			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_VENDOR, SizeOfString, buff, NULL);
			memcpy(PlatformVendor, buff, SizeOfString);

			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_EXTENSIONS, NULL, NULL, &SizeOfString);
			error = clGetPlatformInfo(p[Platform], CL_PLATFORM_EXTENSIONS, SizeOfString, buff, NULL);
			memcpy(PlatformExtensions, buff, SizeOfString);
		}	
		return error;
	}catch(...){
		return -1;
	}

#endif

	return 0;

}

int GetDeviceNumber(int Platform){
#ifndef NO_OPENCL
	try{
		if(Platform != -1){
			int error;
	
			cl_uint NumberOfPlatforms;
			error = clGetPlatformIDs(NULL, NULL, &NumberOfPlatforms);	
			cl_platform_id *p = new cl_platform_id[NumberOfPlatforms];
			error = clGetPlatformIDs(NumberOfPlatforms, p, NULL);
	
			cl_uint NumberOfDevices;
			error = clGetDeviceIDs(p[Platform], CL_DEVICE_TYPE_ALL, NULL, NULL, &NumberOfDevices);	
	
			return NumberOfDevices;
		}else{
			return 0;
		}
	}catch(...){
		return -1;
	}
#endif
	return 0;
}

int GetDeviceInfo(int Platform, int Device, char DeviceName[], char DeviceInfo[], char VectorInfo[], char Memory[],
						char Maximums[]){
	int error = 0;
	size_t size;
	int offset = 0;
	cl_uint a;	

#ifndef NO_OPENCL

	try{
	cl_uint NumberOfPlatforms;
	error = clGetPlatformIDs(NULL, NULL, &NumberOfPlatforms);	
	cl_platform_id *p = new cl_platform_id[NumberOfPlatforms];	
	error = clGetPlatformIDs(NumberOfPlatforms, p, NULL);

	cl_uint NumberOfDevices;
	error = clGetDeviceIDs(p[Platform], CL_DEVICE_TYPE_ALL, NULL, NULL, &NumberOfDevices);

	if(NumberOfDevices == 0){
		return CL_DEVICE_NOT_FOUND;
	}

	cl_device_id *q = new cl_device_id[	NumberOfDevices];
	error = clGetDeviceIDs(p[Platform], CL_DEVICE_TYPE_ALL, NumberOfDevices, q, NULL);

	char *buff = new char[10000];
	memset(buff, 0, 10000);

	error = clGetDeviceInfo(q[Device], CL_DEVICE_NAME, NULL, NULL, &size);
	error = clGetDeviceInfo(q[Device], CL_DEVICE_NAME, size, &buff[0], NULL);
	strcat(DeviceName, &buff[0]);

	error = clGetDeviceInfo(q[Device], CL_DEVICE_VENDOR, NULL, NULL, &size);
	error = clGetDeviceInfo(q[Device], CL_DEVICE_VENDOR, size, &buff[0], NULL);
	strcat(DeviceInfo, buff);
	strcat(DeviceInfo, "___");

	//memset(buff, 0, 10000);
	error = clGetDeviceInfo(q[Device], CL_DEVICE_PROFILE, NULL, NULL, &size);
	error = clGetDeviceInfo(q[Device], CL_DEVICE_PROFILE, size, &buff[0], NULL);
	strcat(DeviceInfo, &buff[0]);
	strcat(DeviceInfo, "___");

	cl_device_type devicetype; 
	error = clGetDeviceInfo(q[Device], CL_DEVICE_TYPE, sizeof(devicetype), &devicetype, NULL);
	if(devicetype == CL_DEVICE_TYPE_CPU){
		strcat(DeviceInfo, "CL_DEVICE_TYPE_CPU");
		strcat(DeviceInfo, "___");
	}else if(devicetype == CL_DEVICE_TYPE_GPU){
		strcat(DeviceInfo,  "CL_DEVICE_TYPE_GPU");
		strcat(DeviceInfo, "___");
	}else if(devicetype == CL_DEVICE_TYPE_ACCELERATOR){
		strcat(DeviceInfo, "CL_DEVICE_TYPE_ACCELERATOR");
		strcat(DeviceInfo, "___");
	}else if(devicetype == CL_DEVICE_TYPE_DEFAULT){
		strcat(DeviceInfo, "CL_DEVICE_TYPE_DEFAULT");
		strcat(DeviceInfo, "___");
	}

	cl_int t;
	error = clGetDeviceInfo(q[Device], CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Native Vector Width Char: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Native Vector Width Short: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Native Vector Width Int: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Native Vector Width Long: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Native Vector Width Float: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Native Vector Width Double: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Native Vector Width Half: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Preferred Vector Width Char: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Preferred Vector Width Short: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Preferred Vector Width Int: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Preferred Vector Width Long: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Preferred Vector Width Float: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Preferred Vector Width Double: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(VectorInfo, "Preferred Vector Width Half: ");
	strcat(VectorInfo, buff);
	strcat(VectorInfo, "___");

	cl_ulong s;
	error = clGetDeviceInfo(q[Device], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &s, NULL);
	sprintf(buff, "%lu", s);
	//u_itoa(s, &buff[0], 10);
	strcat(Memory, "Global Mem Size: ");
	strcat(Memory, buff);
	strcat(Memory, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong), &s, NULL);
	_itoa(s, &buff[0], 10);
	strcat(Memory, "Global Mem Cache Size: ");
	strcat(Memory, buff);
	strcat(Memory, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_ulong), &s, NULL);
	_itoa(s, &buff[0], 10);
	strcat(Memory, "Global Mem Cache line Size: ");
	strcat(Memory, buff);
	strcat(Memory, "___");

	cl_device_mem_cache_type cache_type;
	error = clGetDeviceInfo(q[Device], CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(cl_device_mem_cache_type), &cache_type, NULL);
	strcat(Memory, "Global Mem Cache Type: ");
	if(cache_type == CL_NONE){
		strcat(Memory, "None");
	}else if(cache_type == CL_READ_ONLY_CACHE){
		strcat(Memory, "Read Only");
	}else if(cache_type == CL_READ_WRITE_CACHE){
		strcat(Memory, "Write Only");
	}
	strcat(Memory, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &s, NULL);
	_itoa(s, &buff[0], 10);
	strcat(Memory, "Local Mem Size: ");
	strcat(Memory, buff);
	strcat(Memory, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(Maximums, "Maximum Clock Freq: ");
	strcat(Maximums, buff);
	strcat(Maximums, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_int), &t, NULL);
	_itoa(t, &buff[0], 10);
	strcat(Maximums, "Maximum Compute Units: ");
	strcat(Maximums, buff);
	strcat(Maximums, "___");

	error = clGetDeviceInfo(q[Device], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &s, NULL);
	_itoa(s, &buff[0], 10);
	strcat(Maximums, "Maximum Memory Allocation Size: ");
	strcat(Maximums, buff);
	strcat(Maximums, "___");

	size_t r;
	error = clGetDeviceInfo(q[Device], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &r, NULL);
	_itoa(r, &buff[0], 10);
	strcat(Maximums, "Maximum Work Group Size: ");
	strcat(Maximums, buff);
	strcat(Maximums, "___");

	return error;
	}catch(...){
		return -1;
	}

#endif

	return 0;

}

EXPORT clLabviewDevice *CreateDevice(int Platform, int Device, int *Error){
	clLabviewDevice *d = new clLabviewDevice;

	try{
	*Error = d->CreateContext(Platform, Device);

	if(*Error == 0){
		return d;
	}else{
		return NULL;
	}
	}catch(...){
		return NULL;
	}
}
