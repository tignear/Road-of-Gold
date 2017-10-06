#pragma once

//����̃}�b�v�ł͏�ɓ���Route���쐬����邱�Ƃ��ۏ؂���Ȃ���΂Ȃ�Ȃ��B

struct Path;
struct Urban;

struct Route
{
	Array<Path*> paths;
	Urban*	fromUrban;
	Urban*	toUrban;
	double	movingCost;
	bool	isSeaRoute;
	int		numVehicles;
	bool	drawFlag;

	Route(Urban* _fromUrban, Urban* _toUrban, Array<Path*> _paths, bool _isSeaRoute, double _movingCost)
		: fromUrban(_fromUrban)
		, toUrban(_toUrban)
		, isSeaRoute(_isSeaRoute)
		, movingCost(_movingCost)
		, numVehicles(0)
		, drawFlag(false)
	{
		paths = _paths;
	}
	void	addVehicle();
	void	pullVehicle();
	void	draw();
};

extern Array<Route> routes;

void	initRoutes();
void	drawRotue(const Route& _r, const Color& _color, double _width);