#include"Planet.h"
#include"Node.h"
#include"BiomeData.h"

void	initPlanet()
{
	//VoronoiMapの読み込み
	Image image(L"assets/nodemap/voronoiMap.png");

	for (auto p : step(image.size()))
	{
		auto& n = nodes.at(image[p.y][p.x].r + (image[p.y][p.x].g << 8) + (image[p.y][p.x].b << 16));

		image[p.y][p.x] = n.color;
	}

	//mapTextureに適用
	planet.mapTexture = Texture(image);
	Log(L"MapTextrueの作成が完了");
}