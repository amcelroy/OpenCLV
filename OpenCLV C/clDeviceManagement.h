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

