#include "stdafx.h"

namespace SharedMemory {

namespace {

class SharedMemoryImpl : boost::noncopyable {
public:
	SharedMemoryImpl(const char *shared_name, unsigned int size) : memory(boost::interprocess::open_or_create, shared_name, size) {
	}
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> Lock(const char *key) {
		boost::interprocess::interprocess_mutex *mtx = memory.find_or_construct<boost::interprocess::interprocess_mutex>(key)();
		return boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>(*mtx);
	}
	char *GetChar(const char *key, unsigned int size) {
		return memory.find_or_construct<char>(key)[size]();
	}
	char *GetPathChar(const char *key) {
		return GetChar(key, MAX_PATH);
	}
protected:
	boost::interprocess::managed_shared_memory memory;
};

const char * const shared_memory_name = "org.click3.TouhouHitChecker";
const unsigned int shared_memory_size = 65535;
const char * const mutex_name = "org.click3.TouhouHitChecker.Mutex";
const char * const capture_out_path_name = "org.click3.TouhouHitChecker.CaptureOutPath";
const char * const hit_data_list_out_path_name = "org.click3.TouhouHitChecker.HitDataListOutPath";

boost::shared_ptr<SharedMemoryImpl> impl(new SharedMemoryImpl(shared_memory_name, shared_memory_size));

} // anonymous

boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> Lock(void) {
	return impl->Lock(mutex_name);
}

std::string GetCaptureOutPath(void) {
	return impl->GetPathChar(capture_out_path_name);
}

bool SetCaptureOutPath(const boost::filesystem::path &path) {
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lk = Lock();
	char *buf = impl->GetPathChar(capture_out_path_name);
	if(0 != ::strcpy_s(buf, MAX_PATH, boost::filesystem::absolute(path).string().c_str())) {
		return false;
	}
	return true;
}

std::string GetHitDataListOutPath(void) {
	return impl->GetPathChar(hit_data_list_out_path_name);
}

bool SetHitDataListOutPath(const boost::filesystem::path &path) {
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lk = Lock();
	char *buf = impl->GetPathChar(hit_data_list_out_path_name);
	if(0 != ::strcpy_s(buf, MAX_PATH, boost::filesystem::absolute(path).string().c_str())) {
		return false;
	}
	return true;
}

} // SharedMemory
