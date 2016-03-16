#ifndef ZSOUNDPROC_OBJECTS_FFT_H
#define ZSOUNDPROC_OBJECTS_FFT_H

#include "common.hpp"
#include "objects/FFT_wrapper.hpp"

namespace zsoundproc {
	class FFT : public Object<FFT_wrapper> {
	public:
		explicit FFT(State * state) : Object<FFT_wrapper>(state){
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

		FFT_wrapper * constructor(State & state, bool & managed);
		void destructor(State & state, FFT_wrapper * object);

		int getExecutionPlan(State & state, FFT_wrapper * object);
		int setExecutionPlan(State & state, FFT_wrapper * object);

		int getInput(State & state, FFT_wrapper * object);
		int setInput(State & state, FFT_wrapper * object);

		int getOutput(State & state, FFT_wrapper * object);
		int setOutput(State & state, FFT_wrapper * object);

		int execute(State & state, FFT_wrapper * object);

		int getDataType(State & state, FFT_wrapper * object);
		int getMode(State & state, FFT_wrapper * object);
		int getFFTLength(State & state, FFT_wrapper * object);
		int getIsRealTransform(State & state, FFT_wrapper * object);
		int getNoBitTraversal(State & state, FFT_wrapper * object);
		int getTestRun(State & state, FFT_wrapper * object);

		int getModeMax(State & state, FFT_wrapper * object);
		int getTypeAvailability(State & state, FFT_wrapper * object);
		int getSizeOfReal(State & state, FFT_wrapper * object);
		int getSizeOfVect(State & state, FFT_wrapper * object);
		int getVectorLen(State & state, FFT_wrapper * object);
		int getModeAvailability(State & state, FFT_wrapper * object);
		int getModeName(State & state, FFT_wrapper * object);
	};
};

#endif	
