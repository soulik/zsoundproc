#include "objects/Processor.hpp"
#include "zmq.h"

namespace zsoundproc {
	int Processor::getCallback(State & state, Node * object){
		Stack * stack = state.stack;
		stack->push<void*>(object->callbackFunction);
		stack->push<void*>(object);
		return 2;
	}

	int Processor::getEndpointIn(State & state, Node * object){
		Stack * stack = state.stack;
		stack->push<const std::string &>(object->endpointIn);
		return 1;
	}

	int Processor::setEndpointIn(State & state, Node * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			object->endpointIn = stack->to<const std::string>(1);
			object->reconnect();
		}
		return 0;
	}

	int Processor::getEndpointOut(State & state, Node * object){
		Stack * stack = state.stack;
		stack->push<const std::string &>(object->endpointOut);
		return 1;
	}

	int Processor::setEndpointOut(State & state, Node * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			object->endpointOut = stack->to<const std::string>(1);
			object->reconnect();
		}
		return 0;
	}

	int Processor::setContext(State & state, Node * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TUSERDATA>(1)){
			object->setContext(*(static_cast<void**>(stack->to<void*>(1))));
		}
		else{
			object->disconnect();
			object->setNewContext();
		}
		return 0;
	}

	Node * Processor::constructor(State & state, bool & managed){
		Stack * stack = state.stack;
		Node * object = nullptr;
		if (stack->is<LUA_TNUMBER>(1) && stack->is<LUA_TNUMBER>(2)){
			if (stack->is<LUA_TBOOLEAN>(3)){
				object = new Node(stack->to<int>(1), stack->to<int>(2), stack->to<bool>(3));
			}
			else{
				object = new Node(stack->to<int>(1), stack->to<int>(2));
			}
		}
		else{
			object = new Node();
		}
		return object;
	}

	void Processor::destructor(State & state, Node * object){
		delete object;
	}

	void initProcessor(State * state, Module & module){
		INIT_OBJECT(Processor);
	}
};
