#pragma once
#include <mutex>
#include <math.h>
#include <numbers>
#include <Siv3D.hpp>
#include "TinyASIO/TinyASIO.hpp"

namespace effector
{
	class Icon
	{
		Vec2 center;
		float radius = 10.0;
		float volume = 0.0;
		float movedup = 8.0;
		float dot_radius = 2.0;

		const float half_angle = 165.0;

	public:
		Icon(const Vec2& _center, const float _radius, const float _volume, const float _movedup, const float _dot_radius)
			: radius(_radius), center(_center), volume(_volume), movedup(_movedup), dot_radius(_dot_radius)
		{

		}

		void update()
		{
			const float sign = (volume > 0.0) - (volume < 0.0);	// 符号を取る
			volume = sign < 0.0 ? 0.0 : (volume > 1.0 ? 1.0 : volume);	// [0,1]の値を取らせる
		}

		void draw()
		{
			const auto tsumami = Circle(center, radius).draw(Palette::Darkgray);

			const auto identity = Vec2{ 1.0, 0.0 }.rotate(2.0 * half_angle * volume - half_angle);

			Circle(center + identity * movedup, dot_radius).draw(Palette::Gray);
		}

		void setVolume(const float _volume) { volume = _volume; }

		const float getVolume() const { return volume; }
	};

	class Effector
	{
	protected:
		const double slider_width = 200.0;
		const double label_width = 200.0;
		const Vec2 pad = Vec2{ 0.0, 4.0 };

		const RectF drawSlider(const String& label, double& uidata, const Vec2& pos, const double minimum, const double maximum)
		{
			SimpleGUI::Slider(label, uidata, minimum, maximum, pos, label_width, slider_width);
			return SimpleGUI::SliderRegion(pos, label_width, slider_width);
		}

		const RectF region(const RectF& title, const RectF& tail) const
		{
			return RectF{
				title.tl(), tail.br() - title.tl()
			};
		}

	public:
		virtual void apply(asio::SampleType* input, asio::SampleType* output, const long length) = 0;

		virtual ~Effector() {}

		virtual const RectF draw(const Vec2& pos, const Font& font) = 0;
		virtual void load(INIData& data, const String& path) = 0;
		virtual void save(INIData& data, const String& path) = 0;
	};
}