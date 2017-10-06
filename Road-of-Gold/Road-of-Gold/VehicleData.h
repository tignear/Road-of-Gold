#pragma once

struct VehicleData
{
	String	name;
	double	speed;
	int		volume;
	double	range;
	bool	isShip;
	Texture	icon;
	int		constructionCost;
	int		tier;

	VehicleData(const JSONValue& _json);
	VehicleData()
		: name(L"hoge")
		, speed(1.0)
		, volume(1000)
		, range(10.0)
		, isShip(true)
		, icon()
		, constructionCost(1000)
		, tier(0)
	{}
	int		id() const;
};

extern Array<VehicleData>	vehicleData;

VehicleData*	getVehicleData(const String& _name);
int	getVehicleType(const String& _name);