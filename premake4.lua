solution "kismet"
    -- Need c++11
    if _ACTION == 'gmake' then
        buildoptions { '-std=c++11' }
    end

    configurations { "debug", "release" }
    location "build"
    includedirs "include"
    libdirs { "lib" }

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
            defines { "DEBUG" }
            targetname "commond"

        configuration "release"
            defines { "NDEBUG" }
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
            defines { "DEBUG" }
            targetname "mathd"

        configuration "release"
            defines { "NDEBUG" }
            targetname "math"

    project "test"
        kind "ConsoleApp"
        language "C++"
        targetdir "test"
        links { "boost_unit_test_framework" }
        files
        {
            "source/test/**.cpp",
        }

        configuration "debug"
            links { "mathd" }
            defines { "DEBUG" }
            targetname "testd"

        configuration "release"
            links { "math" }
            defines { "NDEBUG" }
            targetname "testd"
