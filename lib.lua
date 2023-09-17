project "graphgen"

location "."
kind "StaticLib"
language "C++"
externalincludedirs "include"

files { 
    "include/graphgen/**.h",
    "lib/**.h",
    "lib/**.cpp"
}

