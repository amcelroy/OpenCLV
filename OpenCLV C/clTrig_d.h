#ifndef __CL_TRIG_64F_H__
#define __CL_TRIG_64F_H__

#include "clLabviewDevice.h"

//Basic Trig Functions.  Input memory is different from output memory
EXPORT int Sin_64f(cl_mem *Input, cl_mem *Output, int Width, int Height, int Depth);
EXPORT int Cos_64f(cl_mem *Input, cl_mem *Output, int Width, int Height, int Depth);
EXPORT int Tan_64f(cl_mem *Input, cl_mem *Output, int Width, int Height, int Depth);

//Basic Anti Trig Functions.  Input memory is different from output memory
EXPORT int ASin_64f(cl_mem *Input, cl_mem *Output, int Width, int Height, int Depth);
EXPORT int ACos_64f(cl_mem *Input, cl_mem *Output, int Width, int Height, int Depth);
EXPORT int ATan_64f(cl_mem *Input, cl_mem *Output, int Width, int Height, int Depth);

//Basic Hyper Bolic Trig Functions.  Input memory is different from output memory
EXPORT int Sinh_64f(cl_mem *Input, cl_mem *Output, int Width, int Height, int Depth);
EXPORT int Cosh_64f(cl_mem *Input, cl_mem *Output, int Width, int Height, int Depth);
EXPORT int Tanh_64f(cl_mem *Input, cl_mem *Output, int Width, int Height, int Depth);

//Basic Trig Functions.  Input memory is the same as output memory
EXPORT int Sin_64f_I(cl_mem *InputOutput, int Width, int Height, int Depth);
EXPORT int Cos_64f_I(cl_mem *InputOutput, int Width, int Height, int Depth);
EXPORT int Tan_64f_I(cl_mem *InputOutput, int Width, int Height, int Depth);

//Basic Anti Trig Functions.  Input memory is the same as output memory
EXPORT int ASin_64f_I(cl_mem *InputOutput, int Width, int Height, int Depth);
EXPORT int ACos_64f_I(cl_mem *InputOutput, int Width, int Height, int Depth);
EXPORT int ATan_64f_I(cl_mem *InputOutput, int Width, int Height, int Depth);

//Basic Hyper Bolic Trig Functions.  Input memory is the same as output memory
EXPORT int Sinh_64f_I(cl_mem *InputOutput, int Width, int Height, int Depth);
EXPORT int Cosh_64f_I(cl_mem *InputOutput, int Width, int Height, int Depth);
EXPORT int Tanh_64f_I(cl_mem *InputOutput, int Width, int Height, int Depth);

//ATan2 Funtion
EXPORT int ATan_64f_I(cl_mem *OutputAndX, cl_mem *Y, int Width, int Height, int Depth);
EXPORT int ATan_64f(cl_mem X, cl_mem *Y, cl_mem *Output, int Width, int Height, int Depth);

#endif