local win32
if string.find(_ACTION, 'vs') then
    win32 = true
    if not os.getenv('BOOST_INCLUDE') then
        error('BOOST_INCLUDE not specified')
    end
    if not os.getenv('BOOST_LIB') then
        error('BOOST_LIB not specified')
    end
end

local function disable_lang_ext()
    if win32 then
        -- disable language extensions
        buildoptions { '/Za' }
    end
end

solution "kismet"
    -- Need c++11
    if _ACTION == 'gmake' then
        buildoptions { '-std=c++11' }
    end

    configurations { "debug", "release" }
    location "build"
    includedirs { "include" }
    libdirs { "lib" }
    if win32 then
        includedirs { os.getenv('BOOST_INCLUDE') }
        libdirs { os.getenv('BOOST_LIB') }
    end
    flags { "StaticRuntime" }

    if win32 then
        defines { "WIN32", "_SCL_SECURE_NO_WARNINGS" }
        configuration { "debug" }
        defines { "_DEBUG" }
    end

    configuration { "debug" }
        flags { "Symbols" }
        defines { "DEBUG" }

    configuration { "release" }
        flags { "OptimizeSpeed" }
        defines { "NDEBUG" }

    project "common"
        kind "StaticLib"
        language "C++"
        targetdir "lib"
        files
        {
            "include/kismet/*.h",
            "source/*.cpp",
        }
        configuration "debug"
            targetname "commond"
            disable_lang_ext()

        configuration "release"
            targetname "common"
            disable_lang_ext()

    project "ai"
        kind "StaticLib"
        language "C++"
        targetdir "lib"
        files
        {
            "include/kismet/ai/**.h",
            "source/ai/**.cpp",
        }
        configuration "debug"
            targetname "aid"
            disable_lang_ext()

        configuration "release"
            targetname "ai"
            disable_lang_ext()

    project "math"
        kind "StaticLib"
        language "C++"
        targetdir "lib"
        files
        {
            "include/kismet/math/**.h",
            "source/math/**.cpp",
        }

        configuration "debug"
            targetname "mathd"
            disable_lang_ext()

        configuration "release"
            targetname "math"
            disable_lang_ext()

    project "test"
        kind "ConsoleApp"
        language "C++"
        targetdir "test"
        if not win32 then
            links { "boost_unit_test_framework" }
        end
        files
        {
            "source/test/**.cpp",
        }

        flags { "SEH" }
        configuration "debug"
            links { "mathd" }
            targetname "testd"

        configuration "release"
            links { "math" }
            targetname "test"
