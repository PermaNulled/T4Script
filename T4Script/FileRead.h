#pragma once
#include <CkByteData.h>

class FileReader {
	private:
		unsigned long file_pos = 0;
		CkByteData *ck_reader;

	public:
		FileReader(const char* file);

		const char* get_string();
		const char* get_string(unsigned long len);

		int get_int();
		unsigned int get_uint();

		short get_short();
		unsigned short get_ushort();
		
		char get_char();
		unsigned char get_byte();
		
		float get_float();

		bool end_of_file();

		void seek(unsigned long len);
		unsigned long get_pos();
		
};	