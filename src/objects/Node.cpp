#include "zmq.h"
#include "objects/Node.hpp"

namespace zsoundproc {
	Node::Node() : bufferSize(4096), bufferCount(8){
		this->bothDirections = false;
		init();
	}

	Node::Node(void * ctx) : bufferSize(4096), bufferCount(8){
		this->bothDirections = false;
		init(ctx);
	}

	Node::Node(size_t bufferSize, size_t bufferCount){
		this->bufferSize = bufferSize;
		this->bufferCount = bufferCount;
		this->bothDirections = false;
		init();
	}

	Node::Node(size_t bufferSize, size_t bufferCount, bool bothDirections){
		this->bufferSize = bufferSize;
		this->bufferCount = bufferCount;
		this->bothDirections = bothDirections;
		init();
	}

	Node::Node(void * ctx, size_t bufferSize, size_t bufferCount){
		this->bufferSize = bufferSize;
		this->bufferCount = bufferCount;
		this->bothDirections = false;
		init(ctx);
	}

	Node::Node(void * ctx, size_t bufferSize, size_t bufferCount, bool bothDirections){
		this->bufferSize = bufferSize;
		this->bufferCount = bufferCount;
		this->bothDirections = bothDirections;
		init(ctx);
	}

	Node::~Node(){
		destroyContext();
	}

	void Node::init(void * ctx){
		buffers = SoundBuffers(bufferCount, SoundBuffer(bufferSize, 0));
		currentBufferNum = 0;

		endpointIn = "inproc://zsoundprocIn";
		endpointOut = "inproc://zsoundprocOut";
		socketIn = nullptr;
		socketOut = nullptr;

		if (ctx){
			setContext(ctx);
		}
		else{
			setNewContext();
		}
	}

	void Node::destroyContext(){
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

	void Node::setContext(void * newCtx){
		if (newCtx){
			destroyContext();
			contextOwner = false;
			ctx = newCtx;
			connect();
		}
	}

	void Node::setNewContext(){
		contextOwner = true;
		ctx = zmq_ctx_new();
		connect();
	}

	size_t Node::recv(){
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

	size_t Node::send(uint8_t * buffer, size_t len){
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

	int Node::connect(){
		int linger = 5000;
		socketIn = zmq_socket(ctx, ZMQ_PULL);
		if (socketIn){
			zmq_setsockopt(socketIn, ZMQ_LINGER, &linger, sizeof(int));
			if (zmq_bind(socketIn, endpointIn.c_str()) != 0){
				zmq_close(socketIn);
				socketIn = nullptr;
				return zmq_errno();
			}
		}
		else{
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

	void Node::disconnect(){
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

	SoundBuffer & Node::getNextBuffer(){
		SoundBuffer & currentBuffer = buffers[currentBufferNum];
		currentBufferNum = (currentBufferNum + 1) % bufferCount;
		return currentBuffer;
	}

	SoundBuffer & Node::getCurrentBuffer(){
		return buffers[currentBufferNum];
	}

	void Node::callbackFunction(void * userdata, uint8_t * stream, int len){
		if (userdata && stream && len > 0){
			Node * processor = reinterpret_cast<Node *>(userdata);

			//processor->send(stream, len);
			processor->recv();
			SoundBuffer & buffer = processor->getCurrentBuffer();
			size_t bufferSize = buffer.size();
			memcpy(stream, buffer.data(), (bufferSize < len) ? bufferSize : len);
		}
	}
}