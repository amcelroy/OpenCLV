#ifndef __MIN_MAX_H__
#define __MIN_MAX_H__

#include "clLabviewDevice.h"

class clMinMax : public clLabviewDevice{
public:
	clMinMax();
	~clMinMax();

	void SetData(float *p_Data, int Width, int Height, int Depth);
	float *GetMax() {return &m_Max;}
	float *GetMin() {return &m_Min;}

	void TestMinMax(float *p_Data, int Width, int Height, int Depth);
private:

	int m_Chunk;
	float m_Max;
	float m_Min;

	void __RecursiveSearchMin(cl_mem Input, int ItemsToSearch);
	void __RecursiveSearchMax(cl_mem Input, int ItemsToSearch);

};


#endif