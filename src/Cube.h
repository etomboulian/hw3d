#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>

class Cube
{
public:
	template<typename V>
	static IndexedTriangleList<V> Make()
	{
		namespace dx = DirectX; // make an alias for the namespace DirectX
		constexpr float side = 1.0f / 2.0f;

		std::vector<dx::XMFLOAT3> verticies;
		verticies.emplace_back(-side, -side, -side);
		verticies.emplace_back(side, -side, -side);
		verticies.emplace_back(-side, side, -side);
		verticies.emplace_back(side, side, -side);
		verticies.emplace_back(-side, -side, side);
		verticies.emplace_back(side, -side, side);
		verticies.emplace_back(-side, side, side);
		verticies.emplace_back(side, side, side);


		std::vector<V> verts(verticies.size());
		for (size_t i = 0; i < verticies.size(); ++i)
		{
			verts[i].pos = verticies[i];
		}
		return {
			std::move(verts),
			{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4,
			}
		};
	}
};

