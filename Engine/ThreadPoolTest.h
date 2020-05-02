#pragma once
#include "ThreadPool.h"
#include "VitThreadPool.h"
#include <future>
#include <debugapi.h>

#include "ParallelPrefixSum.h"
#include "ClusterManager.h"
#include "Math.h"

class ThreadPoolTest
{

	/*
	// This is not how it works but I didn't update it yet! Incorrect use of function.
	static void multiTester(UINT totalSize)
	{
		// Obviously this will use a pool when testing is done, not spawn every frame!
		auto nThreads = std::thread::hardware_concurrency();

		UINT chunk = totalSize / nThreads;

		std::vector<std::thread> _threads;
		std::vector<std::promise<UINT>> _promises;
		std::vector<std::future<UINT>> _futures;

		_threads.resize(nThreads);
		_promises.resize(nThreads);
		_futures.reserve(nThreads);

		for (int i = 0; i < nThreads; i++)
		{
			UINT start = i * chunk;
			UINT end = start + chunk;	//min((i + 1u) * chunk, _gridSize - 1);

			_futures.push_back(_promises[i].get_future());

			// For member functions
			//_threads[i] = new std::thread(&PrefixSumParallel::prefixSumParallel, this, start, end, std::move(_promises[i]));

			// For free functions
			//_threads[i] = std::thread(&prefixSumParallel, start, end, std::move(_promises[i]));

			_threads.emplace_back(&prefixSumParallel, i * chunk, (i + 1u) * chunk, _promises[i]);

		}

		for (int i = 0; i < nThreads; i++)
		{
			_threads[i].join();
			UINT wat = _futures[i].get();

			char buffer[50];
			sprintf(buffer, "Result is: %d \n", wat);

			OutputDebugStringA(buffer);
		}
	}
	*/







	static void ree(
		const std::vector<PLight>& pLights, std::vector<LightBounds>& lightBounds, std::vector<OffsetListItem>& _offsetGrid, 
		float zn, float zf, float szdlfdn, float ln,
		SMatrix v, SMatrix p,
		UINT plFirst, UINT plLast)
	{
		UINT zOffset;
		UINT yOffset;

		std::array<UINT, 3> gDims = { 30, 17, 16 };

		UINT sliceSize = gDims[0] * gDims[1];
		UINT rowSize = gDims[0];

		for (int i = plFirst; i < plLast; ++i)
		{
			const PLight& pl = pLights[i];

			SVec3 ws_lightPos(pLights[i]._posRange);
			float lightRadius = pl._posRange.w;

			// Get light bounds in view space, radius stays the same
			SVec4 vs_lightPosRange = Math::fromVec3(SVec3::TransformNormal(ws_lightPos, v), lightRadius);
			SVec2 viewZMinMax = SVec2(vs_lightPosRange.z) + SVec2(-lightRadius, lightRadius);

			// Convert XY light bounds to clip space, clamps to [-1, 1] Z is calculated from view
			SVec4 rect = ClusterManager::getProjectedRectangle(vs_lightPosRange, zn, zf, p);

			LightBounds indexSpan = ClusterManager::getLightMinMaxIndices(rect, viewZMinMax, zn, zf, gDims, szdlfdn, ln);

			lightBounds[i] = indexSpan;

			// First step of binning, increase counts per cluster
			for (int z = indexSpan[4]; z < indexSpan[5]; ++z)
			{
				zOffset = z * sliceSize;

				for (uint8_t y = indexSpan[1]; y < indexSpan[3]; ++y)
				{
					yOffset = y * rowSize;

					for (uint8_t x = indexSpan[0]; x < indexSpan[2]; ++x)
					{
						_offsetGrid[zOffset + yOffset + x]._count.fetch_add(1, std::memory_order_relaxed);
					}
				}
			}
		}
	}


public:

	ctpl::thread_pool _tp;
	static const UINT _nThreads = 4u;
	std::vector<std::future<void>> _results;

	ThreadPoolTest() : _tp(_nThreads)
	{
		_results.resize(_nThreads);
	}



	void assignLightsMT(ClusterManager& cm, const std::vector<PLight>& pLights, Camera& cam)
	{
		// Reset data from previous frame. Consider assigning 0's instead but it's probably negiligible difference
		cm._lightIndexList.clear();

		SMatrix v = cam.GetViewMatrix();
		SMatrix p = cam.GetProjectionMatrix();

		float p33 = p._33;
		float p43 = p._43;

		float zn = cam._frustum._zn;
		float zf = cam._frustum._zf;

		// Not in the constructor because I don't want it to require the camera, still calculated 1/frame instead of 1/light
		float _sz_div_log_fdn = static_cast<float>(cm._gridDims[2]) / log(zf / zn);
		float _log_n = log(zn);

		UINT chunkSize = pLights.size() / _nThreads;

		cm._lightBounds.resize(pLights.size());

		for (int i = 0; i < _nThreads; i++)
		{
			UINT minOff = i * chunkSize;
			UINT maxOff = (i + 1) * chunkSize;

			if (i == _nThreads - 1)
				maxOff = pLights.size();

			// Lockless, uses atomic to count and threads do not touch same _lightBounds indices...
			_results[i] = 
			_tp.push(
				std::bind(	ree, std::ref(pLights), std::ref(cm._lightBounds), std::ref(cm._offsetGrid),
							zn, zf, _sz_div_log_fdn, _log_n,
							v, p,
							minOff, maxOff
				)
			);
		}
		
		// Use the main thread for the same thing, just fiddle with load distribution a bit (min/max offsets etc)

		/*
		int cellListStart = cm._offsetGrid[cellIndex]._index;
		int listOffset = cm._offsetGrid[cellIndex]._count.fetch_sub(1, std::memory_order_relaxed); // use atomic on GPU
		cm._lightIndexList[cellListStart + listOffset - 1] = i;
		*/
	}

};