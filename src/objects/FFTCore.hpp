#ifndef ZSOUNDPROC_OBJECTS_FFT_WRAPPER_H
#define ZSOUNDPROC_OBJECTS_FFT_WRAPPER_H

#include <string>
#include <stdint.h>
#include <complex>
#include <vector>

namespace zsoundproc {
	typedef double REAL;
	typedef void DFT;
	typedef std::complex<REAL> REAL_element;
	typedef std::vector<REAL_element> REAL_row;
	typedef std::vector<REAL_row> REAL_matrix;

#define FFT_TYPE SIMDBase_TYPE_DOUBLE

	class FFTCore {
	private:
		//FFT INTERNAL
		int mode;
		REAL * sx;
		DFT * p;
		int veclen;
		int sizeOfVect;
		int elements;
		size_t sxSize;

		void init(const int n = 256, int32_t flags = 0);
		void destroy();
	public:
		FFTCore();
		FFTCore(const int n);
		FFTCore(const int n, int32_t flags);
		~FFTCore();

		REAL_matrix inputMatrix;
		REAL_matrix outputMatrix;

		const std::string getMode();
		int32_t readExecutionPlan(const std::string plan);
		const std::string writeExecutionPlan();
		int getModeParam(int32_t key) const;
		const std::string getModeParamString(int32_t key) const;
		int getPlanParam(int32_t key) const;

		void execute(int dir=-1);
	};
};

#endif