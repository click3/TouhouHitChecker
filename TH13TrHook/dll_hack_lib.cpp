
//外部ライブラリーの警告は無効化しておく
#pragma warning(push)
#pragma warning(disable: 4061) // case定義されていない列挙子があります
#pragma warning(disable: 4100) // 宣言された引数を使用していません
#pragma warning(disable: 4127) // 条件式が定数です
#pragma warning(disable: 4180) // 関数ポインターに対して無効な装飾子を使用しています
#pragma warning(disable: 4189) // 変数に代入後参照されていません
#pragma warning(disable: 4201) // 非標準の拡張機能、無名の構造体/共用体を使用しています
#pragma warning(disable: 4263) // 仮想関数をオーバーライドしていますが、引数の型が違います
#pragma warning(disable: 4264) // 引数の型のみ違う同名関数が定義されたため、仮想関数にはアクセスできなくなります
#pragma warning(disable: 4266) // 仮想関数のすべてのオーバーロードをオーバーライドしていません
#pragma warning(disable: 4350) // constなしの参照渡しはリテラルを受け取ることができないため、別のコンストラクタが呼ばれます
#pragma warning(disable: 4365) // signed/unsignedが違う型変換が行われました
#pragma warning(disable: 4512) // 代入演算子を生成するために必要なコンストラクタがアクセスできません、代入演算子を作れませんでした
#pragma warning(disable: 4548) // 無効なカンマ式
#pragma warning(disable: 4555) // 副作用のない式
#pragma warning(disable: 4619) // 無効化を試みた警告番号は存在しません
#pragma warning(disable: 4625) // 基本クラスのコピーコンストラクタがアクセス不能なのでコピーコンストラクタが作れませんでした
#pragma warning(disable: 4626) // 基本クラスの代入演算子がアクセス不能なので代入演算子が作れませんでした
#pragma warning(disable: 4640) // staticなローカル変数の初期化がスレッドセーフではありません
#pragma warning(disable: 4668) // 定義されていないシンボルが#if/#elifで使用されました
#pragma warning(disable: 4738) // 浮動小数点の計算結果を32bitに格納しているためパフォーマンスが低下しています
#pragma warning(disable: 4820) // 構造体のパッティングが発生しました
#pragma warning(disable: 4986) // 詳細不明

// RELEASEビルド時のみ発生する警告の無効化
#ifndef _DEBUG
#pragma warning(disable: 4710) // inline宣言されている関数/メソッドをinline展開しませんでした
#pragma warning(disable: 4711) // inline宣言されていない関数/メソッドをinline展開しました
#endif

#include <stdio.h>

//wchar_tを既定の文字列として使用しない
//#define UNICODE
//std::minとstd::maxと定義が重複するため
#define NOMINMAX
#include <windows.h>

#include "boost/assert.hpp"
#include "boost/static_assert.hpp"

#pragma warning(pop)
//外部ライブラリーの警告無効ここまで

#pragma warning(disable: 4061) // case定義されていない列挙子があります
#pragma warning(disable: 4127) // 条件式が定数です
#pragma warning(disable: 4350) // constなしの参照渡しはリテラルを受け取ることができないため、別のコンストラクタが呼ばれます
#pragma warning(disable: 4503) // 装飾名が4096文字を超えたため切り捨てられます
#pragma warning(disable: 4514) // 使用されていない関数/メソッドが削除されました
#pragma warning(disable: 4640) // staticなローカル変数の初期化がスレッドセーフではありません
#pragma warning(disable: 4710) // インライン関数として選択されましたがインライン展開できませんでした

// RELEASEビルド時のみ発生する警告の無効化
#ifndef _DEBUG
#pragma warning(disable: 4100) // 宣言された引数を使用していません
#pragma warning(disable: 4189) // 変数に代入後参照されていません
#pragma warning(disable: 4710) // inline宣言されている関数/メソッドをinline展開しませんでした
#pragma warning(disable: 4711) // inline宣言されていない関数/メソッドをinline展開しました
#endif

namespace org { namespace click3 { namespace DllHackLib {

namespace {

FILE *stdout_fp;
FILE *stderr_fp;
FILE *stdin_fp;

} // anonymous

unsigned int GetPageSize(void) {
	SYSTEM_INFO info;
	::GetSystemInfo(&info);
	return info.dwPageSize;
}

void *GetBaseAddr(const void *addr) {
	BOOST_ASSERT(addr != NULL);
	MEMORY_BASIC_INFORMATION mbi;
	::VirtualQuery(addr, &mbi, sizeof(mbi));
	return mbi.BaseAddress;
}

bool ChangeProtect(unsigned int *old_protect, const void *addr, unsigned int new_protect) {
	void * const base_addr = GetBaseAddr(addr);
	const unsigned int page_size = GetPageSize();
	BOOST_STATIC_ASSERT(sizeof(DWORD) == sizeof(unsigned int));
	if(0 == ::VirtualProtect(base_addr, page_size, new_protect, reinterpret_cast<DWORD *>(old_protect))) {
		return false;
	}
	return true;
}

bool ChangeCode(unsigned int addr, const unsigned char *old_code, const unsigned char *new_code, unsigned int size) {
	BOOST_ASSERT(old_code != NULL);
	BOOST_ASSERT(new_code != NULL);
	unsigned char *write_ptr = reinterpret_cast<unsigned char *>(addr);
	unsigned int old_protect;
	if(!ChangeProtect(&old_protect, write_ptr, PAGE_EXECUTE_READWRITE)) {
		return false;
	}
	if(::memcmp(write_ptr, old_code, size) != 0) {
		return false;
	}
	::memcpy(write_ptr, new_code, size);
	unsigned int protect;
	if(!ChangeProtect(&protect, write_ptr, old_protect) || protect != PAGE_EXECUTE_READWRITE) {
		return false;
	}
	return true;
}

bool WriteCode(unsigned char *old_code, unsigned int addr, const unsigned char *new_code, unsigned int size) {
	BOOST_ASSERT(new_code != NULL);
	unsigned char *write_ptr = reinterpret_cast<unsigned char *>(addr);
	unsigned int old_protect;
	if(!ChangeProtect(&old_protect, write_ptr, PAGE_EXECUTE_READWRITE)) {
		return false;
	}
	if(old_code != NULL) {
		::memcpy(old_code, write_ptr, size);
	}
	::memcpy(write_ptr, new_code, size);
	unsigned int protect;
	if(!ChangeProtect(&protect, write_ptr, old_protect) || protect != PAGE_EXECUTE_READWRITE) {
		return false;
	}
	return true;
}

bool SetupConsole() {
	if(FALSE == ::AllocConsole()) {
		return false;
	}
	const errno_t out_error = ::freopen_s(&stdout_fp, "CONOUT$", "w", stdout);
	if(out_error != 0 || stdout_fp == NULL) {
		return false;
	}
	const errno_t in_error = ::freopen_s(&stdin_fp, "CONIN$", "r", stdin);
	if(in_error != 0 || stdin_fp == NULL) {
		return false;
	}
	const errno_t err_error = ::freopen_s(&stderr_fp, "CONOUT$", "w", stderr);
	if(err_error != 0 || stderr_fp == NULL) {
		return false;
	}
	return true;
}

} // org

} // click3

} //DllHackLib
