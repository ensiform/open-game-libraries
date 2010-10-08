-- Prevent from script error when no action is given
if _ACTION then
	isVisualStudio = string.startswith(_ACTION, "vs")

	-- Some Paths
	rootPath = ".."
	
	objectDir		= "out/obj"
	libDir			= "out"
	binaryDir		= rootPath .. "/Binaries"
	
	librariesPath	= rootPath .. "/Libraries"
	examplesPath	= rootPath .. "/Examples"
	toolsPath		= rootPath .. "/Tools"

	-- Thirdparty Paths
	thirdPartyPath	= rootPath .. "/Thirdparty"
	zlibPath		= thirdPartyPath .. "/zlib"
	pngPath			= thirdPartyPath .. "/libpng"
	jpegPath		= thirdPartyPath .. "/jpeg"
	oggPath			= thirdPartyPath .. "/ogg"
	vorbisPath		= thirdPartyPath .. "/vorbis"
	oalPath			= thirdPartyPath .. "/AL"

	-- The Solutions
	dofile "Thirdparty.lua"
	dofile "Libraries.lua"
	dofile "Examples.lua"
end