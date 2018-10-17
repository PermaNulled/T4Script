#pragma once

typedef struct
{
	float x;
	float y;
	float z;
} vector3;


typedef struct 
{
	unsigned char type;
	unsigned char UnkByte;
	unsigned long size;
	unsigned char namelen;
	char* name;

	union {
		unsigned char bData;
		unsigned int nData;
		long long i64Data;
		double dblData;
		struct {
			int cbData;
			char* pwszData;
		} string;
		float fData;
		struct {
			int cbData;
			char* pbData;
		}binary;

		vector3 flVector;
		FILETIME ftdata;
	};

} EVENT_SUB_PROPERTY, *PEVENT_SUB_PROPERTY;

typedef struct  
{
	unsigned char type;
	unsigned char UnkByte;
	unsigned long size;

	unsigned char EVENTLEN;
	char* EVENT;
	unsigned char EVENT_prop_len;
	char* EVENT_prop;
	
	PEVENT_SUB_PROPERTY eProps;

} EVENT_PROPERTY, *PEVENT_PROPERTY;


typedef struct
{
	unsigned char type;
	unsigned char unkByte;
	unsigned char namelen;
	char* name;
	unsigned long size;

	union {
		unsigned char bData;
		unsigned int nData;
		long long i64Data;
		double dblData;
		struct {
			int cbData;
			char* pwszData;
		} string;
		float fData;
		struct {
			int cbData;
			char* pbData;
		}binary;

		vector3 flVector;
		FILETIME ftdata;
	};

} CAUSE_SUB_PROPERTY, *PCAUSE_SUB_PROPERTY;

typedef struct
{
	unsigned char type;
	unsigned char UnkByte;
	unsigned long size;
	char* CauseName;
	unsigned char CauseLen;

	char* CauseEvent;
	unsigned char CauseEventLen;
	
	PCAUSE_SUB_PROPERTY pProperties;
	
	unsigned char EventsBlockType;
	unsigned char EventsUnkByte;
	unsigned short EventsBlockSize;
	unsigned char EventsUnkByte2;
	unsigned char EventsUnkByte3;
	unsigned char EventsNameLength;
	char* EventsName;

	PEVENT_PROPERTY pEvents;

} CAUSE_PROPERTY, *PCAUSE_PROPERTY;

typedef struct 
{
	unsigned char type;
	unsigned char unkByte;
	unsigned char NameLen;
	char* Name;

	unsigned long size;


	union {
		unsigned char bData;
		unsigned int nData;
		long long i64Data;
		double dblData;
		struct {
			int cbData;
			char* pwszData;
		} string;
		float fData;
		struct {
			int cbData;
			char* pbData;
		}binary;

		vector3 flVector;
		FILETIME ftdata;
	};
} OBJECT_PROPERTY, *POBJECT_PROPERTY;

typedef struct  
{
	unsigned char type;
	unsigned char unkByte;
	unsigned char NameLen;
	char* Name;

	unsigned long size;
	union {
		unsigned char bData;
		unsigned int nData;
		long long i64Data;
		double dblData;
		struct {
			int cbData;
			char* pwszData;
		} string;
		float fData;
		struct {
			int cbData;
			char* pbData;
		}binary;

		vector3 flVector;
		FILETIME ftdata;
	};
} CHILD_PROPERTY, *PCHILD_PROPERTY;

typedef struct
{
	unsigned char type;
	unsigned char unkByte;
	unsigned char NameLen;
	char* Name;

	unsigned char child_count;
	unsigned char unkByte2;

	unsigned long size;
	PCHILD_PROPERTY childProperties;

}ACTOR_VARIABLES, *PACTOR_VARIABLES;

typedef struct
{
	unsigned char type;
	unsigned char Unk1;

	unsigned long size;

	unsigned char ObjectName_Len;
	char* ObjectName;

	unsigned char ActorLen;
	char* ActorPath;

	POBJECT_PROPERTY pObjects;
} OBJECT;