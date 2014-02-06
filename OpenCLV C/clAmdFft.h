/***********************************************************************
**	Copyright (C) 2010,2011 Advanced Micro Devices, Inc. All Rights Reserved.
***********************************************************************/

/*! @file clAmdFft.h
 * clAmdFft.h defines all of the public interfaces and types that are meant to be used by clFFT clients
 * This is the one public header file that should be consumed by clFFT clients.  It is written to adhere to native "C"
 * interfaces to make clAmdFft library as portable as possible; it should be callable from C, C++, .NET and Fortran,
 * either with the proper linking or using wrapper classes.
 *
 */

#pragma once
#if !defined( CLAMDFFT_DOTH )
#define CLAMDFFT_DOTH

#if defined(__APPLE__) || defined(__MACOSX)
	#include <OpenCL/cl.h>
#else
	#include <CL/cl.h>
#endif

#include "clAmdFft.version.h"

#include "clLabviewDevice.h"

/*! This preprocessor definition is the standard way of making exporting APIs
 *  from a DLL simpler. All files within this DLL are compiled with the CLAMDFFT_EXPORTS
 *  symbol defined on the command line. This symbol should not be defined on any project
 *  that uses this DLL. This way any other project whose source files include this file see 
 *  clAmdFft functions as being imported from a DLL, whereas this DLL sees symbols
 *  defined with this macro as being exported.
 */
#if defined( _WIN32 )
	#if !defined( __cplusplus )
		#define inline __inline
	#endif

	#if defined( CLAMDFFT_EXPORTS )
		#define CLAMDFFTAPI __declspec( dllexport )
	#else
		#define CLAMDFFTAPI __declspec( dllimport )
	#endif
#else
	#define CLAMDFFTAPI
#endif

/*	In general, you can not use namespaces for strict C compliance, so we prefix our public accessible names
 *	with the string clAmdFft
 */

/*	All functions will return pre-defined error codes, and will NOT throw exceptions to the caller
 */

/*!  @brief clAmdFft error codes definition, incorporating OpenCL error definitions 
 *
 *   This enumeration is a superset of the OpenCL error codes.  For example, CL_OUT_OF_HOST_MEMORY,
 *   which is defined in cl.h is aliased as CLFFT_OUT_OF_HOST_MEMORY.  The set of basic OpenCL
 *   error codes is extended to add extra values specific to the clAmdFft package.
 */
enum clAmdFftStatus_
{
	CLFFT_INVALID_GLOBAL_WORK_SIZE			= CL_INVALID_GLOBAL_WORK_SIZE,
	CLFFT_INVALID_MIP_LEVEL					= CL_INVALID_MIP_LEVEL,
	CLFFT_INVALID_BUFFER_SIZE				= CL_INVALID_BUFFER_SIZE,
	CLFFT_INVALID_GL_OBJECT					= CL_INVALID_GL_OBJECT,
	CLFFT_INVALID_OPERATION					= CL_INVALID_OPERATION,
	CLFFT_INVALID_EVENT						= CL_INVALID_EVENT,
	CLFFT_INVALID_EVENT_WAIT_LIST			= CL_INVALID_EVENT_WAIT_LIST,
	CLFFT_INVALID_GLOBAL_OFFSET				= CL_INVALID_GLOBAL_OFFSET,
	CLFFT_INVALID_WORK_ITEM_SIZE			= CL_INVALID_WORK_ITEM_SIZE,
	CLFFT_INVALID_WORK_GROUP_SIZE			= CL_INVALID_WORK_GROUP_SIZE,
	CLFFT_INVALID_WORK_DIMENSION			= CL_INVALID_WORK_DIMENSION,
	CLFFT_INVALID_KERNEL_ARGS				= CL_INVALID_KERNEL_ARGS,
	CLFFT_INVALID_ARG_SIZE					= CL_INVALID_ARG_SIZE,
	CLFFT_INVALID_ARG_VALUE					= CL_INVALID_ARG_VALUE,
	CLFFT_INVALID_ARG_INDEX					= CL_INVALID_ARG_INDEX,
	CLFFT_INVALID_KERNEL					= CL_INVALID_KERNEL,
	CLFFT_INVALID_KERNEL_DEFINITION			= CL_INVALID_KERNEL_DEFINITION,
	CLFFT_INVALID_KERNEL_NAME				= CL_INVALID_KERNEL_NAME,
	CLFFT_INVALID_PROGRAM_EXECUTABLE		= CL_INVALID_PROGRAM_EXECUTABLE,
	CLFFT_INVALID_PROGRAM					= CL_INVALID_PROGRAM,
	CLFFT_INVALID_BUILD_OPTIONS				= CL_INVALID_BUILD_OPTIONS,
	CLFFT_INVALID_BINARY					= CL_INVALID_BINARY,
	CLFFT_INVALID_SAMPLER					= CL_INVALID_SAMPLER,
	CLFFT_INVALID_IMAGE_SIZE				= CL_INVALID_IMAGE_SIZE,
	CLFFT_INVALID_IMAGE_FORMAT_DESCRIPTOR	= CL_INVALID_IMAGE_FORMAT_DESCRIPTOR,
	CLFFT_INVALID_MEM_OBJECT				= CL_INVALID_MEM_OBJECT,
	CLFFT_INVALID_HOST_PTR					= CL_INVALID_HOST_PTR,
	CLFFT_INVALID_COMMAND_QUEUE				= CL_INVALID_COMMAND_QUEUE,
	CLFFT_INVALID_QUEUE_PROPERTIES			= CL_INVALID_QUEUE_PROPERTIES,
	CLFFT_INVALID_CONTEXT					= CL_INVALID_CONTEXT,
	CLFFT_INVALID_DEVICE					= CL_INVALID_DEVICE,
	CLFFT_INVALID_PLATFORM					= CL_INVALID_PLATFORM,
	CLFFT_INVALID_DEVICE_TYPE				= CL_INVALID_DEVICE_TYPE,
	CLFFT_INVALID_VALUE						= CL_INVALID_VALUE,
	CLFFT_MAP_FAILURE						= CL_MAP_FAILURE,
	CLFFT_BUILD_PROGRAM_FAILURE				= CL_BUILD_PROGRAM_FAILURE,
	CLFFT_IMAGE_FORMAT_NOT_SUPPORTED		= CL_IMAGE_FORMAT_NOT_SUPPORTED,
	CLFFT_IMAGE_FORMAT_MISMATCH				= CL_IMAGE_FORMAT_MISMATCH,
	CLFFT_MEM_COPY_OVERLAP					= CL_MEM_COPY_OVERLAP,
	CLFFT_PROFILING_INFO_NOT_AVAILABLE		= CL_PROFILING_INFO_NOT_AVAILABLE,
	CLFFT_OUT_OF_HOST_MEMORY				= CL_OUT_OF_HOST_MEMORY,
	CLFFT_OUT_OF_RESOURCES					= CL_OUT_OF_RESOURCES,
	CLFFT_MEM_OBJECT_ALLOCATION_FAILURE		= CL_MEM_OBJECT_ALLOCATION_FAILURE,
	CLFFT_COMPILER_NOT_AVAILABLE			= CL_COMPILER_NOT_AVAILABLE,
	CLFFT_DEVICE_NOT_AVAILABLE				= CL_DEVICE_NOT_AVAILABLE,
	CLFFT_DEVICE_NOT_FOUND					= CL_DEVICE_NOT_FOUND,
	CLFFT_SUCCESS							= CL_SUCCESS,
	//-------------------------- Extended status codes for clAmdFft ----------------------------------------
	CLFFT_BUGCHECK =  4*1024,	/*!< Bugcheck. */
	CLFFT_NOTIMPLEMENTED,		/*!< Functionality is not implemented yet. */
	CLFFT_TRANSPOSED_NOTIMPLEMENTED, /*!< Transposed functionality is not implemented for this transformation. */
	CLFFT_FILE_NOT_FOUND,		/*!< Tried to open an existing file on the host system, but failed. */
	CLFFT_FILE_CREATE_FAILURE,	/*!< Tried to create a file on the host system, but failed. */
	CLFFT_VERSION_MISMATCH,		/*!< Version conflict between client and library. */
	CLFFT_INVALID_PLAN,			/*!< Requested plan could not be found. */
	CLFFT_DEVICE_NO_DOUBLE,		/*!< Double precision not supported on this device. */
	CLFFT_ENDSTATUS				/* This value will always be last, and marks the length of clAmdFftStatus. */
};
typedef enum clAmdFftStatus_ clAmdFftStatus;

/*!  @brief The dimension of the input and output buffers that will be fed into all FFT transforms */
typedef enum clAmdFftDim_
{
	CLFFT_1D		= 1,		/*!< 1 Dimensional FFT transform (default). */
	CLFFT_2D,					/*!< 2 Dimensional FFT transform. */
	CLFFT_3D,					/*!< 3 Dimensional FFT transform. */
	ENDDIMENSION			/*!< This value will always be last, and marks the length of clAmdFftDim. */
} clAmdFftDim;

/*!  @brief What are the expected layout's of the complex numbers <p>
 *   <b> For Release 1.0,</b> only the CLFFT_COMPLEX_INTERLEAVED and CLFFT_COMPLEX_PLANAR formats are supported.
 *   The real and hermitian formats should be supported in a future release.
 */
typedef enum clAmdFftLayout_
{
	CLFFT_COMPLEX_INTERLEAVED	= 1,	/*!< An array of complex numbers, with real and imaginary components together (default). */
	CLFFT_COMPLEX_PLANAR,				/*!< Arrays of real componets and arrays of imaginary components that have been seperated out. */
	CLFFT_HERMITIAN_INTERLEAVED,		/*!< Compressed form of complex numbers; complex-conjugates not stored, real and imaginary components in same array. TODO: Document layout */
	CLFFT_HERMITIAN_PLANAR,				/*!< Compressed form of complex numbers; complex-conjugates not stored, real and imaginary components in separate arrays. TODO: Document layout */
	CLFFT_REAL,							/*!< An array of real numbers, with no corresponding imaginary components. */
	ENDLAYOUT			/*!< This value will always be last, and marks the length of clAmdFftLayout. */
} clAmdFftLayout;

/*!  @brief What is the expected precision of each FFT.
 *   @ref DistanceStridesandPitches
 */
typedef enum clAmdFftPrecision_
{
	CLFFT_SINGLE	= 1,	/*!< An array of complex numbers, with real and imaginary components as floats (default). */
	CLFFT_DOUBLE,			/*!< An array of complex numbers, with real and imaginary components as doubles. */
	CLFFT_SINGLE_FAST,		/*!< Faster implementation preferred. */
	CLFFT_DOUBLE_FAST,		/*!< Faster implementation preferred. */
	ENDPRECISION	/*!< This value will always be last, and marks the length of clAmdFftPrecision. */
} clAmdFftPrecision;

/*!  @brief What is the expected direction of each FFT, time or the frequency domains */
typedef enum clAmdFftDirection_
{
	CLFFT_FORWARD	= -1,		/*!< FFT transform from the time to the frequency domain. */
	CLFFT_BACKWARD	= 1,		/*!< FFT transform from the frequency to the time domain. */
	CLFFT_MINUS		= -1,		/*!< Alias for the forward transform. */
	CLFFT_PLUS		= 1,		/*!< Alias for the backward transform. */
	ENDDIRECTION			/*!< This value will always be last, and marks the length of clAmdFftDirection. */
} clAmdFftDirection;

/*!  @brief Are the input buffers overwritten with the results */
typedef enum clAmdFftResultLocation_
{
	CLFFT_INPLACE		= 1,		/*!< The input and output buffers are the same (default). */
	CLFFT_OUTOFPLACE,				/*!< Seperate input and output buffers. */
	ENDPLACE				/*!< This value will always be last, and marks the length of clAmdFftPlaceness. */
} clAmdFftResultLocation;

/*! @brief whether the result will be returned in original order; only valid for dimensions greater than 1 */
typedef enum clAmdFftResultTransposed_ {
	CLFFT_NOTRANSPOSE = 1,		/*!< The results are returned in the original preserved order (default) */
	CLFFT_TRANSPOSED,			/*!< The result is transposed where transpose kernel is supported (possibly faster) */
	ENDTRANSPOSED			/*!< This value will always be last, and marks the length of clAmdFftResultTransposed */
} clAmdFftResultTransposed;

/*! 	BitMasks to be used with clAmdFftSetupData.debugFlags */
#define CLFFT_DUMP_PROGRAMS 0x1

/*! @brief Data structure that can be passed to clAmdFftSetup() to control the behavior of the FFT runtime
 *  @details This structure contains values that can be initialized before instantiation of the FFT runtime 
 *  with ::clAmdFftSetup().  To initialize this structure, pass a pointer to a user struct to ::clAmdFftInitSetupData( ),
 *  which will clear the structure and set the version member variables to the current values.
 */
struct clAmdFftSetupData_
{
	cl_uint major;		/*!< Major version number of the project; signifies major API changes. */
	cl_uint minor;		/*!< Minor version number of the project; minor API changes that could break backwards compatibility. */
	cl_uint patch;		/*!< Patch version number of the project; Always incrementing number, signifies change over time. */

	/*! 	Bitwise flags that control the behavior of library debug logic. */
	cl_ulong debugFlags;  /*! This should be set to zero, except when debugging the clAmdFft library.
	                       *  <p> debugFlags can be set to CLFFT_DUMP_PROGRAMS, in which case the dynamically generated OpenCL kernels will
	                       *  be written to text files in the current working directory.  These files will have a *.cl suffix.
	                       */
};
typedef struct clAmdFftSetupData_ clAmdFftSetupData;

/*!  @brief An abstract handle to the object that represents the state of the FFT(s) */
typedef size_t clAmdFftPlanHandle;

#ifdef __cplusplus
extern "C" {
#endif
	/*! @brief Initialize an clAmdFftSetupData struct for the client
	 *  @details clAmdFftSetupData is passed to clAmdFftSetup to control behavior of the FFT runtime
	 *  @param[out] setupData Data structure is cleared, initialized with version information and default values
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	inline clAmdFftStatus clAmdFftInitSetupData( clAmdFftSetupData* setupData )
	{
		setupData->major	= clAmdFftVersionMajor;
		setupData->minor	= clAmdFftVersionMinor;
		setupData->patch	= clAmdFftVersionPatch;
		setupData->debugFlags	= 0;

		return	CLFFT_SUCCESS;
	}

	/*! @brief Initialize internal FFT resources.
	 *  @details AMD's FFT implementation caches kernels, programs and buffers for its internal use.
	 *  @param[in] setupData Data structure that can be passed into the setup routine to control FFT generation behavior 
	 * 	and debug functionality
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetup( const clAmdFftSetupData* setupData );

	/*! @brief Release all internal resources.
	 *  @details Call when client is done with this FFT library, allowing the library to destroy all resources it has cached
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftTeardown( );

	/*! @brief Query the FFT library for version information
	 *  @details Return the major, minor and patch version numbers associated with this FFT library
	 *  @param[out] major Major functionality change
	 *  @param[out] minor Minor functionality change
	 *  @param[out] patch Bug fixes, documentation changes, no new features introduced
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetVersion( cl_uint* major, cl_uint* minor, cl_uint* patch );

	/*! @brief Create a plan object initialized entirely with default values.
	 *  @details A plan is a repository of state for calculating FFT's.  Allows the runtime to pre-calculate kernels, programs
	 * 	and buffers and associate them with buffers of specified dimensions.
	 *  @param[out] plHandle Handle to the newly created plan
	 *  @param[in] context Client is responsible for providing an OpenCL context for the plan
	 *  @param[in] dim The dimensionality of the FFT transform; describes how many elements are in the array
	 *  @param[in] clLengths An array of lengths, of size 'dim'.  Each value describes the length of additional dimensions
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftCreateDefaultPlan( clAmdFftPlanHandle* plHandle, cl_context context, const clAmdFftDim dim, 
								const size_t* clLengths );

	/*! @brief Create a copy of an existing plan.
	 *  @details This API allows a client to create a new plan based upon an existing plan.  This is a convenience function 
	 *  provided for quickly creating plans that are similar, but may differ slightly.
	 *  @param[out] out_plHandle Handle to the newly created plan that is based on in_plHandle
	 *  @param[in] new_context Client is responsible for providing a new context for the new plan
	 *  @param[in] in_plHandle Handle to a plan to be copied, previously created
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftCopyPlan( clAmdFftPlanHandle* out_plHandle, cl_context new_context, clAmdFftPlanHandle in_plHandle );

	/*! @brief Prepare the plan for execution.
	 *  @details After all plan parameters are set, the client has the option of 'baking' the plan, which tells the runtime that
	 *  no more changes to the plan's parameters are expected, and the OpenCL kernels should be compiled.  This optional function
	 *  allows the client application to perform this function when the application is being initialized instead of on the first
	 *  execution. 
	 *  At this point, the clAmdFft runtime will apply all implimented optimizations, possibly including
	 *  running kernel experiments on the devices in the plan context.
	 *  <p>  Users should assume that this function will take a long time to execute.  If a plan is not baked before being executed, 
	 *  users should assume that the first call to clAmdFftEnqueueTransform will take a long time to execute.
	 *  <p>  If any significant parameter of a plan is changed after the plan is baked (by a subsequent call to one of 
	 *  the clAmdFftSetPlan____ functions), that will not be considered an error.  Instead, the plan will revert back to
	 *  the unbaked state, discarding the benefits of the baking operation.
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] numQueues Number of command queues in commQueueFFT; 0 is a valid value, in which case client does not want 
	 * 	the runtime to run load experiments and only pre-calculate state information
	 *  @param[in] commQueueFFT An array of cl_command_queues created by the client; the command queues must be a proper subset of 
	 * 	the devices included in the plan context
	 *  @param[in] pfn_notify A function pointer to a notification routine. The notification routine is a callback function that 
	 *  an application can register and which will be called when the program executable has been built (successfully or unsuccessfully)
	 *  <b> For Release 1.0,</b> this parameter MUST be NULL or nullptr.
	 *  @param[in] user_data Passed as an argument when pfn_notify is called. 
	 *  <b> For Release 1.0,</b> this parameter MUST be NULL or nullptr.
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftBakePlan( clAmdFftPlanHandle plHandle, cl_uint numQueues, cl_command_queue* commQueueFFT,
							void (CL_CALLBACK *pfn_notify)(clAmdFftPlanHandle plHandle, void *user_data), void* user_data );

	/*! @brief Release the resources of a plan.
	 *  @details A plan may include kernels, programs and buffers associated with it that consume memory.  When a plan 
	 *  is not needed anymore, the client should release the plan.
	 *  @param[in,out] plHandle Handle to a plan previously created
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftDestroyPlan( clAmdFftPlanHandle* plHandle );
	
	/*! @brief Retrieve the OpenCL context of a previously created plan.
	 *  @details User should pass a reference to an cl_context variable, which will be changed to point to a 
	 *  context set in the specified plan.
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] context Reference to user allocated cl_context, which will point to context set in plan
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanContext( const clAmdFftPlanHandle plHandle, cl_context* context );

	/*! @brief Retrieve the floating point precision of the FFT data
	 *  @details User should pass a reference to an clAmdFftPrecision variable, which will be set to the 
	 *  precision of the FFT complex data in the plan.
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] precision Reference to user clAmdFftPrecision enum
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanPrecision( const clAmdFftPlanHandle plHandle, clAmdFftPrecision* precision );

	/*! @brief Set the floating point precision of the FFT data
	 *  @details Set the plan property which will be the precision of the FFT complex data in the plan.
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] precision Reference to user clAmdFftPrecision enum <p>
	 *  <b> For Release 1.0,</b> only CLFFT_SINGLE and CLFFT_SINGLE_FAST are supported.
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetPlanPrecision( clAmdFftPlanHandle plHandle, clAmdFftPrecision precision );

	/*! @brief Retrieve the scaling factor that should be applied to the FFT data
	 *  @details User should pass a reference to an cl_float variable, which will be set to the 
	 *  floating point scaling factor that will be multiplied across the FFT data.
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dir Which direction does the scaling factor apply to
	 *  @param[out] scale Reference to user cl_float variable
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanScale( const clAmdFftPlanHandle plHandle, clAmdFftDirection dir, cl_float* scale );

	/*! @brief Set the scaling factor that should be applied to the FFT data
	 *  @details Set the plan property which will be the floating point scaling factor that will be 
	 *  multiplied across the FFT data.
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dir Which direction does the scaling factor apply to
	 *  @param[in] scale Reference to user cl_float variable
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetPlanScale( clAmdFftPlanHandle plHandle, clAmdFftDirection dir, cl_float scale );

	/*! @brief Retrieve the number of discrete arrays that this plan can handle concurrently
	 *  @details User should pass a reference to an cl_uint variable, which will be set to the 
	 *  number of discrete arrays (1D or 2D) that will be batched together for this plan
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] batchSize How many discrete number of FFT's are to be performed
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanBatchSize( const clAmdFftPlanHandle plHandle, size_t* batchSize );

	/*! @brief Set the number of discrete arrays that this plan can handle concurrently
	 *  @details Set the plan property which will be set to the number of discrete arrays (1D or 2D) 
	 *  that will be batched together for this plan
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] batchSize How many discrete number of FFT's are to be performed
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetPlanBatchSize( clAmdFftPlanHandle plHandle, size_t batchSize );

	/*! @brief Retrieve the dimensionality of FFT's to be transformed in the plan
	 *  @details Queries a plan object and retrieves the dimensionality that the plan is set for.  A size is returned to
	 *  help the client allocate the proper storage to hold the dimensions in a further call to clAmdFftGetPlanLength
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] dim The dimensionality of the FFT's to be transformed
	 *  @param[out] size Value used to allocate an array to hold the FFT dimensions.
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanDim( const clAmdFftPlanHandle plHandle, clAmdFftDim* dim, cl_uint* size );

	/*! @brief Set the dimensionality of FFT's to be transformed by the plan
	 *  @details Set the dimensionality of FFT's to be transformed by the plan
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dim The dimensionality of the FFT's to be transformed
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetPlanDim( clAmdFftPlanHandle plHandle, const clAmdFftDim dim );

	/*! @brief Retrieve the length of each dimension of the FFT
	 *  @details User should pass a reference to a size_t array, which will be set to the 
	 *  length of each discrete dimension of the FFT
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dim The dimension of the length parameters; describes how many elements are in the array
	 *  @param[out] clLengths An array of lengths, of size 'dim'.  Each array value describes the length of each dimension
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanLength( const clAmdFftPlanHandle plHandle, const clAmdFftDim dim, size_t* clLengths );

	/*! @brief Set the length of each dimension of the FFT
	 *  @details Set the plan property which will be the length of each discrete dimension of the FFT
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dim The dimension of the length parameters; describes how many elements are in the array
	 *  @param[in] clLengths An array of lengths, of size 'dim'.  Each value describes the length of additional dimensions
	 *  <p><b> For Release 1.0, </b> All lengths must be powers of 2.  Non-power-of-two dimensions should be supported in a future release.
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetPlanLength( clAmdFftPlanHandle plHandle, const clAmdFftDim dim, const size_t* clLengths );

	/*! @brief Retrieve the distance between consecutive elements for input buffers in a dimension.
	 *  @details Depending on how the dimension is set in the plan (for 2D or 3D FFT's), strideY or strideZ can be safely
	 *  ignored
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dim The dimension of the stride parameters; describes how many elements are in the array
	 *  @param[out] clStrides An array of strides, of size 'dim'.  Usually strideX=1 so that successive elements in the first dimension are stored contiguously.
	 * 	Typically strideY=LenX, strideZ=LenX*LenY such that successive elements in the second and third dimensions are stored contiguously.
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanInStride( const clAmdFftPlanHandle plHandle, const clAmdFftDim dim, size_t* clStrides );

	/*! @brief Set the distance between consecutive elements for input buffers in a dimension.
	 *  @details Set the plan properties which will be the distance between elements in a given dimension
	 *  (units are in terms of clAmdFftPrecision)
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dim The dimension of the stride parameters; describes how many elements are in the array
	 *  @param[in] clStrides An array of strides, of size 'dim'. 
	 *  See  @ref DistanceStridesandPitches for details.
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetPlanInStride( clAmdFftPlanHandle plHandle, const clAmdFftDim dim, size_t* clStrides );

	/*! @brief Retrieve the distance between consecutive elements for output buffers in a dimension.
	 *  @details Depending on how the dimension is set in the plan (for 2D or 3D FFT's), strideY or strideZ can be safely
	 *  ignored
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dim The dimension of the stride parameters; describes how many elements are in the array
	 *  @param[out] clStrides An array of strides, of size 'dim'.  Usually strideX=1 so that successive elements in the first dimension are stored contiguously.
	 *  Typically strideY=LenX, strideZ=LenX*LenYsuch that successive elements in the second and third dimensions are stored contiguously.
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanOutStride( const clAmdFftPlanHandle plHandle, const clAmdFftDim dim, size_t* clStrides );

	/*! @brief Set the distance between consecutive elements for output buffers in a dimension.
	 *  @details Set the plan properties which will be the distance between elements in a given dimension
	 *  (units are in terms of clAmdFftPrecision)
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dim The dimension of the stride parameters; describes how many elements are in the array
	 *  @param[in] clStrides An array of strides, of size 'dim'.  Usually strideX=1 so that successive elements in the first dimension are stored contiguously.
	 * 	Typically strideY=LenX, strideZ=LenX*LenY such that successive elements in the second and third dimensions are stored contiguously.
	 *  @sa clAmdFftSetPlanInStride
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetPlanOutStride( clAmdFftPlanHandle plHandle, const clAmdFftDim dim, size_t* clStrides );

	/*! @brief Retrieve the distance between Array objects
	 *  @details Pitch is the distance between each discrete array object in an FFT array. This is only used 
	 *  for 'array' dimensions in clAmdFftDim; see clAmdFftSetPlanDimension (units are in terms of clAmdFftPrecision)
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] iDist The distance between the beginning elements of the discrete array objects in memory on input.
	 *  For contiguous arrays in memory, iDist=(strideX*strideY*strideZ)
	 *  @param[out] oDist The distance between the beginning elements of the discrete array objects in memory on output.
	 *  For contiguous arrays in memory, oDist=(strideX*strideY*strideZ)
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanDistance( const clAmdFftPlanHandle plHandle, size_t* iDist, size_t* oDist );

	/*! @brief Set the distance between Array objects
	 *  @details Pitch is the distance between each discrete array object in an FFT array. This is only used 
	 *  for 'array' dimensions in clAmdFftDim; see clAmdFftSetPlanDimension (units are in terms of clAmdFftPrecision)
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] iDist The distance between the beginning elements of the discrete array objects in memory on input.
	 *  For contiguous arrays in memory, iDist=(strideX*strideY*strideZ)
	 *  @param[out] oDist The distance between the beginning elements of the discrete array objects in memory on output.
	 *  For contiguous arrays in memory, oDist=(strideX*strideY*strideZ)
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetPlanDistance( clAmdFftPlanHandle plHandle, size_t iDist, size_t oDist );

	/*! @brief Retrieve the expected layout of the input and output buffers
	 *  @details Output buffers can be filled with either hermitian or complex numbers.  Complex numbers can be stored 
	 *  in various layouts; this informs the FFT engine what layout to produce on output
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] iLayout Indicates how the input buffers are laid out in memory
	 *  @param[out] oLayout Indicates how the output buffers are laid out in memory
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetLayout( const clAmdFftPlanHandle plHandle, clAmdFftLayout* iLayout, clAmdFftLayout* oLayout );

	/*! @brief Set the expected layout of the input and output buffers
	 *  @details Output buffers can be filled with either hermitian or complex numbers.  Complex numbers can be stored 
	 *  in various layouts; this informs the FFT engine what layout to produce on output
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] iLayout Indicates how the input buffers are laid out in memory
	 *  @param[in] oLayout Indicates how the output buffers are laid out in memory
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetLayout( clAmdFftPlanHandle plHandle, clAmdFftLayout iLayout, clAmdFftLayout oLayout );

	/*! @brief Retrieve whether the input buffers are going to be overwritten with results
	 *  @details If the setting is to do an in-place transform, the input buffers are overwritten with the results of the
	 *  transform.  If the setting is for out-of-place transforms, the engine knows to look for separate output buffers
	 *  on the Enqueue call.
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] placeness Tells the FFT engine to clobber the input buffers or to expect output buffers for results
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetResultLocation( const clAmdFftPlanHandle plHandle, clAmdFftResultLocation* placeness );

	/*! @brief Set whether the input buffers are going to be overwritten with results
	 *  @details If the setting is to do an in-place transform, the input buffers are overwritten with the results of the
	 *  transform.  If the setting is for out-of-place transforms, the engine knows to look for separate output buffers
	 *  on the Enqueue call.
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] placeness Tells the FFT engine to clobber the input buffers or to expect output buffers for results
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetResultLocation( clAmdFftPlanHandle plHandle, clAmdFftResultLocation placeness );
	
	/*! @brief Retrieve the final transpose setting of a muti-dimensional FFT 
	 *  @details A multi-dimensional FFT typically transposes the data several times during calculation.  If the client 
	 *  does not care about the final transpose to put data back in proper dimension, the final transpose can be skipped 
	 *  for possible speed improvements 
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] transposed Parameter specifies whether the final transpose can be skipped
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftGetPlanTransposeResult( const clAmdFftPlanHandle plHandle, clAmdFftResultTransposed * transposed );

	/*! @brief Set the final transpose setting of a muti-dimensional FFT 
	 *  @details A multi-dimensional FFT typically transposes the data several times during calculation.  If the client 
	 *  does not care about the final transpose to put data back in proper dimension, the final transpose can be skipped 
	 *  for possible speed improvements 
	 *  @param[in] plHandle Handle to a plan previously created 
	 *  @param[in] transposed Parameter specifies whether the final transpose can be skipped
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftSetPlanTransposeResult( clAmdFftPlanHandle plHandle, clAmdFftResultTransposed transposed );


	/*! @brief Get buffer size (in bytes), which may be needed internally for an intermediate buffer
	 *  @details Very large FFT transforms may need multiple passes, and the operation would need a temporary buffer to hold
	 *  intermediate results. This function is only valid after the plan is baked, otherwise an invalid operation error
	 *  is returned. If buffersize returns as 0, the runtime needs no temporary buffer.
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[out] buffersize Size in bytes for intermediate buffer
	 */
	CLAMDFFTAPI clAmdFftStatus clAmdFftGetTmpBufSize( const clAmdFftPlanHandle plHandle, size_t* buffersize );

	/*! @brief Enqueue an FFT transform operation, and return immediately (non-blocking)
	 *  @details This transform API is specific to the interleaved complex format, taking an input buffer with real and imaginary
	 *  components paired together, and outputting the results into an output buffer in the same format
	 *  @param[in] plHandle Handle to a plan previously created
	 *  @param[in] dir Forwards or backwards transform
	 *  @param[in] numQueuesAndEvents Number of command queues in commQueues; number of expected events to be returned in outEvents
	 *  @param[in] commQueues An array of cl_command_queues created by the client; the command queues must be a proper subset of 
	 * 	the devices included in the plan context
	 *  @param[in] numWaitEvents Specify the number of elements in the eventWaitList array
	 *  @param[in] waitEvents Events that this transform should wait to complete before executing on the device
	 *  @param[out] outEvents The runtime fills this array with events corresponding 1 to 1 with the input command queues passed
	 *	in commQueues.  This parameter can be NULL or nullptr, in which case client is not interested in receiving notifications
	 *	when transforms are finished, otherwise if not NULL the client is responsible for allocating this array, with at least 
	 *	as many elements as specified in numQueuesAndEvents.
	 *  @param[in] inputBuffers An array of cl_mem objects that contain data for processing by the FFT runtime.  If the transform
	 *  is in place, the FFT results will overwrite the input buffers
	 *  @param[out] outputBuffers An array of cl_mem objects that will store the results of out of place transforms.  If the transform
	 *  is in place, this parameter may be NULL or nullptr.  It is completely ignored
	 *  @param[in] tmpBuffer A cl_mem object that is reserved as a temporary buffer for FFT processing. If clTmpBuffers is NULL or nullptr,
	 *  and the runtime needs temporary storage, an internal temporary buffer will be created on the fly managed by the runtime.
	 *  @return Enum describing error condition; superset of OpenCL error codes
	 */
	CLAMDFFTAPI clAmdFftStatus	clAmdFftEnqueueTransform(
												clAmdFftPlanHandle plHandle,
												clAmdFftDirection dir,
												cl_uint numQueuesAndEvents,
												cl_command_queue* commQueues,
												cl_uint numWaitEvents,
												const cl_event* waitEvents,
												cl_event* outEvents,
												cl_mem* inputBuffers,
												cl_mem* outputBuffers,
												cl_mem tmpBuffer
												);


	EXPORT void AllocateFFTFramework(int *Error);
	EXPORT clAmdFftPlanHandle CreateFFTPlan(clLabviewDevice *d, 
					int FFTType, int Dimension,  
					int Width, int Height, int Depth,
					int StrideW, int StrideH, int StrideD, int StrideT, 
					int PaddingW, int PaddingH, int PaddingD, 
					size_t *OutputWidthFloat, int SingleOrDouble, int *Error);
	EXPORT void ComputeFFT(clAmdFftPlanHandle Handle, clLabviewDevice *d, cl_mem InR, cl_mem InC, 
							cl_mem OutR, cl_mem OutC, int Direction, int *Error);

	EXPORT void DestroyFFTPlan(clAmdFftPlanHandle Handle, int *Error);
	EXPORT void DestroyFFTFramework(int *Error);

#ifdef __cplusplus
}
#endif

#endif
