#include"Node.h"
#include"Planet.h"
#include"BiomeData.h"
#include"EnergyData.h"
#include"Urban.h"
#include"GlobalVariables.h"
#include"River.h"


bool loadMapData(const FilePath& _path)
{
	if (!FileSystem::IsDirectory(_path)) return false;

	urbans.clear();
	rivers.clear();

	//�o�C�I�[���f�[�^�̃��[�h
	if (FileSystem::Exists(_path + L"BiomeData.bin"))
	{
		Array<Node*> list;
		BinaryReader reader(_path + L"BiomeData.bin");
		for (auto& n : nodes)
		{
			reader.read(n.biomeType);
			if (n.biomeType >= int(biomeData.size())) return false;
			list.emplace_back(&n);
		}
		planet.updateImage(list);
	}
	else return false;

	//Urbans�f�[�^�̃��[�h
	if (FileSystem::Exists(_path + L"Urbans.json"))
	{
		JSONReader reader(_path + L"Urbans.json");
		for (auto json : reader.arrayView())
			urbans.emplace_back(json);
	}

	//Rivers�f�[�^�̃��[�h
	if (FileSystem::Exists(_path + L"Rivers.json"))
	{
		JSONReader reader(_path + L"Rivers.json");
		for (auto json : reader.arrayView())
			rivers.emplace_back(json);
	}
	

	textBox.setText(FileSystem::BaseName(_path));

	return true;
}
bool saveMapData(const FilePath& _path)
{
	//�o�C�I�[���f�[�^�̃Z�[�u
	{
		BinaryWriter writer(_path + L"BiomeData.bin");
		for (auto& n : nodes) writer.write(n.biomeType);
	}

	//�}�b�v��png�ɂ��ĕۑ�
	{
		planet.image.save(_path + L"MapImage.png");
	}

	//Urbans�f�[�^�̃Z�[�u
	if (!urbans.isEmpty())
	{
		String text(L"[");

		for (auto i : step(int(urbans.size())))
		{
			auto& u = urbans[i];

			if (i == 0) text += L"\r\t{";
			else text += L",\r\t{";

			//�s�s���W�̕ۑ�
			text += Format(L"\r\t\t\"JoinedNodeID\": ", u.joinedNodeID);

			//�s�s���̕ۑ�
			text += Format(L",\r\t\t\"Name\": \"", u.name, L"\"");

			//�l���̕ۑ�
			text += Format(L",\r\t\t\"NumCitizens\": ", u.numCitizens);

			//���Y���̕ۑ�
			//text += Format(L",\r\t\t\t\"Productivity\": ", u.productivity);

			//Resources�f�[�^�̕ۑ�
			text += L",\r\t\t\"Energies\": {";
			bool isFirst = true;
			for (auto j : step(energyData.size()))
			{
				if (u.energies[j] > 0)
				{
					if (isFirst) { text += L"\r\t\t\t\""; isFirst = false; }
					else text += L",\r\t\t\t\"";
					text += Format(energyData[j].name, L"\": ", u.energies[j]);
				}
			}
			text += L"\r\t\t}";

			text += L"\r\t}";
		}
		text += L"\r]";
		TextWriter writer(_path + L"Urbans.json");
		writer.write(text);
		writer.close();
	}

	//Rivers�f�[�^�̃Z�[�u
	{
		String text(L"[");
		for (auto i : step(int(rivers.size())))
		{
			auto& r = rivers[i];
			if (i == 0) text += L"\r\t{";
			else text += L",\r\t{";

			text += Format(L"\r\t\t\"Width\": ", r.width);

			text += L",\r\t\t\"Paths\": [";
			text += Format(r.riverPaths.front()->parentNodeID);
			for (auto* p : r.riverPaths)
			{
				text += L",";
				text += Format(p->childNodeID);
			}

			text += L"]";

			text += L"\r\t}";
		}
		text += L"\r]";
		TextWriter writer(_path + L"Rivers.json");
		writer.write(text);
		writer.close();
	}
	return true;
}