#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"

class Sphere
{
public:
	template<typename V>
	static IndexedTriangleList<V> MakeTesselated(int latDiv, int longDiv)
	{
		namespace dx = DirectX;
		assert(latDiv >= 3);
		assert(longDiv >= 3);

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float lattitudeAngle = PI / latDiv;
		const float longitudeAngle = 2.0f * PI / longDiv;
	
		std::vector<V> verticies;
		for (int iLat = 1; iLat < latDiv; ++iLat)
		{
			const auto latBase = dx::XMVector3Transform(base, dx::XMMatrixRotationX(lattitudeAngle * iLat));

			for (int iLong = 0; iLong < longDiv; ++iLong)
			{
				verticies.emplace_back();
				auto v = dx::XMVector3Transform(latBase, dx::XMMatrixRotationZ(longitudeAngle * iLong));
				dx::XMStoreFloat3(&verticies.back().pos, v);
			}
		}

		// add the cap verticies
		const auto iNorthPole = (unsigned short)verticies.size();
		verticies.emplace_back();
		dx::XMStoreFloat3(&verticies.back().pos, base);

		const auto iSouthPole = (unsigned short)verticies.size();
		verticies.emplace_back();
		dx::XMStoreFloat3(&verticies.back().pos, dx::XMVectorNegate(base));

		const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong)
		{
			return iLat * longDiv + iLong;
		};

		std::vector<unsigned short> indicies;
		for (unsigned short iLat = 0; iLat < latDiv - 2; ++iLat)
		{
			for (unsigned short iLong = 0; iLong < longDiv; ++iLong)
			{
				indicies.push_back(calcIdx(iLat, iLong));
				indicies.push_back(calcIdx(iLat + 1, iLong));
				indicies.push_back(calcIdx(iLat, iLong + 1));
				indicies.push_back(calcIdx(iLat, iLong + 1));
				indicies.push_back(calcIdx(iLat + 1, iLong));
				indicies.push_back(calcIdx(iLat + 1, iLong + 1));
			}
			//wrap band
			indicies.push_back(calcIdx(iLat, longDiv - 1));
			indicies.push_back(calcIdx(iLat + 1, longDiv - 1));
			indicies.push_back(calcIdx(iLat, 0));
			indicies.push_back(calcIdx(iLat, 0));
			indicies.push_back(calcIdx(iLat + 1, longDiv - 1));
			indicies.push_back(calcIdx(iLat + 1, 0));
		}

		//cap fans
		for (unsigned short iLong = 0; iLong < longDiv - 1; ++iLong)
		{
			//north
			indicies.push_back(iNorthPole);
			indicies.push_back(calcIdx(0, iLong));
			indicies.push_back(calcIdx(0, iLong + 1));
			//south
			indicies.push_back(calcIdx(latDiv - 2, iLong + 1));
			indicies.push_back(calcIdx(latDiv - 2, iLong));
			indicies.push_back(iSouthPole);
		}

		//wrap triangles
		// north
		indicies.push_back(iNorthPole);
		indicies.push_back(calcIdx(0, longDiv - 1));
		indicies.push_back(calcIdx(0, 0));
		//south
		indicies.push_back(calcIdx(latDiv - 2, 0));
		indicies.push_back(calcIdx(latDiv - 2, longDiv - 1));
		indicies.push_back(iSouthPole);

		return { std::move(verticies), std::move(indicies) };
	}

	template<typename V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(12, 24);
	}

};
