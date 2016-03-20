#include "objects/decoders/OGGVorbis.hpp"

namespace zsoundproc {
	namespace decoders {
		OGGVorbis::OGGVorbis(){
			callbacks.read_func = read;
			callbacks.seek_func = seek;
			callbacks.close_func = close;
			callbacks.tell_func = tell;
		}

		OGGVorbis::~OGGVorbis(){
		}

		bool OGGVorbis::load(const std::string & fileName){
			int rc = 0;
			OggVorbis_File vf;
			DataSource src;
			rc = ov_open_callbacks(&src, &vf, nullptr, 0, callbacks);
			if (rc == 0){

				vorbis_info * info = ov_info(&vf, -1);

				ov_clear(&vf);
				return true;
			}
			return false;
		}

		size_t OGGVorbis::read(void *ptr, size_t size, size_t nmemb, void *datasource){
			DataSource * src = reinterpret_cast<DataSource*>(datasource);

			const std::string data = src->pull(size);
			const size_t count = data.length();

			memcpy(ptr, data.c_str(), count);
			return count;
		}
		
		int OGGVorbis::seek(void *datasource, ogg_int64_t offset, int whence){
			DataSource * src = reinterpret_cast<DataSource*>(datasource);
			return 0;
		}
		
		int OGGVorbis::close(void *datasource){
			DataSource * src = reinterpret_cast<DataSource*>(datasource);
			return 0;
		}

		long OGGVorbis::tell(void *datasource){
			DataSource * src = reinterpret_cast<DataSource*>(datasource);
			return src->getPosition();
		}
	};
};