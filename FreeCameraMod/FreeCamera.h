#pragma once

namespace FreeCamera
{
	// CONFIG SYSTEM
	inline int key = 192; // ~ or `
	inline float controlSpeed = 0.03f;
	inline float turboSpeed = 0.07f;
	// CONFIG SYSTEM END
	
	inline bool active = false;

	bool CanRun() noexcept;
	void Run() noexcept;
	void LoadConfig() noexcept;
	void SaveConfig() noexcept;
}