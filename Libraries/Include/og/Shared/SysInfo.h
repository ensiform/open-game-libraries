// ==============================================================================
//! @file
//! @brief	System Information Retrieval
//! @author	Santo Pfingsten (TTK-Bandit)
//! @note	Copyright (C) 2007-2010 Lusito Software
// ==============================================================================
//
// The Open Game Libraries.
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// ==============================================================================

#ifndef __OG_SYSINFO_H__
#define __OG_SYSINFO_H__

#include <string.h>

//! Open Game Libraries
namespace og {
//! @defgroup Common Common (Library)
//! @{

	// ==============================================================================
	//! CPU Information
	// ==============================================================================
	namespace CPU {
		// ==============================================================================
		//! CPU types
		// ==============================================================================
		enum Vendor {
			UNKNOWN = -1,	//!< Unknown vendor
			INTEL,			//!< Integrated electronics
			AMD,			//!< Advanced Micro Device
			CYRIX,			//!< Cyrix
			CENTAUR,		//!< Centaur
			SIS,			//!< Silicon Integrated Systems
			NEXTGEN,		//!< NextGenDriven
			TRANSMETA,		//!< Transmeta
			RISE,			//!< Rise Technology
			UMC,			//!< United Microelectronics Corporation
			NSC				//!< AMD/NSC Geode
		};

		// ==============================================================================
		//! CPU features
		//!
		//! @see	ftp://download.intel.com/design/processor/applnots/24161831.pdf
		//! @see	http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25481.pdf
		// ==============================================================================
		class Features {
		public:
			String		vendorName;			//!< Name of the vendor
			String		processorName;		//!< Name of the processor
			Vendor		vendorID;			//!< Identifier for the vendor
			uLong		largestStdFuncNr;	//!< The largest std func nr
			uLong		largestExtFuncNr;	//!< The largest extent func nr
			uInt		speed;				//!< Speed in MHz
			uLong		coresPerSocket;		//!< Multicore: x >= 2

			// ==============================================================================
			//! Processor signature
			// ==============================================================================
			struct processorSignature_s {
				uLong	stepping		: 4, //!< Processor stepping (revision) for a specific model
						model			: 4, //!< Model number
						family			: 4, //!< Family Code
						processortype	: 2, //!< Processor Type
						unused			: 2, //!< Reserved
						extModel		: 4, //!< Extended model
						extFamily		: 8; //!< Extended family
			} processorSignature;

			// ==============================================================================
			//! Misc information
			// ==============================================================================
			struct miscInfo_s {
				uLong	brandID			: 8, //!< Brand ID
						chunks			: 8, //!< CLFLUSH size?
						count			: 8, //!< Number of Logical Processors per Physical Package
						apicID			: 8; //!< Advanced Programmable Interface Controller ID
			} miscInfo;

			// ==============================================================================
			//! General features
			// ==============================================================================
			struct general_s {
				uLong	FPU				: 1, //!< Floating-point unit on-Chip
						VME				: 1, //!< Virtual Mode Extension
						DE				: 1, //!< Debugging Extension
						PSE				: 1, //!< Page Size Extension
						TSC				: 1, //!< Time-Stamp Counter
						MSR				: 1, //!< Model Specific Registers
						PAE				: 1, //!< Physical Address Extension
						MCE				: 1, //!< Machine Check Exception
						CX8				: 1, //!< CMPXCHG8 Instruction Supported
						APIC			: 1, //!< On-chip APIC Hardware Supported
						UNKNOWN1		: 1, //!< Reserved
						SEP				: 1, //!< Fast System Call
						MTRR			: 1, //!< Memory Type Range Registers
						PGE				: 1, //!< Page Global Enable
						MCA				: 1, //!< Machine Check Architecture
						CMOV			: 1, //!< Conditional Move Instruction Supported
						PAT				: 1, //!< Page Attribute Table
						PSE36			: 1, //!< 36-bit Page Size Extension
						PSNUM			: 1, //!< Processor serial number is present and enabled
						CLFLUSH			: 1, //!< CLFLUSH Instruction supported
						UNKNOWN2		: 1, //!< Reserved
						DTS				: 1, //!< Debug Store
						ACPI			: 1, //!< Thermal Monitor and Software Controlled Clock Facilities supported
						MMX				: 1, //!< Intel Architecture MMX technology supported
						FXSR			: 1, //!< Fast floating point save and restore
						SSE				: 1, //!< Streaming SIMD Extensions supported
						SSE2			: 1, //!< Streaming SIMD Extensions 2
						SS				: 1, //!< Self-Snoop
						HTT				: 1, //!< Multi-Threading
						TM				: 1, //!< Thermal Monitor supported
						IA64			: 1, //!< IA64 Capabilities
						PBE				: 1; //!< Pending Break Enable
			} general;

			// ==============================================================================
			//! Extended features
			// ==============================================================================
			struct featuresExt_s {
				uLong	SSE3			: 1, //!< Streaming SIMD Extensions 3
						UNKNOWN1		: 2, //!< Reserved
						MONITOR			: 1, //!< MONITOR/MWAIT
						DS_CPL			: 1, //!< CPL Qualified Debug Store
						VME				: 1, //!< Virtual Machine Extensions
						UNKNOWN2		: 1, //!< Reserved
						EIST			: 1, //!< Enhanced Intel SpeedStep® Technology
						TM2				: 1, //!< Thermal Monitor 2
						SSSE3			: 1, //!< Supplemental Streaming SIMD Extensions 3
						CID				: 1, //!< Context ID
						UNKNOWN3		: 2, //!< Reserved
						CX16			: 1, //!< CMPXCHG16B
						XTPR			: 1, //!< Send Task Priority Messages
						UNKNOWN4		: 3, //!< Reserved
						DCA				: 1, //!< Direct Cache Access
						UNKNOWN5		: 4, //!< Reserved
						POPCNT			: 1; //!< POPCNT instructions (AMD)
			} extended;

			// ==============================================================================
			//! AMD misc information
			// ==============================================================================
			struct AMD_miscInfo_s {
				uLong	brandID			: 16,//!< Brand ID
						unknown			: 12,//!< Reserved
						pkgType			: 4; //!< Package type
			} AMD_miscInfo;

			// ==============================================================================
			//! AMD features
			//!
			//! This uLong contains a lot of bits that are already set in features_s,
			//! so I removed them here to avoid confusion
			// ==============================================================================
			struct AMD_general_s {
				uLong	UNKNOWN1		: 20,//!< Reserved
						NX				: 1, //!< No-execute page protection
						UNKNOWN2		: 1, //!< Reserved
						MMX_EXT			: 1, //!< AMD extensions to MMX instructions
						UNKNOWN3		: 2, //!< Reserved
						FFXSR			: 1, //!< FXSAVE and FXRSTOR instruction optimizations
						PAGE_1GB		: 1, //!< 1-GB large page support
						RDTSCP			: 1, //!< RDTSCP instruction
						UNKNOWN4		: 1, //!< Reserved
						LM				: 1, //!< Long Mode
						_3DNOW_EXT		: 1, //!< AMD extensions to 3DNow! instructions
						_3DNOW			: 1; //!< 3DNow! instructions
			} AMD_general;

			// ==============================================================================
			//! AMD extended features
			// ==============================================================================
			struct AMD_extended_s {
				uLong	LAHF_SAHF		: 1, //!< LAHF SAHF insturctions in 64bit mode
						CMP_LEGACY		: 1, //!< Core multi-processing legacy mode
						SVM				: 1, //!< Secure virtual machine feature
						EAS				: 1, //!< presence of extended APIC register space
						ALTMOVCR8		: 1, //!< LOCK MOV CR0 means MOV CR8
						ABM				: 1, //!< Advanced bit manipulation. LZCNT instruction support
						SSE4A			: 1, //!< EXTRQ, INSERTQ, MOVNTSS, and MOVNTSD instruction support
						MA_SSE			: 1, //!< Misaligned SSE Mode
						_3DNOW_PREFETCH	: 1, //!< PREFETCH and PREFETCHW instruction support
						OSVW			: 1, //!< OS visible workaround
						UNKNOWN1		: 2, //!< Reserved
						SKINIT			: 1, //!< SKINIT, STGI, and DEV support
						WDT				: 1; //!< Watchdog timer support
			} AMD_extended;
		};
	}

	// ==============================================================================
	//! Information about the operating system
	//!
	//! @todo	better/more generic way ?
	// ==============================================================================
	class OSInfo {
	public:
		uLong	majorVersion;	//!< The major version
		uLong	minorVersion;	//!< The minor version
		uLong	buildNumber;	//!< The build number
		String	name;			//!< A string that tries to match the OS best
	};

	// ==============================================================================
	//! System information
	// ==============================================================================
	namespace SysInfo {
		// ==============================================================================
		//! Get high resolution time
		//!
		//! @return	The time in microseconds
		//!
		//!	@note	Resolution: 1-6 microseconds (depending on cpu)
		// ==============================================================================
		uLongLong	GetHiResTime( void );

		// ==============================================================================
		//! Get the OS time
		//!
		//! @return	The time in milliseconds
		//!
		//!	@note	Resolution: resolution: 10ms
		// ==============================================================================
		uLong		GetTime( void );

		// ==============================================================================
		//! Get information about the Operating System
		//!
		//! @param data	An OSInfo object to store the result in.
		//!
		//! @return	true on success
		// ==============================================================================
		bool		GetOSInfo( OSInfo &data );

		extern CPU::Features	cpu;	//!< CPU information
		extern uLongLong		ramB;	//!< Available RAM in bytes
		extern uLongLong		ramMB;	//!< Available RAM in megabytes
	};

	// ==============================================================================
	//! Date and time informtion
	// ==============================================================================
	struct DateTime : public tm {
		// ==============================================================================
		//! Default Constructor
		//!
		//! @param	initTime	true to init with current time
		// ==============================================================================
		DateTime( bool initTime=false ) { if ( initTime ) UseCurrentTime(); }

		// ==============================================================================
		//! Time Constructor
		//!
		//! @param	time	A time_t object to init from
		// ==============================================================================
		DateTime( const time_t &time ) { FromTime(time); }

		// ==============================================================================
		//! Initialize from a time_t object
		//!
		//! @param	time	A time_t object to init from
		// ==============================================================================
		void		FromTime( const time_t &time );

		// ==============================================================================
		//! Use current time
		// ==============================================================================
		void		UseCurrentTime( void );

		// ==============================================================================
		//! Get the seconds part
		//!
		//! @return	The seconds ( 0-59 )
		// ==============================================================================
		int			GetSeconds( void ) { return tm_sec; }

		// ==============================================================================
		//! Get the minutes part
		//!
		//! @return	The seconds ( 0-59 )
		// ==============================================================================
		int			GetMinutes( void ) { return tm_min; }

		// ==============================================================================
		//! Get the hours part
		//!
		//! @return	The hours ( 0-23 )
		// ==============================================================================
		int			GetHours( void ) { return tm_hour; }

		// ==============================================================================
		//! Get the month part
		//!
		//! @return	The month ( 1-12 )
		// ==============================================================================
		int			GetMonth( void ) { return tm_mon + 1; }

		// ==============================================================================
		//! Get the year part
		//!
		//! @return	The year ( 1900 - ? )
		// ==============================================================================
		int			GetYear( void ) { return tm_year + 1900; }

		// ==============================================================================
		//! Get the day of the week
		//!
		//! @return	The day of the week ( 0-6, 0 = sunday )
		// ==============================================================================
		int			GetDayOfWeek( void ) { return tm_wday; }

		// ==============================================================================
		//! Get the day of the month
		//!
		//! @return	The day of the month ( 1-31 )
		// ==============================================================================
		int			GetDayOfMonth( void ) { return tm_mday; }

		// ==============================================================================
		//! Get the day of the year
		//!
		//! @return	The day of the year ( 0-365 )
		// ==============================================================================
		int			GetDayOfYear( void ) { return tm_yday; }

		// ==============================================================================
		//! Get the day light savings mode
		//!
		//! @return	true when using daylight savings 
		// ==============================================================================
		int			GetDayLightSavings( void ) { return tm_isdst; }

		// ==============================================================================
		//! Compares this object with another one
		//!
		//! @param	other	The time_t to compare with
		//!
		//! @return	Negative if 'other' is less than '', 0 if they are equal, or positive if it is
		//! greater. 
		// ==============================================================================
		int			Compare( const struct tm &other );

		// Copying operators
		DateTime &	operator=( const time_t &other ) { FromTime(other); return *this; }
		DateTime &	operator=( const struct tm &other ) { memcpy( this, &other, sizeof(struct tm) ); return *this; }
		DateTime &	operator=( const DateTime &other ) { memcpy( this, &other, sizeof(DateTime) ); return *this; }

		// Comparing operators
		bool		operator>( const DateTime &other ) { return Compare(other) > 0; }
		bool		operator<( const DateTime &other ) { return Compare(other) < 0; }
		bool		operator>=( const DateTime &other ) { return Compare(other) >= 0; }
		bool		operator<=( const DateTime &other ) { return Compare(other) <= 0; }
		bool		operator==( const DateTime &other ) { return Compare(other) == 0; }
		bool		operator!=( const DateTime &other ) { return Compare(other) != 0; }
		
		bool		operator>( const struct tm &other ) { return Compare(other) > 0; }
		bool		operator<( const struct tm &other ) { return Compare(other) < 0; }
		bool		operator>=( const struct tm &other ) { return Compare(other) >= 0; }
		bool		operator<=( const struct tm &other ) { return Compare(other) <= 0; }
		bool		operator==( const struct tm &other ) { return Compare(other) == 0; }
		bool		operator!=( const struct tm &other ) { return Compare(other) != 0; }
	};
//! @}
}

#endif
