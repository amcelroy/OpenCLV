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

cl_mem AllocateImage(clLabviewDevice *d, size_t Width, size_t Height, size_t Depth, 
			int ChannelOrder, int DataType, int *Error){
	
	cl_mem Image = 0;

#ifndef NO_OPENCL
	*Error = clLabviewDevice::Error(clLabviewDevice::SanitizeDevice(d));
	if(*Error != 0)
		return NULL;		

	cl_channel_order channel_order;
	cl_channel_type data_type;

	switch(DataType){
		case 0:
		data_type = CL_FLOAT;
		break;

		case 1:
		data_type = CL_UNSIGNED_INT8;
		break;

		case 3:
		data_type = CL_UNSIGNED_INT16;
		break;

		case 5:
		data_type = CL_UNSIGNED_INT32;
		break;

		case 2:
		data_type = CL_SIGNED_INT8;
		break;

		case 4:
		data_type = CL_SIGNED_INT16;
		break;

		case 6:
		data_type = CL_SIGNED_INT32;
		break;
	}

	switch(ChannelOrder){
		case 0:
		channel_order = CL_LUMINANCE;
		break;

		case 1:
		channel_order = CL_INTENSITY;
		break;

		case 2:
		channel_order = CL_RGBA;
		break;

		case 3:
		channel_order = CL_ARGB;
		break;

		case 4:
		channel_order = CL_BGRA;
		break;

		case 5:
		channel_order = CL_RG;
		break;

		case 6:
		channel_order = CL_RA;
		break;

		case 7:
		channel_order = CL_R;
		break;

		case 8:
		channel_order = CL_A;
		break;
	}

	cl_image_format image_format;
	image_format.image_channel_data_type = data_type;
	image_format.image_channel_order = channel_order;


//#ifdef CL_VERSION_1_2
//	cl_image_desc image_disc;
//	image_disc.num_mip_levels = 0;
//	image_disc.num_samples = 0;
//	image_disc.image_row_pitch = 0;
//	image_disc.image_slice_pitch = 0;
//	image_disc.image_depth = Depth;
//	image_disc.image_height = Height;
//	image_disc.image_width = Width;
//	image_disc.image_array_size = Width*Height*Depth;
//	image_disc.buffer = NULL;
//
//	if(Depth == 1){
//		image_disc.image_type = CL_MEM_OBJECT_IMAGE2D;
//	}else{
//		image_disc.image_type = CL_MEM_OBJECT_IMAGE3D;
//	}
//
//	cl_mem Image = clCreateImage(d->GetContext(), CL_MEM_ALLOC_HOST_PTR, &image_format, &image_disc, NULL, Error);
//#endif

//#ifdef CL_VERSION_1_1

	if(Depth == 1){
		Image = clCreateImage2D(d->GetContext(), CL_MEM_ALLOC_HOST_PTR, &image_format, Width, Height, NULL, NULL, Error);
	}else{
		Image = clCreateImage3D(d->GetContext(), CL_MEM_ALLOC_HOST_PTR, &image_format, Width, Height, Depth, NULL, NULL, NULL, Error);
	}
//#endif
	*Error = clLabviewDevice::Error(*Error);

#endif

	if(*Error == 0){	
		return Image;
	}else{
		return NULL;
	}

}

