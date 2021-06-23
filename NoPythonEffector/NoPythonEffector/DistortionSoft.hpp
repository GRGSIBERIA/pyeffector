#pragma once
#include <Siv3D.hpp>
#include "Effector.hpp"

namespace effector
{

	class DistortionSoft : public Effector
	{
		float _gain = 0.0;
		float _quarity = 0.0;
		float _level = 0.0;

		double _ui_gain = 0.0;
		double _ui_quarity = 0.0;
		double _ui_level = 0.0;

		std::mutex mugain;
		std::mutex muquarity;
		std::mutex mulevel;
	public:
		virtual ~DistortionSoft() {}

		void apply(asio::SampleType* input, asio::SampleType* output, const long length) override
		{
			std::scoped_lock mutex{ mugain, muquarity, mulevel };

			const float pi2 = 1.0 / std::numbers::pi * 0.5;

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				input[i] = atanf(output[i]) * pi2;
			}

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				input[i] = input[i] > 0.0 ? input[i] * _gain : input[i] * _gain * _quarity;
			}

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				output[i] = (input[i] > 0.0) - (input[i] < 0.0);
				output[i] = output[i] * input[i] > 1.0 ? output[i] : input[i];
			}

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				output[i] *= _level;
			}
		}

		const RectF draw(const Vec2& pos, const Font& font) override
		{
			font(U"Overdrive").draw(pos, Palette::Black);
			const auto title_reg = font(U"Soft Clipping").region(pos);

			const auto gain_reg = drawSlider(U"GAIN", _ui_gain, title_reg.bl() + pad, 0.0, 20.0);
			const auto quarity_reg = drawSlider(U"QUARITY", _ui_quarity, gain_reg.bl() + pad, 0.0, 1.0);
			const auto level_reg = drawSlider(U"LEVEL", _ui_level, quarity_reg.bl() + pad, 0.0, 1.0);

			{
				std::scoped_lock mutex{ mugain, muquarity, mulevel };
				_gain = (float)_ui_gain;
				_quarity = (float)_ui_quarity;
				_level = (float)_ui_level;
			}

			return region(title_reg, level_reg);
		}

		void load(INIData& data, const String& path) override
		{
			if (!data.hasSection(U"Overdrive"))
			{
				data.addSection(U"Overdrive");
				data[U"Overdrive.gain"] = Format(1.0);
				data[U"Overdrive.quarity"] = Format(0.3);
				data[U"Overdrive.level"] = Format(1.0);
				data.save(path);
			}

			const auto& sec = data.getSection(U"Overdrive");
			for (const auto& key : sec.keys)
			{
				if (key.name == U"gain")
				{
					_ui_gain = Parse<double>(key.value);
				}
				else if (key.name == U"quarity")
				{
					_ui_quarity = Parse<double>(key.value);
				}
				else if (key.name == U"level")
				{
					_ui_level = Parse<double>(key.value);
				}
			}
		}

		void save(INIData& data, const String& path) override
		{
			data[U"Overdrive.gain"] = Format(_ui_gain);
			data[U"Overdrive.quarity"] = Format(_ui_quarity);
			data[U"Overdrive.level"] = Format(_ui_level);
			data.save(path);
		}
	};
}