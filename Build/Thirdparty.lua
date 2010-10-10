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
		uuid "02FB1266-F6D0-9E40-A46F-EE7595E452DD"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { zlibPath .. "/*.h", zlibPath .. "/*.c" }
		objdir( objectDir .. "/zlib" )

	-- png Static Library
	project "png"
		uuid "4B006485-4BE3-874A-9B7C-C7580D2178D3"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { pngPath .. "/*.h", pngPath .. "/*.c" }
		objdir( objectDir .. "/png" )
		includedirs { thirdPartyPath }

	-- jpeg Static Library
	project "jpeg"
		uuid "3190DB1D-F671-3040-9E1C-945A8FA02FC0"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { jpegPath .. "/*.h", jpegPath .. "/*.c" }
		objdir( objectDir .. "/jpeg" )

	-- ogg Static Library
	project "ogg"
		uuid "7C41CBE5-0DAA-1B40-B4FF-B4F4DE4B8C17"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { oggPath .. "/*.h", oggPath .. "/*.c" }
		objdir( objectDir .. "/ogg" )
		includedirs { thirdPartyPath }

	-- vorbis Static Library
	project "vorbis"
		uuid "8F9C9FF9-8DA1-1445-9070-698E0B76DAD6"
		kind "StaticLib"
		language "C"
		targetdir( libDir )
		files { vorbisPath .. "/*.h", vorbisPath .. "/*.c" }
		objdir( objectDir .. "/vorbis" )
		includedirs { thirdPartyPath }
		excludes { vorbisPath .. "/psytune.c", vorbisPath .. "/tone.c" }
		
	-- soft_oal Static Library
	project "soft_oal"
		uuid "0968CDDA-5D5A-E74F-838D-A4FDDAA24B05"
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
