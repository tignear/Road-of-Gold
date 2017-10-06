#pragma once
#include"Chain.h"
#include"Export.h"
#include"VehicleData.h"
#include"Casket.h"

struct Vehicle;
struct Route;
struct Wallet;
struct Urban;

struct Fleet
{
	String	name;
	Array<Vehicle*>	ownVehicles;
	int		walletID;
	double	routeProgress;
	double	sleepTimer;
	Urban*	nowUrban;
	Route*	route;
	Export	exportLog;
	double	timer;	//�^�p����
	double	period;	//�������̂ɗv���鎞��
	Chain	chain;
	Casket	cargo;
	VehicleData	data;

	Fleet(int _ownVehicleType, Urban* _nowUrban);
	void	add(Vehicle* _v);
	Vec2	pos() const;
	double	angle() const;
	Wallet&	wallet() const;
	int		id() const;
	bool	canMoveTo(const Urban& _u) const;
	void	setMoveTo(const Urban& _u);
	void	addMoveTo(Urban* _from, Urban* _to);
	bool	mouseOver() const;
	RectF	shape() const;
	bool	isInSelectedRegion() const;
};

extern Array<Fleet>	fleets;

void	updateFleets();
void	drawFleets();