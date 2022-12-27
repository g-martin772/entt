project "entt"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    architecture "x64"

    configurations {
        "Debug",
        "Resease",
        "Dist"
    }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("int/" .. outputdir .. "/%{prj.name}")

    files {
        "**.h",
        "**.cpp",
        "**.hpp",
        "**.inl",
        "**.c",
    }

    excludes { "tests/**.hpp", "tests/**.cpp", "tests/**.h" }

    includedirs {
        "",
        "single_include",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        optimize "On"