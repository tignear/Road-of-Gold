#include"Group.h"
#include"Vehicle.h"
#include"Urban.h"
#include"VehicleData.h"
#include"Fleet.h"
#include"Route.h"

void	initGroups()
{
	if (urbans.isEmpty())
	{
		Output << L"�s�s�����݂��Ȃ����߂Ɍ��Ճ��j�b�g�W�J�Ɏ��s���܂���";
		return;
	}

	vehicles.reserve(65536);
	fleets.reserve(65536);
	/*
	INIReader iniReader(L"assets/config.ini");
	int numVehicles = iniReader.getOr<int>(L"Groups", L"NumVehicles", 0);

	for (int j = 0; j < numVehicles; j++)
	{
		for (;;)
		{
			auto* u = &urbans.choice();
			int i = vehicleData.choice().id();
			bool flag = false;
			for (auto* r : u->ownRoutes)
			{
				if (r->movingCost < vehicleData[i].range || r->isSeaRoute == vehicleData[i].isShip)
				{
					flag = true;
					fleets.emplace_back(i, u);
					fleets.back().name = Format(L"��", fleets.size(), L"�D�c");
					break;
				}
			}
			if (flag) break;
		}
	}
	*/
	for (int i = 0; i < 300; i++)
	{
		urbans.choice().tickets.emplace_back(&groups.front(), vehicleData.choice().id());
	}
}