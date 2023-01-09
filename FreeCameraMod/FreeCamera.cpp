#define PI 3.14159265359f

#include "FreeCamera.h"
#include <cCameraGame.h>
#include <cCameraGame.cpp>
#include <shared.h>
#include <cmath>
#include <GameMenuStatus.h>
#include "IniReader.h"

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

	unsigned int& stpFlags = *(unsigned int*)(shared::base + 0x17EA070);
	static bool once = false;

	if (shared::IsKeyPressed(key, false))
		active = !active;

	if (!active && !once)
	{
		stpFlags &= ~(0x80000000 | 0x00010000);

		if (g_GameMenuStatus == InGame)
			*(bool*)(shared::base + 0x19C2D54) = false;

		once = true;
	}

	if (active)
	{
		cCameraGame* CameraGame = &g_cCameraGame;

		float& yaw = CameraGame->field_364;
		float& pitch = CameraGame->field_218;
		float& fov = CameraGame->m_vecViewAngle.z;

		cVec4& rotate = CameraGame->m_vecRotationAt;
		cVec4& pos = CameraGame->m_vecPosition;

		float speed = controlSpeed;

		if (shared::IsKeyPressed(VK_SHIFT))
			speed = turboSpeed;

		if (shared::IsKeyPressed(87))
		{
			pos.x += speed * sin(yaw);
			pos.y += speed * sin(pitch);
			pos.z += speed * cos(yaw);

			rotate.x += speed * sin(yaw);
			rotate.y += speed * sin(pitch);
			rotate.z += speed * cos(yaw);
		}
		else if (shared::IsKeyPressed(83))
		{
			pos.x -= speed * sin(yaw);
			pos.y -= speed * sin(pitch);
			pos.z -= speed * cos(yaw);

			rotate.x -= speed * sin(yaw);
			rotate.y -= speed * sin(pitch);
			rotate.z -= speed * cos(yaw);
		}

		if (shared::IsKeyPressed(65))
		{
			pos.x -= speed * sin(yaw - PI / 2);
			pos.z -= speed * cos(yaw - PI / 2);

			rotate.x -= speed * sin(yaw - PI / 2);
			rotate.z -= speed * cos(yaw - PI / 2);
		}
		else if (shared::IsKeyPressed(68))
		{
			pos.x += speed * sin(yaw - PI / 2);
			pos.z += speed * cos(yaw - PI / 2);

			rotate.x += speed * sin(yaw - PI / 2);
			rotate.z += speed * cos(yaw - PI / 2);
		}

		if (shared::IsKeyPressed(VK_SPACE))
		{
			pos.y += speed;
			rotate.y += speed;
		}
		else if (shared::IsKeyPressed(VK_CONTROL))
		{
			pos.y -= speed;
			rotate.y -= speed;
		}

		if (shared::IsKeyPressed(VK_UP))
			rotate.y += speed * fov;
		else if (shared::IsKeyPressed(VK_DOWN))
			rotate.y -= speed * fov;

		if (shared::IsKeyPressed(VK_LEFT))
		{
			yaw += speed;

			rotate.x = pos.x + sin(yaw) * (speed * 20);
			rotate.z = pos.z + cos(yaw) * (speed * 20);
		}
		if (shared::IsKeyPressed(VK_RIGHT))
		{
			yaw -= speed;

			rotate.x = pos.x + sin(yaw) * (speed * 20);
			rotate.z = pos.z + cos(yaw) * (speed * 20);
		}

		if (shared::IsKeyPressed(VK_ADD))
			fov = shared::clamp(fov + 0.005f, 0.01f, 2.87079525f);
		else if (shared::IsKeyPressed(VK_SUBTRACT))
			fov = shared::clamp(fov - 0.005f, 0.01f, 2.87079525f);

		if (g_GameMenuStatus == InGame)
			*(bool*)(shared::base + 0x19C2D54) = true;

		stpFlags |= (0x80000000 | 0x00010000);

		once = false;
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
