-- Prevent from script error when no action is given
if not _ACTION then
	printf "Error: No action defined!"
	return
end

-- Check for supported OS and action
if os.is( "windows" ) then
	isWindows = true
	if string.startswith(_ACTION, "vs") then
		isVisualStudio = true
	else
		printf "Warning: Not tested for this action yet!"
	end
	
	-- OpenAL Soft's have's
	oal_soft_have = { WINMM=true, DSOUND=true, -- audio
						-- todo: which of these are compiler specific ?
						STAT=true, POWF=true, SQRTF=true, ACOSF=true,
						ATANF=true, FABSF=true, __INT64=true, FLOAT_H=true, _CONTROLFP=true }
elseif os.is( "linux" ) then
	isLinux = true
	printf "Warning: Untested, probably needs adaption!"
	
	-- OpenAL Soft's have's
	-- todo: add the ones available
	oal_soft_have = { ALSA=true, OSS=true }
elseif os.is( "macosx" ) then
	isMac = true
	printf "Warning: Untested, probably needs adaption!"
	
	-- OpenAL Soft's have's
	-- todo: add the ones available
	oal_soft_have = { PORTAUDIO=true, PULSEAUDIO=true } -- fixme: decide on one or both ?
else
	printf "Error: Your OS is not supported yet"
	return
end

-- Some Paths
rootPath = ".."

objectDir		= "out/obj"
libDir			= "out"
binaryDir		= rootPath .. "/Binaries"

librariesPath	= rootPath .. "/Libraries"
libIncludePath	= librariesPath .. "/Include/og"
libSourcePath	= librariesPath .. "/Source/og"
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

-- OpenAL Soft config generation:
dofile "oal_config.lua"

-- The Solutions
dofile "Thirdparty.lua"
dofile "Libraries.lua"
dofile "Examples.lua"