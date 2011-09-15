#include "stdafx.h"

using namespace org::click3::DllHackLib;
using namespace boost::assign;

bool windowed;
HWND main_hwnd = NULL;
boost::shared_ptr<IDirect3D9> d3d9_ptr;
boost::shared_ptr<IDirect3DDevice9> d3d9_device_ptr;
TH13Global * const th13_global = reinterpret_cast<TH13Global *>(0x004BB320);
const char * const sound_filename[SOUND_EFFECT_BUFFER_COUNT] = {
	"se_plst00",		"se_plst00",		"se_pldead00",	"se_enep00",		"se_enep00",
	"se_enep01",		"se_enep02",		"se_ok00",		"se_ok00",		"se_cancel00",
	"se_select00",	"se_timeout",		"se_timeout2",	"se_powerup",		"se_pause",
	"se_cardget",		"se_invalid",		"se_extend",		"se_lazer00",		"se_lazer01",
	"se_lazer02",		"se_tan00",		"se_tan01",		"se_tan02",		"se_tan00",
	"se_tan01",		"se_tan02",		"se_tan00",		"se_power0",		"se_power1",
	"se_ch00",		"se_ch01",		"se_gun00",		"se_cat00",		"se_damage00",
	"se_damage01",	"se_nodamage",	"se_item00",		"se_kira00",		"se_kira01",
	"se_kira02",		"se_kira00",		"se_graze",		"se_graze",		"se_slash",
	"se_slash",		"se_cardget",		"se_bonus",		"se_bonus2",		"se_nep00",
	"se_boon00",		"se_don00",		"se_boon01",		"se_boon01",		"se_ch02",
	"se_ch03",		"se_extend2",		"se_pin00",		"se_pin01",		"se_lgods1",
	"se_lgods2",		"se_lgods3",		"se_lgods4",		"se_lgodsget",	"se_msl",
	"se_msl2",		"se_pldead01",	"se_heal",		"se_astralup",	"se_fault",
	"se_noise"
};

void *(__cdecl * const TH13Malloc)(unsigned int) = reinterpret_cast<void *(__cdecl *)(unsigned int)>(0x0047A0ED);

void Present(void) {
}

unsigned int GetProcessorCount(void) {
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class ScreenCapture : boost::noncopyable {
public:
	ScreenCapture(const boost::filesystem::path &out_dir, const boost::filesystem::path &relative_dir, boost::shared_ptr<IDirect3DDevice9> d3d9_device_ptr, D3DFORMAT format, bool windowed, HWND hwnd = NULL) :
		out_dir(out_dir), relative_dir(relative_dir), d3d9_device_ptr(d3d9_device_ptr), format(format), windowed(windowed), hwnd(hwnd)
	{
		BOOST_ASSERT(windowed != true || hwnd != NULL);
		boost::filesystem::create_directories(out_dir);
	}
	~ScreenCapture(void) {
		while(true) {
			boost::shared_ptr<boost::thread> th;
			{
				boost::mutex::scoped_lock lock(thread_list_mtx);
				if(thread_list.empty()) {
					break;
				}
				th = thread_list.front();
			}
			th->join();
		}
	}

	bool Capture(boost::shared_ptr<IDirect3DSurface9> &surface) {
		while(true) {
			if(thread_list.size() < std::max(GetProcessorCount(), static_cast<unsigned int>(1))) {
				surface = CreateSurface();
				if(surface) {
					break;
				}
			}
			boost::shared_ptr<boost::thread> th;
			{
				boost::mutex::scoped_lock lock(thread_list_mtx);
				if(thread_list.empty()) {
					return false;
				}
				th = thread_list.front();
			}
			th->join();
		}
		if(D3D_OK != d3d9_device_ptr->GetFrontBufferData(0, surface.get())) {
			return false;
		}
		return true;
	}

	bool SaveSurface(boost::function<void (const char *)> callback, boost::shared_ptr<IDirect3DSurface9> surface) {
		boost::shared_ptr<boost::thread> thread_ptr(new boost::thread(boost::bind(&ScreenCapture::SaveSurfaceThread, this, callback, surface)));
		if(!thread_ptr) {
			return false;
		}
		boost::mutex::scoped_lock lock(thread_list_mtx);
		thread_list.push_back(thread_ptr);
		return true;
	}

	bool SaveCurrentScreen(boost::function<void (const char *)> callback) {
		boost::shared_ptr<IDirect3DSurface9> surface;
		if(!Capture(surface) || !surface) {
			return false;
		}
		return SaveSurface(callback, surface);
	}
	
protected:
	const boost::filesystem::path out_dir;
	const boost::filesystem::path relative_dir;
	const boost::shared_ptr<IDirect3DDevice9> d3d9_device_ptr;
	const D3DFORMAT format;
	const bool windowed;
	unsigned char padding[3]; // unused
	const HWND hwnd;
	boost::mutex capture_mtx;
	boost::mutex thread_list_mtx;
	std::list<boost::shared_ptr<boost::thread> > thread_list;

	static void ReleaseSurface(IDirect3DSurface9 *pSurface) {
		if(pSurface != NULL) {
			pSurface->Release();
		}
	}

	boost::shared_ptr<IDirect3DSurface9> CreateSurface(void) {
		const D3DPOOL pool = D3DPOOL_SCRATCH;
		boost::shared_ptr<IDirect3DSurface9> result;
		D3DDISPLAYMODE d3dDispMode;
		if(D3D_OK != d3d9_device_ptr->GetDisplayMode(0, &d3dDispMode)) {
			return result;
		}
		IDirect3DSurface9 *pSurface;
		if(D3D_OK != d3d9_device_ptr->CreateOffscreenPlainSurface(d3dDispMode.Width, d3dDispMode.Height, format, pool, &pSurface, NULL) || pSurface == NULL) {
			return result;
		}
		result.reset(pSurface, &ScreenCapture::ReleaseSurface);
		return result;
	}

	static bool ConvertBmp(boost::function<void (const boost::gil::bgr8_view_t &)> write_function, const boost::filesystem::path &bmp_path) {
		const HBITMAP hbmp = reinterpret_cast<HBITMAP>(::LoadImageW(NULL, bmp_path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		if(hbmp == NULL) {
			return false;
		}
		BITMAP bmp;
		if(0 == ::GetObject(hbmp, sizeof(bmp), &bmp) || bmp.bmWidth <= 0 || bmp.bmHeight <= 0 || bmp.bmWidthBytes <= 0 || INT_MAX/bmp.bmWidthBytes < bmp.bmHeight) {
			return false;
		}
		std::vector<unsigned char> bits(static_cast<unsigned int>(bmp.bmWidthBytes * bmp.bmHeight));
		if(bits.size() == 0) {
			return false;
		}
		GetBitmapBits(hbmp, static_cast<int>(bits.size()), &bits.front());
		::DeleteObject(hbmp);
		for(unsigned int h = 0; h < static_cast<unsigned int>(bmp.bmHeight); h++) {
			unsigned char *in = &bits[bmp.bmWidthBytes * h];
			unsigned char *out = in;
			for(unsigned int w = 0; w < static_cast<unsigned int>(bmp.bmWidth); w++) {
				if(0 != ::memmove_s(out, 3, in, 3)) {
					return false;
				}
				in+=4;
				out+=3;
			}
		}
		boost::gil::bgr8_view_t view = boost::gil::interleaved_view(static_cast<unsigned int>(bmp.bmWidth), static_cast<unsigned int>(bmp.bmHeight), reinterpret_cast<boost::gil::bgr8_pixel_t*>(&bits.front()), bmp.bmWidthBytes);
		write_function(view);
		return true;
	}

	static bool Bmp2Jpeg(const boost::filesystem::path &path, int quality = 85) {
		boost::function<void (const boost::gil::bgr8_view_t &)> write_function = boost::bind(reinterpret_cast<void (*)(const std::string &, const boost::gil::bgr8_view_t &, int)>(&boost::gil::jpeg_write_view), path.string(), _1, quality);
		return ConvertBmp(write_function, path);
	}

	static bool Bmp2Png(const boost::filesystem::path &path) {
		boost::function<void (const boost::gil::bgr8_view_t &)> write_function = boost::bind(reinterpret_cast<void (*)(const std::string &, const boost::gil::bgr8_view_t &)>(&boost::gil::png_write_view), path.string(), _1);
		return ConvertBmp(write_function, path);
	}

	void SaveSurfaceThread(boost::function<void (const char *)> callback, boost::shared_ptr<IDirect3DSurface9> surface) {
		BOOST_ASSERT(surface);
		RECT rect;
		RECT *rect_ptr = NULL;
		if(windowed) {
			POINT point = {0, 0};
			const BOOL c2s_result = ::ClientToScreen(hwnd, &point);
			BOOST_ASSERT(FALSE != c2s_result);
			const BOOL get_client_result = ::GetClientRect(hwnd, &rect);
			BOOST_ASSERT(FALSE != get_client_result);
			rect.left = std::max(0, static_cast<int>(point.x));
			rect.top = std::max(0, static_cast<int>(point.y));
			rect.right = std::max(0, static_cast<int>(rect.right + point.x));
			rect.bottom = std::max(0, static_cast<int>(rect.bottom + point.y));
			if(rect.right == 0 || rect.bottom == 0) {
				return;
			}
			rect_ptr = &rect;
		}

		boost::filesystem::path put_path;
		{
			boost::mutex::scoped_lock lock(capture_mtx);
			for(unsigned int i = 0; put_path.empty() || boost::filesystem::exists(put_path); i++) {
				char temp[256];
				::sprintf_s(temp, sizeof(temp), "%05d.jpeg", i);
				put_path = out_dir / temp;
			}
			const HRESULT save_result = D3DXSaveSurfaceToFileW(put_path.c_str(), D3DXIFF_BMP, surface.get(), NULL, rect_ptr);
			BOOST_ASSERT(D3D_OK == save_result);
			const bool convert_result = Bmp2Jpeg(put_path);
			BOOST_ASSERT(convert_result);
		}
		BOOST_ASSERT(std::equal(relative_dir.begin(), relative_dir.end(), put_path.begin()));
		const unsigned int relative_size = std::distance(relative_dir.begin(), relative_dir.end());
		boost::filesystem::path relative_path;
		boost::filesystem::path::const_iterator it = put_path.begin();
		for(std::advance(it, relative_size); it != put_path.end(); ++it) {
			relative_path /= *it;
		}
		callback(relative_path.string().c_str());

		boost::thread th(&ScreenCapture::CheckThredList, this);
		th.detach();
	}

	void CheckThredList(void) {
		boost::mutex::scoped_lock lock(thread_list_mtx);
		while(true) {
			for(std::list<boost::shared_ptr<boost::thread> >::iterator it = thread_list.begin(); it != thread_list.end();) {
				if((*it)->timed_join(boost::posix_time::seconds(0))) {
					it = thread_list.erase(it);
					return;
				} else {
					++it;
				}
			}
		}
	}
};
#pragma warning(pop)
#pragma pack(pop)

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class SceneChecker : boost::noncopyable {
public:
	SceneChecker(const TH13Global *th13_global) : th13_global(th13_global) {
		boost::shared_ptr<SceneChecker *> user_data(new SceneChecker*(this));
		org::click3::NotificationCenter::GetDefaultCenter()->AddObserver(NOTIFY_EVENT_PLAY_SOUND_EFFECT, &SceneChecker::PlaySoundCallBack, user_data);
		Reset();
	}
	void Check(void) {
		if(th13_global->stage_number == 0) {
			return;
		}
		if(prev_stage_number != th13_global->stage_number || prev_stage_frame_count > th13_global->stage_frame_count) {
			Reset();
			prev_stage_number = th13_global->stage_number;
		}

		unsigned int touhou_scene_id = th13_global->scene_id;
		unsigned int touhou_scene_frame_count = th13_global->scene_frame_count;
		if(th13_global->stage_number == 3 && st3_latter) {
			if(touhou_scene_id == 2) {
				touhou_scene_id = 0;
				touhou_scene_frame_count -= st3_latter_half_start_frame;
			} else {
				st3_latter = false;
			}
		}

		if(prev_touhou_scene != touhou_scene_id) {
			const bool result = THSceneChange(touhou_scene_id);
			BOOST_ASSERT(result);
		}
		BOOST_ASSERT(prev_touhou_scene == touhou_scene_id);
		BOOST_ASSERT(prev_touhou_scene_frame_count <= touhou_scene_frame_count);

		if(!is_msg_run) {
			touhou_scene_frame_count -= msg_end_frame;
		}
		if(!is_msg_run && th13_global->msg != NULL && th13_global->msg->code != NULL) {
			is_msg_run = true;
		} else if(is_msg_run && (th13_global->msg == NULL || th13_global->msg->code == NULL)) {
			msg_end_frame = touhou_scene_frame_count;
			is_msg_run = false;
		}
		if(th13_global->spell == NULL) {
			prev_spell_frame_count = 0;
		} else {
			if(prev_spell_frame_count < th13_global->spell->frame_count) {
				is_spell_frame_up = true;
			} else {
				is_spell_frame_up = false;
			}
			prev_spell_frame_count = th13_global->spell->frame_count;
		}
		prev_touhou_scene_frame_count = touhou_scene_frame_count;
		prev_stage_frame_count = th13_global->stage_frame_count;
	}
	bool IsLatterHalf(void) const {
		return (prev_touhou_scene == 0 && touhou_scene_count == 1);
	}
	void Reset(void) {
		touhou_scene_count = 0;
		prev_stage_number = 0;
		prev_stage_frame_count = 0;
		prev_touhou_scene = 0;
		prev_touhou_scene_frame_count = 0;
		prev_spell_frame_count = 0;
		st3_latter = false;
		is_msg_run = false;
		is_spell_frame_up = false;
		mid_boss_frame_count = 0;
		st3_latter_half_start_frame = 0;
		msg_end_frame = 0;
	}
	unsigned int GetSceneFrameCount(void) const {
		return prev_touhou_scene_frame_count;
	}
	bool CreateSaveData(std::map<std::string, unsigned long long int> &result) {
		result.clear();
		result["player"] = th13_global->player_id;
		result["play_mode"] = th13_global->play_mode;
		result["stage_number"] = prev_stage_number;
		result["stage_frame_count"] = prev_stage_frame_count;
		result["scene_id"] = prev_touhou_scene;
		result["scene_frame_count"] = prev_touhou_scene_frame_count;
		result["scene_count"] = touhou_scene_count;
		result["mid_boss_frame_count"] = mid_boss_frame_count;
		result["score"] = th13_global->score;
		result["graze"] = th13_global->graze;
		result["bomb_count"] = th13_global->spell_count;
		result["bomb_fragment_count"] = th13_global->spell_fragment;
		result["life"] = static_cast<unsigned int>(th13_global->life);
		result["life_fragment"] = th13_global->life_fragment;
		result["extend_count"] = th13_global->extend_count;
		result["astral_count"] = th13_global->astral_count;
		time_t t;
		if(-1 == ::time(&t)) {
			return false;
		}
		result["time"] = static_cast<unsigned int>(t);
		return true;
	}
protected:
	const TH13Global * const th13_global;
	unsigned int touhou_scene_count;
	unsigned int prev_stage_number;
	unsigned int prev_stage_frame_count;
	unsigned int prev_touhou_scene;
	unsigned int prev_touhou_scene_frame_count;
	unsigned int prev_spell_frame_count;
	bool st3_latter;
	bool is_msg_run;
	bool is_spell_frame_up;
	unsigned char padding[1]; // unused
	unsigned int mid_boss_frame_count;
	unsigned int st3_latter_half_start_frame;
	unsigned int msg_end_frame;

	bool THSceneChange(unsigned int new_scene) {
		BOOST_ASSERT(prev_touhou_scene != new_scene);
		BOOST_ASSERT(!is_msg_run);
		if(new_scene == 0) {
			mid_boss_frame_count = prev_touhou_scene_frame_count;
			touhou_scene_count = 1; // 後半道中
		} else {
			touhou_scene_count = 0;
		}
		prev_touhou_scene = new_scene;
		prev_touhou_scene_frame_count = 0;
		msg_end_frame = 0;
		return true;
	}

	void SoundPlay(unsigned int id) {
		if(th13_global->stage_number == 3 && th13_global->scene_id == 2 && id == 5) {
			st3_latter = true;
			st3_latter_half_start_frame = th13_global->scene_frame_count;
		}
		if((prev_touhou_scene == 2 || prev_touhou_scene == 3) && id == 27 && !is_spell_frame_up) {
			touhou_scene_count++;
		}
	}

	static void PlaySoundCallBack(const char *notify_name, boost::shared_ptr<void> user_data, boost::shared_ptr<void> data) {
		struct FindSound {
			const IDirectSoundBuffer8 *sound_buffer;
			FindSound(const IDirectSoundBuffer8 *sound_buffer) : sound_buffer(sound_buffer) { }
			bool operator()(const SoundInfo &sound) const {
				return sound.buffer == sound_buffer;
			}
		};
		BOOST_ASSERT(std::string(notify_name) == NOTIFY_EVENT_PLAY_SOUND_EFFECT);
		SceneChecker * const this_ptr = *boost::static_pointer_cast<SceneChecker *>(user_data);
		IDirectSoundBuffer8 * const sound_buffer = *boost::static_pointer_cast<IDirectSoundBuffer8 *>(data);
		const SoundInfo * const sound = std::find_if(&this_ptr->th13_global->sound[0], &this_ptr->th13_global->sound[SOUND_EFFECT_BUFFER_COUNT], FindSound(sound_buffer));
		if(sound < &this_ptr->th13_global->sound[SOUND_EFFECT_BUFFER_COUNT]) {
			const unsigned int id = static_cast<unsigned int>(std::distance(reinterpret_cast<const SoundInfo *>(&this_ptr->th13_global->sound[0]), sound));
			BOOST_ASSERT(id < SOUND_EFFECT_BUFFER_COUNT);
			this_ptr->SoundPlay(id);
		}
	}

};
#pragma warning(pop)
#pragma pack(pop)

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class HitChecker : boost::noncopyable {
public:
	HitChecker(const boost::filesystem::path &hit_data_list_out_path, const boost::filesystem::path &capture_out_dir, const boost::filesystem::path &relative_dir, const TH13Global *th13_global, boost::shared_ptr<IDirect3DDevice9> d3d9_device_ptr, D3DFORMAT format, bool windowed, HWND hwnd) :
		hit_data_list_out_path(hit_data_list_out_path), th13_global(th13_global), scene_checker(th13_global), capture(capture_out_dir, relative_dir, d3d9_device_ptr, format, windowed, hwnd), prev_mode(0)
	{
		const boost::filesystem::path dir = boost::filesystem::path(hit_data_list_out_path).remove_filename();
		boost::filesystem::create_directories(dir);
	}

	void Check(void) {
		scene_checker.Check();
		if(th13_global->trans == NULL || th13_global->player == NULL || th13_global->stage == NULL) {
			surface.reset();
			prev_mode = 0;
			return;
		}
		boost::function<void (const char *)> callback = boost::bind(&HitChecker::HitInfoSave, this, _1);
		if(surface) {
			if(th13_global->player->status == 1) {
				surface.reset();
			} else if(th13_global->player->status == 2) {
				const bool result = capture.SaveSurface(callback, surface);
				BOOST_ASSERT(result);
				surface.reset();
			}
		}
		if(th13_global->player->status_frame_count == 0 && th13_global->player->status == 4) {
			BOOST_ASSERT(!surface);
			const bool capture_result = capture.Capture(surface);
			BOOST_ASSERT(capture_result);
			boost::mutex::scoped_lock lk(save_data_mtx);
			const bool create_data_result = scene_checker.CreateSaveData(save_data);
			BOOST_ASSERT(create_data_result);
		}
		if(prev_mode != th13_global->trans->mode) {
			prev_mode = th13_global->trans->mode;
			if(prev_mode == 3) {
				{
					boost::mutex::scoped_lock lk(save_data_mtx);
					const bool result = scene_checker.CreateSaveData(save_data);
					BOOST_ASSERT(result);
				}
				const bool result = capture.SaveCurrentScreen(callback);
				BOOST_ASSERT(result);
			}
		}
	}
protected:
	const boost::filesystem::path hit_data_list_out_path;
	const TH13Global * const th13_global;
	SceneChecker scene_checker;
	std::map<std::string, unsigned long long int> save_data;
	mutable boost::mutex save_data_mtx;
	ScreenCapture capture;
	boost::shared_ptr<IDirect3DSurface9> surface;
	unsigned char prev_mode;
	unsigned char padding[3]; // unused
	
	void HitInfoSave(const char *image_filename) {
		boost::property_tree::ptree ptree;
		{
			boost::mutex::scoped_lock lk(save_data_mtx);
			typedef std::pair<std::string, unsigned long long int> SAVE_DATA_PAIR;
			BOOST_FOREACH(const SAVE_DATA_PAIR &pair, save_data) {
				ptree.add(pair.first, pair.second);
			}
			ptree.add("capture_filename", image_filename);
		}
		if(!boost::filesystem::exists(hit_data_list_out_path)) {
			boost::property_tree::ptree root;
			boost::property_tree::ptree container;
			container.insert(container.end(), std::pair<std::string, boost::property_tree::ptree>("", ptree));
			root.add_child("hit_data_list", container);
			boost::filesystem::ofstream ofs(hit_data_list_out_path);
			BOOST_ASSERT(ofs.is_open());
			write_json(ofs, root);
			ofs.close();
			return;
		}
		unsigned int seek_size = 0;
		{
			boost::filesystem::ifstream stream(hit_data_list_out_path, boost::filesystem::ifstream::binary);
			BOOST_ASSERT(stream.is_open());
			stream.seekg(-20, boost::filesystem::fstream::end);
			std::string buf(20, '\0');
			stream.read(&buf.front(), buf.size());
			stream.close();
			BOOST_ASSERT(!stream.bad());
			for(int i = 16; i >= 0; i--) {
				std::string temp(buf.begin() + i, buf.end());
				temp.erase(std::remove(temp.begin(), temp.end(), '\r'), temp.end());
				temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());
				temp.erase(std::remove(temp.begin(), temp.end(), '\t'), temp.end());
				temp.erase(std::remove(temp.begin(), temp.end(), ' '), temp.end());
				if(temp.compare(0, 2, "}]}", 2) == 0) {
					seek_size = 19 - static_cast<unsigned int>(i);
					break;
				}
			}
		}
		BOOST_ASSERT(seek_size != 0);
		boost::filesystem::ofstream stream(hit_data_list_out_path, boost::filesystem::ofstream::in || boost::filesystem::ofstream::ate);
		BOOST_ASSERT(stream.is_open());
		stream.seekp(-static_cast<int>(seek_size), boost::filesystem::fstream::end);
		stream << ", ";
		boost::property_tree::json_parser::write_json_helper(stream, ptree, 2, true);
		stream << std::endl << "    ]" << std::endl << "}" << std::endl;
		stream.close();
	}
};
#pragma warning(pop)
#pragma pack(pop)

void EndScene(void) {
	if(!d3d9_device_ptr) {
		return;
	}
#ifdef _DEBUG
	static LPD3DXFONT pD3DFont = NULL;
	if(pD3DFont == NULL) {
		D3DXFONT_DESC lf;
		lf.Height          = 16;
		lf.Width           = 0;
		lf.Weight          = 200;//FW_NORMAL;
		lf.MipLevels       = 0;
		lf.Italic          = FALSE;
		lf.CharSet         = SHIFTJIS_CHARSET;
		lf.OutputPrecision = OUT_DEFAULT_PRECIS;
		lf.Quality         = DEFAULT_QUALITY;
		lf.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
		::strcpy_s(lf.FaceName, sizeof(lf.FaceName), "ＭＳ ゴシック");
		HRESULT result = D3DXCreateFontIndirect(d3d9_device_ptr.get(), &lf, &pD3DFont);
		BOOST_ASSERT(D3D_OK == result);
		BOOST_ASSERT(pD3DFont != NULL);
	}
	RECT r = {0, 0, 100, 100};
	const int result = pD3DFont->DrawText(NULL, "DLLハッキングなう！", -1, &r, DT_LEFT | DT_NOCLIP, 0xFFFFFFFF);
	BOOST_ASSERT(result != 0);
#else
	if(th13_global->engine_mode != 1 && th13_global->engine_mode != 4 && th13_global->engine_mode != 5) {
		return;
	}
#endif

	static boost::shared_ptr<HitChecker> checker;
	if(!checker) {
		const boost::filesystem::path relative_dir = SharedMemory::GetCaptureOutPath();
		const boost::filesystem::path hit_data_list_out_path = boost::filesystem::path(SharedMemory::GetHitDataListOutPath()) / "th13_hit_data_list.json";
		const boost::filesystem::path capture_out_dir = relative_dir / "th13";
		checker.reset(new HitChecker(hit_data_list_out_path, capture_out_dir, relative_dir, th13_global, d3d9_device_ptr, D3DFMT_A8R8G8B8, windowed, main_hwnd));
	}
	checker->Check();
}

void _main() {
	{
		const unsigned char old_code = 0x1B;
		const unsigned char new_code = 0x18;
		bool result;
		// 霊夢ボムのSEを変更
		result = ChangeCode(0x0040B2B6, &old_code, &new_code, 1);
		BOOST_ASSERT(result);
		result = ChangeCode(0x0040B36F, &old_code, &new_code, 1);
		BOOST_ASSERT(result);
		result = ChangeCode(0x0040B806, &old_code, &new_code, 1);
		BOOST_ASSERT(result);
		// 妖夢ボムのSEを変更
		result = ChangeCode(0x0040CD3A, &old_code, &new_code, 1);
		BOOST_ASSERT(result);
	}
	{
		const unsigned char old_code = 0x1A;
		const unsigned char new_code = 0x17;
		// 早苗ボムのSEを変更
		const bool result = ChangeCode(0x0040C38C, &old_code, &new_code, 1);
		BOOST_ASSERT(result);
	}
}
