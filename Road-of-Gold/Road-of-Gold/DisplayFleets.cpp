#include"Display.h"
#include"Fleet.h"
#include"VehicleData.h"

void	DisplayFleets::update()
{
	const auto frameColor = Color(20);
	const auto baseColor = Color(40);
	const auto fontColor = Color(255);
	const auto mouseOverColor = Color(70);
	const auto font16 = (*globalFonts[16]);
	const auto font24 = (*globalFonts[24]);
	const auto font36 = (*globalFonts[36]);
	const auto thickness = 4;
	auto conv = [](int value) {
		if (value < 1000) return Format(value);
		if (value > 1000) return Format(value / 1000, L"K");
		if (value > 1000000) return Format(value / 1000000, L"M");
		else return String(L"HugeValue");
	};
	Transformer2D t1;
	if (selecter.selectedFleets.isEmpty())  t1 = Transformer2D(Mat3x2::Translate(EaseOut(Easing::Expo, 0.0, -480.0, Min(1.0, closeElapsedTime.ms() / 500.0)), 0));
	else t1 = Transformer2D(Mat3x2::Translate(EaseOut(Easing::Expo, -480.0, 0.0, Min(1.0, openElapsedTime.ms() / 500.0)), 0));


	//�S�̘g�̕\��
	{
		const Rect(0, 0, 480, 720).draw(baseColor).drawFrame(thickness, frameColor);
	}

	//�f�U�C��
	{
		Rect rect(8, 8, 464, 88);
		rect.draw(Color(80)).drawFrame(thickness, Color(60));
		font36(L"�D�c�^�p�w��").drawAt(rect.center());
	}

	if (selecter.selectedFleets.isEmpty()) return;


	//�D�c�̕\��
	if (selecter.selectedFleets.size() == 1)	//�P�̑D�c��\������ꍇ
	{
		Point pos(0, 96);
		auto* sf = selecter.selectedFleets.front();
		//�S�̘g	
		{
			Rect rect(8, 8, 464, 48);
			rect.moveBy(pos);
			rect.draw(Color(80)).drawFrame(thickness, Color(60));
		}
		//�D�c��
		{
			Rect rect(8, 8, 12 * 20, 24);
			rect.moveBy(pos);
			rect.draw(Color(80)).drawFrame(thickness, Color(60));
			font16(sf->name).drawAt(rect.center(), fontColor);
		}
		//�D�c�p�^�[��
		{
			Rect rect(8, 32, 12 * 20, 24);
			rect.moveBy(pos);
			rect.draw(Color(80)).drawFrame(thickness, Color(60));
			if (sf->chain.size() > sf->chain.readerPos && sf->chain.readerPos >= 0)
			{
				font16(sf->chain.nowRing().valueText()).drawAt(rect.center(), fontColor);
			}
		}
		//���\
		{
			{
				Rect rect(8, 160, 464, 552);
				rect.draw(Color(80)).drawFrame(thickness, Color(60));
			}
			{
				Rect rect(16, 168, 216, 36);
				rect.drawFrame(thickness, Color(60));
				font24(L"�D�c�f�[�^").drawAt(rect.center());
			}
			{
				Rect rect(16, 204, 104, 24);
				rect.drawFrame(thickness, Color(60));
				font16(L"�q������").drawAt(rect.center());
			}
			{
				auto f = font16(sf->data.range*1000.0, L"mt");
				Rect rect(120, 204, 112, 24);
				rect.drawFrame(thickness, Color(60));
				f.draw(rect.tr().movedBy(-4 - int(f.region().w), 0));
			}
			{
				Rect rect(16, 228, 104, 24);
				rect.drawFrame(thickness, Color(60));
				font16(L"�ړ����x").drawAt(rect.center());
			}
			{
				auto f = font16(sf->data.speed);
				Rect rect(120, 228, 112, 24);
				rect.drawFrame(thickness, Color(60));
				f.draw(rect.tr().movedBy(-4 - int(f.region().w), 0));
			}
			{
				Rect rect(16, 252, 104, 24);
				rect.drawFrame(thickness, Color(60));
				font16(L"�ݕ��e��").drawAt(rect.center());
			}
			{
				auto f = font16(sf->data.volume);
				Rect rect(120, 252, 112, 24);
				rect.drawFrame(thickness, Color(60));
				f.draw(rect.tr().movedBy(-4 - int(f.region().w), 0));
			}
		}
		//�������j�b�g
		{
			{
				Rect rect(16, 284, 216, 36);
				rect.drawFrame(thickness, Color(60));
				font24(L"�����D").drawAt(rect.center());
			}
			int j = 0;
			for (int i = 0; i < int(vehicleData.size()); i++)
			{
				int	num = int(sf->ownVehicles.count_if([&i](const Vehicle* v) { return v->vehicleType == i; }));
				if (num > 0)
				{
					{
						Rect rect(16, 320 + j * 24, 104, 24);
						rect.drawFrame(thickness, Color(60));
						font16(vehicleData[i].name).drawAt(rect.center());
					}
					{
						Rect rect(120, 320 + j * 24, 112, 24);
						auto f = font16(num, L"��");
						rect.drawFrame(thickness, Color(60));
						f.draw(rect.tr().movedBy(-4 - int(f.region().w), 0));
					}
					j++;
				}
			}
		}
		//���[�g�쐬
		{
			{
				Rect rect(240, 168, 224, 36);
				if (rect.mouseOver()) rect.draw(Color(120));
				rect.drawFrame(thickness, Color(60));
				if (rect.leftClicked())
				{
					routeMaker.set(sf);
					selecter.selectedFleets.clear();
					closeElapsedTime.restart();
					return;
				}
				font24(L"���[�g�쐬").drawAt(rect.center());
			}

		}
		//�R�}���h�\��
		{
			for (int i = 0; i < int(sf->chain.size()); i++)
			{
				auto& r = sf->chain.rings[i];
				Rect rect(240, 212 + i * 24, 224, 24);
				if (int(sf->chain.readerPos) == i) rect.draw(Color(120));
				rect.drawFrame(thickness, Color(60));
				font16(r.valueText()).draw(rect.pos.movedBy(4, 0));
			}
		}
	}
	else	//�����D�c��\������ꍇ
	{


		for (int i = 0; i < Min(11, int(selecter.selectedFleets.size())); i++)
		{
			Point pos(0, i * 56 + 56 + 40);
			auto* sf = selecter.selectedFleets[i];
			//�S�̘g
			{
				Rect rect(8, 8, 464, 48);
				rect.moveBy(pos);
				rect.draw(Color(80)).drawFrame(thickness, Color(60));
			}
			//�D�c��
			{
				Rect rect(8, 8, 12 * 20, 24);
				rect.moveBy(pos);
				rect.draw(Color(80)).drawFrame(thickness, Color(60));
				font16(sf->name).drawAt(rect.center(), fontColor);
			}
			//�D�c�p�^�[��
			{
				Rect rect(8, 32, 12 * 20, 24);
				rect.moveBy(pos);
				rect.draw(Color(80)).drawFrame(thickness, Color(60));
				if (sf->chain.size() > sf->chain.readerPos && sf->chain.readerPos >= 0)
				{
					font16(sf->chain.nowRing().valueText()).drawAt(rect.center(), fontColor);
				}
			}
			//�I��
			{
				Rect rect(424, 8, 48, 24);
				rect.moveBy(pos);
				rect.draw(Color(rect.mouseOver() ? 100 : 80)).drawFrame(thickness, Color(60));
				if (rect.leftClicked())
				{
					selecter.selectedFleets.clear();
					selecter.selectedFleets.emplace_back(sf);
					return;
				}
				font16(L"�I��").drawAt(rect.center(), fontColor);
			}
			//�D�c�̌�
			{
				int j = 0;
				for (int k = 0; k < int(vehicleData.size()); k++)
				{
					size_t size = sf->ownVehicles.count_if([&k](Vehicle* _v) { return _v->vehicleType == k; });
					if (size > 0)
					{
						auto p = Vec2(256 + j * 64, 128 + i * 56);
						vehicleData[k].icon.resize(24, 24).draw(p);
						font16(L"x", size).draw(p.movedBy(32.0, 0.0));
						j++;
					}
				}
			}
		}
	}
}