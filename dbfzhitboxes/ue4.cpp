#include "ue4.h"
#include "sigscan.h"
#include <Windows.h>

using K2_DrawLine_t = void(*)(UCanvas*, FVector2D, FVector2D, float, const FLinearColor&);
K2_DrawLine_t K2_DrawLine_internal;

using K2_Project_t = void(*)(UCanvas*, FVector*, const FVector&);
K2_Project_t K2_Project_internal;

using K2_DrawText_t = void(*)(UCanvas*, UFont*, const FString&, FVector2D, const FLinearColor&, float, const FLinearColor&, FVector2D, bool, bool, bool, const FLinearColor&);
K2_DrawText_t K2_DrawText_internal;

struct get_functions
{
	get_functions()
	{
		sigscan sig("RED-Win64-Shipping.exe");
		K2_DrawLine_internal = (K2_DrawLine_t)(sig.sig("\x0F\x2F\xC8\x76\x72", "xxxxx") - 0x53);
		K2_Project_internal = (K2_Project_t)(sig.sig("\x48\x83\xEC\x30\xF2\x41\x0F\x10\x00\x48\x8B\xDA", "xxxxxxxxxxxx") - 2);
		K2_DrawText_internal = (K2_DrawText_t)(sig.sig("\x41\x83\x78\x08\x01\x48\x8B\xFA", "xxxxxxxx") - 0x1F);
	}
} get_functions_;

void UCanvas::K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, float Thickness, const FLinearColor &RenderColor)
{
	K2_DrawLine_internal(this, ScreenPositionA, ScreenPositionB, Thickness, RenderColor);
}

FVector UCanvas::K2_Project(const FVector &WorldPosition)
{
	FVector out;
	K2_Project_internal(this, &out, WorldPosition);
	return out;
}

void UCanvas::K2_DrawText(
	UFont *RenderFont,
	const FString &RenderText,
	FVector2D ScreenPosition,
	const FLinearColor &RenderColor,
	float Kerning,
	const FLinearColor &ShadowColor,
	FVector2D ShadowOffset,
	bool bCentreX,
	bool bCentreY,
	bool bOutlined,
	const FLinearColor &OutlineColor)
{
	K2_DrawText_internal(this, RenderFont, RenderText, ScreenPosition, RenderColor, Kerning, ShadowColor, ShadowOffset, bCentreX, bCentreY, bOutlined, OutlineColor);
}

UEngine* UEngine::EnginePtr = nullptr;

float UEngine::GetFixedFrameRate()
{
	return *(float*)((char*)(this) + 0x64C);
}

void UEngine::SetFixedFrameRate(float NewFixedFrameRate)
{
	float* FixedFrameRatePtr = (float*)((char*)(this) + 0x64C);
	*FixedFrameRatePtr = NewFixedFrameRate;
}

UEngine* UEngine::Get()
{
	if (EnginePtr == nullptr)
	{
		sigscan sig("RED-Win64-Shipping.exe");
		uintptr_t AHUD_GetTextSizePtr = sig.sig("\x48\x89\x5c\x24\x00\x48\x89\x6c\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x00\x49\x8B\x00\x49\x8B\x00\x48\x8B\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x84\xC0\x74\x3B", "xxxx?xxxx?xxxx?xxxx?xx?xx?xx?xx?x????xxxx");
		
		uintptr_t GetMediumFontCallSite = AHUD_GetTextSizePtr + 0x33; // AHUD::GetTextSize calls GEngine->GetMediumFont() 0x33 bytes into it
		uintptr_t GetMediumFontRelativePtr = *(int*)(GetMediumFontCallSite + 1); // One Byte for the Call Opcode (E8)
		int FunctionCallSize = 5; // call UEngine::GetMediumFont
		uintptr_t GetMediumFontFunction = GetMediumFontCallSite + GetMediumFontRelativePtr + FunctionCallSize;

		uintptr_t GEngineRelativePtr = *(int*)(GetMediumFontFunction + 3); // Three bytes for mov rax from mov rax, GEngineRelativePtr
		int PointerMoveSize = 7; //mov rax, GEngineRelativePtr
		EnginePtr = *(UEngine**)(GetMediumFontFunction + GEngineRelativePtr + PointerMoveSize);
	}
	return EnginePtr;
}
