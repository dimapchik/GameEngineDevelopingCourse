local ecs = require "ecs"

local colliders = ecs.query("[in] Position, [in] BoxCollider")
local lifetimes = ecs.query("Lifetime")

local function LifetimeSystem(it)
    local timers = ecs.query_iter(lifetimes)
    while ecs.query_next(timers) do
        for lifetime, ent in ecs.each(timers) do
            lifetime.remaining = lifetime.remaining - it.delta_time
            if lifetime.remaining <= 0 then
                ecs.delete(ent)
            end
        end
    end
end

local function CollisionSystem(it)
    for pos, box, state, ent in ecs.each(it) do
        state.count = 0
        local others = ecs.query_iter(colliders)
        -- each(query) skips rows in this version of flecs-lua.
        while ecs.query_next(others) do
            for otherPos, otherBox, other in ecs.each(others) do
                if ent ~= other
                    and math.abs(pos.x - otherPos.x) <= box.x + otherBox.x
                    and math.abs(pos.y - otherPos.y) <= box.y + otherBox.y
                    and math.abs(pos.z - otherPos.z) <= box.z + otherBox.z then
                    state.count = state.count + 1
                end
            end
        end
    end
end

local function rand_flt(from, to)
	local maxNumber = 32767
    return from + (math.random(maxNumber) / maxNumber) * (to - from)
end

local function move(it)
    for pos, vel, ent in ecs.each(it) do
        pos.x = pos.x + vel.x * it.delta_time
        pos.y = pos.y + vel.y * it.delta_time
		pos.z = pos.z + vel.z * it.delta_time
    end
end

local function gravity(it)
    for pos, vel, grav, plane, ent in ecs.each(it) do
        local planeEpsilon = 0.1
		
		if plane.x * pos.x + plane.y * pos.y + plane.z * pos.z < plane.w + planeEpsilon then
			do return end
		end
		
		vel.x = vel.x + grav.x * it.delta_time
        vel.y = vel.y + grav.y * it.delta_time
		vel.z = vel.z + grav.z * it.delta_time
    end
end

local function FrictionSystem(it)
    for vel, friction, ent in ecs.each(it) do
        vel.x = vel.x - vel.x * friction.value * it.delta_time
		vel.y = vel.y - vel.y * friction.value * it.delta_time
		vel.z = vel.z - vel.z * friction.value * it.delta_time
    end
end

local function ShiverSystem(it)
    for pos, shiver, ent in ecs.each(it) do
        pos.x = pos.x + rand_flt(-shiver.value, shiver.value)
		pos.y = pos.y + rand_flt(-shiver.value, shiver.value)
		pos.z = pos.z + rand_flt(-shiver.value, shiver.value)
    end
end

local function BounceSystem(it)
    for pos, vel, plane, bounciness, ent in ecs.each(it) do
        local dotPos = plane.x * pos.x + plane.y * pos.y + plane.z * pos.z
		local dotVel = plane.x * vel.x + plane.y * vel.y + plane.z * vel.z
		
		if dotPos < plane.w then
			pos.x = pos.x - (dotPos - plane.w) * plane.x
			pos.y = pos.y - (dotPos - plane.w) * plane.y
			pos.z = pos.z - (dotPos - plane.w) * plane.z

			vel.x = vel.x - (1.0 + bounciness.value) * plane.x * dotVel
			vel.y = vel.y - (1.0 + bounciness.value) * plane.y * dotVel
			vel.z = vel.z - (1.0 + bounciness.value) * plane.z * dotVel
		end
    end
end

-- Flush deletions before the other systems run.
ecs.system(LifetimeSystem, "LifetimeSystem", ecs.PreUpdate, "[out] *()")
ecs.system(move, "Move", ecs.OnUpdate, "Position, Velocity")
ecs.system(gravity, "grav", ecs.OnUpdate, "Position, Velocity, Gravity, BouncePlane")
ecs.system(FrictionSystem, "FrictionSystem", ecs.OnUpdate, "Velocity, FrictionAmount")
ecs.system(ShiverSystem, "ShiverSystem", ecs.OnUpdate, "Position, ShiverAmount")
ecs.system(BounceSystem, "BounceSystem", ecs.OnUpdate, "Position, Velocity, BouncePlane, Bounciness")
ecs.system(CollisionSystem, "CollisionSystem", ecs.PostUpdate,
    "[in] Position, [in] BoxCollider, [out] CollisionState")

