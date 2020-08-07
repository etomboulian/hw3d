#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"

class Prism
{
public:
	template<typename V>
	static IndexedTriangleList<V> MakeTesselated(int longDiv)
	{
		namespace dx = DirectX;
		assert(longDiv >= 3);
		
		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const auto offset = dx::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float logitudeAngle = 2.0f * PI / longDiv;

		// near center
		std::vector<V> verticies;
		verticies.emplace_back();
		verticies.back().pos = { 0.0f, 0.0f, -1.0f };
		const auto iCenterNear = (unsigned short)(verticies.size() - 1);
		// far center
		verticies.emplace_back();
		verticies.back().pos = { 0.0f, 0.0f, 1.0f };
		const auto iCenterFar = (unsigned short)(verticies.size() - 1);

		// base verticies
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			// near base
			{
				verticies.emplace_back();
				auto v = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitudeAngle * iLong));
				dx::XMStoreFloat3(&verticies.back().pos, v);
			}
			// far base
			{
				verticies.emplace_back();
				auto v = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitudeAngle * iLong));
				v = dx::XMVectorAdd(v, offset);
				dx::XMStoreFloat3(&verticies.back().pos.v)
			}
		}

		// side indicies
		std::vector<unsigned short> indicies;
		for (unsigned short iLong = 0; iLong < longDiv; ++iLong)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			indicies.push_back(	i + 2);
			indicies.push_back((i + 2) % mod + 2);
			indicies.push_back(	i + 1 + 2);
			indicies.push_back((i + 2) % mod + 2);
			indicies.push_back((i + 3) % mod + 2);
			indicies.push_back( i + 1 + 2);
		}

		// base indicies
		for (unsigned short iLong = 0; iLong < longDiv; ++iLong)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			indicies.push_back(i + 2);
			indicies.push_back(iCenterNear);
			indicies.push_back((i + 2) % mod + 2);
			indicies.push_back(iCenterFar);
			indicies.push_back(i + 1 + 2);
			indicies.push_back((i + 3) % mod + 2);
		}

		return { std::move(verticies), std::move(indicies) };
	}

	template<typename V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(24);
	}
};
