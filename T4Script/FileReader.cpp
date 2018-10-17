#include "FileRead.h"
#include <CKByteData.h>
#include <iostream>
FileReader::FileReader(const char* file)
{
	ck_reader = new CkByteData;
	ck_reader->loadFile(file);
}

const char* FileReader::get_string(unsigned long len)
{
	const char* ret = ck_reader->getRangeStr(file_pos, len);
	file_pos += len;

	return ret;
}

const char* FileReader::get_string()
{
	unsigned long temp_pos = file_pos;
	while (ck_reader->getByte(temp_pos) != 0)
	{
		temp_pos++;
	}

	int string_len = file_pos - temp_pos;

	const char* ret =  ck_reader->getRangeStr(file_pos, string_len);
	file_pos += string_len;

	return ret;
}

int FileReader::get_int()
{
	int ret = ck_reader->getInt(file_pos);
	file_pos += 4;

	return ret;
}

unsigned int FileReader::get_uint()
{
	unsigned int ret = ck_reader->getUInt(file_pos);
	file_pos += 4;

	return ret;
}

short FileReader::get_short()
{
	short ret = ck_reader->getShort(file_pos);
	file_pos += 2;

	return ret;
}

unsigned short FileReader::get_ushort()
{
	unsigned short ret = ck_reader->getUShort(file_pos);
	file_pos += 2;

	return ret;
}

char FileReader::get_char()
{
	char ret = ck_reader->getChar(file_pos);
	file_pos += 1;

	return ret;
}

unsigned char FileReader::get_byte()
{
	unsigned char ret = ck_reader->getByte(file_pos);
	file_pos += 1;

	return ret;
}

float FileReader::get_float()
{
	

	unsigned char* float_data = new unsigned char[4];
	float_data[0] = ck_reader->getByte(file_pos);
	float_data[1] = ck_reader->getByte(file_pos + 1);
	float_data[2] = ck_reader->getByte(file_pos + 2);
	float_data[3] = ck_reader->getByte(file_pos + 3);

	float ret;
	memcpy(&ret, float_data, sizeof ret);
	
	file_pos += 4;

	return ret;
}

void FileReader::seek(unsigned long len)
{
	file_pos += len;
}

bool FileReader::end_of_file()
{
	return (ck_reader->getSize() <= file_pos);
}

unsigned long FileReader::get_pos()
{
	return file_pos;
}