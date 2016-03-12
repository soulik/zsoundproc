#ifndef ZSOUNDPROC_OBJECTS_PROCESSOR_H
#define ZSOUNDPROC_OBJECTS_PROCESSOR_H

#include "common.hpp"

namespace zsoundproc {
	class ZSProcessor {

	};

	class Processor : public Object<ZSProcessor> {
	public:
		explicit Processor(State * state) : Object<ZSProcessor>(state){
		}

		ZSProcessor * constructor(State & state, bool & managed);

		void destructor(State & state, ZSProcessor * object);
	};
};

#endif	
