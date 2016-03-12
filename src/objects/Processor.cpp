#include "objects/Processor.hpp"

namespace zsoundproc {
	ZSProcessor * Processor::constructor(State & state, bool & managed){
		return nullptr;
	}

	void Processor::destructor(State & state, ZSProcessor * object){
		delete object;
	}

	void initProcessor(State * state, Module & module){
		INIT_OBJECT(Processor);
	}
};
