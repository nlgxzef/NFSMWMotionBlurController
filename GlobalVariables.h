#pragma once

CIniReader Settings("NFSMWMotionBlurControllerSettings.ini");

bool MotionBlur = 1;
bool g_MotionBlurEnable = 1;
int ThreadDelay = 5;

float MotionBlurAmount = 0.25f; // 1.10f in vanilla game
float MotionBlurEffectiveSpeed = 1.0f; // 25.0f in vanilla game
#define _MotionBlurEffectiveSpeed 0x8F9B10
#define _g_MotionBlurEnable 0x9017DC