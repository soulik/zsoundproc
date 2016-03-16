#ifndef ZSOUNDPROC_OBJECTS_NODE_H
#define ZSOUNDPROC_OBJECTS_NODE_H

#include <string>
#include <vector>

namespace zsoundproc {

	typedef union zmq_msg_t { unsigned char _[64]; void *p; } zmq_msg_t;
	typedef std::vector<uint8_t> SoundBuffer;
	typedef std::vector<SoundBuffer> SoundBuffers;

	class Node {
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
		Node();
		Node(void * ctx);
		Node(size_t bufferSize, size_t bufferCount);
		Node(size_t bufferSize, size_t bufferCount, bool bothDirections);
		Node(void * ctx, size_t bufferSize, size_t bufferCount);
		Node(void * ctx, size_t bufferSize, size_t bufferCount, bool bothDirections);
		~Node();

		void init(void * ctx = nullptr);
		int connect();
		void disconnect();
		inline int reconnect() {
			disconnect();
			return connect();
		};
		void setContext(void * newCtx);
		void setNewContext();
		void destroyContext();

		SoundBuffer & getNextBuffer();
		inline SoundBuffer & getCurrentBuffer();

		size_t recv();
		size_t send(uint8_t * buffer, size_t len);

		static void callbackFunction(void * userdata, uint8_t * stream, int len);
	};

};

#endif