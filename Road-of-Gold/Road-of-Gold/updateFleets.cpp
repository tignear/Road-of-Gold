#include"Fleet.h"
#include"Planet.h"
#include"Route.h"
#include"Urban.h"

void	updateFleets()
{
	for (auto& f : fleets)
	{
		double actionTime = planet.timeSpeed;

		for (;;)
		{
			if (f.route != nullptr)	//���s���̃��[�g������Ȃ��
			{
				if (f.route->movingCost / f.data.speed <= f.routeProgress + actionTime)
				{
					actionTime -= f.route->movingCost / f.data.speed - f.routeProgress;
					f.timer += f.route->movingCost / f.data.speed - f.routeProgress;
					f.routeProgress = 0;
					f.nowUrban = f.route->toUrban;
					f.route->pullVehicle();
					f.route = nullptr;
					f.chain.readerPos++;
				}
				else
				{
					f.routeProgress += actionTime;
					f.timer += actionTime;
					actionTime = 0;
					break;
				}
			}
			else if (f.sleepTimer > 0)	//�x�e���Ȃ��
			{
				if (f.sleepTimer <= actionTime)
				{
					f.sleepTimer = 0.0;
					actionTime -= f.sleepTimer;
					f.timer += f.sleepTimer;
					f.chain.readerPos++;
				}
				else
				{
					f.sleepTimer -= actionTime;
					f.timer += actionTime;
					actionTime = 0;
					break;
				}
			}
			else if (f.chain.update(&f)) break;	//error�̏ꍇ�E�o
		}
	}
}