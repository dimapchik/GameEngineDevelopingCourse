#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <Keyboard.h>

namespace GameEngine
{
	Game::Game(
		std::function<bool()> PlatformLoopFunc,
		const Core::ControlConfig& controls
	) :
		PlatformLoop(PlatformLoopFunc),
		m_Controls(controls)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_renderThread = std::make_unique<Render::RenderThread>();

		// How many objects do we want to create
		for (int i = 0; i < 3; ++i)
		{
			m_Objects.push_back(new GameObject());
			Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
			m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
		}
	}

	void Game::Run()
	{
		assert(PlatformLoop != nullptr);

		m_GameTimer.Reset();

		while (PlatformLoop())
		{
			m_GameTimer.Tick();
			float dt = m_GameTimer.GetDeltaTime();

			Update(dt);
			
			m_renderThread->OnEndFrame();
		}
	}

	void Game::Update(float dt)
	{
		auto axis = [](int positive, int negative)
		{
			return static_cast<float>(Core::IsKeyDown(positive)) - static_cast<float>(Core::IsKeyDown(negative));
		};
		Math::Vector3f forward = Core::g_MainCamera->GetViewDir().Normalized();
		Math::Vector3f up(0.0f, 1.0f, 0.0f);
		Math::Vector3f right = up.CrossProduct(forward).Normalized();
		Math::Vector3f movement = forward * axis(m_Controls.forward, m_Controls.backward)
			+ right * axis(m_Controls.right, m_Controls.left)
			+ up * axis(m_Controls.up, m_Controls.down);
		if (movement.GetLength() > 0.0001f)
		{
			float speed = m_Controls.moveSpeed;
			if (Core::IsKeyDown(m_Controls.boost)) speed *= m_Controls.boostMultiplier;
			Core::g_MainCamera->SetPosition(Core::g_MainCamera->GetPosition() + movement.Normalized() * (speed * dt));
		}

		for (int i = 0; i < m_Objects.size(); ++i)
		{
			Math::Vector3f pos = m_Objects[i]->GetPosition();

			// Showcase
			if (i == 0)
			{
				pos.x += 0.5f * dt;
			}
			else if (i == 1)
			{
				pos.y -= 0.5f * dt;
			}
			else if (i == 2)
			{
				pos.x += 0.5f * dt;
				pos.y -= 0.5f * dt;
			}
			m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame());
		}
	}
}
