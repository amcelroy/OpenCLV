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