#pragma once

#include"Casket.h"
#include"Export.h"

struct Wallet;
struct VehicleData;
struct Fleet;
struct Pos;
struct Urban;
struct Route;

enum struct Code
{
	Move,	//������ID�̓s�s�Ɉړ�
	Jump,	//�����̔Ԓn�ɃW�����v
	Wait,	//����x�~
	Buy,
	Sell,
	MVol,	//�ő�e��
};

struct Vehicle
{
	int		vehicleType;
	int		walletID;
	Casket	cargo;
	double	routeProgress;
	double	sleepTimer;
	Urban*	nowUrban;
	Route*	route;
	Export	exportLog;
	bool	stopFlag;	//true�Ȃ�Ύ��Ƃ𒆒f

	int		maxVolume;
	int		reader;
	double	timer;	//�^�p����
	double	period;	//�������̂ɗv���鎞��
	Array<std::pair<Code, int>> chain;

	Vehicle(int _vehicleType, Urban* _nowUrban);
	VehicleData&	data() const;
	Vec2	pos() const;
	double	angle() const;
	Wallet&	wallet() const;
	bool	mouseOver() const;
};

extern Array<Vehicle> vehicles;

void	updateVehicles();
void	drawVehicles();