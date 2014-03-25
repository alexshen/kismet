local win32
if string.find(_ACTION, 'vs') then
    win32 = true
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

        configuration "release"
            targetname "common"

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

        configuration "release"
            targetname "math"

    project "test"
        kind "ConsoleApp"
        language "C++"
        targetdir "test"
        if win32 then
            if not os.getenv('BOOST_INCLUDE') then
                error('BOOST_INCLUDE not specified')
            end
            if not os.getenv('BOOST_LIB') then
                error('BOOST_LIB not specified')
            end
            includedirs { os.getenv('BOOST_INCLUDE') }
            libdirs { os.getenv('BOOST_LIB') }
        else
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
