#include "pch.h"
#include "stdio.h"
#include <string>
#include <windows.h>
#include "includes\IniReader.h"
#include "includes\injector\injector.hpp"

#include "GlobalVariables.h"
//#include "InGameFunctions.h"
//#include "Helpers.h"

void Init()
{
	// Get values from INI
	CIniReader Settings("NFSMWMotionBlurControllerSettings.ini");

	MotionBlurAmount = Settings.ReadFloat("Main", "MotionBlurAmount", 0.25f);
	MotionBlurEffectiveSpeed = Settings.ReadFloat("Main", "MotionBlurMinEffectiveSpeed", 1.0f);

	// Set Motion Blur Amount (referencing to a new value as original is also used in different places)
	injector::WriteMemory(0x6DBD46, &MotionBlurAmount, true); // sub_6DBB20
	injector::WriteMemory(0x6DBD56, &MotionBlurAmount, true);
	injector::WriteMemory(0x6DBD66, &MotionBlurAmount, true);
	injector::WriteMemory(0x6DBD74, &MotionBlurAmount, true);

	// Set Motion Blur Minimum Effective Speed
	injector::WriteMemory<float>(_MotionBlurEffectiveSpeed, MotionBlurEffectiveSpeed, true); // 1 reference: sub_6DBB20
}