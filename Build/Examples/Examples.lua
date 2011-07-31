-- Examples Solution
solution "Examples"
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

	-- SHaGen Executable
	project "SHaGen"
		uuid "5AB4EE5D-0AF1-CA45-8AA5-ED4F2DD9CCDD"
		kind "ConsoleApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { librariesPath .. "/Include" }
		links { "ogShared" }
		files { toolsPath .. "/SHaGen/**.h", toolsPath .. "/SHaGen/**.inl", toolsPath .. "/SHaGen/**.cpp" }
		objdir( objectDir .. "/Examples/SHaGen" )

	-- TestPluginEngine Executable
	project "TestPluginEngine"
		uuid "CD5F3292-C3E8-5F4D-8684-7FAA20B46EDF"
		kind "ConsoleApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { librariesPath .. "/Include" }
		links { "ogShared", "ogCommon", "ogPlugin" }
		files { examplesPath .. "/TestPlugin/Game.h", examplesPath .. "/TestPlugin/Engine.cpp", examplesPath .. "/Shared/User.cpp" }
		objdir( objectDir .. "/Examples/TestPlugin" )

	-- TestPluginGame Shared Library
	project "TestPluginGame"
		uuid "75566441-1262-234E-9F72-130D44CAE76D"
		kind "SharedLib"
		language "C++"
		targetdir( binaryDir )
		includedirs { librariesPath .. "/Include" }
		links { "ogShared" }
		files { examplesPath .. "/TestPlugin/Game.h", examplesPath .. "/TestPlugin/Game.cpp", examplesPath .. "/TestPlugin/Game.def", examplesPath .. "/Shared/User.cpp" }
		objdir( objectDir .. "/Examples/TestPlugin" )
		
	-- TestFS Executable
	project "TestFS"
		uuid "81E27090-9EF9-574C-9744-6C2A47AB3374"
		kind "ConsoleApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { librariesPath .. "/Include", liblfdsPath .. "/inc" }
		links { "ogFileSystem", "ogShared", "ogCommon", "zLib", "liblfds" }
		if isWindows then
			links { "winmm" }
		end
		if isLinux then
			links{ "boost_thread" }
		end
		files { examplesPath .. "/TestFS/**.h", examplesPath .. "/TestFS/**.inl", examplesPath .. "/TestFS/**.cpp", examplesPath .. "/Shared/User.cpp" }
		objdir( objectDir .. "/Examples/TestFS" )

	-- TestGloot Executable
	project "TestGloot"
		uuid "0A6C6FD3-97EC-4B49-8958-31AF782C07D7"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogShared", "ogCommon", "ogMath", "ogGloot", "liblfds", "opengl32" }
		if isWindows then
			links { "winmm" }
		end
		files { examplesPath .. "/TestGloot/**.h", examplesPath .. "/TestGloot/**.inl", examplesPath .. "/TestGloot/**.cpp", examplesPath .. "/Shared/User.cpp" }
		objdir( objectDir .. "/Examples/TestGloot" )

	-- TestAudio Executable
	project "TestAudio"
		uuid "E089D8F8-23C2-184B-B3FA-EC19550AB970"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogShared", "ogCommon", "ogMath", "ogGloot", "ogAudio", "ogImage", "ogFileSystem", "ogFont", "soft_oal", "ogg", "vorbis", "jpeg", "png", "zLib", "liblfds", "opengl32" }
		if isWindows then
			links { "winmm" }
		end
		files { examplesPath .. "/TestAudio/**.h", examplesPath .. "/TestAudio/**.inl", examplesPath .. "/TestAudio/**.cpp" }
		files { examplesPath .. "/Shared/SoundManager.*", examplesPath .. "/Shared/FontText.*", examplesPath .. "/Shared/User.cpp" }
		objdir( objectDir .. "/Examples/TestAudio" )

	-- TestModel Executable
	project "TestModel"
		uuid "313093B4-A4E7-BD41-8184-3BCB6353F661"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogShared", "ogCommon", "ogMath", "ogGloot", "ogModel", "ogImage", "ogFileSystem", "jpeg", "png", "zLib", "ogConsole", "liblfds", "opengl32" }
		if isWindows then
			links { "winmm" }
		end
		files { examplesPath .. "/TestModel/**.h", examplesPath .. "/TestModel/**.inl", examplesPath .. "/TestModel/**.cpp", examplesPath .. "/Shared/User.cpp" }
		objdir( objectDir .. "/Examples/TestModel" )

	-- DemoAsteroids Executable
	project "DemoAsteroids"
		uuid "C2F4FA57-04FC-1744-A72F-9007C32FB203"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogShared", "ogCommon", "ogMath", "ogGloot", "ogAudio", "ogImage", "ogFileSystem", "ogFable", "ogFont", "soft_oal", "jpeg", "png", "zLib", "ogg", "vorbis", "liblfds", "opengl32" }
		if isWindows then
			links { "winmm", "dinput8", "xinput", "dxguid" }
		end
		files { examplesPath .. "/DemoAsteroids/**.h", examplesPath .. "/DemoAsteroids/**.inl", examplesPath .. "/DemoAsteroids/**.cpp" }
		files { examplesPath .. "/Shared/**.h", examplesPath .. "/Shared/**.inl", examplesPath .. "/Shared/**.cpp" }
		objdir( objectDir .. "/Examples/DemoAsteroids" )
