#include "objects/Processor.hpp"
#include "zmq.h"

namespace zsoundproc {
	ZSProcessor::ZSProcessor() : bufferSize(4096), bufferCount(8){
		this->bothDirections = false;
		init();
	}

	ZSProcessor::ZSProcessor(size_t bufferSize, size_t bufferCount){
		this->bufferSize = bufferSize;
		this->bufferCount = bufferCount;
		this->bothDirections = false;
		init();
	}

	ZSProcessor::ZSProcessor(size_t bufferSize, size_t bufferCount, bool bothDirections){
		this->bufferSize = bufferSize;
		this->bufferCount = bufferCount;
		this->bothDirections = bothDirections;
		init();
	}

	ZSProcessor::~ZSProcessor(){
		destroyContext();
	}

	void ZSProcessor::init(){
		buffers = SoundBuffers(bufferCount, SoundBuffer(bufferSize, 0));
		currentBufferNum = 0;

		endpointIn = "inproc://zsoundprocIn";
		endpointOut = "inproc://zsoundprocOut";
		socketIn = nullptr;
		socketOut = nullptr;

		setNewContext();
	}

	void ZSProcessor::destroyContext(){
		if (ctx){
			disconnect();
			if (contextOwner){
				zmq_ctx_shutdown(ctx);
				zmq_ctx_term(ctx);
				ctx = nullptr;
				contextOwner = false;
			}
		}
	}

	void ZSProcessor::setContext(void * newCtx){
		if (newCtx){
			destroyContext();
			contextOwner = false;
			ctx = newCtx;
			connect();
		}
	}

	void ZSProcessor::setNewContext(){
		contextOwner = true;
		ctx = zmq_ctx_new();
		connect();
	}

	size_t ZSProcessor::recv(){
		if (socketIn){
			
			::zmq_msg_t * msg = reinterpret_cast<::zmq_msg_t*> (&msgIn);
			int rc = zmq_msg_init(msg);
			rc = zmq_msg_recv(msg, socketIn, ZMQ_DONTWAIT);

			if (rc != -1){
				SoundBuffer & currentBuffer = this->getCurrentBuffer();
				size_t inSize = zmq_msg_size(msg);
				if ((currentBuffer.size() > 0) && (inSize > currentBuffer.size())){
					inSize = currentBuffer.size();
				}
				memcpy(currentBuffer.data(), zmq_msg_data(msg), inSize);

				zmq_msg_close(msg);
				return inSize;
			}
			else{
				SoundBuffer & currentBuffer = this->getCurrentBuffer();
				memset(currentBuffer.data(), 0, currentBuffer.size());
				zmq_msg_close(msg);
				return 0;
			}
		}
		return 0;
	}

	size_t ZSProcessor::send(uint8_t * buffer, size_t len){
		if (bothDirections){
			if (socketOut){
				::zmq_msg_t * msg = reinterpret_cast<::zmq_msg_t*> (&msgOut);

				int rc = zmq_msg_init_size(msg, len);
				memcpy(zmq_msg_data(msg), buffer, len);
				rc = zmq_msg_send(msg, socket, 0);
				if (rc > 0){
					zmq_msg_close(msg);
					return rc;
				}
				else {
					zmq_msg_close(msg);
					return 0;
				}
			}
		}
		return 0;
	}

	int ZSProcessor::connect(){
		int linger = 5000;
		socketIn = zmq_socket(ctx, ZMQ_PULL);
		if (socketIn){
			zmq_setsockopt(socketIn, ZMQ_LINGER, &linger, sizeof(int));
			if (zmq_bind(socketIn, endpointIn.c_str()) != 0){
				zmq_close(socketIn);
				socketIn= nullptr;
				return zmq_errno();
			}
		}else{
			return zmq_errno();
		}

		if (bothDirections){
			socketOut = zmq_socket(ctx, ZMQ_PUSH);
			if (socketOut){
				zmq_setsockopt(socketOut, ZMQ_LINGER, &linger, sizeof(int));
				if (zmq_bind(socketOut, endpointOut.c_str()) != 0){
					zmq_close(socketOut);
					socketOut = nullptr;
					return zmq_errno();
				}
			}
			else{
				return zmq_errno();
			}
		}
		return 0;
	}

	void ZSProcessor::disconnect(){
		if (socketIn){
			zmq_close(socketIn);
			socketIn = nullptr;
		}

		if (bothDirections){
			if (socketOut){
				zmq_close(socketOut);
				socketOut = nullptr;
			}
		}
	}

	int ZSProcessor::reconnect(){
		disconnect();
		return connect();
	}

	SoundBuffer & ZSProcessor::getNextBuffer(){
		SoundBuffer & currentBuffer = buffers[currentBufferNum];
		currentBufferNum = (currentBufferNum + 1) % bufferCount;
		return currentBuffer;
	}

	SoundBuffer & ZSProcessor::getCurrentBuffer(){
		return buffers[currentBufferNum];
	}

	void ZSProcessor::callbackFunction(void * userdata, uint8_t * stream, int len){
		if (userdata && stream && len>0){
			ZSProcessor * processor = reinterpret_cast<ZSProcessor *>(userdata);
			
			//processor->send(stream, len);
			processor->recv();
			SoundBuffer & buffer = processor->getCurrentBuffer();
			size_t bufferSize = buffer.size();
			memcpy(stream, buffer.data(), (bufferSize < len) ? bufferSize : len);
		}
	}

	int Processor::getCallback(State & state, ZSProcessor * object){
		Stack * stack = state.stack;
		stack->push<void*>(object->callbackFunction);
		stack->push<void*>(object);
		return 2;
	}

	int Processor::getEndpointIn(State & state, ZSProcessor * object){
		Stack * stack = state.stack;
		stack->push<const std::string &>(object->endpointIn);
		return 1;
	}

	int Processor::setEndpointIn(State & state, ZSProcessor * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			object->endpointIn = stack->to<const std::string>(1);
			object->reconnect();
		}
		return 0;
	}

	int Processor::getEndpointOut(State & state, ZSProcessor * object){
		Stack * stack = state.stack;
		stack->push<const std::string &>(object->endpointOut);
		return 1;
	}

	int Processor::setEndpointOut(State & state, ZSProcessor * object){
		Stack * stack = state.stack;
		if (stack->is<LUA_TSTRING>(1)){
			object->endpointOut = stack->to<const std::string>(1);
			object->reconnect();
		}
		return 0;
	}

	int Processor::setContext(State & state, ZSProcessor * object){
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

	ZSProcessor * Processor::constructor(State & state, bool & managed){
		Stack * stack = state.stack;
		ZSProcessor * object = nullptr;
		if (stack->is<LUA_TNUMBER>(1) && stack->is<LUA_TNUMBER>(2)){
			if (stack->is<LUA_TBOOLEAN>(3)){
				object = new ZSProcessor(stack->to<int>(1), stack->to<int>(2), stack->to<bool>(3));
			}
			else{
				object = new ZSProcessor(stack->to<int>(1), stack->to<int>(2));
			}
		}
		else{
			object = new ZSProcessor();
		}
		return object;
	}

	void Processor::destructor(State & state, ZSProcessor * object){
		delete object;
	}

	void initProcessor(State * state, Module & module){
		INIT_OBJECT(Processor);
	}
};
