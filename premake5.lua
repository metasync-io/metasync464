workspace "AsioRSPS"
	architecture "x64"
	startproject "Network-Skeleton"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "AsioRSPS-Skeleton"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "epch.h"
	pchsource "src/epch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp"
	}
	
	includedirs
    {
		"vendor/asio/include",
		"vendor/spdlog/include",
	}

	filter { "system:windows", "configurations:Debug" }
		links
		{
		}
		
	filter { "system:windows", "configurations:Release or configurations:Dist" }	
		links
		{
		}

	filter "system:windows"
		systemversion "latest"
		defines { "SKELETON_PLATFORM_WINDOWS" }
		buildoptions { "/utf-8" }

	filter "configurations:Debug"
		defines "EL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "EL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "EL_DIST"
		runtime "Release"
		optimize "on"
