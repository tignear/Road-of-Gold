﻿#include"Display.h"
#include"Urban.h"
#include"CitizenData.h"
#include<numeric>

void	DisplayUrban::update()
{
	if (selectedUrban != nullptr)
	{
		if (KeyRight.down())
		{
			if (selectedUrban == &urbans.back()) selectedUrban = &urbans.front();
			else selectedUrban++;
			openElapsedTime.restart();
		}
		if (KeyLeft.down())
		{
			if (selectedUrban == &urbans.front()) selectedUrban = &urbans.back();
			else selectedUrban--;
			openElapsedTime.restart();
		}
	}

	Transformer2D t1;
	if (selectedUrban == nullptr)  t1 = Transformer2D(Mat3x2::Translate(EaseOut(Easing::Expo, 0.0, -480.0, Min(1.0, closeElapsedTime.ms() / 500.0)), 0));
	else t1 = Transformer2D(Mat3x2::Translate(EaseOut(Easing::Expo, -480.0, 0.0, Min(1.0, openElapsedTime.ms() / 500.0)), 0));
	const auto frameColor = Color(20);
	const auto baseColor = Color(40);
	const auto fontColor = Color(255);
	const auto mouseOverColor = Color(70);
	const auto font16 = (*display.fonts[16]);
	const auto font24 = (*display.fonts[24]);
	const auto font36 = (*display.fonts[36]);
	const auto thickness = 4;
	auto* su = selectedUrban;
	auto conv = [](int value) {
		if (value < 1000) return Format(value);
		if (value > 1000) return Format(value / 1000, L"K");
		if (value > 1000000) return Format(value / 1000000, L"M");
		else return String(L"HugeValue");
	};

	//全体枠の表示
	{
		const Rect(0, 0, 480, 720).draw(baseColor).drawFrame(thickness, frameColor);
	}

	if (selectedUrban == nullptr) return;

	//都市名の表示
	{
		const Rect rect(0, 0, 480, 48);
		font36(su->name).drawAt(rect.center(), fontColor);
	}

	//選択
	{
		const Array<String>	list = { L"労働", L"市場", L"政治", L"造船" };
		const auto w = int(448 / list.size());
		{
			const auto backgroundColor = Color(60);
			for (int i = 0; i < int(list.size()); i++)
			{
				const Rect rect1(w*i + 16, 48, w, 36);
				const Polygon rect2{
					{ 12 + w*i, 84 },
					{ 20 + w*i, 48 },
					{ 12 + w*i + w, 48 },
					{ 20 + w*i + w, 84 },
				};
				if (rect2.leftClicked()) urbanInfoState = UrbanInfoState(i);
				rect2.draw(rect1.mouseOver() ? mouseOverColor : backgroundColor).drawFrame(thickness, frameColor);
				font24(list[i]).drawAt(rect1.center(), fontColor);
			}
		}
		{
			const auto backgroundColor = Color(80);
			const Rect rect1(w*int(urbanInfoState) + 16, 48, w, 36);
			Polygon{
				{ 12 + w*int(urbanInfoState), 84 },
				{ 20 + w*int(urbanInfoState), 48 },
				{ 12 + w*int(urbanInfoState) + w, 48 },
				{ 20 + w*int(urbanInfoState) + w, 84 },
				{ 472, 84 },
				{ 472, 712 },
				{ 8, 712 },
				{ 8, 84 },
			}.draw(backgroundColor).drawFrame(thickness, frameColor);
			font24(list[int(urbanInfoState)]).drawAt(rect1.center(), fontColor);
		}
	}
	switch (urbanInfoState)
	{
	case UrbanInfoState::Worker:
	{
		//基礎情報の描画
		{
			Rect(16, 100, 160, 72).draw(Color(100));
			{
				const Rect rect(16, 100, 160, 24);
				rect.drawFrame(thickness, frameColor);
				font16(L"基礎情報").drawAt(rect.center(), fontColor);
			}
			//人口の描画
			{
				const Rect rect1(16, 124, 80, 24);
				const Rect rect2(96, 124, 80, 24);
				rect1.drawFrame(thickness, frameColor);
				rect2.drawFrame(thickness, frameColor);
				const String string1(L"総人口");
				const String string2(Format(conv(int(su->citizens.size())), L"人"));
				font16(string1).drawAt(rect1.center(), fontColor);
				auto w = (int)font16(string2).region().w;
				font16(string2).draw(rect2.tr().movedBy(-4 - w, 0), fontColor);
			}
			//市民収入の描画
			{
				const Rect rect1(16, 148, 80, 24);
				const Rect rect2(96, 148, 80, 24);
				rect1.drawFrame(thickness, frameColor);
				rect2.drawFrame(thickness, frameColor);
				const String string1(L"市民収入");
				const String string2 = Format(conv(su->averageIncome), L"G");
				font16(string1).drawAt(rect1.center(), fontColor);
				auto w = (int)font16(string2).region().w;
				font16(string2).draw(rect2.tr().movedBy(-4 - w, 0), fontColor);
			}
		}

		//労働人口グラフ
		{
			{
				typedef std::pair<CitizenData*, double> List;

				Rect rect(16, 180, 320, 320);
				Circle circle(rect.center(), 160);
				Array<List> list;
				
				rect.drawFrame(thickness, frameColor);

				for (auto& cd : citizenData)
				{
					list.emplace_back(&cd, 360_deg*double(su->numCitizens(cd.id())) / double(su->citizens.size()));
				}
				list.sort_by([](List& x, List& y) { return x.second > y.second; });
				for (auto& l : list)
				{
					if (l.second < 10_deg) l.first = nullptr;
				}
				{
					for (auto& l : list)
					{
						const double startAngle = std::accumulate(list.begin(), list.begin() + int(&l - &list.front()), 0.0, [](double sum, List& l) { return sum + l.second; });
						const auto color = l.first == nullptr ? Color(80) : HSV(360 * double(l.first->id()) / double(citizenData.size()), 0.5);
						circle.drawPie(startAngle, l.second, color);
					}
					circle.drawFrame(thickness, frameColor);
					for (auto& l : list)
					{
						const double startAngle = std::accumulate(list.begin(), list.begin() + int(&l - &list.front()), 0.0, [](double sum, List& l) { return sum + l.second; });
						if (l.first != nullptr) font16(l.first->name).drawAt(rect.center().movedBy(Vec2(0, -100).rotated(startAngle + l.second / 2.0)));
					}
				}
			}
		}
	}

	break;

	case UrbanInfoState::Shelves:
	{

	}
	break;

	default:
		break;
	}
	{

	}
}