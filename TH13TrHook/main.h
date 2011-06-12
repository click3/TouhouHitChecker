
void Present(void);
void EndScene(void);
void _main(void);


#define SOUND_EFFECT_COUNT 59
#define SOUND_EFFECT_BUFFER_COUNT 70
#define NOTIFY_EVENT_PLAY_SOUND_EFFECT "notify_event_play_sound_effect"


#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
template<class T>
class SingleLinkedList : boost::noncopyable {
public:
	SingleLinkedList(T *owner, SingleLinkedList<T> *next = NULL) : owner(owner), next(next) { }
	T * const owner;
	SingleLinkedList<T> *next;
};
#pragma warning(pop)
#pragma pack(pop)

#pragma pack(push, 4)
struct SpellInfo {
	unsigned int unknown1;
	unsigned int unknown2;
	void *unknown3;
	void *unknown4;
	unsigned char unknown5[20];
	unsigned int frame_count;
	unsigned int frame_count2;
	float frame_count3;
	void *unknown6;
	unsigned int unknown7;
	char name[64];
	unsigned int unknown8;
	unsigned int unknown9;
	int bonus;
	unsigned char unknown10[40];
	float unknown11;			// 残り時間に合わせて縮んでいく輪っかエフェクト関係っぽい
	float unknown12;
	float unknown13;
	unsigned char unknown14[8];
};
#pragma pack(pop)

#pragma pack(push, 4)
struct MsgInfo {
	unsigned char unknown1[408];
	void *code;
	unsigned char unknown2[280];
};
#pragma pack(pop)

#pragma pack(push, 4)
struct PlayerInfo {
	unsigned char unknown[1524];
	unsigned int status;			// (0初期化中or復帰中、1通常、2死亡中、3不明、4喰らいボム受付時間中
	unsigned char unknown2[8];
	unsigned int status_frame_count;	// 現在のstatusになってからの経過フレーム数
};
#pragma pack(pop)

#pragma pack(push, 4)
struct StageInfo {
	unsigned int number;
	const char *std_filename;
	const char *stgenm_filename;
	const char *ecl_filename;
	const char *unknown1;
	const char *unknown2;
	const char *msg_filename[4];
	const char *logo_filename;
	unsigned int unknown3;
	unsigned int unknown4;
	unsigned int unknown5;
};
#pragma pack(pop)

#pragma pack(push, 4)
struct TransStruct {
	unsigned char unknown1[20];
	unsigned char mode;			// 0初期値or手動トランス終了後、1手動トランス中、2自動トランス終了後、3自動トランス中
	unsigned char unknown2[3];
};
#pragma pack(pop)

#pragma pack(push, 4)
struct BombInfoBase {
	void **vtable;		// 仮想関数テーブル
	unsigned int unknown1;
	unsigned int unknown2;
	void *unknown3;
	void *unknown4;
	unsigned char unknown5[44];
	bool is_run;
	unsigned char padding[3]; // unused
	unsigned char unknown6[1272];
};
#pragma pack(pop)

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4355)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
class ObjList : boost::noncopyable {
public:
	ObjList() : link(this) { }
	unsigned int unknown1;	// [0x00] = 0
	void *unknown2;		// [0x04] = -2
	unsigned int unknown3;	// [0x08] = 0
	unsigned int unknown4;	// [0x0C] = 0
	unsigned int unknown5;	// [0x10] = 0
	SingleLinkedList<ObjList> link;
	unsigned int unknown7;	// [0x1C] = 0
	unsigned int unknown8;	// [0x20] = 0
	void *unknown9;		// [0x24]
};
#pragma warning(pop)
#pragma pack(pop)

#pragma pack(push, 4)
#pragma warning(push)
#pragma warning(disable: 4625)
#pragma warning(disable: 4626)
struct Unknown2 {
	ObjList unknown1;
	ObjList unknown2;
	unsigned int unknown3;	// [0x50]
	unsigned int unknown4;	// [0x54] = 0
};
#pragma warning(pop)
#pragma pack(pop)

#pragma pack(push, 4)
struct SoundInfo {
	IDirectSoundBuffer8 *buffer;
	int unknown1;
	struct UnknownStruct1 {
		unsigned int id;
		unsigned int unknown1;
		unsigned int unknown2;
		unsigned int unknown3;
		unsigned int unknown4;
	};
	UnknownStruct1 *unknown2;
	unsigned char unknown3[12];
};
#pragma pack(pop)

#pragma pack(push, 4)
struct TH13Global {
	char *se_filename[SOUND_EFFECT_COUNT];
	unsigned char unknown1[11444];
	unsigned int player_id;		// 0霊夢、1魔理沙、2早苗、3妖夢
	unsigned char unknown2[4];
	unsigned int score;			// 1/10された数値
	unsigned int play_mode;		// 0easy,1normal,2hard,3lunatic,4extra?
	unsigned char unknown3[8];
	unsigned int graze;
	unsigned char unknown4[4];
	unsigned int max_point;
	unsigned int initial_max_point;
	unsigned int max_point_max;
	unsigned int power;
	unsigned int power_max;
	unsigned int power_unit;		// パワーの単位
	int life;
	unsigned int life_fragment;
	unsigned int extend_count;		// 残機が増えた回数
	unsigned int spell_count;
	unsigned int spell_fragment;
	unsigned int astral_count;		// 1ゲージあたり200、マックス600
	unsigned char unknown5[4];
	unsigned int hi_score;		// 1/10された数値
	unsigned char unknown6[8];
	unsigned int stage_number;
	unsigned int start_stage_number;
	unsigned int scene_id;		// 0道中、2中ボス、3ボス、4ボス戦終了～次ステージ開始まで
	unsigned int stage_frame_count;
	unsigned int scene_frame_count;
	unsigned int flags;			// 0x02ステージ移行中、0x10ハイスコア更新中、0x20タイトル画面に戻る？、0x80トランス中
	unsigned char unknown7[14596];
	TransStruct *trans;
	unsigned char unknown8[8];
	BombInfoBase *bomb;
	unsigned char unknown9[4];
	SpellInfo *spell;
	unsigned char unknown10[16];
	MsgInfo *msg;
	unsigned char unknown11[304];
	PlayerInfo *player;
	unsigned char unknown12[20];
	StageInfo *stage;
	unsigned char unknown13[107384];
	Unknown2 *unknown14;
	unsigned char unknown15[24];
	unsigned int engine_mode;		// 1通常、2リプレイ、3不明
	unsigned char unknown16[56];
	IDirect3D9 *d3d9_ptr;
	IDirect3DDevice9 *d3d9_device_ptr;
	unsigned char unknown17[483];
	unsigned char window_mode;		//0:フルスクリーン640*480、1:ウィンドウ640*480、2:ウィンドウ960*720、3:ウィンドウ1280*960
	unsigned char unknown18[2032];
	HWND main_hwnd;
	unsigned char unknown19[15228];
	SoundInfo sound[SOUND_EFFECT_BUFFER_COUNT];
	unsigned char *wav_buffer[SOUND_EFFECT_COUNT];
};
#pragma pack(pop)


extern bool windowed;
extern HWND main_hwnd;
extern boost::shared_ptr<IDirect3D9> d3d9_ptr;
extern boost::shared_ptr<IDirect3DDevice9> d3d9_device_ptr;
extern TH13Global * const th13_global;
extern const char * const sound_filename[SOUND_EFFECT_BUFFER_COUNT];
