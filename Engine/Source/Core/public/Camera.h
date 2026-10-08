#pragma once

#include <Core/export.h>
#include <Matrix.h>
#include <Vector.h>
#include <mutex>

namespace GameEngine
{
	namespace Core
	{
		class CORE_API Camera final
		{
		public:
			Camera() = default;

		public:
			Math::Matrix4x4f GetViewMatrix();
			Math::Vector3f GetPosition() const { std::lock_guard lock(m_Mutex); return m_Position; }
			void SetPosition(Math::Vector3f position) { std::lock_guard lock(m_Mutex); m_Position = position; }
			Math::Vector3f GetViewDir() const { std::lock_guard lock(m_Mutex); return m_ViewDir; }
			void SetViewDir(Math::Vector3f viewDir) { std::lock_guard lock(m_Mutex); m_ViewDir = viewDir; }
			void Rotate(float yaw, float pitch);

		private:
			mutable std::mutex m_Mutex;
			Math::Vector3f m_Position;
			Math::Vector3f m_ViewDir;
		};

		extern CORE_API Camera* g_MainCamera;
	}
}
