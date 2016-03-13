#ifndef ZSOUNDPROC_OBJECTS_PROCESSOR_H
#define ZSOUNDPROC_OBJECTS_PROCESSOR_H

#include "common.hpp"
#include <string>
#include <vector>

namespace zsoundproc {
	typedef union zmq_msg_t { unsigned char _[64]; void *p; } zmq_msg_t;
	typedef std::vector<uint8_t> SoundBuffer;
	typedef std::vector<SoundBuffer> SoundBuffers;

	class ZSProcessor {
	private:
		//buffer data
		size_t bufferSize;
		size_t bufferCount;
		SoundBuffers buffers;
		size_t currentBufferNum;

		//zmq
		void * ctx;
		void * socketIn;
		void * socketOut;
		zmq_msg_t msgIn;
		zmq_msg_t msgOut;

		bool bothDirections;
		bool contextOwner;
	public:
		std::string endpointIn;
		std::string endpointOut;
	public:
		ZSProcessor();
		ZSProcessor(size_t bufferSize, size_t bufferCount);
		ZSProcessor(size_t bufferSize, size_t bufferCount, bool bothDirections);
		~ZSProcessor();

		void init();
		int connect();
		void disconnect();
		inline int reconnect();
		void setContext(void * newCtx);
		void setNewContext();
		void destroyContext();

		SoundBuffer & getNextBuffer();
		inline SoundBuffer & getCurrentBuffer();

		size_t recv();
		size_t send(uint8_t * buffer, size_t len);

		static void callbackFunction(void * userdata, uint8_t * stream, int len);
	};

	class Processor : public Object<ZSProcessor> {
	public:
		explicit Processor(State * state) : Object<ZSProcessor>(state){
			LUTOK_METHOD("getCallback", &Processor::getCallback);
			LUTOK_METHOD("setContext", &Processor::setContext);
			LUTOK_PROPERTY("endpointIn", &Processor::getEndpointIn, &Processor::setEndpointIn);
			LUTOK_PROPERTY("endpointOut", &Processor::getEndpointOut, &Processor::setEndpointOut);
		}

		ZSProcessor * constructor(State & state, bool & managed);
		void destructor(State & state, ZSProcessor * object);

		int getCallback(State & state, ZSProcessor * object);
		int setContext(State & state, ZSProcessor * object);

		int getEndpointIn(State & state, ZSProcessor * object);
		int setEndpointIn(State & state, ZSProcessor * object);
		int getEndpointOut(State & state, ZSProcessor * object);
		int setEndpointOut(State & state, ZSProcessor * object);
	};
};

#endif	
