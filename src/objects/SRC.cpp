#include "objects/SRC.hpp"

namespace zsoundproc {
	SampleRateConvert * SRC::constructor(State & state, bool & managed){
		Stack * stack = state.stack;
		if (stack->is<LUA_TNUMBER>(1) && stack->is<LUA_TNUMBER>(2)){
			return new SampleRateConvert(stack->to<int>(1), stack->to<int>(2));
		}
		return nullptr;
	}

	void SRC::destructor(State & state, SampleRateConvert * object){
		delete object;
	}

	int SRC::process(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		stack->push<int>(object->process());
		return 1;
	}

	int SRC::reset(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		stack->push<int>(object->reset());
		return 1;
	}

	int SRC::getRatio(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		stack->push<LUA_NUMBER>(object->getRatio());
		return 1;
	}

	int SRC::setRatio(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TNUMBER>(1)){
			object->setRatio(stack->to<LUA_NUMBER>(1));
		}
		return 0;
	}

	int SRC::getErrorStr(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		stack->push<const std::string &>(object->getErrorStr());
		return 1;
	}

	int SRC::getInput(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		size_t length=0;
		const char * buf = reinterpret_cast<const char*>(object->getInput(length));
		if (buf){
			stack->pushLString(std::string(buf, sizeof(float) * length));
		}
		else{
			stack->pushNil();
		}
		return 1;
	}

	int SRC::setInput(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			const std::string bufStr = stack->toLString(1);
			const float * buf = reinterpret_cast<const float*>(bufStr.c_str());
			size_t length = static_cast<size_t>(floor(bufStr.size() / sizeof(float)));
			object->setInput(buf, length);
		}
		return 0;
	}

	int SRC::getOutput(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		size_t length = 0;
		const char * buf = reinterpret_cast<const char*>(object->getOutput(length));
		if (buf){
			stack->pushLString(std::string(buf, sizeof(float) * length));
		}
		else{
			stack->pushNil();
		}
		return 1;
	}

	int SRC::setOutput(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		return 0;
	}

	int SRC::getEndOfInput(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		stack->push<bool>(object->getIsEndOfInput());
		return 1;
	}

	int SRC::setEndOfInput(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TBOOLEAN>(1)){
			object->setIsEndOfInput(stack->to<bool>(1));
		}
		return 0;
	}

	int SRC::getInputFramesUsed(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getInputFramesUsed());
		return 1;
	}

	int SRC::getOutputFramesGenerated(State & state, SampleRateConvert * object){
		Stack * stack = state.stack;
		stack->push<int>(object->getOutputFramesGenerated());
		return 1;
	}

	int SRC::s16tof(State & state){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			const std::string bufStr = stack->toLString(1);
			const int16_t * in = reinterpret_cast<const int16_t*>(bufStr.c_str());
			const size_t length = static_cast<size_t>(floor(bufStr.size() / sizeof(int16_t)));

			float * out = new float[length];
			SampleRateConvert::s16tof(in, out, length);
			stack->pushLString(std::string(reinterpret_cast<const char*>(out), sizeof(float) * length));
			delete out;

			return 1;
		}
		return 0;
	}

	int SRC::ftos16(State & state){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			const std::string bufStr = stack->toLString(1);
			const float * in = reinterpret_cast<const float*>(bufStr.c_str());
			const size_t length = static_cast<size_t>(floor(bufStr.size() / sizeof(float)));

			int16_t * out = new int16_t[length];
			SampleRateConvert::ftos16(in, out, length);
			stack->pushLString(std::string(reinterpret_cast<const char*>(out), sizeof(int16_t) * length));
			delete out;

			return 1;
		}
		return 0;
	}

	int SRC::s32tof(State & state){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			const std::string bufStr = stack->toLString(1);
			const int32_t * in = reinterpret_cast<const int32_t*>(bufStr.c_str());
			const size_t length = static_cast<size_t>(floor(bufStr.size() / sizeof(int32_t)));

			float * out = new float[length];
			SampleRateConvert::s32tof(in, out, length);
			stack->pushLString(std::string(reinterpret_cast<const char*>(out), sizeof(float) * length));
			delete out;

			return 1;
		}
		return 0;
	}

	int SRC::ftos32(State & state){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			const std::string bufStr = stack->toLString(1);
			const float * in = reinterpret_cast<const float*>(bufStr.c_str());
			const size_t length = static_cast<size_t>(floor(bufStr.size() / sizeof(float)));

			int32_t * out = new int32_t[length];
			SampleRateConvert::ftos32(in, out, length);
			stack->pushLString(std::string(reinterpret_cast<const char*>(out), sizeof(int32_t) * length));
			delete out;

			return 1;
		}
		return 0;
	}

	void initSRC(State * state, Module & module){
		INIT_OBJECT(SRC);
		module["s16tof"] = SRC::s16tof;
		module["ftos16"] = SRC::ftos16;
		module["s32tof"] = SRC::s32tof;
		module["ftos32"] = SRC::ftos32;
	}
};
