#include "objects/InputNode.hpp"

#include <sstream>
#include <fstream>

namespace zsoundproc {
	DataSource::DataSource(){
		mode = DataSourceMode::string;
		position = 0;
		buffer = new stringbuf();
	}

	DataSource::DataSource(const string & fileName){
		mode = DataSourceMode::file;
		position = 0;
		buffer = new filebuf();
		static_cast<filebuf*>(buffer)->open(fileName, ios::binary | ios::in);
	}

	DataSource::~DataSource(){
		if (mode == DataSourceMode::file){
			static_cast<filebuf*>(buffer)->close();
		}
		delete buffer;
	}

	const std::string DataSource::pull(size_t length){
		size_t available = buffer->in_avail();
		if (available > 0){
			size_t bytesToRead = (length > available) ? available : length;

			std::string tmp = std::string(bytesToRead, 0);

			char * begin = &*tmp.begin();
			buffer->sgetn(begin, bytesToRead);

			position += bytesToRead;

			return std::string(tmp);
		}
		else{
			return "";
		}
	}

	void DataSource::load(const string & fileName){
		if (mode == DataSourceMode::file){
			static_cast<filebuf*>(buffer)->close();
			position = 0;
			static_cast<filebuf*>(buffer)->open(fileName, ios::binary | ios::in);
		}
		else{
			delete buffer;
			position = 0;
			buffer = new filebuf();
			static_cast<filebuf*>(buffer)->open(fileName, ios::binary | ios::in);
		}
	}

	void DataSource::push(const string & data){
		if (mode == DataSourceMode::string){
			buffer->sputn(data.c_str(), data.length());
		}
	}

	long DataSource::getPosition() const {
		return position;
	};

	InputNode::InputNode(){

	}

	InputNode::~InputNode(){

	}

	bool InputNode::load(const string & fileName){
		return false;
	}
};
