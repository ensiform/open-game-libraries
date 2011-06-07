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

	-- ogShared Static Library
	project "ogShared"
		uuid "F17E8FA5-B6DE-4FD8-9A04-A446D2D55945"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { libIncludePath .. "/Setup.h", libIncludePath .. "/Config.h", libIncludePath .. "/Shared.h" }
		files { libIncludePath .. "/Shared/**.h", libIncludePath .. "/Shared/**.inl", libSourcePath .. "/Shared/**.cpp" }
		objdir( objectDir .. "/ogShared" )

	-- ogCommon Static Library
	project "ogCommon"
		uuid "3F587580-96AD-8142-94F9-6DB09A06E6F2"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { libIncludePath .. "/Common.h" }
		files { libIncludePath .. "/Common/**.h", libIncludePath .. "/Common/**.inl", libSourcePath .. "/Common/**.cpp" }
		objdir( objectDir .. "/ogCommon" )

	-- ogMath Static Library
	project "ogMath"
		uuid "FF5B2A74-C2DA-D34A-A780-086B4BEB2F7B"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { libIncludePath .. "/Math.h" }
		files { libIncludePath .. "/Math/**.h", libIncludePath .. "/Math/**.inl", libSourcePath .. "/Math/**.cpp" }
		objdir( objectDir .. "/ogMath" )

	-- ogFont Static Library
	project "ogFont"
		uuid "3CA644BA-99E4-DC47-B800-13AB49737240"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { libIncludePath .. "/Font.h" }
		files { libIncludePath .. "/Font/**.h", libIncludePath .. "/Font/**.inl", libSourcePath .. "/Font/**.cpp", libSourcePath .. "/Font/**.h" }
		objdir( objectDir .. "/ogFont" )

	-- ogModel Static Library
	project "ogModel"
		uuid "7050D2CC-7C8E-894D-95AD-DFD068195DBD"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { libIncludePath .. "/Model.h" }
		files { libIncludePath .. "/Model/**.h", libIncludePath .. "/Model/**.inl", libSourcePath .. "/Model/**.cpp", libSourcePath .. "/Model/**.h" }
		objdir( objectDir .. "/ogModel" )
		
	-- ogPlugin Static Library
	project "ogPlugin"
		uuid "234F3C1E-5568-4647-BD9E-C32E3471FE3C"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { libIncludePath .. "/Plugin.h" }
		files { libIncludePath .. "/Plugin/**.h", libIncludePath .. "/Plugin/**.inl", libSourcePath .. "/Plugin/**.cpp", libSourcePath .. "/Plugin/**.h" }
		objdir( objectDir .. "/ogPlugin" )

	-- ogImage Static Library
	project "ogImage"
		uuid "1DCEA802-9B01-8B46-8F90-53BC18F64E9B"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { libIncludePath .. "/Image.h" }
		files { libIncludePath .. "/Image/**.h", libIncludePath .. "/Image/**.inl", libSourcePath .. "/Image/**.cpp", libSourcePath .. "/Image/**.h" }
		objdir( objectDir .. "/ogImage" )

	-- ogAudio Static Library
	project "ogAudio"
		uuid "6AA5A8D8-BCE0-9041-8706-D4886C3395D9"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { libIncludePath .. "/Audio.h" }
		files { libIncludePath .. "/Audio/**.h", libIncludePath .. "/Audio/**.inl", libSourcePath .. "/Audio/**.cpp", libSourcePath .. "/Audio/**.h" }
		objdir( objectDir .. "/ogAudio" )

	-- ogFileSystem Static Library
	project "ogFileSystem"
		uuid "D0E9A670-1E29-8446-9F86-531EE69E9642"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { libIncludePath .. "/FileSystem.h" }
		files { libIncludePath .. "/FileSystem/**.h", libIncludePath .. "/FileSystem/**.inl", libSourcePath .. "/FileSystem/**.cpp", libSourcePath .. "/FileSystem/**.h" }
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
		files { libIncludePath .. "/Fable.h" }
		files { libIncludePath .. "/Fable/**.h", libIncludePath .. "/Fable/**.inl", libSourcePath .. "/Fable/**.cpp", libSourcePath .. "/Fable/**.h" }
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
		files { libIncludePath .. "/Gloot.h" }
		files { libIncludePath .. "/Gloot/**.h", libIncludePath .. "/Gloot/**.inl", libSourcePath .. "/Gloot/**.cpp", libSourcePath .. "/Gloot/**.h" }
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
		files { libIncludePath .. "/Console.h" }
		files { libIncludePath .. "/Console/**.h", libIncludePath .. "/Console/**.inl", libSourcePath .. "/Console/**.cpp", libSourcePath .. "/Console/**.h" }
		objdir( objectDir .. "/ogConsole" )
		if isWindows then
			defines { "UNICODE" }
		end
