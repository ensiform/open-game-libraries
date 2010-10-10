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
		links { "ogShared", "ogCommon" }
		files { toolsPath .. "/SHaGen/**.h", toolsPath .. "/SHaGen/**.inl", toolsPath .. "/SHaGen/**.cpp" }
		objdir( objectDir .. "/Examples/SHaGen" )

	-- TestFS Executable
	project "TestFS"
		uuid "81E27090-9EF9-574C-9744-6C2A47AB3374"
		kind "ConsoleApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { librariesPath .. "/Include" }
		links { "ogShared", "ogCommon", "ogFileSystem", "zLib", "winmm" }
		files { examplesPath .. "/TestFS/**.h", examplesPath .. "/TestFS/**.inl", examplesPath .. "/TestFS/**.cpp" }
		objdir( objectDir .. "/Examples/TestFS" )

	-- TestGloot Executable
	project "TestGloot"
		uuid "0A6C6FD3-97EC-4B49-8958-31AF782C07D7"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogShared", "ogCommon", "ogGloot", "opengl32", "winmm" }
		files { examplesPath .. "/TestGloot/**.h", examplesPath .. "/TestGloot/**.inl", examplesPath .. "/TestGloot/**.cpp" }
		objdir( objectDir .. "/Examples/TestGloot" )

	-- TestModel Executable
	project "TestModel"
		uuid "313093B4-A4E7-BD41-8184-3BCB6353F661"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogShared", "ogCommon", "ogGloot", "ogModel", "ogImage", "ogFileSystem", "jpeg", "png", "zLib", "ogConsole", "opengl32", "winmm" }
		files { examplesPath .. "/TestModel/**.h", examplesPath .. "/TestModel/**.inl", examplesPath .. "/TestModel/**.cpp" }
		objdir( objectDir .. "/Examples/TestModel" )

	-- DemoAsteroids Executable
	project "DemoAsteroids"
		uuid "C2F4FA57-04FC-1744-A72F-9007C32FB203"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogShared", "ogCommon", "ogGloot", "ogAudio", "ogImage", "ogFileSystem", "ogFable", "ogFont", "soft_oal", "jpeg", "png", "zLib", "ogg", "vorbis", "opengl32", "winmm", "dinput8", "xinput", "dxguid" }
		files { examplesPath .. "/DemoAsteroids/**.h", examplesPath .. "/DemoAsteroids/**.inl", examplesPath .. "/DemoAsteroids/**.cpp" }
		files { examplesPath .. "/Shared/**.h", examplesPath .. "/Shared/**.inl", examplesPath .. "/Shared/**.cpp" }
		objdir( objectDir .. "/Examples/DemoAsteroids" )
