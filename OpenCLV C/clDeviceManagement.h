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

#ifndef __CL_DEVICE_MANAGEMENT_H__
#define __CL_DEVICE_MANAGEMENT_H__

#include "clLabviewDevice.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class clDeviceManagement
{
public:
	clDeviceManagement(void);
	~clDeviceManagement(void);

	cl_uint GetPlatforms();

	cl_uint GetNumberOfPlatforms() {return m_NumberOfPlatforms;}
	std::vector<cl_platform_id> GetListOfPlatforms() {return m_PlatformList;}
	cl_platform_id GetGPUPlatform();
	cl_platform_id GetCPUPlatform();

private:	
	//Different OpenCL platforms (NVIDIA, Intel, AMD, etc)
	std::vector<cl_platform_id> m_PlatformList;
	cl_uint m_NumberOfPlatforms;
	
	//List of different GPUs
	std::vector<cl_device_id> m_GPUList;
	cl_uint m_NumberOfGPUs;

	//List of different CPUs
	std::vector<cl_device_id> m_CPUList;
	cl_uint m_NumberOfCPUs;

	//List of Accelerators (IBM Cell)
	std::vector<cl_device_id> m_AcceleratorList;
	cl_uint m_NumberOfAccelerators;
	HMODULE mOpenCLDLL;
};

extern "C"{
	EXPORT int GetPlatformInfo(int Platform, char PlatformNames[], char PlatformProfile[], char PlatformVersion[], 
		char PlatformVendor[], char PlatformExtensions[]);
	EXPORT int GetDeviceInfo(int Platform, int Device, char DeviceName[], char DeviceInfo[], char VectorInfo[], char Memory[],
						char Maximums[]);
	EXPORT int GetPlatformNumber();
	EXPORT int GetDeviceNumber(int Platform);
	EXPORT clLabviewDevice *CreateDevice(int Platform, int Device, int *Error);
}

#endif

