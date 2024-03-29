#define PI 3.14159265359f

#include "FreeCamera.h"
#include <cmath>
#include "Pl0000.h"
#include "cCameraGame.h"
#include "shared.h"
#include "IniReader.h"
#include "cGameUIManager.h"
#include "GameMenuStatus.h"
#include "StpFlags.h"

#include "cCameraGame.cpp"
#include "cGameUIManager.cpp"
#include "cVec4.cpp"

struct cVec2
{
	float x;
	float y;
};

using namespace shared;

// IT CAN'T FIND CPP FILES

bool FreeCamera::CanRun() noexcept
{
	if (!*(bool*)(shared::base + 0x19D509C))
		return false;

	if (!key)
		return false;

	return true;
}

void FreeCamera::Run() noexcept
{
	if (!CanRun())
		return;

	StpFlags &stpFlags = g_StpFlags;
	static float lastFov = 0.0f;
	static bool once = false;

	if (IsKeyPressed(key, false))
		active = !active;

	cCameraGame* CameraGame = &g_cCameraGame;

	float& yaw = CameraGame->field_364;
	float& pitch = CameraGame->field_218;
	float& fov = CameraGame->m_vecViewAngle.z;
	float& roll = CameraGame->m_vecViewAngle.x;

	cVec4& rotate = CameraGame->m_vecRotationAt;
	cVec4& pos = CameraGame->m_vecPosition;

	static bool onceSleep = false;

	if (!active && !once)
	{
		stpFlags.STP_OBJ = false;
		stpFlags.STP_UI = false;
		stpFlags.STP_ESP = false;
		stpFlags.STP_TRIGGER = false;
		if (g_GameMenuStatus == InGame)
		{
			*(bool*)(base + 0x19C2D54) = false;
			stpFlags.STP_UI = false;
		}
		fov = lastFov;
		once = true;
		onceSleep = false;
	}

	if (active)
	{
		float speed = controlSpeed;

		static bool fovChangeOnce = false;

		if (!fovChangeOnce)
		{
			lastFov = fov;
			fovChangeOnce = true;
		}

		if (IsKeyPressed(VK_SHIFT))
			speed = turboSpeed;
		else if (IsKeyPressed(VK_LMENU))
			speed /= 2;

		if (IsKeyPressed(87))
		{
			pos.x += speed * sin(yaw);
			pos.y += speed * sin(pitch);
			pos.z += speed * cos(yaw);

			rotate.x += speed * sin(yaw);
			rotate.y += speed * sin(pitch);
			rotate.z += speed * cos(yaw);
		}
		else if (IsKeyPressed(83))
		{
			pos.x -= speed * sin(yaw);
			pos.y -= speed * sin(pitch);
			pos.z -= speed * cos(yaw);

			rotate.x -= speed * sin(yaw);
			rotate.y -= speed * sin(pitch);
			rotate.z -= speed * cos(yaw);
		}

		if (IsKeyPressed(65))
		{
			pos.x -= speed * sin(yaw - DegreeToRadian(90));
			pos.z -= speed * cos(yaw - DegreeToRadian(90));

			rotate.x -= speed * sin(yaw - DegreeToRadian(90));
			rotate.z -= speed * cos(yaw - DegreeToRadian(90));
		}
		else if (IsKeyPressed(68))
		{
			pos.x += speed * sin(yaw - DegreeToRadian(90));
			pos.z += speed * cos(yaw - DegreeToRadian(90));

			rotate.x += speed * sin(yaw - DegreeToRadian(90));
			rotate.z += speed * cos(yaw - DegreeToRadian(90));
		}

		if (IsKeyPressed(VK_SPACE))
		{
			pos.y += speed;
			rotate.y += speed;
		}
		else if (IsKeyPressed(VK_CONTROL))
		{
			pos.y -= speed;
			rotate.y -= speed;
		}

		tagPOINT absRes = *(tagPOINT*)(base + 0x14CE9A4);
		tagPOINT center = tagPOINT(absRes.x / 2, absRes.y / 2);

		tagPOINT mousePosition = *(tagPOINT*)(base + 0x14CE9CC);

		float mouseDelta[2] = { (float)(center.x - mousePosition.x) / 1000.0f, (float)(center.y - mousePosition.y) / 1000.0f};

		if (g_GameMenuStatus == InGame)
		{
			if (mouseDelta[1] != 0.0f)
				rotate.y += clamp(mouseDelta[1], -5.0f, 5.0f) * fov;
		}
		else
		{
			if (IsKeyPressed(VK_UP))
				rotate.y += speed * fov;
			else if (IsKeyPressed(VK_DOWN))
				rotate.y -= speed * fov;
		}

		if (g_GameMenuStatus == InGame)
		{
			if (mouseDelta[0] != 0.0f)
			{
				yaw += clamp(mouseDelta[0], -5.0f, 5.0f);

				rotate.x = pos.x + sin(yaw) * (speed * 20) * fov;
				rotate.z = pos.z + cos(yaw) * (speed * 20) * fov;
			}
		}
		else
		{
			if (IsKeyPressed(VK_LEFT))
			{
				yaw += speed;

				rotate.x = pos.x + sin(yaw) * (speed * 20) * fov;
				rotate.z = pos.z + cos(yaw) * (speed * 20) * fov;
			}
			else if (IsKeyPressed(VK_RIGHT))
			{
				yaw -= speed;

				rotate.x = pos.x + sin(yaw) * (speed * 20) * fov;
				rotate.z = pos.z + cos(yaw) * (speed * 20) * fov;
			}
		}

		if (IsKeyPressed(VK_ADD))
			fov = clamp(fov + 0.005f, 0.01f, 2.87079525f);
		else if (IsKeyPressed(VK_SUBTRACT))
			fov = clamp(fov - 0.005f, 0.01f, 2.87079525f);

		if (IsKeyPressed(VK_NUMPAD0))
			roll = clamp(roll - 0.005f, -0.8f, 0.8f);
		else if (IsKeyPressed(VK_DECIMAL))
			roll = clamp(roll + 0.005f, -0.8f, 0.8f);

		if (g_GameMenuStatus == InGame)
		{
			*(bool*)(base + 0x19C2D54) = true;
			stpFlags.STP_UI = true;
			if (!onceSleep)
			{
				Sleep(100u);
				onceSleep = true;
			}
		}
		else
			stpFlags.STP_UI = false;

		stpFlags.STP_OBJ = true;
		stpFlags.STP_ESP = true;
		stpFlags.STP_TRIGGER = true;

		once = false;

		if (IsKeyPressed(VK_OEM_5))
		{
			Pl0000* player = (Pl0000*)g_cGameUIManager.m_pPl;

			if (!player)
				return;

			player->m_vecOffset = pos;
			player->m_vecPos = pos;
		}
		
		CameraGame->field_4D0.x = 0.0f;
		CameraGame->field_4D0.y = 0.0f;
		CameraGame->field_4D0.z = 0.0f;
		CameraGame->field_4D0.w = 0.0f;
	}

	if (!active)
		Sleep(20u);
}

void FreeCamera::LoadConfig() noexcept
{
	CIniReader iniReader("FreeCamera.ini");

	key = iniReader.ReadInteger("Settings", "key", 192);
	controlSpeed = iniReader.ReadFloat("Settings", "speed", 0.03f);
	turboSpeed = iniReader.ReadFloat("Settings", "turboSpeed", 0.07f);
}

void FreeCamera::SaveConfig() noexcept
{
	CIniReader iniReader("FreeCamera.ini");

	iniReader.WriteInteger("Settings", "key", key);
	iniReader.WriteFloat("Settings", "speed", controlSpeed);
	iniReader.WriteFloat("Settings", "turboSpeed", turboSpeed);
}
