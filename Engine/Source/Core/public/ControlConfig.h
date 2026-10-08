#pragma once

#include <Core/export.h>
#include <filesystem>
#include <string>

namespace GameEngine::Core
{
	struct CORE_API ControlConfig
	{
		int forward = 'W';
		int backward = 'S';
		int left = 'A';
		int right = 'D';
		int up = 'E';
		int down = 'Q';
		int boost = 0x10;
		float moveSpeed = 3.0f;
		float boostMultiplier = 3.0f;

		bool Load(const std::filesystem::path& path, std::string& error);
	};
}
