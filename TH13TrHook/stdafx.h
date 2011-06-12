
//外部ライブラリーの警告は無効化しておく
#pragma warning(push)
#pragma warning(disable: 4061) // case定義されていない列挙子があります
#pragma warning(disable: 4100) // 宣言された引数を使用していません
#pragma warning(disable: 4127) // 条件式が定数です
#pragma warning(disable: 4180) // 関数ポインターに対して無効な装飾子を使用しています
#pragma warning(disable: 4189) // 変数に代入後参照されていません
#pragma warning(disable: 4191) // 関数ポインターのキャストは危険です
#pragma warning(disable: 4201) // 非標準の拡張機能、無名の構造体/共用体を使用しています
#pragma warning(disable: 4242) // サイズが小さな型へと型変換が行われました
#pragma warning(disable: 4244) // 浮動小数点型から整数型へ型変換が行われました
#pragma warning(disable: 4245) // const付き変数からsigned/unsignedが違う型変換が行われました
#pragma warning(disable: 4263) // 仮想関数をオーバーライドしていますが、引数の型が違います
#pragma warning(disable: 4264) // 引数の型のみ違う同名関数が定義されたため、仮想関数にはアクセスできなくなります
#pragma warning(disable: 4265) // 仮想関数を含むクラスのデストラクタが非仮想関数です
#pragma warning(disable: 4266) // 仮想関数のすべてのオーバーロードをオーバーライドしていません
#pragma warning(disable: 4302) // キャスト先の型の方が小さいため情報が欠けている可能性があります
#pragma warning(disable: 4350) // constなしの参照渡しはリテラルを受け取ることができないため、別のコンストラクタが呼ばれます
#pragma warning(disable: 4365) // signed/unsignedが違う型変換が行われました
#pragma warning(disable: 4505) // 参照されていないローカル関数は削除されました
#pragma warning(disable: 4512) // 代入演算子を生成するために必要なコンストラクタがアクセスできません、代入演算子を作れませんでした
#pragma warning(disable: 4548) // 無効なカンマ式
#pragma warning(disable: 4555) // 副作用のない式
#pragma warning(disable: 4571) // catch(...)で構造化例外(SEH)はキャッチされません
#pragma warning(disable: 4611) // 環境により動作が異なる可能性のある機能が使用されています
#pragma warning(disable: 4619) // 無効化を試みた警告番号は存在しません
#pragma warning(disable: 4625) // 基本クラスのコピーコンストラクタがアクセス不能なのでコピーコンストラクタが作れませんでした
#pragma warning(disable: 4626) // 基本クラスの代入演算子がアクセス不能なので代入演算子が作れませんでした
#pragma warning(disable: 4640) // staticなローカル変数の初期化がスレッドセーフではありません
#pragma warning(disable: 4668) // 定義されていないシンボルが#if/#elifで使用されました
#pragma warning(disable: 4738) // 浮動小数点の計算結果を32bitに格納しているためパフォーマンスが低下しています
#pragma warning(disable: 4820) // 構造体のパッティングが発生しました
#pragma warning(disable: 4928) // 複数のユーザー定義の暗黙の型変換が行われています
#pragma warning(disable: 4986) // 詳細不明
#pragma warning(disable: 4987) // 非標準の拡張機能が使用されています
#pragma warning(disable: 4996) // セキュリティーホールになりうる標準C関数が使用されています

// RELEASEビルド時のみ発生する警告の無効化
#ifndef _DEBUG
#pragma warning(disable: 4710) // __inline宣言されている関数/メソッドをinline展開しませんでした
#pragma warning(disable: 4711) // __inline宣言されていない関数/メソッドをinline展開しました
#pragma warning(disable: 4714) // __forceinline宣言されている関数/メソッドをinline展開しませんでした
#endif

#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <numeric>

//wchar_tを既定の文字列として使用しない
//#define UNICODE
//std::minとstd::maxと定義が重複するため
#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>

// boost/bind.hppで各種呼び出し規約に対応させるためのマクロ定義
#define BOOST_BIND_ENABLE_STDCALL
#define BOOST_MEM_FN_ENABLE_CDECL
#define BOOST_MEM_FN_ENABLE_STDCALL
#define BOOST_BIND_ENABLE_FASTCALL
#define BOOST_MEM_FN_ENABLE_FASTCALL
#define BOOST_MEM_FN_ENABLE_CDECL

#include "boost/version.hpp"
#include "boost/optional.hpp"
#include "boost/assign.hpp"
#include "boost/utility.hpp"
#include "boost/assert.hpp"
#include "boost/static_assert.hpp"
#include "boost/type_traits/remove_pointer.hpp"
#include "boost/exception/all.hpp"
#include "boost/thread.hpp"
#include "boost/gil/gil_all.hpp"
#include "boost/gil/extension/io/png_io.hpp"
#include "boost/gil/extension/io/jpeg_io.hpp"
#include "boost/filesystem.hpp"
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/preprocessor.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/foreach.hpp"
#include "boost/filesystem/detail/utf8_codecvt_facet.hpp"
#include "boost/crc.hpp"
#include "boost/interprocess/managed_shared_memory.hpp"
#include "boost/interprocess/sync/interprocess_mutex.hpp"
#include "boost/interprocess/sync/scoped_lock.hpp"

#pragma comment (lib, "user32.lib")
#pragma comment (lib, "gdi32.lib")
#pragma comment (lib, "shlwapi.lib")
#pragma comment (lib, "d3dx9.lib")

#pragma warning(pop)
//外部ライブラリーの警告無効ここまで

#pragma warning(disable: 4127) // 条件式が定数です
#pragma warning(disable: 4191) // 関数ポインターのキャストは危険です
#pragma warning(disable: 4350) // constなしの参照渡しはリテラルを受け取ることができないため、別のコンストラクタが呼ばれます
#pragma warning(disable: 4503) // 装飾名が4096文字を超えたため切り捨てられます
#pragma warning(disable: 4505) // 参照されていないローカル関数は削除されました
#pragma warning(disable: 4514) // 使用されていない関数/メソッドが削除されました
#pragma warning(disable: 4640) // staticなローカル変数の初期化がスレッドセーフではありません
#pragma warning(disable: 4710) // インライン関数として選択されましたがインライン展開できませんでした

// RELEASEビルド時のみ発生する警告の無効化
#ifndef _DEBUG
#pragma warning(disable: 4100) // 宣言された引数を使用していません
#pragma warning(disable: 4189) // 変数に代入後参照されていません
#pragma warning(disable: 4710) // inline宣言されている関数/メソッドをinline展開しませんでした
#pragma warning(disable: 4711) // inline宣言されていない関数/メソッドをinline展開しました
#pragma warning(disable: 4714) // __forceinline宣言されている関数/メソッドをinline展開しませんでした
#endif

#include "org/click3/utility.h"
#include "org/click3/notification_center.h"

#include "dll_hack_lib.h"
#include "../common/shared_memory.h"
#include "main.h"
