#pragma once

#include <ECS/ecsSystems.h>
#include <flecs.h>
#include <Geometry.h>
#include <RenderThread.h>
#include <RenderObject.h>

namespace GameEngine::EntitySystem::ECS
{
	void RegisterEcsCoreSystems(flecs::world& world)
	{
		auto* renderThread = world.get<RenderThreadPtr>()->ptr;

		world.observer<RenderObjectPtr>()
			.event(flecs::OnRemove)
			.each([renderThread](RenderObjectPtr& object)
		{
			renderThread->DestroyRenderObject(object.ptr);
		});

		world.system<const GeometryPtr, RenderObjectPtr>()
			.each([renderThread](flecs::entity e, const GeometryPtr& geometry, RenderObjectPtr& renderObject)
		{
			renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, geometry.ptr, renderObject.ptr);
			e.remove<GeometryPtr>();
		});
	}
}
