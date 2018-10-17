// T4Script.cpp : Defines the entry point for the console application.
//

#include <CkGlobal.h>
#include <iostream>
#include <fstream>
#include "FileRead.h"
#include "T4Script.h"

using namespace std;
/* Re-write and write a generic Object parser pass void* and then access the type and data elments and fill them, abort on size greater than what's assume for the name of the object so it can be corrected... 

Will need to ensure same struct for all objects before being able to do this... */
void ReadGenericObject(void* pObject)
{
	
}

//FileReader *fr = new FileReader("C:\\TUROK\\data\\levels\\u_jcowlishaw\\screens\\MultiPlayerJoin\\MultiPlayerJoin.ati");
FileReader *fr = new FileReader("C:\\TUROK\\data\\levels\\u_jcowlishaw\\screens\\InGamePause\\InGamePause.ati");
ofstream dfile;

void ReadEvents(CAUSE_PROPERTY *cProperty)
{


	cProperty->EventsBlockType = fr->get_byte();
	cProperty->EventsUnkByte = fr->get_byte();
	
	switch (cProperty->EventsBlockType)
	{
		case 0xA1:
			cProperty->EventsBlockSize = fr->get_ushort();
		break;

		case 0x61:
		case 0x6C:
			cProperty->EventsBlockSize = fr->get_byte();
		break;

		default:
			printf("Unknown event block type: %02X\r\n", cProperty->EventsBlockType);
			exit(0);
		break;
	}

	cProperty->EventsUnkByte2 = fr->get_byte();
	cProperty->EventsUnkByte3 = fr->get_byte();
	cProperty->EventsNameLength = fr->get_byte();

	cProperty->EventsName = new char[cProperty->EventsNameLength + 1];
	memset(cProperty->EventsName, 0x00, cProperty->EventsNameLength+1);
	memcpy(cProperty->EventsName, fr->get_string(cProperty->EventsNameLength), cProperty->EventsNameLength);
	fr->seek(1);

	printf("------- Reading Events Block: %s -------\r\n", cProperty->EventsName);
	printf("BlockType: %02X\r\n", cProperty->EventsBlockType);
	printf("BlockSize: %i\r\n", cProperty->EventsBlockSize);
	printf("UnkByte1: %i\r\n", cProperty->EventsUnkByte);
	printf("UnkByte2: %i\r\n", cProperty->EventsUnkByte2);
	printf("UnkByte3: %i\r\n", cProperty->EventsUnkByte3);
	printf("EventsNameSize: %i\r\n", cProperty->EventsNameLength);

	unsigned long event_start_pos = fr->get_pos();
	unsigned long current_event_pos = 0;
	unsigned long event_count = 0;

	dfile << "\t\t\t" << cProperty->EventsName << " = " << endl << "\t\t\t" << "{" << endl;
	while (current_event_pos != cProperty->EventsBlockSize)
	{
		PEVENT_PROPERTY pEvent = new EVENT_PROPERTY;
		pEvent->type = fr->get_byte();
		pEvent->UnkByte = fr->get_byte();

		switch (pEvent->type)
		{

			case 0x41:
			case 0x42:
				pEvent->size = fr->get_byte();
			break;

			case 0x82:
				pEvent->size = fr->get_ushort();
				printf("pEvent->size: %i\r\n", pEvent->size);
			break;

			default:
				printf("pEvent->type unknown: %02X\r\n", pEvent->type);
				exit(0);
			break;
		}
	

		//pEvent->size = fr->get_byte();


		pEvent->EVENTLEN = fr->get_byte();

		pEvent->EVENT = new char[pEvent->EVENTLEN+1];
		memset(pEvent->EVENT, 0x00, pEvent->EVENTLEN+1);
		memcpy(pEvent->EVENT, fr->get_string(pEvent->EVENTLEN), pEvent->EVENTLEN);
		fr->seek(1);

		unsigned long event_prop_start_pos = fr->get_pos();
		unsigned long current_event_prop_pos = 0;
		
		pEvent->EVENT_prop_len = fr->get_byte();
		pEvent->EVENT_prop = new char[pEvent->EVENT_prop_len + 1];
		memset(pEvent->EVENT_prop, 0x00, pEvent->EVENT_prop_len + 1);
		memcpy(pEvent->EVENT_prop, fr->get_string(pEvent->EVENT_prop_len), pEvent->EVENT_prop_len);
		fr->seek(1);

		printf("---- EVENT %s ----\r\n",pEvent->EVENT_prop);
		printf("type: %02X\r\n", pEvent->type);
		printf("UnkByte: %i\r\n", pEvent->UnkByte);
		printf("size: %i\r\n", pEvent->size);
		printf("EventLen: %i\r\n", pEvent->EVENTLEN);
		printf("EVENT_prop_len: %i\r\n", pEvent->EVENT_prop_len);
		
		dfile << "\t\t\t\t" << pEvent->EVENT << " = " << "\"" << pEvent->EVENT_prop << "\"" << endl << "\t\t\t\t" << "{" << endl;

		event_count++;
		unsigned long event_sub_count = 0;
		while (current_event_prop_pos != pEvent->size)
		{
			event_sub_count++;

			PEVENT_SUB_PROPERTY seProp = new EVENT_SUB_PROPERTY;
			seProp->type = fr->get_byte();
			seProp->UnkByte = fr->get_byte();

			switch (seProp->type)
			{
				case 0x43:
				case 0x44:
				case 0x45:
				case 0x46:
				case 0x47:
				case 0x48:
				case 0x49:
				case 0x4A:
				case 0x4B:
				case 0x4C:
				case 0x4D:
				case 0x4F:
					seProp->size = fr->get_byte();
				break;

				case 0x81:
				case 0x82:
					seProp->size = fr->get_ushort();
				break;

				default:
					printf("Could not match sub event property type: %02X pos: %i\r\n", seProp->type,fr->get_pos());
					exit(0);
				break;
			}


			seProp->namelen = fr->get_byte();

			seProp->name = new char[seProp->namelen + 1];
			memset(seProp->name, 0x00, seProp->namelen + 1);
			memcpy(seProp->name, fr->get_string(seProp->namelen), seProp->namelen);
			fr->seek(1);


			printf("-- SUB_PROPERTY: %s --\r\n", seProp->name);
			printf("type: %02X\r\n", seProp->type);
			printf("UnkByte: %i\r\n", seProp->UnkByte);
			printf("size: %i\r\n", seProp->size);
			if (seProp->type == 0x81 || seProp->type == 0x82)
			{
				exit(0);
			}

			dfile << "\t\t\t\t\t" << seProp->name << " = ";

			bool match = false;

			if (strcmp(seProp->name,"TY") == 0)
			{
				match = true;
				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}

			if (strcmp(seProp->name, "SU") == 0)
			{
				match = true;
				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}

			if (strcmp(seProp->name, "ACTN") == 0)
			{
				match = true;
				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}

			if (strcmp(seProp->name, "TARGETID") == 0)
			{
				match = true;
				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n",seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}

			if (strcmp(seProp->name, "LCG") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}

			if (strcmp(seProp->name, "LCT") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}
			

			if (strcmp(seProp->name, "LSCR_TXT_TITLE") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}

			
			if (strcmp(seProp->name, "LSCR_TXT_FILE") == 0)
			{
				match = true;

				seProp->string.cbData = seProp->size;
				seProp->string.pwszData = new char[seProp->size];
				memset(seProp->string.pwszData, 0x00, seProp->size);
				memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

				printf("seProp value: %s\r\n", seProp->string.pwszData);

				dfile << "\"" << seProp->string.pwszData << "\"" << endl;
			}

			if (strcmp(seProp->name, "BACKGROUNDFILENAME") == 0)
			{
					match = true;

					seProp->string.cbData = seProp->size;
					seProp->string.pwszData = new char[seProp->size];
					memset(seProp->string.pwszData, 0x00, seProp->size);
					memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

					printf("seProp value: %s\r\n", seProp->string.pwszData);

					dfile << "\"" << seProp->string.pwszData << "\"" << endl;

			}

			if (strcmp(seProp->name, "H") == 0)
			{
				match = true;

				seProp->string.cbData = seProp->size;
				seProp->string.pwszData = new char[seProp->size];
				memset(seProp->string.pwszData, 0x00, seProp->size);
				memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

				printf("seProp value: %s\r\n", seProp->string.pwszData);

				dfile << "\"" << seProp->string.pwszData << "\"" << endl;

			}

			if (strcmp(seProp->name, "M") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "G") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}


			if (strcmp(seProp->name, "L") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}
			if (strcmp(seProp->name, "D") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "DV") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "VT") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "K") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "N") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}
			
			if (strcmp(seProp->name, "C") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "F") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "T") == 0)
			{
				match = true;

				if (seProp->size != 4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}
				seProp->nData = fr->get_uint();
				printf("seProp value: %i\r\n", seProp->nData);

				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "S") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_uint();
				printf("seProp value: %i\r\n", seProp->nData);

				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "LSCR_TXT_1") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "LSCR_TXT_2") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "OK") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "X") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "Y") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			
			if (strcmp(seProp->name, "LSCR_TXT_LINE1_XPOS") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "LSCR_TXT_LINE2_XPOS") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "LSCR_TXT_LINE1_YPOS") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "LSCR_TXT_LINE2_YPOS") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "LSCR_TXT_TITLE_XPOS") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "LSCR_TXT_TITLE_YPOS") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}



			if (strcmp(seProp->name, "Z") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "FI") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "FO") == 0)
			{
				match = true;

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;

			}

			if (strcmp(seProp->name, "V1") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "V2") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}


			if (strcmp(seProp->name, "EF") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "ED") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_uint();
				printf("seProp value: %i\r\n", seProp->nData);

				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "EX") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_uint();
				printf("seProp value: %i\r\n", seProp->nData);

				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "EP") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_uint();
				printf("seProp value: %i\r\n", seProp->nData);

				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "ER") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "EL") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "EE") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "ES") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "EA") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "W_BM") == 0)
			{
				match = true;

					seProp->string.cbData = seProp->size;
					seProp->string.pwszData = new char[seProp->size];
					memset(seProp->string.pwszData, 0x00, seProp->size);
					memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

					printf("seProp value: %s\r\n", seProp->string.pwszData);
				
					dfile << "\"" << seProp->string.pwszData << "\"" << endl;
			}

			

			if (strcmp(seProp->name, "SOUNDFILE") == 0)
			{
				match = true;

					seProp->string.cbData = seProp->size;
					seProp->string.pwszData = new char[seProp->size];
					memset(seProp->string.pwszData, 0x00, seProp->size);
					memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

					printf("seProp value: %s\r\n", seProp->string.pwszData);
				
					dfile << "\"" << seProp->string.pwszData << "\"" << endl;
			}

			if (strcmp(seProp->name, "LG") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "SC") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "PIT") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}




			if (strcmp(seProp->name, "EN") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}


			if (strcmp(seProp->name, "PAT") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "HPN") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}
			
			if (strcmp(seProp->name, "OX") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "OY") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "OZ") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "PIP") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}


			if (strcmp(seProp->name, "PAP") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}



			if (strcmp(seProp->name, "W_FON") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}
	
			if (strcmp(seProp->name, "DO") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "LN") == 0)
			{
				match = true;

				seProp->string.cbData = seProp->size;
				seProp->string.pwszData = new char[seProp->size];
				memset(seProp->string.pwszData, 0x00, seProp->size);
				memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

				printf("seProp value: %s\r\n", seProp->string.pwszData);

				dfile << "\"" << seProp->string.pwszData << "\"" << endl;
			}

			if (strcmp(seProp->name, "W_TMF") == 0)
			{
				match = true;

				if (seProp->size == 0)
				{
					printf("Exiting, size for %s was 0\r\n", seProp->name);
					exit(0);
				}

				seProp->string.cbData = seProp->size;
				seProp->string.pwszData = new char[seProp->size];
				memset(seProp->string.pwszData, 0x00, seProp->size);
				memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

				printf("seProp value: %s\r\n", seProp->string.pwszData);

				dfile << "\"" << seProp->string.pwszData << "\"" << endl;
			}

			if (strcmp(seProp->name, "W_TMN") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "W_T") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "W_FSX") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "W_FSY") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "W_FSZ") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "W_BC") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}

			if (strcmp(seProp->name, "W_BA") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}

			if (strcmp(seProp->name, "VV") == 0)
			{
				match = true;

				if (seProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->fData = fr->get_float();
				printf("seProp value: %f\r\n", seProp->fData);

				dfile << seProp->fData << endl;
			}



			if (strcmp(seProp->name, "W_FC") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;

			}

			if (strcmp(seProp->name, "SF") == 0)
			{
				match = true;

				if (seProp->size == 0)
				{
					printf("Exiting, size for %s was 0\r\n", seProp->name);
					exit(0);
				}

				seProp->string.cbData = seProp->size;
				seProp->string.pwszData = new char[seProp->size];
				memset(seProp->string.pwszData, 0x00, seProp->size);
				memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

				printf("seProp value: %s\r\n", seProp->string.pwszData);


				dfile << "\"" << seProp->string.pwszData << "\"" << endl;
			}

			if (strcmp(seProp->name, "S1") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "S2") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "TF") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "T1") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "HSTF") == 0)
			{
				match = true;

				if (seProp->size == 0)
				{
					printf("Exiting, size for %s was 0\r\n", seProp->name);
					exit(0);
				}

				seProp->string.cbData = seProp->size;
				seProp->string.pwszData = new char[seProp->size];
				memset(seProp->string.pwszData, 0x00, seProp->size);
				memcpy(seProp->string.pwszData, fr->get_string(seProp->size),seProp->size);

				printf("seProp value: %s\r\n", seProp->string.pwszData);

				dfile << "\"" << seProp->string.pwszData << "\"" << endl;
			}

			if (strcmp(seProp->name, "ITM") == 0)
			{
				match = true;

				if (seProp->size == 0)
				{
					printf("Exiting, size for %s was 0\r\n", seProp->name);
					exit(0);
				}

				seProp->string.cbData = seProp->size;
				seProp->string.pwszData = new char[seProp->size];
				memset(seProp->string.pwszData, 0x00, seProp->size);
				memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

				printf("seProp value: %s\r\n", seProp->string.pwszData);

				dfile << "\"" << seProp->string.pwszData << "\"" << endl;
			}



			if (strcmp(seProp->name, "VAL") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}


			if (strcmp(seProp->name, "FNC") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}


			if (strcmp(seProp->name, "TACT") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "PACT") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "CNUM") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}


			if (strcmp(seProp->name, "A") == 0)
			{
				match = true;

				switch (seProp->size)
				{
					case 1:
						seProp->nData = fr->get_byte();


					break;

					case 2:
						seProp->nData = fr->get_ushort();
					break;

					default:
						printf("WARNING size was greater than 2 or less than 1 on %s sub-property!", seProp->name);
						exit(0);
					break;
				}

				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "CN") == 0)
			{
				match = true;

				if (seProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", seProp->name);
					exit(0);
				}

				seProp->nData = fr->get_byte();
				printf("seProp value: %i\r\n", seProp->nData);
				dfile << seProp->nData << endl;
			}

			if (strcmp(seProp->name, "MT") == 0)
			{
				match = true;

				if (seProp->size != 4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", seProp->name);
					exit(0);
				}

				seProp->bData = fr->get_uint();
				printf("seProp value: %i\r\n", seProp->bData);
				dfile << seProp->nData << endl;

			}

			if (strcmp(seProp->name, "NAME") == 0)
			{
				match = true;

				seProp->string.cbData = seProp->size;
				seProp->string.pwszData = new char[seProp->size];
				memset(seProp->string.pwszData, 0x00, seProp->size);

				memcpy(seProp->string.pwszData, fr->get_string(seProp->size), seProp->size);

				printf("seProp Value: %s\r\n", seProp->string.pwszData);

				dfile << "\"" << seProp->string.pwszData << "\"" << endl;
			}

			if (match == false)
			{
				printf("Could not match seProp Name: %s\r\n",seProp->name);
				exit(0);
			}

			current_event_prop_pos = fr->get_pos() - event_prop_start_pos;

			printf("current_event_prop_pos: %i event_size: %i\r\n", current_event_prop_pos, pEvent->size);
			printf("event_sub_count: %i\r\n", event_sub_count);
		}
		
		dfile << "\t\t\t\t" << "}" << endl << endl;
	

		printf("event_count: %i\r\n", event_count);
		current_event_pos = fr->get_pos() - event_start_pos; // This should break the loop once we've finished processing all events for this link/cause/event block.
	}

	dfile << "\t\t\t" << "}" << endl << endl;
}

void ReadCause(CAUSE_PROPERTY *cProperty)
{
	dfile << "\t\t" << cProperty->CauseName << " = " << "\"" << cProperty->CauseEvent << "\"" << endl << "\t\t\t" "{" << endl;

	unsigned long cause_start = fr->get_pos();
	unsigned long current_cause_pos = 0;
	unsigned long real_size = cProperty->size - cProperty->CauseEventLen - 2;
	while (current_cause_pos != real_size)
	{
		PCAUSE_SUB_PROPERTY csProperty = new CAUSE_SUB_PROPERTY;
		csProperty->type = fr->get_byte();
		csProperty->unkByte = fr->get_byte();
		csProperty->size = fr->get_byte();
		csProperty->namelen = fr->get_byte();


		csProperty->name = new char[csProperty->namelen+1];
		memset(csProperty->name, 0x00, csProperty->namelen+1);
		memcpy(csProperty->name, fr->get_string(csProperty->namelen), csProperty->namelen);
		fr->seek(1);

		printf("------ Reading Cause Property: %s -----\r\n", csProperty->name);
		printf("Type: %02X\r\n", csProperty->type);
		printf("unkByte: %02X\r\n", csProperty->unkByte);
		printf("size: %i\r\n", csProperty->size);
		printf("namelen: %i\r\n\r\n", csProperty->namelen);

		bool match = false;

		dfile << "\t\t\t\t" << csProperty->name << " = ";

		if (strcmp(csProperty->name, "C") == 0)
		{
			match = true;

			if (csProperty->size > 1)
			{
				printf("Exiting size for %s was greater than 1!\r\n", csProperty->name);
				exit(0);
			}

			csProperty->nData = fr->get_byte();
			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;

		}

		if (strcmp(csProperty->name, "B") == 0)
		{
			match = true;

			if (csProperty->size > 1)
			{
				printf("Exiting size for %s was greater than 1!\r\n", csProperty->name);
				exit(0);
			}

			csProperty->nData = fr->get_byte();
			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;


		}

		if (strcmp(csProperty->name, "CM") == 0)
		{
			match = true;
			if (csProperty->size > 1)
			{
				printf("Exiting size for %s was greater than 1!\r\n", csProperty->name);
				exit(0);
			}

			csProperty->nData = fr->get_byte();
			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;
		}

		if (strcmp(csProperty->name, "CR") == 0)
		{
			match = true;

			csProperty->fData = fr->get_float();
			printf("Cause Property Value: %f\r\n", csProperty->fData);

			dfile << csProperty->fData << endl;
		}

		if (strcmp(csProperty->name, "CO") == 0)
		{
			match = true;

			switch (csProperty->size)
			{
				case 1:
					csProperty->nData = fr->get_byte();
				break;

				case 2:
					csProperty->nData = fr->get_ushort();
				break;

				case 4:
					csProperty->nData = fr->get_uint();
				break;

				default:
					printf("Could not handle size of %s : %i\r\n", csProperty->name, csProperty->size);
					exit(0);
				break;
			}


			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;


		}

		if (strcmp(csProperty->name, "CP") == 0)
		{
			match = true;
			if (csProperty->size > 1)
			{
				printf("Exiting size for %s was greater than 1!\r\n", csProperty->name);
				exit(0);
			}

			csProperty->nData = fr->get_byte();
			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;

		}

		if (strcmp(csProperty->name, "CC") == 0)
		{
			match = true;
			if (csProperty->size > 1)
			{
				printf("Exiting size for %s was greater than 1!\r\n", csProperty->name);
				exit(0);
			}

			csProperty->nData = fr->get_byte();
			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;

		}

		if (strcmp(csProperty->name, "COMP") == 0)
		{
			match = true;
			if (csProperty->size > 1)
			{
				printf("Exiting size for %s was greater than 1!\r\n", csProperty->name);
				exit(0);
			}

			csProperty->nData = fr->get_byte();
			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;

		}

		if (strcmp(csProperty->name, "VAL") == 0)
		{
			match = true;
			if (csProperty->size > 1)
			{
				printf("Exiting size for %s was greater than 1!\r\n", csProperty->name);
				exit(0);
			}

			csProperty->nData = fr->get_byte();
			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;;

		}

		if (strcmp(csProperty->name, "NAME") == 0)
		{
			match = true;

			csProperty->string.cbData = csProperty->size;
			csProperty->string.pwszData = new char[csProperty->size];
			memset(csProperty->string.pwszData, 0x00, csProperty->size);

			memcpy(csProperty->string.pwszData, fr->get_string(csProperty->size), csProperty->size);

			printf("Cause Property Value: %s\r\n", csProperty->string.pwszData);

			dfile << "\"" << csProperty->string.pwszData << "\"" << endl;
		}


		if (strcmp(csProperty->name, "HSTF") == 0)
		{
			match = true;

				csProperty->string.cbData = csProperty->size;
				csProperty->string.pwszData = new char[csProperty->size];
				memset(csProperty->string.pwszData, 0x00, csProperty->size);

				memcpy(csProperty->string.pwszData, fr->get_string(csProperty->size), csProperty->size);

				printf("Cause Property Value: %s\r\n", csProperty->string.pwszData);

				dfile << "\"" << csProperty->string.pwszData << "\"" << endl;

		}

		if (strcmp(csProperty->name, "IOS") == 0)
		{
			match = true;

				csProperty->string.cbData = csProperty->size;
				csProperty->string.pwszData = new char[csProperty->size];
				memset(csProperty->string.pwszData, 0x00, csProperty->size);

				memcpy(csProperty->string.pwszData, fr->get_string(csProperty->size), csProperty->size);

				printf("Cause Property Value: %s\r\n", csProperty->string.pwszData);
				
				dfile << "\"" << csProperty->string.pwszData << "\"" << endl;
		}

		if (strcmp(csProperty->name, "ITM") == 0)
		{
			match = true;

			csProperty->string.cbData = csProperty->size;
			csProperty->string.pwszData = new char[csProperty->size];
			memset(csProperty->string.pwszData, 0x00, csProperty->size);

			memcpy(csProperty->string.pwszData, fr->get_string(csProperty->size), csProperty->size);

			printf("Cause Property Value: %s\r\n", csProperty->string.pwszData);
			
			dfile << "\"" << csProperty->string.pwszData << "\"" << endl;

		}


		if (strcmp(csProperty->name, "AGC") == 0)
		{
			match = true;

			if (csProperty->size == 1)
			{
				csProperty->nData = fr->get_byte();
			}
			else 
			{
				csProperty->nData = fr->get_uint();
			}

			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;;


		}


		if (strcmp(csProperty->name, "DOWN") == 0)
		{
			match = true;

			if (csProperty->size > 1)
			{
				printf("Exiting size for %s was greater than 1!\r\n", csProperty->name);
				exit(0);
			}

			csProperty->nData = fr->get_byte();
			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;;

		}


		if (strcmp(csProperty->name, "UP") == 0)
		{
			match = true;

			if (csProperty->size > 1)
			{
				printf("Exiting size for %s was greater than 1!\r\n", csProperty->name);
				exit(0);
			}

			csProperty->nData = fr->get_byte();
			printf("Cause Property Value: %i\r\n", csProperty->nData);

			dfile << csProperty->nData << endl;;

		}


		if (strcmp(csProperty->name, "APS") == 0)
		{
			match = true;

			if (csProperty->size == 1)
			{

				csProperty->nData = fr->get_byte();
			}
			else {
				csProperty->nData = fr->get_uint();
			}


			printf("Cause Property Value: %i\r\n", csProperty->nData);
			dfile << csProperty->nData << endl;;

		}



		if (strcmp(csProperty->name, "ANY") == 0)
		{
			match = true;

			if(csProperty->size == 4)
				csProperty->nData = fr->get_uint();
			if (csProperty->size == 1)
				csProperty->nData = fr->get_byte();

			if (csProperty->size > 1 && csProperty->size < 4)
			{
				printf("cause property %s failed checks in place for size\r\n",csProperty->name);
				exit(0);
			}
			printf("Cause Property Value: %i\r\n", csProperty->nData);
			dfile << csProperty->nData << endl;;


		}

		if (match == false)
		{
			printf("Could not match  csProperty->name: %s\r\n",csProperty->name);
			exit(0);
		}

		current_cause_pos = fr->get_pos() - cause_start;
		printf("pos: %i, cause_start: %i,current_pos: %i, size: %i\r\n", fr->get_pos(), cause_start, current_cause_pos, real_size);


	}
	
	dfile << "\t\t\t}" << endl << endl;

}

void ReadLink(OBJECT_PROPERTY *obProp)
{

	unsigned long link_start = fr->get_pos();
	unsigned long current_link_pos = 0;
	while (current_link_pos != obProp->size)
	{
		PCAUSE_PROPERTY cProperty = new CAUSE_PROPERTY;
		cProperty->type = fr->get_byte();
		cProperty->UnkByte = fr->get_byte();
		cProperty->size = fr->get_byte();
		cProperty->CauseLen = fr->get_byte();

		cProperty->CauseName = new char[cProperty->CauseLen+1];
		memset(cProperty->CauseName, 0x00, cProperty->CauseLen + 1);

		memcpy(cProperty->CauseName, fr->get_string(cProperty->CauseLen), cProperty->CauseLen);
		fr->seek(1);

		cProperty->CauseEventLen = fr->get_byte();
		cProperty->CauseEvent = new char[cProperty->CauseEventLen + 1];
		memset(cProperty->CauseEvent, 0x00, cProperty->CauseEventLen+1);

		memcpy(cProperty->CauseEvent, fr->get_string(cProperty->CauseEventLen), cProperty->CauseEventLen);
		fr->seek(1);

	
		printf("----- Reading LINK PROPERTY: %s : %s------ pos: %i\r\n\r\n", cProperty->CauseName,cProperty->CauseEvent,fr->get_pos());

		ReadCause(cProperty);
		ReadEvents(cProperty);

		current_link_pos =  fr->get_pos() - link_start;
	}
}

void ReadChildProperty(CHILD_PROPERTY *cProp)
{
	bool match = false;

	cProp->type = fr->get_byte();
	switch (cProp->type)
	{
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46:
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4A:
		case 0x4B:
		case 0x4C:
		case 0x4D:
		case 0x4F:
			cProp->unkByte = fr->get_byte();
			cProp->size = fr->get_byte();
			cProp->NameLen = fr->get_byte();
			cProp->Name = new char[cProp->NameLen+1];
			memset(cProp->Name, 0x00, cProp->NameLen+1);

			memcpy(cProp->Name, fr->get_string(cProp->NameLen), cProp->NameLen);
			
			fr->seek(1); // Account for null termination
			printf("------ Child Property: %s ------\r\n", cProp->Name);
			printf("Child Property Name Len: %i\r\n", cProp->NameLen);
			printf("Child Property Size: %i\r\n", cProp->size);

			dfile << "\t\t" << cProp->Name << " = ";

			if (strcmp(cProp->Name, "W_SP") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;
			}

			if (strcmp(cProp->Name, "W_XLAY") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "W_YLAY") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;
			}

			if (strcmp(cProp->Name, "ITYP") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;
			}

			if (strcmp(cProp->Name, "IPP") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;
			}

			if (strcmp(cProp->Name, "IPS") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;
			}

			if (strcmp(cProp->Name, "F_ED") == 0)
			{
				match = true;

				if (cProp->size != 4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", cProp->Name);
					exit(0);
				}

				cProp->fData = fr->get_float();

				printf("Child Property Value: %f\r\n", cProp->fData);

				dfile << cProp->fData << endl;
			}

			if (strcmp(cProp->Name, "F_T") == 0)
			{
				match = true;

				if (cProp->size != 4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", cProp->Name);
					exit(0);
				}

				cProp->fData = fr->get_float();

				printf("Child Property Value: %f\r\n", cProp->fData);

				dfile << cProp->fData << endl;

			}


			if (strcmp(cProp->Name, "F_C") == 0)
			{
				match = true;

				if (cProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", cProp->Name);
					exit(0);
				}

				cProp->fData = fr->get_float();

				printf("Child Property Value: %f\r\n", cProp->fData);

				dfile << cProp->fData << endl;

			}

			if (strcmp(cProp->Name, "W_BM") == 0)
			{
				match = true;

				cProp->string.cbData = cProp->size;
				cProp->string.pwszData = new char[cProp->size];
				memset(cProp->string.pwszData, 0x00, cProp->size);

				memcpy(cProp->string.pwszData, fr->get_string(cProp->size), cProp->size);

				printf("Child Property Value: %s\r\n", cProp->string.pwszData);

				dfile << "\"" << cProp->string.pwszData << "\"" << endl;

			}

			if (strcmp(cProp->Name, "W_FON") == 0)
			{
				match = true;

				cProp->string.cbData = cProp->size;
				cProp->string.pwszData = new char[cProp->size];
				memset(cProp->string.pwszData, 0x00, cProp->size);

				memcpy(cProp->string.pwszData, fr->get_string(cProp->size), cProp->size);

				printf("Child Property Value: %s\r\n", cProp->string.pwszData);

				dfile << "\"" << cProp->string.pwszData << "\"" << endl;

			}

			if (strcmp(cProp->Name, "W_TMF") == 0)
			{
				match = true;

				cProp->string.cbData = cProp->size;
				cProp->string.pwszData = new char[cProp->size];
				memset(cProp->string.pwszData, 0x00, cProp->size);

				memcpy(cProp->string.pwszData, fr->get_string(cProp->size), cProp->size);
				printf("Child Property Value: %s\r\n", cProp->string.pwszData);

				dfile << "\"" << cProp->string.pwszData << "\" ; file containing text to display on screen" << endl;


			}

			if (strcmp(cProp->Name, "W_THF") == 0)
			{
				match = true;

				cProp->string.cbData = cProp->size;
				cProp->string.pwszData = new char[cProp->size];
				memset(cProp->string.pwszData, 0x00, cProp->size);

				memcpy(cProp->string.pwszData, fr->get_string(cProp->size), cProp->size);
				printf("Child Property Value: %s\r\n", cProp->string.pwszData);

				dfile << "\"" << cProp->string.pwszData << "\"" << endl;

			}

			if (strcmp(cProp->Name, "A_HIF") == 0)
			{
				match = true;

				cProp->string.cbData = cProp->size;
				cProp->string.pwszData = new char[cProp->size];
				memset(cProp->string.pwszData, 0x00, cProp->size);

				memcpy(cProp->string.pwszData, fr->get_string(cProp->size), cProp->size);
				printf("Child Property Value: %s\r\n", cProp->string.pwszData);

				dfile << "\"" << cProp->string.pwszData << "\"" << endl;

			}


			if (strcmp(cProp->Name, "W_SUHF") == 0)
			{
				match = true;

				cProp->string.cbData = cProp->size;
				cProp->string.pwszData = new char[cProp->size];
				memset(cProp->string.pwszData, 0x00, cProp->size);

				memcpy(cProp->string.pwszData, fr->get_string(cProp->size), cProp->size);
				printf("Child Property Value: %s\r\n", cProp->string.pwszData);

				dfile << "\"" << cProp->string.pwszData << "\"" << endl;
			}

			if (strcmp(cProp->Name, "INVISIBLE") == 0)
			{
				match = true;

				cProp->string.cbData = cProp->size;
				cProp->string.pwszData = new char[cProp->size];
				memset(cProp->string.pwszData, 0x00, cProp->size);

				memcpy(cProp->string.pwszData, fr->get_string(cProp->size), cProp->size);
				printf("Child Property Value: %s\r\n", cProp->string.pwszData);

				dfile << "\"" << cProp->string.pwszData << "\"" << endl;
			}

			if (strcmp(cProp->Name, "W_TMN") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << " ; Key value for text to access" << endl;

			}


			if (strcmp(cProp->Name, "A_HII") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "W_THI") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "W_SUHI") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "A_GN") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "W_WC") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "A_FL") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "TYPE") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}


			if (strcmp(cProp->Name, "COLLIDES") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "TOUCHES") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "COUNTERS") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "W_SUB") == 0)
			{
				match = true;

				if (cProp->size > 1)
				{
					printf("Exiting size for %s was greater than 1!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_byte();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;

			}

			if (strcmp(cProp->Name, "W_FC") == 0)
			{
				match = true;

				if (cProp->size !=4)
				{
					printf("Exiting size for %s was greater than 4!\r\n", cProp->Name);
					exit(0);
				}

				cProp->nData = fr->get_uint();

				printf("Child Property Value: %i\r\n", cProp->nData);

				dfile << cProp->nData << endl;
			}



			if (match == false)
			{
				printf("Could not match childProperty name %s! pos: %i\r\n", cProp->Name, fr->get_pos());
				exit(0);
			}

		break;

		default:
			printf("Could not match child property type: %02X\r\n", cProp->type);
			exit(0);
		break;
	}
}

void ReadObjectProperty(OBJECT_PROPERTY *obProp)
{
	obProp->unkByte = fr->get_byte();
	obProp->size = fr->get_byte();
	obProp->NameLen = fr->get_byte();
	obProp->Name = new char[obProp->NameLen + 1];
	memset(obProp->Name, 0x00, obProp->NameLen + 1);

	memcpy(obProp->Name, fr->get_string(obProp->NameLen), obProp->NameLen);

	fr->seek(1); // String null termination.

	printf("------ Property: %s ------\r\n", obProp->Name);

	printf("Property Size: %i\r\n", obProp->size);
	printf("Property Name Len: %i\r\n", obProp->NameLen);
	
	dfile << "\t" << obProp->Name << " = ";
	bool match = false;

	if (strcmp(obProp->Name, "ID") == 0)
	{
		match = true;
	
		if (obProp->size > 1)
		{
			printf("Exiting size for %s was greater than 1!\r\n", obProp->Name);
			exit(0);
		}
		
		obProp->nData = fr->get_byte();
		dfile << obProp->nData << endl;

		printf("Property Value: %i\r\n", obProp->nData);
	}

	if (strcmp(obProp->Name, "NAME") == 0)
	{
		match = true;

		obProp->string.cbData = obProp->size;
		obProp->string.pwszData = new char[obProp->size];
		memset(obProp->string.pwszData, 0x00, obProp->size);
		
		memcpy(obProp->string.pwszData, fr->get_string(obProp->size), obProp->size);

		printf("Property Value: %s\r\n", obProp->string.pwszData);
		dfile << "\"" << obProp->string.pwszData << "\"" << endl;
	}

	if (strcmp(obProp->Name, "POS") == 0)
	{
		match = true;

		obProp->flVector.x = fr->get_float();
		obProp->flVector.y = fr->get_float();
		obProp->flVector.z = fr->get_float();

		dfile << " {" << endl << endl;
		dfile << "\t\tx = " << obProp->flVector.x << endl;
		dfile << "\t\ty = " << obProp->flVector.y << endl;
		dfile << "\t\tz = " << obProp->flVector.z << endl;
		dfile << "\t}" << endl << endl;

		printf("Property Value: %f , %f , %f\r\n", obProp->flVector.x, obProp->flVector.y, obProp->flVector.z);
	}

	if (strcmp(obProp->Name, "ROT") == 0)
	{
		match = true;

		obProp->flVector.x = fr->get_float();
		obProp->flVector.y = fr->get_float();
		obProp->flVector.z = fr->get_float();

		dfile << " {" << endl << endl;
		dfile << "\t\tx = " << obProp->flVector.x << endl;
		dfile << "\t\ty = " << obProp->flVector.y << endl;
		dfile << "\t\tz = " << obProp->flVector.z << endl;
		dfile << "\t}" << endl << endl;

		printf("Property Value: %f , %f , %f \r\n", obProp->flVector.x, obProp->flVector.y, obProp->flVector.z);
	}

	if (strcmp(obProp->Name, "SCALE") == 0)
	{
		match = true;

		obProp->flVector.x = fr->get_float();
		obProp->flVector.y = fr->get_float();
		obProp->flVector.z = fr->get_float();

		dfile << " {" << endl << endl;
		dfile << "\t\tx = " << obProp->flVector.x << endl;
		dfile << "\t\ty = " << obProp->flVector.y << endl;
		dfile << "\t\tz = " << obProp->flVector.z << endl << endl;
		dfile << "\t}" << endl;

		printf("Property Value: %f , %f , %f \r\n", obProp->flVector.x, obProp->flVector.y, obProp->flVector.z);
	}


	if (match == false)
	{
		printf("Could not match Property Name!\r\n");
		exit(0);
	}
}

void ReadGroup(OBJECT_PROPERTY *obProp)
{
	PACTOR_VARIABLES actor_vars = new ACTOR_VARIABLES;
	actor_vars->unkByte = fr->get_byte();

	switch (obProp->type)
	{
		case 0xA1:
			actor_vars->size = fr->get_short();
		break;

		case 0x6C:
		case 0x61:
			actor_vars->size = fr->get_byte();
		break;

		default:
			printf("Unknown group type...\r\n");
		break;
	}
	
	actor_vars->child_count = fr->get_byte();
	actor_vars->unkByte2 = fr->get_byte();
	actor_vars->NameLen = fr->get_byte();


	char* PropertyName = new char[actor_vars->NameLen + 1];
	memset(PropertyName, 0x00, actor_vars->NameLen + 1);

	memcpy(PropertyName, fr->get_string(actor_vars->NameLen), actor_vars->NameLen);
	actor_vars->Name = PropertyName;

	fr->seek(1); // String null termination.

	printf("------------ Group: %s ------------\r\n", actor_vars->Name);
	printf("Group Variables Size: %i\r\n", actor_vars->size);
	printf("Group  Name Len: %i\r\n", actor_vars->NameLen);
	printf("Group Children: %i\r\n\r\n", actor_vars->child_count);

	dfile << "\t" << actor_vars->Name << " = { " << endl << endl;

	int group_current = 0;
	while (group_current != actor_vars->child_count)
	{
		actor_vars->childProperties = (CHILD_PROPERTY*)malloc(actor_vars->child_count * sizeof(CHILD_PROPERTY));
		CHILD_PROPERTY nCProperty;
		ReadChildProperty(&nCProperty);

		group_current++;
		printf("group_current: %i\r\n", group_current);
		printf("actor_vars->child_count: %i\r\n", actor_vars->child_count);

		printf("\r\n");
	}

	dfile << "\t}" << endl;
}

int main()
{
	CkGlobal ckglobal;
	ckglobal.UnlockBundle("T4MPSE.CBh0399_ojROBSTuoDHg");


	char header_unk1 = fr->get_byte();
	const char* header_str = fr->get_string(3);

	printf("File Type: %s\r\n", header_str);

	dfile.open("decompiled2.txt");

	while (!fr->end_of_file())
	{
		OBJECT *ob = new OBJECT;
		ob->type = fr->get_byte();
		ob->Unk1 = fr->get_byte();

		printf("Object Type: %02X\r\n", ob->type);

		switch (ob->type)
		{
			case 0x42:
				ob->size = fr->get_byte();
			break;

			case 0x82:
				ob->size = fr->get_ushort();
			break;

			case 0xC2:
				ob->size = fr->get_uint();
			break;
			
			default:
				printf("Could not match type of object: %02X !\r\n", ob->type);
				exit(0);
			break;
		}

		printf("Object Size: %i\r\n", ob->size);

		ob->ObjectName_Len = fr->get_byte();
		
		printf("Object Name Length: %i\r\n", ob->ObjectName_Len);

		char* ObjectName = new char[ob->ObjectName_Len+1];
		memset(ObjectName, 0x00, ob->ObjectName_Len + 1);

		memcpy(ObjectName, fr->get_string(ob->ObjectName_Len),ob->ObjectName_Len);
		ob->ObjectName = ObjectName;

		fr->seek(1); // account for null after ObjectName "ACTOR"

		printf("Object Name: %s\r\n", ob->ObjectName);
		int object_start = fr->get_pos();

		ob->ActorLen = fr->get_byte();
		
		char* ActorString = new char[ob->ActorLen+1];
		memset(ActorString, 0x00, ob->ActorLen + 1);

		memcpy(ActorString, fr->get_string(ob->ActorLen), ob->ActorLen);
		ob->ActorPath = ActorString;

		fr->seek(1); // account for null string termination

		printf("ActorString: %s\r\n", ActorString);

		dfile << ob->ObjectName << " = \"" << ActorString << "\" {" << endl << endl;
	
	
		int object_pos = 0;
		printf("Object start: %i\r\n", object_start);
		while (object_pos != ob->size)
		{
			POBJECT_PROPERTY obProp = new OBJECT_PROPERTY;

			obProp->type = fr->get_byte();
			bool match = false;

			printf("Property Type: %02X\r\n", obProp->type);
			switch (obProp->type)
			{
				case 0x41:
				case 0x42:
					obProp->unkByte = fr->get_byte();
					obProp->size = fr->get_byte();
					obProp->NameLen = fr->get_byte();
					obProp->Name = new char[obProp->NameLen + 1];
					memset(obProp->Name, 0x00, obProp->NameLen + 1);

					memcpy(obProp->Name, fr->get_string(obProp->NameLen), obProp->NameLen);
					fr->seek(1);
					dfile << "\t" << obProp->Name << " {\n\n\t";
					ReadLink(obProp);
					dfile << "\t" << "}\n\n";
					//printf("obProp->name: %s not handled yet! pos: %i\r\n", obProp->Name,fr->get_pos());
					//exit(0);
					
				break;
				
				case 0x43:
				case 0x44:
				case 0x45:
				case 0x46:
				case 0x47:
				case 0x48:
				case 0x49:
				case 0x4A:
				case 0x4B:
				case 0x4C:
				case 0x4D:
				case 0x4F:
					ReadObjectProperty(obProp);
					printf("\r\n");
				break;

				case 0x61:
				case 0x6C:
					ReadGroup(obProp);
				break;

				case 0xA1:
				case 0xA2:
					ReadGroup(obProp);
				break;
				
				case 0x81: // LINK/EVENT/CAUSE
				case 0x82: // UNKNOWN

					obProp->unkByte = fr->get_byte();
					obProp->size = fr->get_ushort();
					obProp->NameLen = fr->get_byte();
					obProp->Name = new char[obProp->NameLen+1];
					memset(obProp->Name, 0x00, obProp->NameLen + 1);

					memcpy(obProp->Name, fr->get_string(obProp->NameLen), obProp->NameLen);

					fr->seek(1);

					printf("------- Property: %s -------\r\n", obProp->Name);
					printf("UnkByte: %i\r\n", obProp->unkByte);
					printf("size: %i\r\n", obProp->size);
					printf("NameLen: %i\r\n", obProp->NameLen);
					printf("position: %i\r\n", fr->get_pos());

					dfile << "\t" << obProp->Name << " {\n\n\t";
					if (strcmp(obProp->Name, "LINK") == 0)
					{
						match = true;
						ReadLink(obProp);
					}

					if (match == false)
					{
						printf("Could not match obProp->name: %s\r\n",obProp->Name);
						exit(0);
					}
					dfile << "\t" << "}\n\n";

				break;

				default:
					printf("Could not match type of property: %02X - POS: %i\r\n", obProp->type,fr->get_pos());
					exit(0);
				break;
			}

			object_pos = fr->get_pos() - object_start;
			printf("object_pos: %i\r\n", object_pos);
		}

		dfile << "}" << endl << endl;
	

	}
	return 0;
}

