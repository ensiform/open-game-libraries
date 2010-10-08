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
		kind "ConsoleApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { librariesPath .. "/Include" }
		links { "ogCommon" }
		files { toolsPath .. "/SHaGen/**.h", toolsPath .. "/SHaGen/**.inl", toolsPath .. "/SHaGen/**.cpp" }
		objdir( objectDir .. "/Examples/SHaGen" )

	-- TestFS Executable
	project "TestFS"
		kind "ConsoleApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { librariesPath .. "/Include" }
		links { "ogCommon", "ogFileSystem", "zLib", "winmm" }
		files { examplesPath .. "/TestFS/**.h", examplesPath .. "/TestFS/**.inl", examplesPath .. "/TestFS/**.cpp" }
		objdir( objectDir .. "/Examples/TestFS" )

	-- TestGloot Executable
	project "TestGloot"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogCommon", "ogGloot", "opengl32", "winmm" }
		files { examplesPath .. "/TestGloot/**.h", examplesPath .. "/TestGloot/**.inl", examplesPath .. "/TestGloot/**.cpp" }
		objdir( objectDir .. "/Examples/TestGloot" )

	-- TestGloot Executable
	project "TestGloot"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogCommon", "ogGloot", "opengl32", "winmm" }
		files { examplesPath .. "/TestGloot/**.h", examplesPath .. "/TestGloot/**.inl", examplesPath .. "/TestGloot/**.cpp" }
		objdir( objectDir .. "/Examples/TestGloot" )

	-- TestModel Executable
	project "TestModel"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogCommon", "ogGloot", "ogModel", "ogImage", "ogFileSystem", "jpeg", "png", "zLib", "ogConsole", "opengl32", "winmm" }
		files { examplesPath .. "/TestModel/**.h", examplesPath .. "/TestModel/**.inl", examplesPath .. "/TestModel/**.cpp" }
		objdir( objectDir .. "/Examples/TestModel" )

	-- DemoAsteroids Executable
	project "DemoAsteroids"
		kind "WindowedApp"
		language "C++"
		targetdir( binaryDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		links { "ogCommon", "ogGloot", "ogAudio", "ogImage", "ogFileSystem", "ogFable", "ogFont", "soft_oal", "jpeg", "png", "zLib", "ogg", "vorbis", "opengl32", "winmm", "dinput8", "xinput", "dxguid" }
		files { examplesPath .. "/DemoAsteroids/**.h", examplesPath .. "/DemoAsteroids/**.inl", examplesPath .. "/DemoAsteroids/**.cpp" }
		files { examplesPath .. "/Shared/**.h", examplesPath .. "/Shared/**.inl", examplesPath .. "/Shared/**.cpp" }
		objdir( objectDir .. "/Examples/DemoAsteroids" )
