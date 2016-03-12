#include "objects/Processor.hpp"
#include "zmq.h"

namespace zsoundproc {
	ZSProcessor::ZSProcessor() : bufferSize(512), bufferCount(8){
		init();
	}

	ZSProcessor::ZSProcessor(size_t bufferSize, size_t bufferCount){
		this->bufferSize = bufferSize;
		this->bufferCount = bufferCount;
		init();
	}

	ZSProcessor::~ZSProcessor(){
		if (ctx){
			disconnect();
			zmq_ctx_shutdown(ctx);
			zmq_ctx_term(ctx);
		}
	}

	void ZSProcessor::init(){
		buffers = SoundBuffers(bufferCount, SoundBuffer(bufferSize, 0));
		currentBufferNum = 0;

		ctx = zmq_ctx_new();
		endpointIn = "inproc://zsoundprocIn";
		endpointOut = "inproc://zsoundprocOut";
	}

	uint8_t * ZSProcessor::recv(size_t & len, size_t maxLen){
		if (socketIn){
			
			::zmq_msg_t * msg = reinterpret_cast<::zmq_msg_t*> (&msgIn);
			int rc = zmq_msg_init(msg);
			rc = zmq_msg_recv(msg, socketIn, 0);

			if (rc != -1){
				size_t inSize = zmq_msg_size(msg);
				if ((maxLen > 0) && (inSize > maxLen)){
					inSize = maxLen;
				}

				uint8_t * data = new uint8_t[inSize];
				memcpy(data, zmq_msg_data(msg), inSize);

				zmq_msg_close(msg);
				return data;
			}
			else{
				zmq_msg_close(msg);
				return nullptr;
			}
		}
		return nullptr;
	}

	size_t ZSProcessor::send(uint8_t * buffer, size_t len){
		if (socketOut){
			::zmq_msg_t * msg = reinterpret_cast<::zmq_msg_t*> (&msgOut);

			int rc = zmq_msg_init_size(msg, len);
			memcpy(zmq_msg_data(msg), buffer, len);
			rc = zmq_msg_send(msg, socket, 0);
			if (rc > 0){
				zmq_msg_close(msg);
				return rc;
			}else {
				zmq_msg_close(msg);
				return 0;
			}
		}
		return 0;
	}

	int ZSProcessor::connect(){
		socketIn = zmq_socket(ctx, ZMQ_PULL);
		if (socketIn){
			if (!zmq_bind(socketIn, endpointIn.c_str())){
				zmq_close(socketIn);
				socketIn= nullptr;
				return zmq_errno();
			}
		}else{
			return zmq_errno();
		}

		socketOut = zmq_socket(ctx, ZMQ_PUSH);
		if (socketOut){
			if (!zmq_bind(socketOut, endpointOut.c_str())){
				zmq_close(socketOut);
				socketOut = nullptr;
				return zmq_errno();
			}
		}else{
			return zmq_errno();
		}

		return 0;
	}

	void ZSProcessor::disconnect(){
		if (socketIn){
			zmq_close(socketIn);
			socketIn = nullptr;
		}
		if (socketOut){
			zmq_close(socketOut);
			socketOut = nullptr;
		}
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
			
			processor->send(stream, len);

			SoundBuffer & buffer = processor->getCurrentBuffer();
			memcpy(stream, buffer.data(), len);
		}
	}

	int Processor::getCallback(State & state, ZSProcessor * object){
		Stack * stack = state.stack;
		stack->push<void*>(object->callbackFunction);
		stack->push<void*>(object);
		return 2;
	}

	ZSProcessor * Processor::constructor(State & state, bool & managed){
		ZSProcessor * object = new ZSProcessor();
		return object;
	}

	void Processor::destructor(State & state, ZSProcessor * object){
		delete object;
	}

	void initProcessor(State * state, Module & module){
		INIT_OBJECT(Processor);
	}
};
