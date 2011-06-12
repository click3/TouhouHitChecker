
namespace org { namespace click3 { namespace DllHackLib {

unsigned int GetPageSize(void);
void *GetBaseAddr(const void *addr);
bool ChangeProtect(unsigned int *old_protect, const void *addr, unsigned int new_protect);
bool ChangeCode(unsigned int addr, const unsigned char *old_code, const unsigned char *new_code, unsigned int size);
bool WriteCode(unsigned char *old_code, unsigned int addr, const unsigned char *new_code, unsigned int size);

template<typename proc_type>
unsigned int GetOffset(proc_type temp_proc_ptr) {
	const unsigned char *data = *reinterpret_cast<unsigned char **>(&temp_proc_ptr);
	if(data[0] == 0xE9) {
		data += 5 + *reinterpret_cast<const unsigned int *>(&data[1]);
	}
	const unsigned char expected_code[] = {0x8b, 0x44, 0x24, 0x04, 0x8b, 0x00};
	BOOST_ASSERT(::memcmp(data, expected_code, sizeof(expected_code)) == 0);
	const unsigned char op_code[][2] = {{0xff, 0x60}, {0xff, 0xa0}};
	unsigned int op = UINT_MAX;
	for(unsigned int i = 0; i < sizeof(op_code) / sizeof(*op_code); i++) {
		if(::memcmp(&data[sizeof(expected_code)], op_code[i], sizeof(*op_code)) == 0) {
			op = i;
			break;
		}
	}
	const unsigned char * const ptr = &data[sizeof(expected_code) + sizeof(*op_code)];
	unsigned int result;
	switch(op) {
		case 0:
			result = *reinterpret_cast<const unsigned char *>(ptr) / sizeof(void *);
			break;
		case 1:
			result = *reinterpret_cast<const unsigned int *>(ptr) / sizeof(void *);
			break;
		default:
			BOOST_ASSERT(false);
			result = UINT_MAX;
			break;
	}
	return result;
}

template<typename proc_type>
bool ChangeVartualProcAddress(void *class_ptr, void **original_ptr, proc_type temp_proc_ptr, const void *new_addr) {
	const unsigned int offset = GetOffset(temp_proc_ptr);
	const unsigned int write_addr = *reinterpret_cast<unsigned int *>(class_ptr) + offset * sizeof(void *);
	return WriteCode(reinterpret_cast<unsigned char *>(original_ptr), write_addr, reinterpret_cast<const unsigned char *>(&new_addr), sizeof(new_addr));
}

bool SetupConsole();

} // org

} // click3

} //DllHackLib
