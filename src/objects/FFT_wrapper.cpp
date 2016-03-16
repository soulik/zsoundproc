#include "objects/FFT_wrapper.hpp"

#include "SIMDBase.h"
#include "DFT.h"

#include <iostream>
#include <sstream>

namespace zsoundproc {
	FFT_wrapper::FFT_wrapper(){
		sx = nullptr;
		p = nullptr;
		veclen = 0;
		sizeOfVect = 0;
		mode = 0;
		elements = 0;

		init();
	}

	FFT_wrapper::FFT_wrapper(const int n){
		sx = nullptr;
		p = nullptr;
		veclen = 0;
		sizeOfVect = 0;
		mode = 0;

		init(n);
	}

	FFT_wrapper::FFT_wrapper(const int n, int32_t flags){
		sx = nullptr;
		p = nullptr;
		veclen = 0;
		sizeOfVect = 0;
		mode = 0;

		init(n, flags);
	}

	FFT_wrapper::~FFT_wrapper(){
		destroy();
	}

	void FFT_wrapper::init(const int n, int32_t flags){
		elements = n;
		mode = SIMDBase_chooseBestMode(FFT_TYPE);

		veclen = SIMDBase_getModeParamInt(SIMDBase_PARAMID_VECTOR_LEN, mode);
		sizeOfVect = SIMDBase_getModeParamInt(SIMDBase_PARAMID_SIZE_OF_VECT, mode);

		p = DFT_init(mode, elements, flags);
		sxSize = sizeOfVect*elements * 2;
		sx = reinterpret_cast<REAL*>(SIMDBase_alignedMalloc(sxSize));

		inputMatrix = REAL_matrix(veclen, REAL_row(n, REAL_element(0)));
		outputMatrix = REAL_matrix(veclen, REAL_row(n, REAL_element(0)));
	}

	void FFT_wrapper::destroy(){
		if (sx && p){
			SIMDBase_alignedFree(sx);
			DFT_dispose(p, mode);
		}
	}

	const std::string FFT_wrapper::getMode(){
		return std::string(SIMDBase_getModeParamString(SIMDBase_PARAMID_MODE_NAME, mode));
	}

	int32_t FFT_wrapper::readExecutionPlan(const std::string plan){
		int32_t errorCode;
		std::istringstream fileStream(plan);

		DFT * p0 = DFT_sread(fileStream, &errorCode);
		if (p0 && errorCode == DFT_ERROR_NOERROR){
			destroy();
			p = p0;
			sx = reinterpret_cast<REAL*>(SIMDBase_alignedMalloc(sizeOfVect*elements * 2));
		}
		return errorCode;
	}

	const std::string FFT_wrapper::writeExecutionPlan(){
		if (p){
			std::ostringstream fileStream;
			int32_t errorCode = DFT_swrite(p, fileStream);
			if (errorCode == 1){
				return fileStream.str();
			}
			else{
				return "";
			}
		}
		else{
			return "";
		}
	}

	void FFT_wrapper::execute(int dir){
		int i, j;

		for (j = 0; j < veclen; j++) {
			for (i = 0; i < elements; i++) {
				const size_t offsetR = (i * 2 + 0)*veclen + j;
				const size_t offsetI = (i * 2 + 1)*veclen + j;

				sx[offsetR] = inputMatrix[j][i].real();
				sx[offsetI] = inputMatrix[j][i].imag();
			}
		}

		DFT_execute(p, mode, sx, dir);

		for (j = 0; j < veclen; j++) {
			for (i = 0; i < elements; i++) {
				const size_t offsetR = (i * 2 + 0)*veclen + j;
				const size_t offsetI = (i * 2 + 1)*veclen + j;

				outputMatrix[j][i].real(sx[offsetR]);
				outputMatrix[j][i].imag(sx[offsetI]);
			}
		}
	}

	int FFT_wrapper::getModeParam(int32_t key) const{
		return SIMDBase_getModeParamInt(key, mode);
	}

	int FFT_wrapper::getPlanParam(int32_t key) const{
		if (p){
			return DFT_getPlanParamInt(key, p);
		}
		else{
			return -1;
		}
	}

	const std::string FFT_wrapper::getModeParamString(int32_t key) const{
		return std::string(SIMDBase_getModeParamString(key, mode));
	}
	

	

};


