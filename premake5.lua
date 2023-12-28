workspace "graphgen"

configurations { "Debug", "Release" }

cppdialect "C++20"

filter "configurations:Debug" 
    symbols "On"
filter "configurations:Release"
    optimize "Speed"
    defines "NDEBUG"
filter {}

flags { "MultiProcessorCompile" }

targetdir "build/bin/%{cfg.longname}"
objdir "build/obj/%{cfg.longname}"

architecture "x86_64"

filter "system:linux or macosx"
    buildoptions { 
        "-Wconversion", 
        "-Wall",
        "-Wpedantic",
        "-Wold-style-cast", 
        "-Wno-sign-compare", 
        "-Wno-unused-parameter",
    }
filter {}

--- Test project ---
project "test"

location "."
kind "ConsoleApp"
language "C++"
externalincludedirs "include"

files { 
    "test/main.cpp",
}

links "graphgen"

--- Lib project ---
include "lib.lua"
