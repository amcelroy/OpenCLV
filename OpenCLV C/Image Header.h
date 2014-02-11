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


#ifndef __IMAGE_HEADER_H__
#define __IMAGE_HEADER_H__

#include "clLabviewDevice.h"

extern "C"{

	EXPORT cl_mem AllocateImage(clLabviewDevice *d, size_t Width, size_t Height, size_t Depth, 
				int ChannelOrder, int DataType, int *Error);

	EXPORT void WriteImage_i32(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, int *Data, double *Time, int *Error);
	EXPORT void ReadImage_i32(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, int *Data, double *Time, int *Error);

	EXPORT void WriteImage_u32(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, unsigned int *Data, double *Time, int *Error);
	EXPORT void ReadImage_u32(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, unsigned int *Data, double *Time, int *Error);

	EXPORT void WriteImage_i8(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, char *Data, double *Time, int *Error);
	EXPORT void ReadImage_i8(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, char *Data, double *Time, int *Error);

	EXPORT void WriteImage_u8(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, unsigned char *Data, double *Time, int *Error);
	EXPORT void ReadImage_u8(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, unsigned char *Data, double *Time, int *Error);

	EXPORT void WriteImage_i16(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, short *Data, double *Time, int *Error);
	EXPORT void ReadImage_i16(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, short *Data, double *Time, int *Error);

	EXPORT void WriteImage_u16(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, unsigned short *Data, double *Time, int *Error);
	EXPORT void ReadImage_u16(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, unsigned short *Data, double *Time, int *Error);

	EXPORT void WriteImage_32f(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, float *Data, double *Time, int *Error);
	EXPORT void ReadImage_32f(clLabviewDevice *d, cl_mem ImageObject, size_t Width, size_t Height, size_t Depth,
					size_t XOffset, size_t YOffset, size_t ZOffset, float *Data, double *Time, int *Error);
}

#endif