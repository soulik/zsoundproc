#ifndef ZSOUNDPROC_OBJECTS_DECODERS_OGGVORBIS_H
#define ZSOUNDPROC_OBJECTS_DECODERS_OGGVORBIS_H

#include "vorbis/vorbisfile.h"
#include "ogg/os_types.h"

#include "objects/InputNode.hpp"
#include <string>

namespace zsoundproc {
	namespace decoders {
		class OGGVorbis: public InputNode {
		private:
			ov_callbacks callbacks;

			static size_t read(void *ptr, size_t size, size_t nmemb, void *datasource);
			static int seek(void *datasource, ogg_int64_t offset, int whence);
			static int close(void *datasource);
			static long tell(void *datasource);
		public:
			OGGVorbis();
			~OGGVorbis();

			bool load(const std::string & fileName);
		};
	};
};

#endif