#include "pch.h"
#include "stdio.h"
#include <string>
#include <windows.h>
#include "includes\IniReader.h"
#include "includes\injector\injector.hpp"

#include "GlobalVariables.h"
//#include "InGameFunctions.h"
//#include "Helpers.h"

void ReloadConfig(); // shut up C3861

DWORD WINAPI HotkeyThread(LPVOID)
{
	while (true)
	{
		Sleep(ThreadDelay);

		// F3
		if ((GetAsyncKeyState(hotkeyReloadConfig) & 1))
		{
			ReloadConfig();

			while ((GetAsyncKeyState(hotkeyReloadConfig) & 0x8000) > 0) { Sleep(ThreadDelay); }
		}
	}
}

void ReadConfig()
{
	// Get values from INI
	CIniReader Settings("NFSMWMotionBlurControllerSettings.ini");

	hotkeyReloadConfig = Settings.ReadInteger("Hotkeys", "ReloadConfig", VK_F3); // F3
	ThreadDelay = Settings.ReadInteger("Hotkeys", "ThreadDelay", 5); // ms

	MotionBlur = Settings.ReadFloat("Main", "MotionBlur", 1) != 0;
	MotionBlurAmount = Settings.ReadFloat("Main", "MotionBlurAmount", 0.25f);
	MotionBlurEffectiveSpeed = Settings.ReadFloat("Main", "MotionBlurMinEffectiveSpeed", 1.0f);
}

void SetValues()
{
	// Enable/Disable Motion Blur (from WS fix)
	injector::WriteMemory<BYTE>(0x6DF1D2, MotionBlur ? 0x74 : 0xEB, true); // Toggle check
	injector::WriteMemory<BYTE>(_g_MotionBlurEnable, MotionBlur, true); // Also change in-game variable

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

void Init()
{
	ReadConfig();
	SetValues();

	// Hotkey detection thread to reload config any time
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&HotkeyThread, NULL, 0, NULL);
}