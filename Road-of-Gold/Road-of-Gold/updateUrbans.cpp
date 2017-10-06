#include"Planet.h"
#include"Urban.h"
#include"Display.h"
#include"CitizenData.h"
#include"ItemData.h"
#include"Sound.h"
#include"VehicleData.h"
#include"Fleet.h"
#include<thread>
#include<boost\range\numeric.hpp>

void	updateUrban(Urban& u)
{

	if (u.sandglass.update())
	{
		//�s�����O�̍X�V
		for (auto& c : u.citizens)
		{
			c.incomeLog.push_front(c.wallet().income);
			c.incomeLog.pop_back();
			c.averageIncome = int(boost::accumulate(c.incomeLog, 0, [](int sum, int i) { return sum + i; }) / c.incomeLog.size());
			c.wallet().income = 0;	//���������Z�b�g
		}
		u.averageIncome = int(boost::accumulate(u.citizens, 0, [](int sum, const Citizen& c) { return sum + c.averageIncome; }) / u.citizens.size());

		//�s���̓]�E
		for (auto& c : u.citizens)
		{
			//�ڕW���v��J���҂̎��v�����ɐݒ�
			c.targetRevenue = int(Random(0.8, 1.0)*u.averageIncome);

			if (u.jobEfficiency[c.citizenType] == 0 || RandomBool(0.003))	//�]�E
			{
				int sum = boost::accumulate(u.citizens, 0, [](int sum, const Citizen& c) {return sum + c.averageIncome; });
				//for (auto& t : u.citizens) sum += t.averageIncome;

				sum = Random(sum);
				for (auto& t : u.citizens)
				{
					sum -= t.averageIncome;
					if (sum < 0)
					{
						if (c.averageIncome < t.averageIncome)
						{
							c.incomeLog.fill(0);
							c.citizenType = t.citizenType;
							//�̔����i�̃R�s�[
							c.wallet().price = t.wallet().price;
						}
						break;
					}
				}
				for (int i = 0; i < int(citizenData.size()); i++)
				{
					if (u.jobEfficiency[i] != 0 && !u.citizens.any([&i](const Citizen& t) { return t.citizenType == i; }))
					{
						c.citizenType = i;
					}
				}
			}
		}
		//�����̍X�V
		for (auto i : step(int(citizenData.size())))
		{
			auto& je = u.jobEfficiency[i];
			auto& data = citizenData[i];
			auto numCitizen = int(u.citizens.count_if([&i](const Citizen& c) { return c.citizenType == i; }));

			if (data.needEnergyType == -1) je = 1;
			else if (!u.energies.any([&data](const Energy& e) { return e.energyType == data.needEnergyType; })) je = 0;
			else
			{
				for (auto j : step(int(u.energies.size())))
				{
					auto& e = u.energies[j];
					if (e.energyType == data.needEnergyType)
					{
						if (e.numEnergy == 0) je = 0;
						else if (numCitizen == 0) je = 1.0;
						else je = Min(1.0, sqrt((double)e.numEnergy / (double)numCitizen)*u.productivity);
					}
				}
			}
		}


		//Seller�̍X�V
		for (auto& s : u.sellers)
		{
			if (s.progress < s.period) s.progress++;
			int numSell = s.casket.numItem - int(s.target*(1 - s.progress / double(s.period)));

			if (numSell > 0)
			{
				u.sellItem(s.casket.itemType, numSell, Max(1, int(1 + s.wallet().price*Random(1.00, 1.10))), s.walletID);
				s.casket.numItem -= numSell;
			}
		}

		//Buyer�̍X�V
		for (auto& b : u.buyers)
		{
			if (b.progress < b.period) b.progress++;

			int numBuy = Min(u.numItem(b.casket.itemType), int(b.target*(b.progress / double(b.period))) - b.casket.numItem);
			if (numBuy <= 1) continue;
			if (u.cost(b.casket.itemType, numBuy) > b.price*numBuy) continue;


			u.buyItem(b.casket.itemType, b.walletID, numBuy);
			b.casket.numItem += numBuy;
		}
		u.sellers.remove_if([](const Seller& s) { return s.progress == s.period; });

		//Customer�̍X�V
		int sum = std::accumulate(u.citizens.begin(), u.citizens.end(), 0, [](int _sum, Citizen& _c) { return _sum + _c.wallet().money; });
		for (auto& c : u.citizens) c.wallet().money = 0;
		for (auto& c : u.customers)
		{
			c.wallet().money = int(c.rate*sum);

			//�w��
			for (int i = 0; i < int(itemData.size()); i++)
			{
				const double h = Random(0.1, 5.0);
				auto itemType = itemData.choice().id();

				if (!u.isSoldOut(itemType))
				{
					if (itemData[itemType].value / double(u.cost(itemType)) > h && u.cost(itemType) < c.wallet().money)
					{
						u.buyItem(itemType, c.walletID);
					}
				}
			}
			c.wallet().pull(c.wallet().money / 10);
		}
		//�s��̍X�V
		for (auto i : step(int(u.shelves.size())))
		{
			auto& b = u.shelves[i];

			for (auto& r : b.baskets) r.price = Max(1, int(r.price*0.95));	//���i�ቺ

			b.tradeLog.push(); //TradeLog�̍X�V
		}
	}

	//�s���̍X�V
	for (auto& c : u.citizens)
	{
		const auto& je = u.jobEfficiency[c.citizenType];
		const auto& data = c.data();

		if (je == 0) c.jobProgress = 0;

		c.jobProgress += je*planet.timeSpeed;

		//���Y
		while (c.jobProgress >= 1.0)
		{
			c.jobProgress -= 1.0;

			//�����̉��Z
			c.wallet().add(data.wage);

			//Item�̔̔�
			if (data.product.numItem > 0)
			{
				//�ڕW���v��B���ł���ꍇ�̂ݐ��Y
				if (u.isSoldOut(data.product.itemType) || u.cost(data.product.itemType) > c.targetRevenue / data.product.numItem)
				{
					u.sellItem(data.product, Max(1, int(1 + c.wallet().price*Random(1.00, 1.10))), c.walletID);
				}
			}
		}
	}

	//Dock�̍X�V
	for (auto& d : u.docks)
	{
		double ap = planet.timeSpeed;

		for (;;)
		{
			if (d.inProcessTicket != nullptr)
			{
				auto* t = d.inProcessTicket;
				if (d.progress + 100.0*ap < vehicleData[t->vehicleType].constructionCost)
				{
					d.progress += 100.0*planet.timeSpeed;
					break;
				}
				else
				{
					ap -= (vehicleData[t->vehicleType].constructionCost - d.progress) / 100.0;

					//�D�c�̓W�J
					fleets.emplace_back(t->vehicleType, &u);
					fleets.back().name = Format(L"��", fleets.size(), L"�D�c");

					for (auto& dt : u.docks)
					{
						//�A�h���X����
						if (&d != &dt && dt.inProcessTicket != nullptr && dt.inProcessTicket > t) dt.inProcessTicket--;
					}
					u.tickets.erase(u.tickets.begin() + (t - &u.tickets.front()));	//�Ώۃ`�P�b�g�̏���
					d.inProcessTicket = nullptr;
					d.progress = 0.0;
				}
			}
			else
			{
				for (auto& t : u.tickets)
				{
					if (!t.isInProcess && vehicleData[t.vehicleType].tier <= d.tier)
					{
						d.inProcessTicket = &t;
						t.isInProcess = true;
						break;
					}
				}
				if (d.inProcessTicket == nullptr) break;
			}
		}
	}
}
void	updateUrbans()
{

	//�}���`�X���b�h�ɂ�鏈�����s��
	{
		Array<std::thread> threads;

		for (auto& u : urbans) threads.emplace_back(updateUrban, std::ref(u));
		for (auto& t : threads)  t.join();
	}
}