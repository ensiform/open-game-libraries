-- Tools Solution
solution "Tools"
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
		objdir( objectDir .. "/Tools/SHaGen" )
