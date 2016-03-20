#ifndef ZSOUNDPROC_OBJECTS_INPUTNODE_H
#define ZSOUNDPROC_OBJECTS_INPUTNODE_H

#include <string>
#include <streambuf>

using namespace std;

namespace zsoundproc {
	namespace DataSourceMode {
		enum Mode {
			string = 0,
			file = 1
		};
	};

	class DataSource {
	private:
		std::streambuf * buffer;
		long position;
		DataSourceMode::Mode mode;
	public:
		DataSource();
		DataSource(const string & fileName);
		~DataSource();

		const string pull(size_t length);
		void load(const string & fileName);
		void push(const string & data);
		long getPosition() const;
	};

	class InputNode {
	public:
		InputNode();
		~InputNode();

		bool load(const string & fileName);
	};
};

#endif