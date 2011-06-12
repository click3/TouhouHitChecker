
namespace SharedMemory {
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> Lock(void);
	std::string GetCaptureOutPath(void);
	bool SetCaptureOutPath(const boost::filesystem::path &path);
	std::string GetHitDataListOutPath(void);
	bool SetHitDataListOutPath(const boost::filesystem::path &path);
} // SharedMemory
