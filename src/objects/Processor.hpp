#ifndef ZSOUNDPROC_OBJECTS_PROCESSOR_H
#define ZSOUNDPROC_OBJECTS_PROCESSOR_H

#include "common.hpp"
#include "objects/Node.hpp"

namespace zsoundproc {
	class Processor : public Object<Node> {
	public:
		explicit Processor(State * state) : Object<Node>(state){
			LUTOK_METHOD("getCallback", &Processor::getCallback);
			LUTOK_METHOD("setContext", &Processor::setContext);
			LUTOK_PROPERTY("endpointIn", &Processor::getEndpointIn, &Processor::setEndpointIn);
			LUTOK_PROPERTY("endpointOut", &Processor::getEndpointOut, &Processor::setEndpointOut);
		}

		Node * constructor(State & state, bool & managed);
		void destructor(State & state, Node * object);

		int getCallback(State & state, Node * object);
		int setContext(State & state, Node * object);

		int getEndpointIn(State & state, Node * object);
		int setEndpointIn(State & state, Node * object);
		int getEndpointOut(State & state, Node * object);
		int setEndpointOut(State & state, Node * object);
	};
};

#endif	
