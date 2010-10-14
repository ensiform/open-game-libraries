/*
===========================================================================
The Open Game Libraries.
Copyright (C) 2007-2010 Lusito Software

Author:  Santo Pfingsten (TTK-Bandit)
Purpose: System Information Retrieval
-----------------------------------------

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
===========================================================================
*/

#include <cstdio>
#include <og/Shared/Thread/Thread.h>

#ifdef OG_WIN32
	#include <windows.h>
#else
	#include <dlfcn.h>
	#include <sys/time.h>
#endif

namespace og {
namespace SysInfo {
static LARGE_INTEGER frequency;
static double		pfcMultiplier;

CPU::Features	cpu;
uLongLong		ramB; // in bytes
uLongLong		ramMB; // in MByte

/*
================
SysInfo::GetHiResTime
================
*/
uLongLong GetHiResTime( void ) {
#ifdef OG_WIN32
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return static_cast<uLongLong>(count.QuadPart*pfcMultiplier);
#else
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return tp.tv_sec*1000000 + tp.tv_usec;
#endif
}

/*
================
SysInfo::GetTime
================
*/
uLong GetTime( void ) {
#ifdef OG_WIN32
	return timeGetTime();
#else
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return tp.tv_sec*1000 + (tp.tv_usec/1000);
#endif
}

/*
================
SysInfo::RetrieveCPUInfo

sources:
Intel: ftp://download.intel.com/design/processor/applnots/24161831.pdf
AMD: http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25481.pdf
================
*/

#if defined(OG_ASM_MSVC)
	#define  dw  dword ptr
#endif

void RetrieveCPUInfo( void ) {
	cpu.vendorName[12] = 0;
	cpu.processorName[48] = 0;

	// eax = 0 -> vendor_id
	// eax = 0x80000000 -> largestExtFuncNr
#if defined(OG_ASM_MSVC)
	__asm {
		mov		eax, 0
		cpuid
		mov		cpu.largestStdFuncNr, eax
		mov		dw cpu.vendorName[0], ebx
		mov		dw cpu.vendorName[4], edx
		mov		dw cpu.vendorName[8], ecx

		mov		eax, 80000000h
		cpuid
		mov		cpu.largestExtFuncNr, eax
	}
#elif defined(OG_ASM_GNU)
	__asm__ __volatile__(
		"movl      $0, %%eax;"
		"cpuid;"
		"movl     %%ebx,0(%%esi);"
		"movl     %%edx,4(%%esi);"
		"movl     %%ecx,8(%%esi);"
		: "=a"(cpu.largestStdFuncNr)
		: "S"(cpu.vendorName)
	);
	__asm__ __volatile__(
		"movl      $0x80000000, %%eax;"
		"cpuid;"
		: "=a"(cpu.largestExtFuncNr)
	);
#endif

	// CPUID not supported
	if ( cpu.largestStdFuncNr < 1 ) {
		cpu.vendorID = CPU::UNKNOWN;
		return;
	}

	// Convert string to enum.
	if ( strcmp(cpu.vendorName, "GenuineIntel") == 0 )
		cpu.vendorID = CPU::INTEL;
	else if ( strcmp(cpu.vendorName, "AuthenticAMD") == 0 )
		cpu.vendorID = CPU::AMD;
	else if ( strcmp(cpu.vendorName, "CyrixInstead") == 0 )
		cpu.vendorID = CPU::CYRIX;
	else if ( strcmp(cpu.vendorName, "CentaurHauls") == 0 )
		cpu.vendorID = CPU::CENTAUR;
	else if ( strcmp(cpu.vendorName, "SiS SiS SiS ") == 0 )
		cpu.vendorID = CPU::SIS;
	else if ( strcmp(cpu.vendorName, "NexGenDriven") == 0 )
		cpu.vendorID = CPU::NEXTGEN;
	else if ( strcmp(cpu.vendorName, "GenuineTMx86") == 0
		|| strcmp(cpu.vendorName, "TransmetaCPU") == 0 )
		cpu.vendorID = CPU::TRANSMETA;
	else if ( strcmp(cpu.vendorName, "RiseRiseRise") == 0 )
		cpu.vendorID = CPU::RISE;
	else if ( strcmp(cpu.vendorName, "UMC UMC UMC ") == 0 )
		cpu.vendorID = CPU::UMC;
	else if ( strcmp(cpu.vendorName, "Geode by NSC") == 0 )
		cpu.vendorID = CPU::NSC;
	else {
		cpu.vendorID = CPU::UNKNOWN;
		return;
	}

	// eax = 4 -> Multicore support
	if ( cpu.largestStdFuncNr >= 4 ) {
#if defined(OG_ASM_MSVC)
		__asm {
			mov eax, 4
			cpuid
			// Clear bits 0 to 25 in EAX
			and eax, 0xFC000000
			shr eax, 26
			// Increase by one
			add eax, 1
			mov cpu.coresPerSocket, eax
		}
#elif defined(OG_ASM_GNU)
		__asm__ __volatile__(
			"movl      $4, %%eax;"
			"cpuid;"
			// Clear bits 0 to 25 in EAX
			"andl      $0xfc000000, %%eax;"
			"shrl      $26, %%eax;"
			"addl      $1, %%eax;"
			: "=a"(cpu.coresPerSocket)
		);
#endif
	} else {
		cpu.coresPerSocket = 1;
	}

	// eax = 1 -> signature & features
#if defined(OG_ASM_MSVC)
	__asm {
		mov		eax, 1
		cpuid
		mov		dw cpu.processorSignature, eax
		mov		dw cpu.miscInfo, ebx
		mov		dw cpu.general, edx
		mov		dw cpu.extended, ecx
	}
#elif defined(OG_ASM_GNU)
	__asm__ __volatile__(
		"movl      $1, %%eax;"
		"cpuid;"
		: "=a"(cpu.processorSignature),
		  "=b"(cpu.miscInfo),
		  "=d"(cpu.general),
		  "=c"(cpu.extended)
	);
#endif

	// eax = 80000001h -> AMD features
	if ( cpu.vendorID == CPU::AMD ) {
#if defined(OG_ASM_MSVC)
		__asm {
			mov		eax, 80000001h
			cpuid
			mov		dw cpu.AMD_miscInfo, ebx
			mov		dw cpu.AMD_general, edx
			mov		dw cpu.AMD_extended, ecx
		}
#elif defined(OG_ASM_GNU)
		__asm__ __volatile__(
			"movl      $0x80000001, %%eax;"
			"cpuid;"
			: "=b"(cpu.AMD_miscInfo),
			  "=d"(cpu.AMD_general),
			  "=c"(cpu.AMD_extended)
		);
#endif
	}

	// Check if extended functions are supported
	if ( cpu.largestExtFuncNr >= 0x80000004 ) {
		// eax = 80000002h/80000003h/80000004h -> CPU Name/Brand String
#if defined(OG_ASM_MSVC)
		__asm {
			// Get the first 16 bytes of the processor name
			mov		eax, 80000002h
			lea		edi, cpu.processorName
			cpuid
			call save_string

			// Get the second 16 bytes of the processor name
			mov eax, 80000003h
			cpuid
			call save_string

			// Get the last 16 bytes of the processor name
			mov eax, 80000004h
			cpuid
			call save_string

			jmp  end

save_string:
			mov dw [edi], eax
			mov dw [edi+4], ebx
			mov dw [edi+8], ecx
			mov dw [edi+12], edx
			add edi, 16
			ret
end:
		}
#elif defined(OG_ASM_GNU)
		__asm__ __volatile__(
			// Get the first 16 bytes of the processor name
			"movl      $0x80000002, %%eax;"
			"cpuid;"
			"call      save_string;"

			// Get the second 16 bytes of the processor name
			"movl      $0x80000003, %%eax;"
			"cpuid;"
			"call      save_string;"

			// Get the last 16 bytes of the processor name
			"movl      $0x80000004, %%eax;"
			"cpuid;"
			"call      save_string;"

			"jmp       end;"

"save_string:;"
			"movl     %%eax,(%%esi);"
			"movl     %%ebx,4(%%esi);"
			"movl     %%ecx,8(%%esi);"
			"movl     %%edx,12(%%esi);"

			"addl     $16, %%esi;"
			"ret;"
"end:;"
				 : /* no output */ : "S" (cpu.processorName)
		);
#endif
	}

//	cpu.vendorName.StripLeadingWhitespaces();
//	cpu.vendorName.StripTrailingWhitespaces();
	if( cpu.processorName[0] )
		strcpy( cpu.processorName, cpu.vendorName );
	else {
//		cpu.processorName.StripLeadingWhitespaces();
//		cpu.processorName.StripTrailingWhitespaces();
	}
}
/*
================
RDTSC
================
*/
#if defined(OG_WIN32)
uLongLong RDTSC( void ) {
	uInt timeLow, timeHigh;
	uLongLong result = 0;

#if defined(OG_ASM_MSVC)
	_asm {
		rdtsc
		mov [timeLow], eax
		mov [timeHigh], edx
	}
#elif defined(OG_ASM_GNU)
	__asm__ __volatile__( "rdtsc" : "=a"(timeLow), "=d"(timeHigh) );
#endif

	result |= timeHigh;
	result <<= 32;
	result |= timeLow;
	return result;
}
#endif


/*
================
SysInfo::RetrieveCPUSpeed
================
*/
void RetrieveCPUSpeed( void ) {
#if defined(OG_WIN32)
	if ( cpu.general.TSC ) {
		const int measureTime = 5; // 5ms should be enough to determine the speed.
		LARGE_INTEGER ulTicks;
		QueryPerformanceCounter(&ulTicks);

		// calculate interval for measureTime ms
		LONGLONG ulValue = ulTicks.QuadPart + static_cast<LONGLONG>(frequency.QuadPart*measureTime*0.001f);

		// loop for measureTime ms
		uLongLong start = RDTSC();
		do { QueryPerformanceCounter(&ulTicks); }
		while (ulTicks.QuadPart <= ulValue);

		cpu.speed = static_cast<int>(static_cast<float>(RDTSC() - start) * 0.001f/measureTime);
	}

#elif defined(OG_LINUX)
	FILE *file = fopen("/proc/cpuinfo", "r");
	if( file != NULL ) {
		char buffer[1024];
		int len = fread (buffer, 1, 1023, file);
		fclose( file );
		if ( len != 0 ) {
			buffer[len] = '\0';
			char *match = strstr( buffer, "cpu MHz" );
			if ( match != NULL ) {
				float speed;
				sscanf( match, "cpu MHz : %f", &speed );
				cpu.speed = static_cast<int>(speed);
				return;
			}
		}
	}
#else
#endif
}

/*
================
SysInfo::RetrieveMemorySize
================
*/
void RetrieveMemorySize( void ) {
#if defined(OG_WIN32)
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof(stat);
	GlobalMemoryStatusEx( &stat );
	// Convert Bytes to MBytes
	ramB = stat.ullTotalPhys;
	ramMB = stat.ullTotalPhys/(1024*1024);

#elif defined(OG_LINUX)
	FILE *file = fopen("/proc/meminfo", "r");
	if( file != NULL ) {
		char buffer[1024];
		int len = fread (buffer, 1, 1023, file);
		fclose( file );
		if ( len != 0 ) {
			buffer[len] = '\0';
			char *match = strstr( buffer, "MemTotal" );
			if ( match != NULL ) {
				long memTotal;
				sscanf( match, "MemTotal: %ld", &memTotal );
				ramMB = memTotal/1024;
				ramB = memTotal*1024;
				return;
			}
		}
	}

	ram = 0;
#else
#endif
}

/*
================
SysInfo::GetOSInfo
================
*/
OSInfo *SysInfo::GetOSInfo( void ) {
	static OSInfo data;
	static bool initialized = false;
	static Mutex writeLock;

	// We only need to get it once, it wont change..
	writeLock.Lock();
	if ( !initialized ) {
#if defined(OG_WIN32)
		OSVERSIONINFO osInfo;
		osInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

		// Failed ?
		if( !GetVersionEx( &osInfo ) ) {
			strcpy( data.name, "Not identified" );
			User::Error(ERR_SYSTEM_REQUIREMENTS, "Couldn't get OS version info" );
		} else {
			data.majorVersion = osInfo.dwMajorVersion;
			data.minorVersion = osInfo.dwMinorVersion;
			data.buildNumber = osInfo.dwBuildNumber;

			// We require Win2K minimum
			if( osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && osInfo.dwMajorVersion >= 5 ) {
				if( osInfo.dwMajorVersion == 5 && osInfo.dwMinorVersion == 0 )
					strcpy( data.name, "Win2K" );
				else if( osInfo.dwMajorVersion == 5 && osInfo.dwMinorVersion == 1 )
					strcpy( data.name, "WinXP" );
				else if( osInfo.dwMajorVersion == 6 ) {
					if( osInfo.dwMinorVersion == 0 )
						strcpy( data.name, "WinVista" );
					else
						strcpy( data.name, "Win7" );
				}

				if ( strnicmp(osInfo.szCSDVersion, "Service Pack ", 13) == 0 ) {
					strcat( data.name, " SP" );
					strcat( data.name, osInfo.szCSDVersion+13 );
				} else {
					strcat( data.name, " " );
					strcat( data.name, osInfo.szCSDVersion );
				}
			}
			if ( data.name[0] == '\0' ) {
				sprintf( data.name, "Win %d.%d", osInfo.dwMajorVersion, osInfo.dwMinorVersion );
				User::Error(ERR_SYSTEM_REQUIREMENTS, "Requires Windows 2000 or greater" );
			}
		}
#elif defined(OG_LINUX)
		FILE *file = fopen("/proc/version", "r");
		if( file != NULL ) {
			char buffer[256];
			int len = fread (buffer, 1, 255, file);
			fclose( file );
			if ( len != 0 ) {
				buffer[len] = '\0';
				char *p = strchr( buffer, '(' );
				if ( p != NULL || (p = strchr( buffer, '\n' )) != NULL )
					*p = '\0';

				data.name = buffer;
				data.name.StripTrailingWhitespaces();
				// caplength here maybe ? just in case..
				return;
			}
		}
		data.name = "unknown unix";
#else
#endif
		initialized = true;
	}
	writeLock.Unlock();
	return &data;
}


// Init SysInfo
struct Initializer {
	Initializer() {
		QueryPerformanceFrequency(&frequency);
		pfcMultiplier = 1000000.0/static_cast<double>(frequency.QuadPart);

		RetrieveCPUInfo();
		RetrieveCPUSpeed();
		RetrieveMemorySize();
	}
};
static Initializer initializer;
}

/*
================
DateTime::FromTime
================
*/
void DateTime::FromTime( const time_t &time ) {
	struct tm *tms = localtime(&time);
	if ( tms )
		memcpy( this, tms, sizeof(struct tm) );
}

/*
================
DateTime::UseCurrentTime
================
*/
void DateTime::UseCurrentTime( void ) {
	FromTime( time(NULL) );
}

/*
================
DateTime::Compare
================
*/
#define COMPARE_MEMBER(name)  if ( name > other.name ) return 1; if ( name < other.name ) return -1;
int DateTime::Compare( const struct tm &other ) {
	COMPARE_MEMBER(tm_year)
	COMPARE_MEMBER(tm_yday)
	COMPARE_MEMBER(tm_hour)
	COMPARE_MEMBER(tm_min)
	COMPARE_MEMBER(tm_sec)
	return 0;
}

}
