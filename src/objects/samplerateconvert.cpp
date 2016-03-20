#include "objects/samplerateconvert.hpp"

namespace zsoundproc {

	SampleRateConvert::SampleRateConvert(){
		smpState = nullptr;
		error = 0;
		data.data_in = nullptr;
		data.data_out = nullptr;
		data.end_of_input = 1;
		data.input_frames = 0;
		data.output_frames = 0;
		data.input_frames_used = 0;
		data.output_frames_gen = 0;
		data.src_ratio = 1.0f;
	}

	SampleRateConvert::SampleRateConvert(int type, int channels) : SampleRateConvert(){
		smpState = src_new(type, channels, &error);
	}

	SampleRateConvert::~SampleRateConvert(){
		if (smpState){
			src_delete(smpState);
		}
		if (data.data_in){
			delete data.data_in;
		}
		if (data.data_out){
			delete data.data_out;
		}
	}

	void SampleRateConvert::setInput(const float  * in, size_t length){
		if (length != data.input_frames){
			if (data.data_in) delete data.data_in;
			if (data.data_out) delete data.data_out;
			const size_t outputLength = length;

			data.data_in = new float[length];
			data.data_out = new float[outputLength];
			data.input_frames = length;
			data.output_frames = outputLength;
		}

		data.input_frames_used = 0;
		data.output_frames_gen = 0;
		data.end_of_input = 0;
		memcpy(data.data_in, in, sizeof(float)*length);
	}

	const float * SampleRateConvert::getOutput(size_t & length) const{
		length = data.output_frames;
		return data.data_out;
	}

	const float * SampleRateConvert::getInput(size_t & length) const{
		length = data.input_frames;
		return data.data_in;
	}

	bool SampleRateConvert::getIsEndOfInput() const{
		return (data.end_of_input == 1);
	}

	void SampleRateConvert::setIsEndOfInput(bool state){
		data.end_of_input = (state) ? 1 : 0;
	}

	size_t SampleRateConvert::getInputFramesUsed() const{
		return data.input_frames_used;
	}

	size_t SampleRateConvert::getOutputFramesGenerated() const{
		return data.output_frames_gen;
	}

	int SampleRateConvert::process(){
		if (data.data_in && data.data_out){
			return src_process(smpState, &data);
		}
		else {
			return -1;
		}
	}

	int SampleRateConvert::reset(){
		return src_reset(smpState);
	}

	const double SampleRateConvert::getRatio() const{
		return data.src_ratio;
	}

	int SampleRateConvert::setRatio(double newRatio){
		data.src_ratio = newRatio;
		return src_set_ratio(smpState, newRatio);
	}

	const std::string SampleRateConvert::getErrorStr() const{
		return std::string(src_strerror(error));
	}

	void SampleRateConvert::s16tof(const int16_t * in, float * out, size_t len){
		src_short_to_float_array(in, out, len);
	}
	void SampleRateConvert::ftos16(const float * in, int16_t * out, size_t len){
		src_float_to_short_array(in, out, len);
	}
	void SampleRateConvert::s32tof(const int32_t * in, float * out, size_t len){
		src_int_to_float_array(in, out, len);
	}
	void SampleRateConvert::ftos32(const float * in, int32_t * out, size_t len){
		src_float_to_int_array(in, out, len);
	}
}