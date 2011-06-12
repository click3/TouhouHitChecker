#include "stdafx.h"

#pragma push_macro("MODULEENTRY32")
#undef MODULEENTRY32
#pragma push_macro("Module32First")
#undef Module32First
#pragma push_macro("Module32Next")
#undef Module32Next
#pragma push_macro("PROCESSENTRY32")
#undef PROCESSENTRY32
#pragma push_macro("Process32First")
#undef Process32First
#pragma push_macro("Process32Next")
#undef Process32Next

namespace {

template<class entry_class, unsigned int flag, BOOL (WINAPI *first_proc)(HANDLE, entry_class*), BOOL (WINAPI *next_proc)(HANDLE, entry_class*)>
bool CreateEntryList(std::vector<boost::shared_ptr<entry_class> > &entry_list, unsigned int process_id = 0) {
	entry_list.clear();
	HANDLE snapshot = ::CreateToolhelp32Snapshot(flag, process_id);
	if(snapshot == INVALID_HANDLE_VALUE) {
		return false;
	}

	while(true) {
		boost::shared_ptr<entry_class> entry(new entry_class());
		entry->dwSize = sizeof(entry_class);
		if(entry_list.size() == 0) {
			if(FALSE == first_proc(snapshot, entry.get())) {
				break;
			}
		} else {
			if(FALSE == next_proc(snapshot, entry.get())) {
				break;
			}
		}
		entry_list.push_back(entry);
	}
	::CloseHandle(snapshot);
	return true;
}

bool CreateModuleList(std::vector<boost::shared_ptr<MODULEENTRY32> > &entry_list, unsigned int process_id = ::GetCurrentProcessId()) {
	return CreateEntryList<MODULEENTRY32, TH32CS_SNAPMODULE, &::Module32First, &::Module32Next>(entry_list, process_id);
}
bool CreateModuleList(std::vector<boost::shared_ptr<MODULEENTRY32W> > &entry_list, unsigned int process_id = ::GetCurrentProcessId()) {
	return CreateEntryList<MODULEENTRY32W, TH32CS_SNAPMODULE, &::Module32FirstW, &::Module32NextW>(entry_list, process_id);
}
bool CreateProcessList(std::vector<boost::shared_ptr<PROCESSENTRY32> > &entry_list) {
	return CreateEntryList<PROCESSENTRY32, TH32CS_SNAPPROCESS, &::Process32First, &::Process32Next>(entry_list);
}
bool CreateProcessList(std::vector<boost::shared_ptr<PROCESSENTRY32W> > &entry_list) {
	return CreateEntryList<PROCESSENTRY32W, TH32CS_SNAPPROCESS, &::Process32FirstW, &::Process32NextW>(entry_list);
}

struct FindModulePath {
	FindModulePath(const std::wstring &module_path) : module_path(module_path) { }
	std::wstring module_path;
	bool operator()(boost::shared_ptr<MODULEENTRY32W> module) const {
		return module_path == module->szExePath;
	}
};

bool FindProcess(std::vector<boost::shared_ptr<PROCESSENTRY32W> > &result, const wchar_t *exe_name, const boost::filesystem::path &dll_abs_path) {
	std::vector<boost::shared_ptr<PROCESSENTRY32W> > process_list;
	if(!CreateProcessList(process_list)) {
		return false;
	}
	struct FindExeName {
		FindExeName(const std::wstring &exe_name, const std::wstring &module_path) : exe_name(exe_name), module_path(module_path) { }
		std::wstring exe_name;
		std::wstring module_path;
		std::vector<boost::shared_ptr<PROCESSENTRY32W> > &operator()(std::vector<boost::shared_ptr<PROCESSENTRY32W> > &list, boost::shared_ptr<PROCESSENTRY32W> value) const {
			if(exe_name == value->szExeFile) {
				while(true) {
					std::vector<boost::shared_ptr<MODULEENTRY32W> > module_list;
					const bool result = CreateModuleList(module_list, value->th32ProcessID);
					if(!result) { // デバッグ中など、特殊なプロセスであれば失敗するので無視する
						break;
					}
					const std::vector<boost::shared_ptr<MODULEENTRY32W> >::const_iterator it = std::find_if(module_list.begin(), module_list.end(), FindModulePath(module_path));
					if(it == module_list.end()) {
						list.push_back(value);
					}
					break;
				}
			}
			return list;
		}
	};
	result.swap(std::accumulate(process_list.begin(), process_list.end(), std::vector<boost::shared_ptr<PROCESSENTRY32W> >(), FindExeName(exe_name, dll_abs_path.wstring())));
	return true;
}

DWORD (WINAPI *GetLoadLibraryPath(unsigned int process_id))(void *) {
	const unsigned int dll_base_addr = reinterpret_cast<unsigned int>(::GetModuleHandleW(L"kernel32.dll"));
	if(dll_base_addr == NULL) {
		return NULL;
	}
	std::vector<boost::shared_ptr<MODULEENTRY32W> > current_module_list;
	if(!CreateModuleList(current_module_list)) {
		return false;
	}
	struct FindModuleAddr {
		FindModuleAddr(unsigned int module_addr) : module_addr(module_addr) { }
		unsigned int module_addr;
		bool operator()(boost::shared_ptr<MODULEENTRY32W> module) {
			return reinterpret_cast<unsigned int>(module->hModule) == module_addr;
		}
	};
	const std::vector<boost::shared_ptr<MODULEENTRY32W> >::const_iterator current_it = std::find_if(current_module_list.begin(), current_module_list.end(), FindModuleAddr(dll_base_addr));
	if(current_it == current_module_list.end()) {
		return NULL;
	}
	std::vector<boost::shared_ptr<MODULEENTRY32W> > target_module_list;
	if(!CreateModuleList(target_module_list, process_id)) {
		return false;
	}
	const std::vector<boost::shared_ptr<MODULEENTRY32W> >::const_iterator target_it = std::find_if(target_module_list.begin(), target_module_list.end(), FindModulePath((*current_it)->szExePath));
	if(target_it == target_module_list.end()) {
		return NULL;
	}
	const unsigned int current_proc_address = reinterpret_cast<unsigned int>(::GetProcAddress(reinterpret_cast<HMODULE>(dll_base_addr), "LoadLibraryW"));
	if(current_proc_address == NULL) {
		return NULL;
	}
	const unsigned int target_base_addr = reinterpret_cast<unsigned int>((*target_it)->hModule);
	const unsigned int result = current_proc_address - dll_base_addr + target_base_addr;
	if(result < target_base_addr || target_base_addr +  (*target_it)->modBaseSize <= result) {
		return NULL;
	}
	return reinterpret_cast<DWORD (WINAPI *)(void *)>(result);
}

typedef boost::shared_ptr<boost::remove_pointer<HANDLE>::type> SHREAD_HANDLE;
void MyVirtualFreeEx(SHREAD_HANDLE process, void *addr) {
	BOOST_ASSERT(process);
	DWORD exit_code;
	if(addr != NULL && FALSE != ::GetExitCodeProcess(process.get(), &exit_code) && exit_code == STILL_ACTIVE) {
		const BOOL result = ::VirtualFreeEx(process.get(), addr, 0, MEM_RELEASE);
		BOOST_ASSERT(result != FALSE);
	}
}

bool CreateThreadParam(boost::shared_ptr<void> &result, SHREAD_HANDLE process, const void *data, unsigned int data_size) {
	boost::function<void (void *)> func = boost::bind(&MyVirtualFreeEx, process, _1);
	result.reset(::VirtualAllocEx(process.get(), NULL, data_size, MEM_COMMIT, PAGE_READWRITE), func);
	if(result.get() == NULL) {
		return false;
	}
	if(FALSE == ::WriteProcessMemory(process.get(), result.get(), data, data_size, NULL)) {
		return false;
	}
	return true;
}

void MyCloseHandle(HANDLE handle) {
	if(handle != NULL) {
		::CloseHandle(handle);
	}
}

} // anonymous

bool DllInjection(const wchar_t *exe_name, const boost::filesystem::path &dll_path) {
	const boost::filesystem::path dll_abs_path = boost::filesystem::absolute(dll_path);
	std::vector<boost::shared_ptr<PROCESSENTRY32W> > th_app_list;
	if(!FindProcess(th_app_list, exe_name, dll_abs_path)) {
		return false;
	}
	BOOST_FOREACH(boost::shared_ptr<PROCESSENTRY32W> app, th_app_list) {
		DWORD (WINAPI * const proc_address)(LPVOID) = GetLoadLibraryPath(app->th32ProcessID);
		if(proc_address == NULL) {
			return false;
		}
		const SHREAD_HANDLE process = SHREAD_HANDLE(::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, app->th32ProcessID), &MyCloseHandle);
		if(process.get() == NULL) {
			return false;
		}
		const std::wstring &thread_param_string = dll_abs_path.wstring();
		boost::shared_ptr<void> thread_param;
		if(!CreateThreadParam(thread_param, process, thread_param_string.c_str(), (thread_param_string.size() + 1) * sizeof(wchar_t))) {
			return false;
		}
		const SHREAD_HANDLE thread = SHREAD_HANDLE(::CreateRemoteThread(process.get(), NULL, 0, proc_address, thread_param.get(), 0, NULL), &MyCloseHandle);
		if(thread.get() == NULL) {
			return false;
		}
		if(WAIT_OBJECT_0 != ::WaitForSingleObject(thread.get(), INFINITE)) {
			return false;
		}
	}

	return true;
}

#pragma pop_macro("MODULEENTRY32")
#pragma pop_macro("Module32First")
#pragma pop_macro("Module32Next")
#pragma pop_macro("PROCESSENTRY32")
#pragma pop_macro("Process32First")
#pragma pop_macro("Process32Next")
