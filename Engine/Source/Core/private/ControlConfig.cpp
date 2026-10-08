#include <ControlConfig.h>
#include <ini.h>

#include <charconv>
#include <cmath>
#include <fstream>
#include <iterator>
#include <string_view>

namespace
{
	int ParseKey(std::string value)
	{
		for (char& c : value)
			if (c >= 'a' && c <= 'z') c -= 'a' - 'A';

		if (value.size() == 1 && ((value[0] >= 'A' && value[0] <= 'Z') ||
			(value[0] >= '0' && value[0] <= '9')))
			return value[0];

		if (value.starts_with("VK_")) value.erase(0, 3);
		struct NamedKey { const char* name; int code; };
		static constexpr NamedKey names[] = {
			{"SPACE", 0x20}, {"SHIFT", 0x10}, {"CONTROL", 0x11}, {"MENU", 0x12},
			{"LSHIFT", 0xA0}, {"RSHIFT", 0xA1}, {"LCONTROL", 0xA2}, {"RCONTROL", 0xA3},
			{"LEFT", 0x25}, {"UP", 0x26}, {"RIGHT", 0x27}, {"DOWN", 0x28},
			{"ESCAPE", 0x1B}, {"RETURN", 0x0D}, {"TAB", 0x09}, {"BACK", 0x08},
			{"HOME", 0x24}, {"END", 0x23}, {"PRIOR", 0x21}, {"NEXT", 0x22},
			{"INSERT", 0x2D}, {"DELETE", 0x2E}
		};
		for (const auto& key : names)
			if (value == key.name) return key.code;

		std::string_view digits(value);
		int base = 10;
		bool functionKey = false;
		if (digits.starts_with("0X")) { base = 16; digits.remove_prefix(2); }
		else if (digits.starts_with("F")) { functionKey = true; digits.remove_prefix(1); }
		int code = 0;
		auto result = std::from_chars(digits.data(), digits.data() + digits.size(), code, base);
		if (result.ec != std::errc() || result.ptr != digits.data() + digits.size()) return 0;
		if (functionKey) return code >= 1 && code <= 24 ? 0x70 + code - 1 : 0;
		return code >= 0x08 && code <= 0xFE ? code : 0;
	}

	int ReadEntry(void* user, const char* section, const char* name, const char* value)
	{
		auto& config = *static_cast<GameEngine::Core::ControlConfig*>(user);
		const std::string_view group(section), entry(name);
		if (group == "Keyboard")
		{
			struct Binding { const char* name; int GameEngine::Core::ControlConfig::* member; };
			static constexpr Binding bindings[] = {
				{"Forward", &GameEngine::Core::ControlConfig::forward},
				{"Backward", &GameEngine::Core::ControlConfig::backward},
				{"Left", &GameEngine::Core::ControlConfig::left},
				{"Right", &GameEngine::Core::ControlConfig::right},
				{"Up", &GameEngine::Core::ControlConfig::up},
				{"Down", &GameEngine::Core::ControlConfig::down},
				{"Boost", &GameEngine::Core::ControlConfig::boost}
			};
			for (const auto& binding : bindings)
			{
				if (entry != binding.name) continue;
				const int key = ParseKey(value);
				if (!key) return 0;
				config.*(binding.member) = key;
				return 1;
			}
		}
		else if (group == "Camera")
		{
			float* setting = nullptr;
			float maximum = 0.0f;
			if (entry == "MoveSpeed") { setting = &config.moveSpeed; maximum = 1000.0f; }
			else if (entry == "BoostMultiplier") { setting = &config.boostMultiplier; maximum = 100.0f; }
			if (!setting) return 0;
			std::string_view text(value);
			float number = 0.0f;
			auto result = std::from_chars(text.data(), text.data() + text.size(), number);
			if (result.ec != std::errc() || result.ptr != text.data() + text.size() ||
				!std::isfinite(number) || number <= 0.0f || number > maximum) return 0;
			*setting = number;
			return 1;
		}
		return 0;
	}
}

namespace GameEngine::Core
{
	bool ControlConfig::Load(const std::filesystem::path& path, std::string& error)
	{
		error.clear();
		std::ifstream file(path, std::ios::binary);
		if (!file) { error = "Cannot open controls.ini; keeping current controls."; return false; }
		const std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if (file.bad()) { error = "Cannot read controls.ini; keeping current controls."; return false; }
		ControlConfig candidate;
		const int result = ini_parse_string(contents.c_str(), ReadEntry, &candidate);
		if (result != 0)
		{
			error = "Invalid controls.ini at line " + std::to_string(result) + "; keeping current controls.";
			return false;
		}
		*this = candidate;
		return true;
	}
}
