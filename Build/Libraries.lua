-- OG Libraries Solution
solution "Libraries"
	configurations { "Debug", "Release" }
	location (_ACTION)
 
	-- [start] Settings that are true for all projects
	if isVisualStudio then
		defines { "WIN32", "_CRT_SECURE_NO_DEPRECATE" }
	end

	configuration "Debug"
		if isVisualStudio then
			defines { "_DEBUG" }
		else
			defines { "DEBUG" }
		end
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }

	-- [end] Settings that are true for all projects

	-- ogCommon Static Library
	project "ogCommon"
		uuid "3F587580-96AD-8142-94F9-6DB09A06E6F2"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Common/**.h", librariesPath .. "/Include/og/Common/**.inl", librariesPath .. "/Source/og/Common/**.cpp" }
		objdir( objectDir .. "/ogCommon" )

	-- ogShared Static Library
	project "ogShared"
		uuid "F17E8FA5-B6DE-4FD8-9A04-A446D2D55945"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Shared/**.h", librariesPath .. "/Include/og/Shared/**.inl", librariesPath .. "/Source/og/Shared/**.cpp" }
		objdir( objectDir .. "/ogShared" )

	-- ogFont Static Library
	project "ogFont"
		uuid "3CA644BA-99E4-DC47-B800-13AB49737240"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Font/**.h", librariesPath .. "/Include/og/Font/**.inl", librariesPath .. "/Source/og/Font/**.cpp", librariesPath .. "/Source/og/Font/**.h" }
		objdir( objectDir .. "/ogFont" )

	-- ogModel Static Library
	project "ogModel"
		uuid "7050D2CC-7C8E-894D-95AD-DFD068195DBD"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Model/**.h", librariesPath .. "/Include/og/Model/**.inl", librariesPath .. "/Source/og/Model/**.cpp", librariesPath .. "/Source/og/Model/**.h" }
		objdir( objectDir .. "/ogModel" )

	-- ogImage Static Library
	project "ogImage"
		uuid "1DCEA802-9B01-8B46-8F90-53BC18F64E9B"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Image/**.h", librariesPath .. "/Include/og/Image/**.inl", librariesPath .. "/Source/og/Image/**.cpp", librariesPath .. "/Source/og/Image/**.h" }
		objdir( objectDir .. "/ogImage" )

	-- ogAudio Static Library
	project "ogAudio"
		uuid "6AA5A8D8-BCE0-9041-8706-D4886C3395D9"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Audio/**.h", librariesPath .. "/Include/og/Audio/**.inl", librariesPath .. "/Source/og/Audio/**.cpp", librariesPath .. "/Source/og/Audio/**.h" }
		objdir( objectDir .. "/ogAudio" )

	-- ogFileSystem Static Library
	project "ogFileSystem"
		uuid "D0E9A670-1E29-8446-9F86-531EE69E9642"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/FileSystem/**.h", librariesPath .. "/Include/og/FileSystem/**.inl", librariesPath .. "/Source/og/FileSystem/**.cpp", librariesPath .. "/Source/og/FileSystem/**.h" }
		objdir( objectDir .. "/ogFileSystem" )
		if isWindows then
			defines { "UNICODE" }
		end

	-- ogFable Static Library
	project "ogFable"
		uuid "62CDA508-7A9F-8144-9A39-2C90A31D1113"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Fable/**.h", librariesPath .. "/Include/og/Fable/**.inl", librariesPath .. "/Source/og/Fable/**.cpp", librariesPath .. "/Source/og/Fable/**.h" }
		objdir( objectDir .. "/ogFable" )
		if isWindows then
			defines { "UNICODE" }
		end

	-- ogGloot Static Library
	project "ogGloot"
		uuid "B87DA50A-D68B-054D-9557-E03A8235CEAF"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Gloot/**.h", librariesPath .. "/Include/og/Gloot/**.inl", librariesPath .. "/Source/og/Gloot/**.cpp", librariesPath .. "/Source/og/Gloot/**.h" }
		objdir( objectDir .. "/ogGloot" )
		if isWindows then
			defines { "UNICODE" }
		end

	-- ogConsole Static Library
	project "ogConsole"
		uuid "8019B46F-6E9B-AC43-8C0F-366BC62B1239"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Console/**.h", librariesPath .. "/Include/og/Console/**.inl", librariesPath .. "/Source/og/Console/**.cpp", librariesPath .. "/Source/og/Console/**.h" }
		objdir( objectDir .. "/ogConsole" )
		if isWindows then
			defines { "UNICODE" }
		end
