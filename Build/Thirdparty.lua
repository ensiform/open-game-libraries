-- Thirdparty Solution
solution "Thirdparty"
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

	-- zLib Static Library
	project "zLib"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { zlibPath .. "/*.h", zlibPath .. "/*.c" }
		objdir( objectDir .. "/zlib" )

	-- png Static Library
	project "png"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { pngPath .. "/*.h", pngPath .. "/*.c" }
		objdir( objectDir .. "/png" )
		includedirs { thirdPartyPath }

	-- jpeg Static Library
	project "jpeg"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { jpegPath .. "/*.h", jpegPath .. "/*.c" }
		objdir( objectDir .. "/jpeg" )

	-- ogg Static Library
	project "ogg"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { oggPath .. "/*.h", oggPath .. "/*.c" }
		objdir( objectDir .. "/ogg" )
		includedirs { thirdPartyPath }

	-- vorbis Static Library
	project "vorbis"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { vorbisPath .. "/*.h", vorbisPath .. "/*.c" }
		objdir( objectDir .. "/vorbis" )
		includedirs { thirdPartyPath }
		excludes { vorbisPath .. "/psytune.c", vorbisPath .. "/tone.c" }
		
	-- soft_oal Static Library
	project "soft_oal"
		kind "SharedLib"
		language "C"
		targetdir( libDir )
		files { oalPath .. "/**.h", oalPath .. "/**.c" }
		objdir( objectDir .. "/soft_oal" )
		includedirs { thirdPartyPath, oalPath .. "/OpenAL32/Include" }
		
		if oal_soft_have["WINMM"] then links { "winmm" } end
		for k, v in pairs (oal_soft_all_outmodules) do 
			if not oal_soft_have[k] then
				excludes { oalPath .. "/Alc/" .. v }
			end
		end
