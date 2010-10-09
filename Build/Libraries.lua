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
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Common/**.h", librariesPath .. "/Include/og/Common/**.inl", librariesPath .. "/Source/og/Common/**.cpp" }
		objdir( objectDir .. "/ogCommon" )

	-- ogFont Static Library
	project "ogFont"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Font/**.h", librariesPath .. "/Include/og/Font/**.inl", librariesPath .. "/Source/og/Font/**.cpp" }
		objdir( objectDir .. "/ogFont" )

	-- ogModel Static Library
	project "ogModel"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Model/**.h", librariesPath .. "/Include/og/Model/**.inl", librariesPath .. "/Source/og/Model/**.cpp" }
		objdir( objectDir .. "/ogModel" )

	-- ogImage Static Library
	project "ogImage"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Image/**.h", librariesPath .. "/Include/og/Image/**.inl", librariesPath .. "/Source/og/Image/**.cpp" }
		objdir( objectDir .. "/ogImage" )

	-- ogAudio Static Library
	project "ogAudio"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Audio/**.h", librariesPath .. "/Include/og/Audio/**.inl", librariesPath .. "/Source/og/Audio/**.cpp" }
		objdir( objectDir .. "/ogAudio" )

	-- ogFileSystem Static Library
	project "ogFileSystem"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/FileSystem/**.h", librariesPath .. "/Include/og/FileSystem/**.inl", librariesPath .. "/Source/og/FileSystem/**.cpp" }
		objdir( objectDir .. "/ogFileSystem" )
		if isWindows then
			defines { "UNICODE" }
		end

	-- ogFable Static Library
	project "ogFable"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Fable/**.h", librariesPath .. "/Include/og/Fable/**.inl", librariesPath .. "/Source/og/Fable/**.cpp" }
		objdir( objectDir .. "/ogFable" )
		if isWindows then
			defines { "UNICODE" }
		end

	-- ogGloot Static Library
	project "ogGloot"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Gloot/**.h", librariesPath .. "/Include/og/Gloot/**.inl", librariesPath .. "/Source/og/Gloot/**.cpp" }
		objdir( objectDir .. "/ogGloot" )
		if isWindows then
			defines { "UNICODE" }
		end

	-- ogConsole Static Library
	project "ogConsole"
		kind "StaticLib"
		language "C++"
		targetdir( libDir )
		includedirs { thirdPartyPath, librariesPath .. "/Include" }
		files { librariesPath .. "/Include/og/Console/**.h", librariesPath .. "/Include/og/Console/**.inl", librariesPath .. "/Source/og/Console/**.cpp" }
		objdir( objectDir .. "/ogConsole" )
		if isWindows then
			defines { "UNICODE" }
		end
