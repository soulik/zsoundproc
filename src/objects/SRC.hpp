#ifndef ZSOUNDPROC_OBJECTS_SRC_H
#define ZSOUNDPROC_OBJECTS_SRC_H

#include "common.hpp"
#include "objects/samplerateconvert.hpp"

namespace zsoundproc {
	class SRC : public Object<SampleRateConvert> {
	public:
		explicit SRC(State * state) : Object<SampleRateConvert>(state){
			LUTOK_METHOD("process", &SRC::process);
			LUTOK_METHOD("reset", &SRC::reset);
			LUTOK_PROPERTY("ratio", &SRC::getRatio, &SRC::setRatio);
			LUTOK_PROPERTY("error", &SRC::getErrorStr, &SRC::nullMethod);

			LUTOK_PROPERTY("input", &SRC::getInput, &SRC::setInput);
			LUTOK_PROPERTY("output", &SRC::getOutput, &SRC::setOutput);
			
			LUTOK_PROPERTY("endOfInput", &SRC::getEndOfInput, &SRC::setEndOfInput);
			LUTOK_PROPERTY("inputFramesUsed", &SRC::getInputFramesUsed, &SRC::nullMethod);
			LUTOK_PROPERTY("outputFramesGenerated", &SRC::getOutputFramesGenerated, &SRC::nullMethod);
		}

		SampleRateConvert * constructor(State & state, bool & managed);

		void destructor(State & state, SampleRateConvert * object);

		int process(State & state, SampleRateConvert * object);
		int reset(State & state, SampleRateConvert * object);
		int getRatio(State & state, SampleRateConvert * object);
		int setRatio(State & state, SampleRateConvert * object);
		int getErrorStr(State & state, SampleRateConvert * object);

		int getInput(State & state, SampleRateConvert * object);
		int setInput(State & state, SampleRateConvert * object);
		int getOutput(State & state, SampleRateConvert * object);
		int setOutput(State & state, SampleRateConvert * object);
		int getEndOfInput(State & state, SampleRateConvert * object);
		int setEndOfInput(State & state, SampleRateConvert * object);
		int getInputFramesUsed(State & state, SampleRateConvert * object);
		int getOutputFramesGenerated(State & state, SampleRateConvert * object);

		static int s16tof(State & state);
		static int ftos16(State & state);
		static int s32tof(State & state);
		static int ftos32(State & state);
	};
};

#endif	
