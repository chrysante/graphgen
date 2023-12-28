project "graphgen"

location "."
kind "StaticLib"
language "C++"
includedirs { "src", "include/graphgen" }
externalincludedirs "include"

files { 
    "include/graphgen/**.h",
    "lib/**.h",
    "lib/**.cpp"
}

filter "system:linux"
    buildoptions "-fPIC"
filter {}
