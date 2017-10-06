#include"Urban.h"
#include"EnergyData.h"
#include"Node.h"
#include"CitizenData.h"
#include"Wallet.h"
#include"TinyCamera.h"
#include"ItemData.h"
#include"Route.h"
#include<numeric>

Urban::Urban(const JSONValue& _json)
	: name(_json[L"Name"].getString())
	, joinedNodeID(_json[L"JoinedNodeID"].getOr<int>(-1))
	, productivity(_json[L"Productivity"].getOr<double>(1.0))
	, sandglass(0.5 + pos().mPos.x / 360_deg)
	, walletID(getNewWalletID())
	, averageIncome(0)
	, isSelected(false)
{
	buyers.reserve(1024);
	//Shelf
	shelves.resize(itemData.size());

	//����
	jobEfficiency.resize(citizenData.size());
	jobEfficiency.fill(1);

	//�����z�u
	for (auto i : _json[L"Energies"].objectView())
	{
		if (getEnergyType(i.name) == -1) Output << L"���݂��Ȃ�Energy���ǂݍ��܂�܂���";
		else energies.emplace_back(getEnergyType(i.name), i.value.getOr<int>(0));
	}

	//Customer��rate����p�֐�
	const auto customersRateFunc = []()
	{
		double k = Random(1.0);
		return 2.0* pow(k, 5) + 1.0*k + 0.4;
		//return k;
	};

	//Citizens��Customers�̒ǉ�
	for (int i = 0; i < _json[L"NumCitizens"].getOr<int>(0); i++)
	{
		citizens.emplace_back(0);
		customers.emplace_back(customersRateFunc());
	}

	//customers�̐���
	double sum = std::accumulate(customers.begin(), customers.end(), 0.0, [](double _sum, Customer& _c) { return _sum + _c.rate; });
	for (auto& c : customers) c.rate /= sum;
	customers.sort_by([](Customer& _x, Customer& _y) { return _x.rate < _y.rate; });

	//Dock�̒ǉ�-��
	for (int i = 0; i < 5; i++)
	{
		if (i < 3) docks.emplace_back(1);
		else docks.emplace_back(0);
	}

	tickets.reserve(1000);	//1000�`�P�b�g�܂ŋ���
}
Pos		Urban::pos() const { return nodes.at(joinedNodeID).pos; }
Circle	Urban::shape() const { return Circle(pos().mPos, 0.015); }
bool	Urban::mouseOver() const
{
	return shape().mouseOver();
}
bool	Urban::leftClicked() const
{
	return shape().leftClicked();
}
bool	Urban::rightClicked() const
{
	return shape().rightClicked();
}
int		Urban::id() const
{
	return int(this - &urbans.front());
}

void	Urban::sellItem(const Casket& _casket, int _price, int _ownerWalletID)
{
	sellItem(_casket.itemType, _casket.numItem, _price, _ownerWalletID);
}
void	Urban::sellItem(int _itemType, int _numItem, int _price, int _ownerWalletID)
{
	auto& b = shelves[_itemType];

	_price = int(b.tradeLog.price.front()*Random(1.0, 1.1)) + 1;

	if (!b.baskets.isEmpty() && _price < b.baskets.back().price)
	{
		b.baskets.emplace_back(_itemType, _numItem, _price, _ownerWalletID);
		b.baskets.sort_by([](const Basket& x, const Basket& y) { return x.price < y.price; });
	}
	else b.baskets.emplace_back(_itemType, _numItem, _price, _ownerWalletID);

	if (wallets[_ownerWalletID].owner == Owner::Citizen) b.tradeLog.numProduction.front() += _numItem;

	b.numItem += _numItem;

}
int		Urban::numItem(int _itemType) const
{
	return shelves[_itemType].numItem;
}
bool	Urban::isSoldOut(int _itemType) const
{
	return shelves[_itemType].baskets.isEmpty();
}
int		Urban::cost(int _itemType, int _numItem) const
{
	const auto& b = shelves[_itemType];
	int	totalCost = 0;
	int num = _numItem;
	for (auto& r : b.baskets)
	{
		if (r.casket.numItem < num)
		{
			num -= r.casket.numItem;
			totalCost += r.casket.numItem*r.price;
		}
		else
		{
			totalCost += num*r.price;
			return totalCost;
		}
	}

	Output << L"�ُ�Ȋ֐��Ăяo��������܂����BUrban::cost()";
	return -1;
}
void	Urban::buyItem(int _itemType, int _walletID, int _numItem)
{
	auto& b = shelves[_itemType];
	int num = _numItem;
	for (;;)
	{
		int	numBuy = 0;
		auto& r = b.baskets.front();

		if (r.casket.numItem < num)
		{
			b.tradeLog.addTrade(r.price, r.casket.numItem);
			b.numItem -= r.casket.numItem;
			wallets[_walletID].sendTo(&wallets[r.ownerWalletID], r.casket.numItem*r.price);
			numBuy = r.casket.numItem;
			wallets[r.ownerWalletID].price = r.price;

			if (wallets[_walletID].owner == Owner::Vehicle) b.tradeLog.numExport.front() += numBuy;
			if (r.owner() == Owner::Vehicle) b.tradeLog.numImport.front() += numBuy;
			if (wallets[_walletID].owner == Owner::Citizen) b.tradeLog.numConsumption.front() += numBuy;
			b.baskets.pop_front();
		}
		else if (r.casket.numItem == num)
		{
			b.tradeLog.addTrade(r.price, num);
			b.numItem -= num;
			wallets[_walletID].sendTo(&wallets[r.ownerWalletID], num*r.price);
			wallets[r.ownerWalletID].price = r.price;
			numBuy = num;
			if (wallets[_walletID].owner == Owner::Vehicle) b.tradeLog.numExport.front() += numBuy;
			if (r.owner() == Owner::Vehicle) b.tradeLog.numImport.front() += numBuy;
			if (wallets[_walletID].owner == Owner::Citizen) b.tradeLog.numConsumption.front() += numBuy;
			b.baskets.pop_front();
		}
		else
		{
			b.tradeLog.addTrade(r.price, num);
			b.numItem -= num;
			wallets[_walletID].sendTo(&wallets[r.ownerWalletID], num*r.price);
			r.casket.numItem -= num;
			numBuy = num;
			if (wallets[_walletID].owner == Owner::Vehicle) b.tradeLog.numExport.front() += numBuy;
			if (r.owner() == Owner::Vehicle) b.tradeLog.numImport.front() += numBuy;
			if (wallets[_walletID].owner == Owner::Citizen) b.tradeLog.numConsumption.front() += numBuy;
			wallets[r.ownerWalletID].price = r.price;
		}


		num -= numBuy;
		if (num == 0) break;
	}
}
void	TradeLog::addTrade(int _price, int _num)
{
	if (_num == 0)
	{
		Output << L"�ُ�Ȋ֐��Ăяo��������܂����BTradeLog::addTrade()";
		return;
	}
	const int n = price.front()*numTrade.front() + _price*_num;
	numTrade.front() += _num;
	price.front() = n / numTrade.front();
}
Urban*	getUrban(const String& _name)
{
	for (auto& u : urbans)
	{
		if (u.name == _name) return &u;
	}
	return nullptr;
}
void	Urban::addEnergy(const String _name, int _num)
{
	auto* data = getEnergyData(_name);

	for (auto& e : energies)
	{
		if (e.energyType == data->id())
		{
			e.numEnergy += _num;
			return;
		}
	}

	energies.emplace_back(data->id(), _num);
}
void	Urban::pullEnergy(const String _name, int _num)
{
	auto* data = getEnergyData(_name);

	for (auto& e : energies)
	{
		if (e.energyType == data->id())
		{
			if (e.numEnergy <= _num)
			{
				e.numEnergy = 0;
			}
			else e.numEnergy -= _num;

			return;
		}
	}
}
bool	Urban::hasRoute() const { return !ownRoutes.isEmpty(); }
bool	Urban::hasSeaRoute() const
{
	return ownRoutes.any([](const Route* _u) { return _u->isSeaRoute; });
}
bool	Urban::hasLandRoute() const
{
	return ownRoutes.any([](const Route* _u) { return !_u->isSeaRoute; });
}
int		Urban::numCitizens(int _citizenType) const
{
	return int(citizens.count_if([&_citizenType](const Citizen& c) { return c.citizenType == _citizenType; }));
}