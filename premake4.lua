solution "kismet"
    configurations { "debug", "release" }
    location "build"
    includedirs "include"
    project "common"
        kind "StaticLib"
        language "C++"
        files
        {
            "include/kismet/*.h",
            "source/*.cpp",
        }

    project "math"
        kind "StaticLib"
        language "C++"
        files
        {
            "include/kismet/math/**.h",
            "source/math/**.cpp",
        }
        targetdir "lib"

        configuration "debug"
            defines { "DEBUG" }
            targetname "mathd"

        configuration "release"
            defines { "NDEBUG" }
            targetname "math"
