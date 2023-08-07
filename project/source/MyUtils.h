#pragma once

#include <engine/Engine.h>
#include <engine/graphics/ShadingGroup.h>

#include <chrono>

class MyUtils
{
public:
	static MyUtils& getInstance();
public:
	float64 secondsSinceStart();
	static bool getClientUV(HWND hwnd, float32 x, float32 y, float32& u, float32& v);
	static bool getClientSize(HWND hwnd, int32& width, int32& height);
	template<class T_Instance>
	static void insertInstanceIDInShadingGroup(
		graphics::ShadingGroup<T_Instance>& shadingGroup,
		const uint32 instanceID,
		const std::shared_ptr<graphics::Model>& model,
		const graphics::Material& material);
	template<class T_Instance>
	static void insertModelInstanceIDInShadingGroup(
		graphics::ShadingGroup<T_Instance>& shadingGroup,
		const uint32 instanceID,
		const std::shared_ptr<graphics::Model>& model,
		const std::vector<graphics::Material>& perMeshMaterials);
private:
	MyUtils() = default;
private:
	std::chrono::steady_clock::time_point programStartTime = std::chrono::high_resolution_clock::now();
};

#include "MyUtils.inl"