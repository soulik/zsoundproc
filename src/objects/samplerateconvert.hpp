#ifndef ZSOUNDPROC_OBJECTS_SAMPLERATECONVERT_H
#define ZSOUNDPROC_OBJECTS_SAMPLERATECONVERT_H

#include <string>
#include <stdint.h>
#include <complex>
#include <vector>

#include "samplerate.h"

namespace zsoundproc {
	class SampleRateConvert {
	private:
		SRC_STATE * smpState;
		SRC_DATA data;
	public:
		SampleRateConvert();
		SampleRateConvert(int type, int channels);
		~SampleRateConvert();

		void setInput(const float * in, size_t length);
		const float * getOutput(size_t & length) const;
		const float * getInput(size_t & length) const;
		bool getIsEndOfInput() const;
		void setIsEndOfInput(bool state);
		size_t getInputFramesUsed() const;
		size_t getOutputFramesGenerated() const;

		int process();
		int reset();

		const double getRatio() const;
		int setRatio(double newRatio);
		const std::string getErrorStr() const;

		static void s16tof(const int16_t * in, float * out, size_t len);
		static void ftos16(const float * in, int16_t * out, size_t len);
		static void s32tof(const int32_t * in, float * out, size_t len);
		static void ftos32(const float * in, int32_t * out, size_t len);

		int error;
	};
}

#endif