
//�O�����C�u�����[�̌x���͖��������Ă���
#pragma warning(push)
#pragma warning(disable: 4061) // case��`����Ă��Ȃ��񋓎q������܂�
#pragma warning(disable: 4100) // �錾���ꂽ�������g�p���Ă��܂���
#pragma warning(disable: 4127) // ���������萔�ł�
#pragma warning(disable: 4180) // �֐��|�C���^�[�ɑ΂��Ė����ȑ����q���g�p���Ă��܂�
#pragma warning(disable: 4189) // �ϐ��ɑ����Q�Ƃ���Ă��܂���
#pragma warning(disable: 4191) // �֐��|�C���^�[�̃L���X�g�͊댯�ł�
#pragma warning(disable: 4201) // ��W���̊g���@�\�A�����̍\����/���p�̂��g�p���Ă��܂�
#pragma warning(disable: 4242) // �T�C�Y�������Ȍ^�ւƌ^�ϊ����s���܂���
#pragma warning(disable: 4244) // ���������_�^���琮���^�֌^�ϊ����s���܂���
#pragma warning(disable: 4245) // const�t���ϐ�����signed/unsigned���Ⴄ�^�ϊ����s���܂���
#pragma warning(disable: 4263) // ���z�֐����I�[�o�[���C�h���Ă��܂����A�����̌^���Ⴂ�܂�
#pragma warning(disable: 4264) // �����̌^�݈̂Ⴄ�����֐�����`���ꂽ���߁A���z�֐��ɂ̓A�N�Z�X�ł��Ȃ��Ȃ�܂�
#pragma warning(disable: 4265) // ���z�֐����܂ރN���X�̃f�X�g���N�^���񉼑z�֐��ł�
#pragma warning(disable: 4266) // ���z�֐��̂��ׂẴI�[�o�[���[�h���I�[�o�[���C�h���Ă��܂���
#pragma warning(disable: 4302) // �L���X�g��̌^�̕������������ߏ�񂪌����Ă���\��������܂�
#pragma warning(disable: 4350) // const�Ȃ��̎Q�Ɠn���̓��e�������󂯎�邱�Ƃ��ł��Ȃ����߁A�ʂ̃R���X�g���N�^���Ă΂�܂�
#pragma warning(disable: 4365) // signed/unsigned���Ⴄ�^�ϊ����s���܂���
#pragma warning(disable: 4505) // �Q�Ƃ���Ă��Ȃ����[�J���֐��͍폜����܂���
#pragma warning(disable: 4512) // ������Z�q�𐶐����邽�߂ɕK�v�ȃR���X�g���N�^���A�N�Z�X�ł��܂���A������Z�q�����܂���ł���
#pragma warning(disable: 4548) // �����ȃJ���}��
#pragma warning(disable: 4555) // ����p�̂Ȃ���
#pragma warning(disable: 4571) // catch(...)�ō\������O(SEH)�̓L���b�`����܂���
#pragma warning(disable: 4611) // ���ɂ�蓮�삪�قȂ�\���̂���@�\���g�p����Ă��܂�
#pragma warning(disable: 4619) // �����������݂��x���ԍ��͑��݂��܂���
#pragma warning(disable: 4625) // ��{�N���X�̃R�s�[�R���X�g���N�^���A�N�Z�X�s�\�Ȃ̂ŃR�s�[�R���X�g���N�^�����܂���ł���
#pragma warning(disable: 4626) // ��{�N���X�̑�����Z�q���A�N�Z�X�s�\�Ȃ̂ő�����Z�q�����܂���ł���
#pragma warning(disable: 4640) // static�ȃ��[�J���ϐ��̏��������X���b�h�Z�[�t�ł͂���܂���
#pragma warning(disable: 4668) // ��`����Ă��Ȃ��V���{����#if/#elif�Ŏg�p����܂���
#pragma warning(disable: 4738) // ���������_�̌v�Z���ʂ�32bit�Ɋi�[���Ă��邽�߃p�t�H�[�}���X���ቺ���Ă��܂�
#pragma warning(disable: 4820) // �\���̂̃p�b�e�B���O���������܂���
#pragma warning(disable: 4928) // �����̃��[�U�[��`�̈Öق̌^�ϊ����s���Ă��܂�
#pragma warning(disable: 4986) // �ڍוs��
#pragma warning(disable: 4987) // ��W���̊g���@�\���g�p����Ă��܂�
#pragma warning(disable: 4996) // �Z�L�����e�B�[�z�[���ɂȂ肤��W��C�֐����g�p����Ă��܂�

// RELEASE�r���h���̂ݔ�������x���̖�����
#ifndef _DEBUG
#pragma warning(disable: 4710) // __inline�錾����Ă���֐�/���\�b�h��inline�W�J���܂���ł���
#pragma warning(disable: 4711) // __inline�錾����Ă��Ȃ��֐�/���\�b�h��inline�W�J���܂���
#pragma warning(disable: 4714) // __forceinline�錾����Ă���֐�/���\�b�h��inline�W�J���܂���ł���
#endif

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <numeric>

#include <windows.h>
#include <windowsx.h>
#include <tlhelp32.h>

#include "jpeglib.h"
#include "png.h"

// boost/bind.hpp�Ŋe��Ăяo���K��ɑΉ������邽�߂̃}�N����`
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
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/detail/utf8_codecvt_facet.hpp"
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "boost/gil/gil_all.hpp"
#include "boost/gil/extension/io/jpeg_io.hpp"
// libpng1.4����ꕔ�萔���폜���ꂽ���߁A�Ȃ���Β�`���Ă��
#if !defined png_infopp_NULL
#define png_infopp_NULL (png_infopp)NULL
#endif
#if !defined int_p_NULL
#define int_p_NULL (int*)NULL
#endif
#include "boost/gil/extension/io/png_io.hpp"
#include "boost/gil/extension/numeric/sampler.hpp"
#include "boost/gil/extension/numeric/resample.hpp"
#include "boost/thread.hpp"
#include "boost/interprocess/managed_shared_memory.hpp"
#include "boost/interprocess/sync/interprocess_mutex.hpp"
#include "boost/interprocess/sync/scoped_lock.hpp"

// atl�����Ōx���������g�p����Ă���̂�include�͍Ō�
#define _ATL_ALL_WARNINGS
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>

#include <atlcrack.h>
#include <atlmisc.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlframe.h>
#include <atlsplit.h>

#pragma warning(pop)
//�O�����C�u�����[�̌x�����������܂�

#pragma warning(disable: 4127) // ���������萔�ł�
#pragma warning(disable: 4191) // �֐��|�C���^�[�̃L���X�g�͊댯�ł�
#pragma warning(disable: 4350) // const�Ȃ��̎Q�Ɠn���̓��e�������󂯎�邱�Ƃ��ł��Ȃ����߁A�ʂ̃R���X�g���N�^���Ă΂�܂�
#pragma warning(disable: 4503) // ��������4096�����𒴂������ߐ؂�̂Ă��܂�
#pragma warning(disable: 4505) // �Q�Ƃ���Ă��Ȃ����[�J���֐��͍폜����܂���
#pragma warning(disable: 4514) // �g�p����Ă��Ȃ��֐�/���\�b�h���폜����܂���
#pragma warning(disable: 4640) // static�ȃ��[�J���ϐ��̏��������X���b�h�Z�[�t�ł͂���܂���
#pragma warning(disable: 4710) // �C�����C���֐��Ƃ��đI������܂������C�����C���W�J�ł��܂���ł���

// RELEASE�r���h���̂ݔ�������x���̖�����
#ifndef _DEBUG
#pragma warning(disable: 4100) // �錾���ꂽ�������g�p���Ă��܂���
#pragma warning(disable: 4189) // �ϐ��ɑ����Q�Ƃ���Ă��܂���
#pragma warning(disable: 4710) // __inline�錾����Ă���֐�/���\�b�h���C�����C���W�J���܂���ł���
#pragma warning(disable: 4711) // __inline�錾����Ă��Ȃ��֐�/���\�b�h���C�����C���W�J���܂���
#pragma warning(disable: 4714) // __forceinline�錾����Ă���֐�/���\�b�h��inline�W�J���܂���ł���
#endif

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include "org/click3/utility.h"
#include "org/click3/image.h"

#include "resource.h"
#include "dll_injection.h"
#include "../common/shared_memory.h"
#include "stage_scene_setting.h"
