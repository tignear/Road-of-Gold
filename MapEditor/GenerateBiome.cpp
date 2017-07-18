#include"Planet.h"
#include"Node.h"

void	Planet::generateBiome()
{
	const int biomeType[6][6] = {
		{ 2, 6, 8, 3, 3, 3 },
		{ 2, 6, 8, 3, 3, 3 },
		{ 2, 6,11,11, 4, 4 },
		{ 2, 6,10,11, 4, 4 },
		{ 2, 6,10, 9, 5, 5 },
		{ 2, 6,10, 7, 5, 5 }
	};

	PerlinNoise	heightNoise(Random(UINT32_MAX - 1));
	PerlinNoise moistureNoise(Random(UINT32_MAX - 1));

	for (auto& n : nodes)
	{
		const double h = Min(Max((heightNoise.octaveNoise(n.pos.ePos, 8) + 1.0) / 2.0, 0.0), 1.0);
		if (h >= 0.60)
		{
			int moisture = Max(Min(int(moistureNoise.octaveNoise(n.pos.ePos*2.0, 5) * 3 + 3), 5), 0);
			int temperature = 5 - Max(Min(int(abs(n.pos.mPos.y) * 4 + 20 * pow(h - 0.6, 2)), 5), 0);
			n.biomeType = biomeType[moisture][temperature];
		}
		else
		{
			if (h < 0.30) n.biomeType = 1;
			else  n.biomeType = 0;
		}
	}

	//ボロノイ図の更新
	Array<Node*> list;
	for (auto& n : nodes) list.emplace_back(&n);
	updateImage(list);
}