#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"

class Cone
{
public:
	template<typename V>
	static IndexedTriangleList<V> MakeTesselated(int longDiv)
	{
		namespace dx = DirectX;
		assert(longDiv >= 3);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float logitudeAngle = 2.0f * PI / longDiv;

		// base verticies
		std::vector<V> verticies;
		for (int iLong = 0; iLong < longDiv; ++iLong)
		{
			verticies.emplace_back();
			auto v = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			dx::XMStoreFloat3(&verticies.back().pos, v);
		}

		// the center
		verticies.emplace_back();
		verticies.back().pos = { 0.0f, 0.0f, -1.0f };
		const auto iCenter = (unsigned short)(verticies.size() - 1);
		// the tip
		verticies.emplace_back();
		verticies.back().pos = { 0.0f, 0.0f, 1.0f };
		const auto iTip = (unsigned short)(verticies.size() - 1); 

		// base indicies
		std::vector<unsigned shrot> indicies;
		for (unsigned short iLong = 0; iLong < longDiv; ++iLong)
		{
			indicies.push_back(iCenter);
			indicies.push_back((iLong + 1) % longDiv);
			indicies.push_back(iLong);
		}

		//cone indicies
		for (unsigned short iLong = 0; iLong < longDiv; ++iLong)
		{
			indicies.push_back(iLong);
			indicies.push_back((iLong + 1) % longDiv);
			indicies.push_back(iTip);
		}
		
		return{ std::move(verticies), std::move(indicies) };
	}

	template<typename V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(24);
	}
};
