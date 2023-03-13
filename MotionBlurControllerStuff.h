#include "pch.h"
#include "stdio.h"
#include <string>
#include <windows.h>
#include "includes\FileWatch.hpp"
#include "includes\IniReader.h"
#include "includes\injector\injector.hpp"

#include "GlobalVariables.h"
//#include "InGameFunctions.h"
//#include "Helpers.h"

void ReadConfig()
{
	// Get values from INI
	MotionBlur = Settings.ReadInteger("Main", "MotionBlur", 1) != 0;
	g_MotionBlurEnable = Settings.ReadInteger("Main", "g_MotionBlurEnable", 1) != 0;
	MotionBlurAmount = Settings.ReadFloat("Main", "MotionBlurAmount", 0.25f);
	MotionBlurEffectiveSpeed = Settings.ReadFloat("Main", "MotionBlurMinEffectiveSpeed", 1.0f);

	ThreadDelay = Settings.ReadInteger("Misc", "ThreadDelay", 5); // ms
}

void SetValues()
{
	// Enable/Disable Motion Blur (from WS fix)
	injector::WriteMemory<BYTE>(0x6DF1D2, MotionBlur ? 0x74 : 0xEB, true); // Toggle check
	injector::WriteMemory<BYTE>(_g_MotionBlurEnable, g_MotionBlurEnable, true); // Also change in-game variable

	// Set Motion Blur Amount (referencing to a new value as original is also used in different places)
	injector::WriteMemory(0x6DBD46, &MotionBlurAmount, true); // sub_6DBB20
	injector::WriteMemory(0x6DBD56, &MotionBlurAmount, true);
	injector::WriteMemory(0x6DBD66, &MotionBlurAmount, true);
	injector::WriteMemory(0x6DBD74, &MotionBlurAmount, true);

	// Set Motion Blur Minimum Effective Speed
	injector::WriteMemory<float>(_MotionBlurEffectiveSpeed, MotionBlurEffectiveSpeed, true); // 1 reference: sub_6DBB20
}

void ReloadConfig()
{
	ReadConfig();
	SetValues();
}

DWORD WINAPI FileWatchThread(LPVOID)
{
	while (true)
	{
		Sleep(ThreadDelay);

		static filewatch::FileWatch<std::string> watch(Settings.GetIniPath(), [&](const std::string& path, const filewatch::Event change_type)
		{
			if (change_type == filewatch::Event::modified)
			{
				ReloadConfig();
			}
		});
	}
}

void Init()
{
	ReloadConfig();

	// Auto-detect config file changes
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&FileWatchThread, NULL, 0, NULL);
}

