#!lua

-- Additional Linux libs: "X11", "Xxf86vm", "Xi", "Xrandr", "stdc++"

includeDirList = {
    "../shared",
    "../shared/include",
    "../shared/gl3w",
    "../shared/imgui",
    "./irrKlang-64bit-1.5.0/include",
    "."
}

libDirectories = {
    "../lib",
    "./irrKlang-64bit-1.5.0/lib/Winx64-visualStudio/irrKlang.lib",
    "./irrKlang-64bit-1.5.0/bin/linux-gcc-64/libIrrKlang.so",
    "./irrKlang-64bit-1.5.0/bin/linux-gcc-64/ikpMP3.so",
    "./irrKlang-64bit-1.5.0/bin/linux-gcc-64/ikpFlac.so",
}


if os.get() == "macosx" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "glfw3",
        "lua"

    }
end

if os.get() == "linux" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "glfw3",
        "lua",
        "GL",
        "Xinerama",
        "Xcursor",
        "Xxf86vm",
        "Xi",
        "Xrandr",
        "X11",
        "stdc++",
        "dl",
        "pthread",
	"irrKlang"
    }
end

-- Build Options:
if os.get() == "macosx" then
    linkOptionList = { "-framework IOKit", "-framework Cocoa", "-framework CoreVideo", "-framework OpenGL" }
end

buildOptions = {"-std=c++11","-I=\"./irrKlang-64bit-1.5.0/include\"", "-L=\"/usr/lib\"", "../irrKlang-64bit-1.5.0/bin/linux-gcc-64/libIrrKlang.so", "-pthread"}

solution "CS488-Projects"
    configurations { "Debug", "Release" }

    project "A5"
        kind "ConsoleApp"
        language "C++"
        location "build"
        objdir "build"
        targetdir "."
        buildoptions (buildOptions)
        libdirs (libDirectories)
        links (linkLibs)
        linkoptions (linkOptionList)
        includedirs (includeDirList)
        files { "*.cpp" }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
