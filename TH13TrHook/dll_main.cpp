#include "stdafx.h"

using namespace org::click3::DllHackLib;

HRESULT (WINAPI *p_CreateDevice)(IDirect3D9 *, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS *, IDirect3DDevice9**) = NULL;
HRESULT (WINAPI *p_Reset)(IDirect3DDevice9 *, D3DPRESENT_PARAMETERS *) = NULL;
HRESULT (WINAPI *p_Present)(IDirect3DDevice9 *, const RECT *, const RECT *, HWND, const RGNDATA *) = NULL;
HRESULT (WINAPI *p_EndScene)(IDirect3DDevice9 *) = NULL;
IDirect3D9 *(WINAPI *p_Direct3DCreate9)(UINT) = NULL;
HRESULT (WINAPI *p_Play)(IDirectSoundBuffer8 *, DWORD, DWORD, DWORD) = NULL;

namespace {

bool ChangeAllVartualProcAddress(void);

void ReleaseDevice(IDirect3DDevice9 *d3d_device) {
	if(d3d_device) {
		d3d_device->Release();
	}
}

void ReleaseD3D(IDirect3D9 *d3d) {
	if(d3d) {
		d3d->Release();
	}
}

HRESULT WINAPI d_Reset(IDirect3DDevice9 *d3d_device, D3DPRESENT_PARAMETERS *pPresentationParameters) {
	if(p_Reset == NULL) {
		return D3DERR_DRIVERINTERNALERROR;
	}
	windowed = (pPresentationParameters->Windowed != FALSE);
	return p_Reset(d3d_device, pPresentationParameters);
}

HRESULT WINAPI d_Present(IDirect3DDevice9 *d3d_device, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion) {
	if(p_Present == NULL) {
		return D3DERR_DRIVERINTERNALERROR;
	}
	Present();
	return p_Present(d3d_device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT WINAPI d_EndScene(IDirect3DDevice9 *d3d_device) {
	if(p_EndScene == NULL) {
		return D3DERR_INVALIDCALL;
	}
	EndScene();
	if(!ChangeAllVartualProcAddress()) {
		return D3DERR_INVALIDCALL;
	}
	return p_EndScene(d3d_device);
}

HRESULT WINAPI d_CreateDevice(IDirect3D9 *d3d, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) {
	if(p_CreateDevice == NULL) {
		return D3DERR_INVALIDCALL;
	}
	windowed = (pPresentationParameters->Windowed != FALSE);
	main_hwnd = hFocusWindow;
	const HRESULT result = p_CreateDevice(d3d, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
	if(D3D_OK != result) {
		return result;
	}
	(*ppReturnedDeviceInterface)->AddRef();
	d3d9_device_ptr.reset(*ppReturnedDeviceInterface, &ReleaseDevice);
	if(!ChangeAllVartualProcAddress()) {
		return D3DERR_INVALIDCALL;
	}
	return D3D_OK;
}

IDirect3D9 * WINAPI d_Direct3DCreate9(UINT SDKVersion) {
	if(p_Direct3DCreate9 == NULL) {
		return NULL;
	}
	IDirect3D9 * const result = p_Direct3DCreate9(SDKVersion);
	if(result == NULL) {
		return NULL;
	}
	result->AddRef();
	d3d9_ptr.reset(result, &ReleaseD3D);
	if(!ChangeAllVartualProcAddress()) {
		return false;
	}
	return result;
}

HRESULT WINAPI d_Play(IDirectSoundBuffer8 *sound_buffer, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) {
	if(p_Play == NULL) {
		return DSERR_INVALIDCALL;
	}

	boost::shared_ptr<const org::click3::NotificationCenter> notify_center = org::click3::NotificationCenter::GetDefaultCenter();
	boost::shared_ptr<IDirectSoundBuffer8 *> data(new IDirectSoundBuffer8*(sound_buffer));
	notify_center->SendNotification(NOTIFY_EVENT_PLAY_SOUND_EFFECT, data);

	return p_Play(sound_buffer, dwReserved1, dwPriority, dwFlags);
}

bool ChangeAllVartualProcAddress(void) {
	if(d3d9_ptr) {
		if(!ChangeVartualProcAddress(d3d9_ptr.get(), reinterpret_cast<void **>(p_CreateDevice == NULL ? &p_CreateDevice : NULL), &IDirect3D9::CreateDevice, &d_CreateDevice)) {
			return false;
		}
	}
	if(d3d9_device_ptr) {
		if(!ChangeVartualProcAddress(d3d9_device_ptr.get(), reinterpret_cast<void **>(p_Reset == NULL ? &p_Reset : NULL), &IDirect3DDevice9::Reset, &d_Reset)) {
			return false;
		}
		if(!ChangeVartualProcAddress(d3d9_device_ptr.get(), reinterpret_cast<void **>(p_Present == NULL ? &p_Present : NULL), &IDirect3DDevice9::Present, &d_Present)) {
			return false;
		}
		if(!ChangeVartualProcAddress(d3d9_device_ptr.get(), reinterpret_cast<void **>(p_EndScene == NULL ? &p_EndScene : NULL), &IDirect3DDevice9::EndScene, &d_EndScene)) {
			return false;
		}
	}
	if(th13_global->sound[0].buffer != NULL) {
		if(!ChangeVartualProcAddress(th13_global->sound[0].buffer, reinterpret_cast<void **>(p_Play == NULL ? &p_Play : NULL), &IDirectSoundBuffer8::Play, &d_Play)) {
			return false;
		}
	}
	return true;
}

bool HookStart(void) {
	if(th13_global->d3d9_ptr == NULL) {
		const unsigned int new_code = reinterpret_cast<unsigned int>(&d_Direct3DCreate9);
		if(!WriteCode(reinterpret_cast<unsigned char *>(&p_Direct3DCreate9), 0x49d298, reinterpret_cast<const unsigned char *>(&new_code), sizeof(new_code))) {
			return false;
		}
		return true;
	}
	th13_global->d3d9_ptr->AddRef();
	d3d9_ptr.reset(th13_global->d3d9_ptr, &ReleaseD3D);
	if(th13_global->d3d9_device_ptr != NULL) {
		windowed = (th13_global->window_mode != 0);
		main_hwnd = th13_global->main_hwnd;
		th13_global->d3d9_device_ptr->AddRef();
		d3d9_device_ptr.reset(th13_global->d3d9_device_ptr, &ReleaseDevice);
	}
	if(!ChangeAllVartualProcAddress()) {
		return false;
	}
	return true;
}

bool CheckTH13Tr(void) {
	const wchar_t * const filename = L"th13.exe";
	const unsigned int file_size = 767488;
	const unsigned int checksum = 0xA955CCEF;
	wchar_t module_filename[MAX_PATH];
	if(0 == ::GetModuleFileNameW(NULL, module_filename, MAX_PATH)) {
		return false;
	}
	const boost::filesystem::path path(module_filename);
	if(path.filename().wstring() != filename) {
		return false;
	}
	if(boost::filesystem::file_size(path) != file_size) {
		return false;
	}
	boost::filesystem::ifstream fs(path, std::ios_base::binary);
	if(!fs.is_open()) {
		return false;
	}
	std::vector<char> data(file_size);
	fs.read(&data.front(), file_size);
	if(!fs.good()) {
		return false;
	}
	fs.close();
	boost::crc_32_type crc;
	crc.process_bytes(&data.front(), file_size);
	if(crc.checksum() != checksum) {
		return false;
	}
	return true;
}

} // anonymous

BOOL APIENTRY DllMain(HANDLE , DWORD ul_reason_for_call, LPVOID) {
	switch(ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			if(!CheckTH13Tr()) {
				return FALSE;
			}
#ifdef _DEBUG
			::setlocale(LC_ALL, "");
			const bool console_result = SetupConsole();
			BOOST_ASSERT(console_result);
#endif
			if(!HookStart()) {
				return FALSE;
			}
			boost::thread dll_thread(_main);
			dll_thread.detach();
			break;
		}
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}
