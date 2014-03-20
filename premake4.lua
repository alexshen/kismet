solution "kismet"
    -- Need c++11
    if _ACTION == 'gmake' then
        buildoptions { '-std=c++11' }
    end

    configurations { "debug", "release" }
    location "build"
    includedirs "include"
    targetdir "lib"

    project "common"
        kind "StaticLib"
        language "C++"
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
