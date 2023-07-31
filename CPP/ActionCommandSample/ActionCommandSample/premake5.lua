project "ActionCommandSample"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../../bin-int/" .. outputdir .. "/%{prj.name}")
    
    pchheader "stdafx.h"
    pchsource "%{wks.location}/NMROSCore/vendor/Camera/JAISamples/CPP/ActionCommandSample/ActionCommandSample/stdafx.cpp"
    
    characterset ("ASCII")
    
    defines
    {
      "_AFXDLL"
    }

    flags
    {
      "MFC"
    }

    includedirs
    {
        "%{IncludeDir.glm}",
        "%{IncludeDir.Eigen}",
        "%{IncludeDir.Camera_JAI}",
        "%{wks.location}/NMROSCore/src",
        "%{wks.location}/NMROSCore/vendor",
        "%{wks.location}/NMROSCore/vendor/spdlog/include",
    }
    libdirs
    {
        "%{wks.location}/NMROSCore/vendor/Camera/libs/JAI/lib/Win64_x64"
    }
    links
    {
        "Jai_Factory.lib",
        "NMROSCore"
    }

    files
    {
        "**.h",
        "**.cpp",
        "**.rc",
    }
    
    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        systemversion "latest"
        defines "NMROS_DEBUG"
        runtime "Debug"
        symbols "on"
        staticruntime "off"

    filter "configurations:Release"
        systemversion "latest"
        defines "NMROS_RELEASE"
        runtime "Release"
        optimize "on"
        staticruntime "on"
