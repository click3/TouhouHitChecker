#include "stdafx.h"

using namespace boost::assign;

#define USER_NOTIFY_APP_SELECT	(0)
#define USER_NOTIFY_MODE_SELECT	(1)

enum TH_APP_ID {
	TH_APP_06 = 0, TH_APP_07, TH_APP_08,
	TH_APP_10, TH_APP_11, TH_APP_12,
	TH_APP_128, TH_APP_13,
	TH_APP_ID_MAX
};

const TCHAR * const app_name_list[] = {
	_T("東方紅魔郷"), _T("東方妖々夢"), _T("東方永夜抄"),
	_T("東方風神録"), _T("東方地霊殿"), _T("東方星蓮船"),
	_T("妖精大戦争"), _T("東方神霊廟")
};

const char * const scene_list_json_path_list[] = {
	"..\\scene_list\\th06.json", "..\\scene_list\\th07.json", "..\\scene_list\\th08.json",
	"..\\scene_list\\th10.json", "..\\scene_list\\th11.json", "..\\scene_list\\th12.json",
	"..\\scene_list\\th128.json", "..\\scene_list\\th13tr.json"
};

const char * const hit_data_list_json_path_list[] = {
	"", "", "",
	"", "", "",
	"", ".\\hit_data_list\\th13_hit_data_list.json"
};

class HitDataList : boost::noncopyable {
protected:
	class HitData : boost::noncopyable {
	public:
		HitData(unsigned int astral_count, unsigned int bomb_count, unsigned int bomb_fragment_count, unsigned int extend_count,
			unsigned int graze, unsigned int life, unsigned int life_fragment, unsigned int mid_boss_frame_count,
			unsigned int play_mode, unsigned int player, unsigned int touhou_scene_count, unsigned int touhou_scene_frame_count,
			unsigned int touhou_scene_id, unsigned int score, unsigned int stage_frame_count, unsigned int stage_number,
			unsigned int time, const boost::filesystem::path &capture_filename, unsigned int scene_id)
		:
			astral_count(astral_count), bomb_count(bomb_count), bomb_fragment_count(bomb_fragment_count), extend_count(extend_count),
			graze(graze), life(life), life_fragment(life_fragment), mid_boss_frame_count(mid_boss_frame_count),
			play_mode(play_mode), player(player), touhou_scene_count(touhou_scene_count), touhou_scene_frame_count(touhou_scene_frame_count),
			touhou_scene_id(touhou_scene_id), score(score), stage_frame_count(stage_frame_count), stage_number(stage_number),
			time(time), capture_filename(capture_filename), scene_id(scene_id)
		{
		}
		static bool Load(boost::shared_ptr<HitData> &result, const boost::property_tree::wptree &tree, const StageSceneSetting &setting) {
#define READ_IMPL(name, type)										\
			type name;										\
			do {											\
			const boost::optional<type> opt_value = tree.get_optional<type>(L ## #name);	\
				if(!opt_value) {								\
					return false;								\
				}										\
				name = opt_value.get();							\
			} while(false)
#define READ(name) READ_IMPL(name, unsigned int)
			READ(astral_count);
			READ(bomb_count);
			READ(bomb_fragment_count);
			READ(extend_count);
			READ(graze);
			READ(life);
			READ(life_fragment);
			READ(mid_boss_frame_count);
			READ(play_mode);
			READ(player);
			READ(scene_count);
			READ(scene_frame_count);
			READ(scene_id);
			READ(score);
			READ(stage_frame_count);
			READ(stage_number);
			READ(time);
			READ_IMPL(capture_filename, boost::filesystem::path);
#undef READ
#undef READ_IMPL

			result.reset(new HitData(astral_count, bomb_count, bomb_fragment_count, extend_count, graze,
				life, life_fragment, mid_boss_frame_count, play_mode, player, scene_count,
				scene_frame_count, scene_id, score, stage_frame_count, stage_number, time, capture_filename,
				setting.CalcSceneId(stage_number, play_mode, scene_id, scene_count, scene_frame_count, mid_boss_frame_count)));
			return true;
		}
	protected:
		const unsigned int astral_count;
		const unsigned int bomb_count;
		const unsigned int bomb_fragment_count;
		const unsigned int extend_count;
		const unsigned int graze;
		const unsigned int life;
		const unsigned int life_fragment;
		const unsigned int mid_boss_frame_count;
		const unsigned int play_mode;
		const unsigned int player;
		const unsigned int touhou_scene_count;
		const unsigned int touhou_scene_frame_count;
		const unsigned int touhou_scene_id;
		const unsigned int score;
		const unsigned int stage_frame_count;
		const unsigned int stage_number;
		const unsigned int time;
		const boost::filesystem::path capture_filename;
		const unsigned int scene_id;
	};
public:
	bool Load(const boost::filesystem::path &scene_setting_json_path, const boost::filesystem::path &hit_data_list_json_path) {
		if(!scene_setting.Load(scene_setting_json_path)) {
			return false;
		}
		if(!boost::filesystem::is_regular_file(hit_data_list_json_path)) {
			return true;
		}
		boost::property_tree::wptree json;
		try {
			read_json(hit_data_list_json_path.string(), json, std::locale(std::locale::empty(), new boost::filesystem::detail::utf8_codecvt_facet));
		} catch(std::exception ex) {
			return false;
		}
		boost::optional<boost::property_tree::wptree &> hit_data_list_ptree = json.get_child_optional(L"hit_data_list");
		if(!hit_data_list_ptree) {
			return false;
		}
		BOOST_FOREACH(const boost::property_tree::wptree::value_type &child, hit_data_list_ptree.get()) {
			boost::shared_ptr<HitData> hit_data;
			if(!HitData::Load(hit_data, child.second, scene_setting)) {
				return false;
			}
			hit_data_list.push_back(hit_data);
		}
		return true;
	}
	const std::vector<const boost::shared_ptr<HitData> > &GetList(void) const {
		return hit_data_list;
	}
protected:
	StageSceneSetting scene_setting;
	std::vector<const boost::shared_ptr<HitData> > hit_data_list;
};

bool GetModeList(std::vector<const TCHAR *> &mode_list, TH_APP_ID id) {
	mode_list.clear();
	switch(id) {
		case TH_APP_06:
		case TH_APP_07:
		case TH_APP_08:
		case TH_APP_10:
		case TH_APP_11:
		case TH_APP_12:
		case TH_APP_128:
			mode_list += _T("未実装");
			break;
		case TH_APP_13:
			mode_list += _T("被弾情報一覧"), _T("被弾率グラフ"), _T("抱え率グラフ");
			break;
		case TH_APP_ID_MAX:
		default:
			BOOST_ASSERT(false);
			break;
	}
	return true;
}

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class AppSelectView : public CWindowImpl<AppSelectView, CStatic>, boost::noncopyable {
public:
	DECLARE_WND_SUPERCLASS(NULL, CStatic::GetWndClassName())

	AppSelectView(void) :
		margin(6), button_id(1000), button_size(24), label_height(UINT_MAX), menu_id_base(1001) {
	}

	BEGIN_MSG_MAP(AppSelectView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnResize)
		COMMAND_HANDLER_EX(button_id, BN_CLICKED, OnButtonClick)
		COMMAND_RANGE_CODE_HANDLER_EX(menu_id_base, menu_id_base + TH_APP_ID_MAX - 1, BN_CLICKED, OnMenuClick)
	END_MSG_MAP()

	bool UpdateLayout(void) {
		CRect my_rect;
		if(FALSE == GetClientRect(my_rect)) {
			return false;
		}
		if(my_rect.right == 0 || my_rect.bottom == 0) {
			return true;
		}
		const int label_x = static_cast<int>(margin);
		const int label_y = static_cast<int>((my_rect.bottom - label_height) / 2);
		const int button_x = static_cast<int>(my_rect.right - margin - button_size);
		const int button_y = static_cast<int>((my_rect.bottom - button_size) / 2);
		BOOST_ASSERT(label_x >= 0);
		BOOST_ASSERT(label_y >= 0);
		BOOST_ASSERT(button_x >= 0);
		BOOST_ASSERT(button_y >= 0);
		BOOST_ASSERT(label_x + static_cast<int>(label_width) < my_rect.right);
		BOOST_ASSERT(label_y + static_cast<int>(label_height) < my_rect.bottom);
		BOOST_ASSERT(button_x + static_cast<int>(button_size) < my_rect.right);
		BOOST_ASSERT(button_y + static_cast<int>(button_size) < my_rect.bottom);
		label.MoveWindow(label_x, label_y, static_cast<int>(label_width), static_cast<int>(label_height));
		button.MoveWindow(button_x, button_y, static_cast<int>(button_size), static_cast<int>(button_size));
		return true;
	}

	void SetCurSel(unsigned int index) {
		if(index == current_index) {
			return;
		}
		label.SetWindowText(app_name_list[index]);
		popup_menu.CheckMenuItem(current_index, MF_UNCHECKED | MF_BYPOSITION);
		current_index = index;
		popup_menu.CheckMenuItem(current_index, MF_CHECKED | MF_BYPOSITION);
	}

	TH_APP_ID GetCurAppID(void) const {
		return static_cast<TH_APP_ID>(current_index);
	}

	void PopupMenu(void) {
		RECT rect;
		button.GetClientRect(&rect);
		POINT pos = {rect.right, rect.top};
		button.ClientToScreen(&pos);
		popup_menu.TrackPopupMenu(
			TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON,
			pos.x, pos.y, m_hWnd);
	}

	unsigned int GetMinWidth(void) const {
		BOOST_ASSERT(label.IsWindow() != FALSE);
		return margin + label_width + margin + button_size + margin;
	}

	unsigned int GetMinHeight(void) const {
		BOOST_ASSERT(label.IsWindow() != FALSE);
		return margin + (std::max)(label_height, button_size) + margin;
	}

	bool GetMinSize(LPSIZE size) const {
		if(label.IsWindow() == FALSE) {
			return false;
		}
		size->cx = static_cast<int>(GetMinWidth());
		size->cy = static_cast<int>(GetMinHeight());
		BOOST_ASSERT(size->cx > 0);
		BOOST_ASSERT(size->cy > 0);
		return true;
	}
protected:
	const unsigned int margin;
	const unsigned int button_id;
	const unsigned int button_size;
	unsigned int label_height;
	unsigned int label_width;
	CStatic label;
	CButton button;
	const unsigned int menu_id_base;
	CMenu popup_menu;
	unsigned int current_index;

	int OnCreate(LPCREATESTRUCT /*lpCreateStruct*/){
		LRESULT lRet = DefWindowProc();

		label.Create(m_hWnd, rcDefault, _T(""), BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP);
		button.Create(m_hWnd, rcDefault, _T("..."),
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
			BS_CENTER | BS_TEXT | BS_PUSHBUTTON, 0, button_id);

		SIZE size;
		const BOOL result = ::GetTextExtentPoint32(label.GetDC(), app_name_list[0], static_cast<int>(::_tcslen(app_name_list[0])), &size);
		BOOST_ASSERT(result != FALSE);
		BOOST_ASSERT(size.cy >= 0);
		label_height = static_cast<unsigned int>(size.cy);

		struct TextWidthMax {
			TextWidthMax(HDC hdc) : hdc(hdc) { }
			HDC hdc;
			unsigned int operator()(unsigned int value, const TCHAR *text) const {
				SIZE size;
				const BOOL result = ::GetTextExtentPoint32(hdc, text, static_cast<int>(::_tcslen(text)), &size);
				BOOST_ASSERT(result != FALSE);
				BOOST_ASSERT(size.cx >= 0);
				return (std::max)(value, static_cast<unsigned int>(size.cx));
			}
		};
		const unsigned int text_width_max = std::accumulate(&app_name_list[0], &app_name_list[TH_APP_ID_MAX], static_cast<unsigned int>(0), TextWidthMax(label.GetDC()));
		label_width = text_width_max;

		popup_menu.CreatePopupMenu();
		unsigned int i = 0;
		BOOST_FOREACH(const TCHAR * const app_name, app_name_list) {
			popup_menu.AppendMenuW(MFS_DEFAULT, menu_id_base + i, app_name);
			i++;
		}
		return lRet;
	}

	void OnResize(UINT flag, CSize) {
		if(flag == SIZE_RESTORED) {
			const bool result = UpdateLayout();
			BOOST_ASSERT(result);
		}
	}

	void OnButtonClick(UINT, int, HWND) {
		PopupMenu();
	}

	void OnMenuClick(UINT, int id, HWND) {
		BOOST_ASSERT(static_cast<unsigned int>(id) >= menu_id_base);
		BOOST_ASSERT(static_cast<unsigned int>(id) < menu_id_base + TH_APP_ID_MAX);
		const unsigned int app_id = static_cast<unsigned int>(id) - menu_id_base;
		SetCurSel(app_id);
		const WPARAM wParam = static_cast<WPARAM>(MAKELONG(GetDlgCtrlID(), USER_NOTIFY_APP_SELECT));
		const LPARAM lParam = reinterpret_cast<LPARAM>(m_hWnd);
		GetParent().PostMessage(WM_COMMAND, wParam, lParam);
	}
};
#pragma pack(pop)
#pragma warning(pop)

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class ModeSelectView : public CWindowImpl<ModeSelectView, CStatic>, boost::noncopyable {
public:
	DECLARE_WND_SUPERCLASS(NULL, CStatic::GetWndClassName())

	ModeSelectView(void) : list_box_id(1000) {
	}

	BEGIN_MSG_MAP(ModeSelectView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnResize)
		COMMAND_HANDLER_EX(list_box_id, LBN_SELCHANGE, OnModeChange)
	END_MSG_MAP()

	bool UpdateLayout(void) {
		CRect my_rect;
		if(FALSE == GetClientRect(my_rect)) {
			return false;
		}
		if(my_rect.right == 0 || my_rect.bottom == 0) {
			return true;
		}
		CRect list_rect;
		list_box.MoveWindow(0, 0, my_rect.right, my_rect.bottom);
		return true;
	}

	bool ChangeModeList(TH_APP_ID id) {
		list_box.ResetContent();
		std::vector<const TCHAR *> mode_list;
		if(!GetModeList(mode_list, id)) {
			return false;
		}
		BOOST_FOREACH(const TCHAR * const mode_name, mode_list) {
			list_box.AddString(mode_name);
		}
		return true;
	}
	bool ChangeModeList(unsigned int id) {
		return ChangeModeList(static_cast<TH_APP_ID>(id));
	}
	int GetCurSel(void) const {
		return list_box.GetCurSel();
	}
	int SetCurSel(int nSelect) {
		return list_box.SetCurSel(nSelect);
	}
protected:
	const unsigned int list_box_id;
	CListBox list_box;

	int OnCreate(LPCREATESTRUCT /*lpCreateStruct*/){
		LRESULT lRet = DefWindowProc();
		list_box.Create(m_hWnd, rcDefault, NULL,
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_NOTIFY,
			WS_EX_CLIENTEDGE, list_box_id);
		return lRet;
	}

	void OnResize(UINT flag, CSize) {
		if(flag == SIZE_RESTORED) {
			const bool result = UpdateLayout();
			BOOST_ASSERT(result);
		}
	}

	void OnModeChange(UINT, int, HWND) {
		const WPARAM wParam = static_cast<WPARAM>(MAKELONG(GetDlgCtrlID(), USER_NOTIFY_MODE_SELECT));
		const LPARAM lParam = reinterpret_cast<LPARAM>(m_hWnd);
		GetParent().PostMessage(WM_COMMAND, wParam, lParam);
	}
};
#pragma pack(pop)
#pragma warning(pop)

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class ImageView : public CWindowImpl<ImageView, CStatic>, boost::noncopyable {
public:
	DECLARE_WND_SUPERCLASS(NULL, CStatic::GetWndClassName())

	ImageView(const boost::filesystem::path &filename) : filename(filename) {
	}

	BEGIN_MSG_MAP(MainView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_SIZE(OnResize)
	END_MSG_MAP()
protected:
	const boost::filesystem::path filename;
	boost::gil::rgb8_image_t image;
	boost::shared_ptr<boost::gil::rgb8_image_t> out_image;
	boost::mutex mtx;

	int OnCreate(LPCREATESTRUCT) {
		try {
			org::click3::Image::ReadImage(filename.string(), image);
		} catch(std::exception &ex) {
			std::cout << ex.what() << std::endl;
		}
		return DefWindowProc();
	}

	void OnResize(unsigned int flag, CSize size) {
		DefWindowProc();
		if(flag != SIZE_RESTORED) {
			return;
		}
		if(size.cx <= 0 || size.cy <= 0 || image.height() < 1 || image.width() < 1) {
			return;
		}
		while(true) {
			boost::mutex::scoped_lock lk(mtx);
			if(!out_image) {
				break;
			}
			if((out_image->height() == size.cy && out_image->width() <= size.cx) ||
				(out_image->height() <= size.cy && out_image->width() == size.cx))
			{
				return;
			}
			out_image.reset();
			break;
		}
		boost::function<void (void)> func = boost::bind(&ImageView::ImageResizeThread, this, size);
		boost::thread th(func);
		th.detach();
	}

	void ImageResizeThread(CSize &size) {
		BOOST_ASSERT(size.cx > 0);
		BOOST_ASSERT(size.cy > 0);

		const double x_mag = static_cast<double>(image.width()) / size.cx;
		const double y_mag = static_cast<double>(image.height()) / size.cy;
		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int width = static_cast<unsigned int>(size.cx);
		unsigned int height = static_cast<unsigned int>(size.cy);
		if(x_mag > y_mag) {
			height = static_cast<unsigned int>(image.height() / x_mag);
			y = size.cy / 2 - height / 2;
		} else {
			width = static_cast<unsigned int>(image.width() / y_mag);
			x = size.cx / 2 - width / 2;
		}
		BOOST_ASSERT(width <= INT_MAX);
		BOOST_ASSERT(height <= INT_MAX);
		boost::gil::rgb8_image_t result(static_cast<int>(width), static_cast<int>(height));
		boost::gil::resize_view(boost::gil::const_view(image), boost::gil::view(result), boost::gil::bilinear_sampler());

		boost::mutex::scoped_lock lk(mtx);
		CRect current_rect;
		GetClientRect(current_rect);
		if(size.cx != current_rect.Width() || size.cy != current_rect.Height()) {
			return;
		}
		out_image.reset(new boost::gil::rgb8_image_t());
		out_image->swap(result);
		{
			const BOOL result = this->InvalidateRect(NULL, FALSE);
			BOOST_ASSERT(result != FALSE);
		}
	}

	void OnPaint(HDC) {
		boost::mutex::scoped_lock lk(mtx);
		if(!out_image) {
			CRect rect;
			if(FALSE == GetUpdateRect(rect)) {
				return;
			}
			CPaintDC dc(m_hWnd);
			const BOOL result = dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), dc, 0, 0, WHITENESS);
			BOOST_ASSERT(result != FALSE);
			return;
		}

		org::click3::Image::SHARED_HBITMAP bmp;
		{
			const bool result = org::click3::Image::GilViewToHBitmap(bmp, boost::gil::const_view(*out_image));
			BOOST_ASSERT(result);
		}
		BOOST_ASSERT(bmp);

		CPaintDC dc(m_hWnd);
		CBitmap bitmap(bmp.get());
		CRect rect;
		GetClientRect(rect);
		const unsigned int image_width = static_cast<unsigned int>(out_image->width());
		const unsigned int image_height = static_cast<unsigned int>(out_image->height());
		const unsigned int x = rect.Width() / 2 -  image_width / 2;
		const unsigned int y = rect.Height() / 2 - image_height / 2;
		CDC dcMem;
		dcMem.CreateCompatibleDC(dc);
		const HBITMAP hOldBitmap = dcMem.SelectBitmap(bitmap);
		{
			BOOST_ASSERT(x <= INT_MAX);
			BOOST_ASSERT(y <= INT_MAX);
			const BOOL result = dc.BitBlt(static_cast<int>(x), static_cast<int>(y), static_cast<int>(image_width), static_cast<int>(image_height), dcMem, 0, 0, SRCCOPY);
			BOOST_ASSERT(result != FALSE);
		}
		dcMem.SelectBitmap(hOldBitmap);
	}
};
#pragma pack(pop)
#pragma warning(pop)

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class MainView : public CWindowImpl<MainView>, boost::noncopyable {
public:
	BEGIN_MSG_MAP(MainView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnResize)
	END_MSG_MAP()

	bool UpdateLayout(void) {
		if(!image_view) {
			return true;
		}
		CRect my_rect;
		if(FALSE == GetClientRect(my_rect)) {
			return false;
		}
		if(my_rect.right == 0 || my_rect.bottom == 0) {
			return true;
		}
		CRect list_rect;
		image_view->MoveWindow(6, 6, 480 + ::GetSystemMetrics(SM_CXEDGE), 360 + ::GetSystemMetrics(SM_CXEDGE));
		return true;
	}
protected:
	boost::shared_ptr<ImageView> image_view;

	int OnCreate(LPCREATESTRUCT) {
		const int ret = DefWindowProc();
		image_view.reset(new ImageView(L"00000.jpeg"));
		image_view->Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER);
		BOOST_ASSERT(image_view);
		return ret;
	}

	void OnResize(UINT flag, CSize) {
		if(flag == SIZE_RESTORED) {
			const bool result = UpdateLayout();
			BOOST_ASSERT(result);
		}
	}
};
#pragma pack(pop)
#pragma warning(pop)

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class AppPathConfigPage : public CPropertyPageImpl<AppPathConfigPage>, boost::noncopyable {
public:
	enum { IDD = IDD_PATH_CONFIG };

	// コンストラクタ
	AppPathConfigPage(ATL::_U_STRINGorID title = reinterpret_cast<LPCTSTR>(NULL)) :
		CPropertyPageImpl<AppPathConfigPage>(title), path_column_index(1), app_name_column_index(0)
	{
	}

	BEGIN_MSG_MAP(AppPathConfigPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		NOTIFY_HANDLER_EX(IDC_PATH_LIST, LVN_ITEMCHANGED, OnSelect)
		COMMAND_HANDLER_EX(IDC_EDIT_PATH, EN_CHANGE, OnPathEdit)
		COMMAND_HANDLER_EX(IDC_BUTTON_PATH_EDIT, BN_CLICKED, OnPathEditButtonClick)
		CHAIN_MSG_MAP(CPropertyPageImpl<AppPathConfigPage>)
	END_MSG_MAP()

	bool SetCurApp(TH_APP_ID id) {
		const unsigned int index = static_cast<unsigned int>(id);
		return SetCurIndex(index);
	}
	bool SetCurIndex(unsigned int index) {
		if(index >= static_cast<unsigned int>(list_view.GetItemCount())) {
			return false;
		}
		last_select_index = index;
		CString str;
		list_view.GetItemText(static_cast<int>(index), path_column_index, str);
		path_edit.SetWindowText(str);
		if(static_cast<unsigned int>(list_view.GetSelectedIndex()) != index) {
			const BOOL result = list_view.SelectItem(static_cast<int>(index));
			BOOST_ASSERT(result == TRUE);
		}
		return true;
	}
protected:
	const int path_column_index;
	const int app_name_column_index;
	unsigned int last_select_index;
	CListViewCtrl list_view;
	CEdit path_edit;
	CButton button_path_edit;

	BOOL OnInitDialog(HWND, LPARAM){
		list_view = GetDlgItem(IDC_PATH_LIST);
		path_edit = GetDlgItem(IDC_EDIT_PATH);
		button_path_edit = GetDlgItem(IDC_BUTTON_PATH_EDIT);

		list_view.SetExtendedListViewStyle(LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
		// リストビューコントロールにカラム挿入
		{
			CRect rcList;
			list_view.GetWindowRect(rcList);
			const unsigned int nScrollWidth = static_cast<unsigned int>(::GetSystemMetrics(SM_CXVSCROLL));
			const unsigned int n3DEdge = static_cast<unsigned int>(::GetSystemMetrics(SM_CXEDGE));
			unsigned int width[] = {100, 280};
			const TCHAR * const name[] = {_T("Game"),_T("Path")};
			BOOST_ASSERT(sizeof(width) / sizeof(*width) == sizeof(name) / sizeof(*name));
			const unsigned int column_count = sizeof(width) / sizeof(*width);
			const unsigned int all_width = std::accumulate(&width[0], &width[column_count - 1], static_cast<unsigned int>(0));
			width[column_count - 1] = rcList.Width() - all_width - nScrollWidth - n3DEdge * 2;
			for(unsigned int i = 0; i < column_count; i++) {
				list_view.InsertColumn(static_cast<int>(i), name[i], LVCFMT_LEFT, static_cast<int>(width[i]), -1);
			}
		}
		const unsigned int column_count = static_cast<unsigned int>(list_view.GetHeader().GetItemCount());
		BOOST_FOREACH(const TCHAR * const app_name, app_name_list) {
			const unsigned int index = static_cast<unsigned int>(list_view.GetItemCount());
			for(unsigned int i = 0; i < column_count; i++) {
				const TCHAR *text = NULL;
				switch(i) {
					case 0:
						text = app_name;
						break;
					case 1:
						text = _T("c:\\Program Files(x86)\\東方紅魔郷\\");
						break;
					default:
						BOOST_ASSERT(false);
						break;
				}
				BOOST_ASSERT(text != NULL);
				list_view.AddItem(static_cast<int>(index), static_cast<int>(i), text);
			}
		}
		list_view.SelectItem(0);
		Update();
		return TRUE;
	}

	bool Update(void) {
		const int index = list_view.GetSelectedIndex();
		if(index == -1) {
			return true;
		}
		BOOST_ASSERT(index >= 0);
		if(!SetCurIndex(static_cast<unsigned int>(index))) {
			return false;
		}
		return true;
	}

	LRESULT OnSelect(LPNMHDR) {
		const bool result = Update();
		BOOST_ASSERT(result);
		return 0; // unused
	}

	void OnPathEdit(void) {
		TCHAR text[MAX_PATH];
		CString str;
		path_edit.GetWindowText(text, MAX_PATH);
		list_view.SetItemText(static_cast<int>(last_select_index), path_column_index, text);
	}
	void OnPathEdit(UINT, int, HWND) {
		OnPathEdit();
	}

	void OnPathEditButtonClick(UINT, int, HWND) {
		CString app_name;
		list_view.GetItemText(static_cast<int>(last_select_index), app_name_column_index, app_name);
		CString app_path;
		list_view.GetItemText(static_cast<int>(last_select_index), path_column_index, app_path);
		CString title;
		title.Format(_T("%s: フォルダを選択してください"), static_cast<LPCTSTR>(app_name));
		CFolderDialog dlg(m_hWnd, title);
		dlg.SetInitialFolder(app_path);

		if(dlg.DoModal() == IDOK){
			path_edit.SetWindowText(dlg.GetFolderPath());
			OnPathEdit();
		}
	}
};
#pragma pack(pop)
#pragma warning(pop)

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class CCustomSheet : public CPropertySheetImpl<CCustomSheet> {
public:
    AppPathConfigPage m_pageCustom;

    // コンストラクタ
    CCustomSheet(ATL::_U_STRINGorID title = (LPCTSTR)NULL,
        UINT uStartPage = 0, HWND hWndParent = NULL)
        : CPropertySheetImpl<CCustomSheet>(title, uStartPage, hWndParent)
    {
        m_psh.dwFlags |= PSH_NOAPPLYNOW;

        AddPage(m_pageCustom);
    }

    BEGIN_MSG_MAP(CCustomSheet)
        CHAIN_MSG_MAP(CPropertySheetImpl<CCustomSheet>)
    END_MSG_MAP()

    static int CALLBACK PropSheetCallback(HWND hWnd, UINT uMsg, LPARAM lParam){
        if(uMsg == PSCB_PRECREATE){
            // コンテキストヘルプボタンを削除
            ((LPDLGTEMPLATE)lParam)->style &= ~DS_CONTEXTHELP;

            return 0;
        }

        return CPropertySheetImpl<CCustomSheet>::PropSheetCallback(hWnd, uMsg, lParam);
    }
};
#pragma pack(pop)
#pragma warning(pop)

#define IDC_APP_SELECTOR	1000
#define IDC_MODE_SELECTOR	1001

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class CMainFrame : public CFrameWindowImpl<CMainFrame>, boost::noncopyable {
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	BEGIN_MSG_MAP(CMainFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_TIMER(OnTimer)
		COMMAND_HANDLER_EX(IDC_APP_SELECTOR, USER_NOTIFY_APP_SELECT, OnAppChange)
		COMMAND_HANDLER_EX(IDC_MODE_SELECTOR, USER_NOTIFY_MODE_SELECT, OnModeChange)
		COMMAND_ID_HANDLER_EX(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER_EX(IDM_OPTION, OnToolOption)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	void OnAppChange(void) {
		const unsigned int app_id = static_cast<unsigned int>(app_select_view.GetCurAppID());
		const bool load_result = list.Load(scene_list_json_path_list[app_id], hit_data_list_json_path_list[app_id]);
		BOOST_ASSERT(load_result);
		const bool result = mode_select_view.ChangeModeList(app_id);
		BOOST_ASSERT(result);
		prev_mode_id = UINT_MAX;
		mode_select_view.SetCurSel(0);
		OnModeChange();
	}

	void OnModeChange(void) {
		const int mode_id = mode_select_view.GetCurSel();
		BOOST_ASSERT(mode_id >= 0);
		if(prev_mode_id == static_cast<unsigned int>(mode_id)) {
			return;
		}
		prev_mode_id = static_cast<unsigned int>(mode_id);
		// TODO
	}
protected:
	HitDataList list;
	CSplitterWindow root_window;
	CHorSplitterWindow left_window;
	ModeSelectView mode_select_view;
	AppSelectView app_select_view;
	MainView right_window;
	unsigned int prev_mode_id;

	int OnCreate(LPCREATESTRUCT) {
		const LRESULT result = DefWindowProc();
		root_window.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		root_window.SetSplitterExtendedStyle(SPLIT_NONINTERACTIVE);
		left_window.Create(root_window, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		left_window.SetSplitterExtendedStyle(SPLIT_NONINTERACTIVE);

		app_select_view.Create(left_window, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, IDC_APP_SELECTOR);
		mode_select_view.Create(left_window, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, IDC_MODE_SELECTOR);
		left_window.SetSplitterPanes(app_select_view, mode_select_view);

		right_window.Create(root_window, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
		root_window.SetSplitterPanes(left_window, right_window);

		m_hWndClient = root_window;
		UpdateLayout();
 
		root_window.SetSplitterPos(static_cast<int>(app_select_view.GetMinWidth()));
		left_window.SetSplitterPos(static_cast<int>(app_select_view.GetMinHeight()));

		app_select_view.SetCurSel(0);
		OnAppChange();

		const unsigned int timer_result = ::SetTimer(m_hWnd, 0, 5000, NULL);
		BOOST_ASSERT(timer_result != 0);

		return result;
	}

	void OnDestroy(){
		SetMsgHandled(false);
	}

	void OnModeChange(UINT, int, HWND) {
		OnModeChange();
	}
	void OnAppChange(UINT, int, HWND) {
		OnAppChange();
	}
	void OnFileExit(UINT , int, HWND) {
		PostMessage(WM_CLOSE);
	}
	void OnToolOption(UINT, int, HWND) {
		CCustomSheet dlg;
		dlg.DoModal(m_hWnd);
	}
	void OnTimer(UINT_PTR) {
		const std::map<const wchar_t *, const wchar_t *> app_list = boost::assign::map_list_of(L"th13.exe", L"TH13TrHook.dll");
		typedef std::pair<const wchar_t *, const wchar_t *> APP_PAIR;
		BOOST_FOREACH(const APP_PAIR &pair, app_list) {
			const bool result = DllInjection(pair.first, pair.second);
			BOOST_ASSERT(result);
		}
	}
};
#pragma pack(pop)
#pragma warning(pop)


void TerminateCAppModule(CAppModule *ptr) {
	if(ptr != NULL) {
		ptr->RemoveMessageLoop();
		ptr->Term();
		delete ptr;
	}
}

bool Initialize(void) {
	const bool set_capture_path = SharedMemory::SetCaptureOutPath("capture");
	if(!set_capture_path) {
		return false;
	}
	const bool set_hit_data_list_path = SharedMemory::SetHitDataListOutPath("hit_data_list");
	if(!set_hit_data_list_path) {
		return false;
	}
	return true;
}

#if !defined(_DEBUG)
int WINAPI _tWinMain(HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
#else
int main() {
	const int nCmdShow = SW_SHOWNORMAL;
#endif

	if(!Initialize()) {
		return false;
	}

	boost::shared_ptr<CAppModule> app_module(new CAppModule(), &TerminateCAppModule);
	if(FAILED(app_module->Init(NULL, ::GetModuleHandle(NULL)))) {
		return 1;
	}

	CMessageLoop theLoop;
	if(FALSE == app_module->AddMessageLoop(&theLoop)) {
		return 1;
	}

	CMainFrame main_window;
	if(NULL == main_window.CreateEx(NULL, NULL, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_GROUP)) {
		return 1;
	}
	main_window.ResizeClient(760, 480);
	main_window.ShowWindow(nCmdShow);

	return theLoop.Run();
}
