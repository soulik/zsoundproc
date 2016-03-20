#ifndef ZSOUNDPROC_OBJECTS_FFT_H
#define ZSOUNDPROC_OBJECTS_FFT_H

#include "common.hpp"
#include "objects/FFTCore.hpp"

namespace zsoundproc {
	class FFT : public Object<FFTCore> {
	public:
		explicit FFT(State * state) : Object<FFTCore>(state){
			LUTOK_PROPERTY("executionPlan", &FFT::getExecutionPlan, &FFT::setExecutionPlan);
			LUTOK_PROPERTY("input", &FFT::getInput, &FFT::setInput);
			LUTOK_PROPERTY("output", &FFT::getOutput, &FFT::setOutput);

			LUTOK_METHOD("execute", &FFT::execute);

			LUTOK_PROPERTY("dataType", &FFT::getDataType, &FFT::nullMethod);
			LUTOK_PROPERTY("mode", &FFT::getMode, &FFT::nullMethod);
			LUTOK_PROPERTY("FFTLength", &FFT::getFFTLength, &FFT::nullMethod);
			LUTOK_PROPERTY("isRealTransform", &FFT::getIsRealTransform, &FFT::nullMethod);
			LUTOK_PROPERTY("noBitTranversal", &FFT::getNoBitTraversal, &FFT::nullMethod);
			LUTOK_PROPERTY("testRun", &FFT::getTestRun, &FFT::nullMethod);

			LUTOK_PROPERTY("modeMax", &FFT::getModeMax, &FFT::nullMethod);
			LUTOK_PROPERTY("typeAvailability", &FFT::getTypeAvailability, &FFT::nullMethod);
			LUTOK_PROPERTY("sizeOfReal", &FFT::getSizeOfReal, &FFT::nullMethod);
			LUTOK_PROPERTY("sizeOfVect", &FFT::getSizeOfVect, &FFT::nullMethod);
			LUTOK_PROPERTY("vectorLen", &FFT::getVectorLen, &FFT::nullMethod);
			LUTOK_PROPERTY("modeAvailability", &FFT::getModeAvailability, &FFT::nullMethod);
			LUTOK_PROPERTY("modeName", &FFT::getModeName, &FFT::nullMethod);

		}

		FFTCore * constructor(State & state, bool & managed);
		void destructor(State & state, FFTCore * object);

		int getExecutionPlan(State & state, FFTCore * object);
		int setExecutionPlan(State & state, FFTCore * object);

		int getInput(State & state, FFTCore * object);
		int setInput(State & state, FFTCore * object);

		int getOutput(State & state, FFTCore * object);
		int setOutput(State & state, FFTCore * object);

		int execute(State & state, FFTCore * object);

		int getDataType(State & state, FFTCore * object);
		int getMode(State & state, FFTCore * object);
		int getFFTLength(State & state, FFTCore * object);
		int getIsRealTransform(State & state, FFTCore * object);
		int getNoBitTraversal(State & state, FFTCore * object);
		int getTestRun(State & state, FFTCore * object);

		int getModeMax(State & state, FFTCore * object);
		int getTypeAvailability(State & state, FFTCore * object);
		int getSizeOfReal(State & state, FFTCore * object);
		int getSizeOfVect(State & state, FFTCore * object);
		int getVectorLen(State & state, FFTCore * object);
		int getModeAvailability(State & state, FFTCore * object);
		int getModeName(State & state, FFTCore * object);
	};
};

#endif	
