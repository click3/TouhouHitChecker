
//�O�����C�u�����[�̌x���͖��������Ă���
#pragma warning(push)
#pragma warning(disable: 4061) // case��`����Ă��Ȃ��񋓎q������܂�
#pragma warning(disable: 4100) // �錾���ꂽ�������g�p���Ă��܂���
#pragma warning(disable: 4127) // ���������萔�ł�
#pragma warning(disable: 4180) // �֐��|�C���^�[�ɑ΂��Ė����ȑ����q���g�p���Ă��܂�
#pragma warning(disable: 4189) // �ϐ��ɑ����Q�Ƃ���Ă��܂���
#pragma warning(disable: 4201) // ��W���̊g���@�\�A�����̍\����/���p�̂��g�p���Ă��܂�
#pragma warning(disable: 4263) // ���z�֐����I�[�o�[���C�h���Ă��܂����A�����̌^���Ⴂ�܂�
#pragma warning(disable: 4264) // �����̌^�݈̂Ⴄ�����֐�����`���ꂽ���߁A���z�֐��ɂ̓A�N�Z�X�ł��Ȃ��Ȃ�܂�
#pragma warning(disable: 4266) // ���z�֐��̂��ׂẴI�[�o�[���[�h���I�[�o�[���C�h���Ă��܂���
#pragma warning(disable: 4350) // const�Ȃ��̎Q�Ɠn���̓��e�������󂯎�邱�Ƃ��ł��Ȃ����߁A�ʂ̃R���X�g���N�^���Ă΂�܂�
#pragma warning(disable: 4365) // signed/unsigned���Ⴄ�^�ϊ����s���܂���
#pragma warning(disable: 4512) // ������Z�q�𐶐����邽�߂ɕK�v�ȃR���X�g���N�^���A�N�Z�X�ł��܂���A������Z�q�����܂���ł���
#pragma warning(disable: 4548) // �����ȃJ���}��
#pragma warning(disable: 4555) // ����p�̂Ȃ���
#pragma warning(disable: 4619) // �����������݂��x���ԍ��͑��݂��܂���
#pragma warning(disable: 4625) // ��{�N���X�̃R�s�[�R���X�g���N�^���A�N�Z�X�s�\�Ȃ̂ŃR�s�[�R���X�g���N�^�����܂���ł���
#pragma warning(disable: 4626) // ��{�N���X�̑�����Z�q���A�N�Z�X�s�\�Ȃ̂ő�����Z�q�����܂���ł���
#pragma warning(disable: 4640) // static�ȃ��[�J���ϐ��̏��������X���b�h�Z�[�t�ł͂���܂���
#pragma warning(disable: 4668) // ��`����Ă��Ȃ��V���{����#if/#elif�Ŏg�p����܂���
#pragma warning(disable: 4738) // ���������_�̌v�Z���ʂ�32bit�Ɋi�[���Ă��邽�߃p�t�H�[�}���X���ቺ���Ă��܂�
#pragma warning(disable: 4820) // �\���̂̃p�b�e�B���O���������܂���
#pragma warning(disable: 4986) // �ڍוs��

// RELEASE�r���h���̂ݔ�������x���̖�����
#ifndef _DEBUG
#pragma warning(disable: 4710) // inline�錾����Ă���֐�/���\�b�h��inline�W�J���܂���ł���
#pragma warning(disable: 4711) // inline�錾����Ă��Ȃ��֐�/���\�b�h��inline�W�J���܂���
#endif

#include <stdio.h>

//wchar_t������̕�����Ƃ��Ďg�p���Ȃ�
//#define UNICODE
//std::min��std::max�ƒ�`���d�����邽��
#define NOMINMAX
#include <windows.h>

#include "boost/assert.hpp"
#include "boost/static_assert.hpp"

#pragma warning(pop)
//�O�����C�u�����[�̌x�����������܂�

#pragma warning(disable: 4061) // case��`����Ă��Ȃ��񋓎q������܂�
#pragma warning(disable: 4127) // ���������萔�ł�
#pragma warning(disable: 4350) // const�Ȃ��̎Q�Ɠn���̓��e�������󂯎�邱�Ƃ��ł��Ȃ����߁A�ʂ̃R���X�g���N�^���Ă΂�܂�
#pragma warning(disable: 4503) // ��������4096�����𒴂������ߐ؂�̂Ă��܂�
#pragma warning(disable: 4514) // �g�p����Ă��Ȃ��֐�/���\�b�h���폜����܂���
#pragma warning(disable: 4640) // static�ȃ��[�J���ϐ��̏��������X���b�h�Z�[�t�ł͂���܂���
#pragma warning(disable: 4710) // �C�����C���֐��Ƃ��đI������܂������C�����C���W�J�ł��܂���ł���

// RELEASE�r���h���̂ݔ�������x���̖�����
#ifndef _DEBUG
#pragma warning(disable: 4100) // �錾���ꂽ�������g�p���Ă��܂���
#pragma warning(disable: 4189) // �ϐ��ɑ����Q�Ƃ���Ă��܂���
#pragma warning(disable: 4710) // inline�錾����Ă���֐�/���\�b�h��inline�W�J���܂���ł���
#pragma warning(disable: 4711) // inline�錾����Ă��Ȃ��֐�/���\�b�h��inline�W�J���܂���
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
