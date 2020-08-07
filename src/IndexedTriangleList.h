#pragma once
#include <vector>
#include <DirectXMath.h>

template<typename T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> verts_in, std::vector<unsigned short> indicies_in)
		: verticies(std::move(verts_in)), indicies(std::move(indicies_in))
	{
		assert(verticies.size() > 2);
		assert(indicies.size() % 3 == 0);
	}

	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& v : verticies)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(&v.pos, DirectX::XMVector3Transform(pos, matrix));
		}
	}

public:
	std::vector<T> verticies;
	std::vector<unsigned short> indicies;
};

