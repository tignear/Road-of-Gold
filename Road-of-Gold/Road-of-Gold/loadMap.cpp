#include"Planet.h"
#include"Node.h"
#include"Urban.h"
#include"BiomeData.h"

#include"Scuttle.h"
#include"Group.h"
#include"Display.h"
#include"Route.h"
#include<thread>
#include"Data.h"
#include"River.h"
#include<lua.hpp>

bool	selectMap()
{
	std::thread thread(initNodes);
	loadData();

	auto items = FileSystem::DirectoryContents(L"assets/map/").filter([](const FilePath& _path) {
		return FileSystem::IsDirectory(_path) && FileSystem::Exists(_path + L"BiomeData.bin");
	});

	for (;;)
	{
		(*globalFonts[32])(L"�g�p����}�b�v��I�����Ă�������").draw();
		for (int i = 0; i < int(items.size()); i++)
		{
			Rect rect(0, 48 + i * 48, Window::Size().x, 48);
			rect.draw(rect.mouseOver() ? ColorF(Palette::White, 0.5) : Color(0, 0)).drawFrame(2, 0, Palette::White);
			(*globalFonts[32])(FileSystem::BaseName(items[i])).draw(0, 48 + i * 48);
			if (rect.leftClicked())
			{
				thread.join();
				loadMap(items[i]);
				return true;
			}
		}
		if (!System::Update()) break;
	}
	thread.join();
	return false;
}
void	loadMap(const FilePath& _path)
{
	//mapPath�̓o�^
	planet.mapPath = _path.removed(FileSystem::CurrentPath());

	//MapImage�̍쐬or�ǂݍ���
	auto mapImageFunc = [&_path]() {
		{
			if (FileSystem::Exists(_path + L"MapImage.png")) planet.mapTexture = Texture(_path + L"MapImage.png");
			else
			{
				//VoronoiMap�̓ǂݍ���
				Image image(L"assets/nodeMap/voronoiMap.png");
				auto size = image.size();

				for (auto p : step(size))
				{
					auto& n = nodes.at(image[p.y][p.x].r + (image[p.y][p.x].g << 8) + (image[p.y][p.x].b << 16));

					image[p.y][p.x] = n.color;
				}

				//mapTexture�ɓK�p
				planet.mapTexture = Texture(image);
			}
		}
	};
	std::thread thread(mapImageFunc);

	//Planet�f�[�^�̃��[�h
	if (FileSystem::Exists(_path + L"Planet.json"))
	{
		JSONReader reader(_path + L"Planet.json");
		auto j = reader[L"StartTime"];

		//���Ԑݒ�
		planet.sandglass.set(j[L"Year"].getOr<int>(0), j[L"Month"].getOr<int>(1), j[L"Day"].getOr<int>(1));
	}

	//�o�C�I�[���f�[�^�̃��[�h
	{
		BinaryReader reader(_path + L"BiomeData.bin");

		for (auto& n : nodes)
		{
			reader.read(n.biomeType);

			//Node�ɐF�̓K�p
			n.color = n.data().color.lerp(RandomColor(), 0.05);
		}
	}

	//Urbans�f�[�^�̃��[�h
	if (FileSystem::Exists(_path + L"Urbans.json"))
	{
		JSONReader reader(_path + L"Urbans.json");

		for (auto json : reader.arrayView()) urbans.emplace_back(json);
		for (auto& u : urbans)
		{
			for (auto& b : u.shelves) b.joinedUrban = &u;
		}
	}

	//Rivers�f�[�^�̃��[�h
	if (FileSystem::Exists(_path + L"Rivers.json"))
	{
		JSONReader reader(_path + L"Rivers.json");
		for (auto json : reader.arrayView()) rivers.emplace_back(json);
	}

	//Route����
	initRoutes();

	//Incidents�f�[�^�̃��[�h
	if (FileSystem::Exists(_path + L"Incidents.lua"))
	{
		auto* lua = planet.incidentsLua;
		const auto& filePath = String(_path + L"Incidents.lua").narrow().c_str();

		if (luaL_loadfile(lua, filePath) || lua_pcall(planet.incidentsLua, 0, 0, 0))
		{
			Output << L"Incidents.lua�̓ǂݍ��݂Ɏ��s";
			Output << CharacterSet::FromUTF8(lua_tostring(lua, -1));
		}
	}

	//Groups�f�[�^�̃��[�h
	if (FileSystem::Exists(_path + L"Groups.json"))
	{
		auto json = JSONReader(_path + L"Groups.json");

		for (auto j : json.arrayView()) groups.emplace_back(j);
	}

	thread.join();	//mapImage�p
}