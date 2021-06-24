#pragma once
#include <Siv3D.hpp>
#include "Effector.hpp"

namespace effector
{
	class DistortionHard : public Effector
	{
		float _gain = 0.0;
		float _level = 0.0;

		double _ui_gain = 0.0;
		double _ui_level = 0.0;

		std::mutex mugain;
		std::mutex mulevel;

	public:
		virtual ~DistortionHard() {}

		void apply(asio::SampleType* input, asio::SampleType* output, const long length) override
		{
			//const float real_gain = pow(10.0, _gain);

			std::scoped_lock mutex{ mugain, mulevel };

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				
				output[i] = input[i] * _gain;

				const float _abs = abs(input[i]);
				
				if (_abs > 1.0)
				{
					const float _sign = (input[i] > 0.0) - (input[i] < 0.0);
					output[i] = _sign;
				}

				output[i] * _level;
			}
		}

		const RectF draw(const Vec2& pos, const Font& font) override
		{
			font(U"Distortion").draw(pos, Palette::Black);
			const auto title_reg = font(U"Compressor").region(pos);

			const auto gain_reg = drawSlider(U"GAIN", _ui_gain, title_reg.bl() + pad, 0.0, 8.0);
			const auto level_reg = drawSlider(U"LEVEL", _ui_level, gain_reg.bl() + pad, 0.0, 1.0);

			{
				std::scoped_lock mutex{ mugain, mulevel };
				_gain = (float)_ui_gain;
				_level = (float)_ui_level;
			}

			return region(title_reg, level_reg);
		}

		void load(INIData& data, const String& path) override
		{
			if (!data.hasSection(U"Distortion"))
			{
				data.addSection(U"Distortion");
				data[U"Distortion.gain"] = Format(0.1);
				data[U"Distortion.level"] = Format(1.0);
			}

			const auto& sec = data.getSection(U"Distortion");
			for (const auto& key : sec.keys)
			{
				if (key.name == U"gain")
				{
					_ui_gain = Parse<double>(key.value);
				}
				else if (key.name == U"level")
				{
					_ui_level = Parse<double>(key.value);
				}
			}
		}

		void save(INIData& data, const String& path) override
		{
			data[U"Distortion.gain"] = Format(_ui_gain);
			data[U"Distortion.level"] = Format(_ui_level);
			data.save(path);
		}
	};
}