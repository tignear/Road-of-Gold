#pragma once
#include"Casket.h"

struct Urban;
struct Wallet;
struct ItemData;
enum struct Owner;


struct Basket
{
	Casket	casket;
	int		price;
	int		ownerWalletID;

	Basket(const Casket& _casket, int _price, int _ownerWalletID);
	Basket(int _itemType, int _numItem, int _price, int _ownerWalletID);
	Wallet& wallet() const;
	Owner	owner() const;
}; 
struct TradeLog
{
	Array<int>	numTrade;		//取引成立数
	Array<int>	price;			//平均取引価格
	Array<int>	numProduction;	//市民による生産量
	Array<int>	numConsumption;	//市民による消費量
	Array<int>	numExport;		//輸出量
	Array<int>	numImport;		//輸入量
	int		time;	//保存している期間

	TradeLog()
		: time(120)
	{
		numTrade.resize(time);
		price.resize(time);
		numProduction.resize(time);
		numConsumption.resize(time);
		numExport.resize(time);
		numImport.resize(time);
		price.fill(1);
	}

	void	push()
	{
		numTrade.push_front(0);
		price.push_front(price.front());
		numProduction.push_front(0);
		numConsumption.push_front(0);
		numImport.push_front(0);
		numExport.push_front(0);
		time++;
	}

	void	addTrade(int _price, int _num = 1);
};
void	drawGraph(const Rect& _rect, Array<int> _log, int _timeScale, Color _color);
void	drawGraph(const Rect& _rect, Array<int> _log, int _timeScale, Color _color, int _max);

struct Shelf
{
	int		numItem;
	Urban*	joinedUrban;
	TradeLog	tradeLog;
	Array<Basket>	baskets;

	Shelf();
	int		itemType() const;
	ItemData&	data() const;
};