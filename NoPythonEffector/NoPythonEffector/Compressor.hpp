#pragma once
#include <Siv3D.hpp>
#include "Effector.hpp"

namespace effector
{
	class Compressor : public Effector
	{
		float _threashold = 0.0;
		float _ratio = 0.0;

		double _ui_threashold = 0.0;
		double _ui_ratio = 0.0;

		std::mutex muthreshold;
		std::mutex muratio;

	public:
		virtual ~Compressor() {}

		void apply(asio::SampleType* input, asio::SampleType* output, const long length) override
		{
			std::scoped_lock mutex{ muthreshold, muratio };

			const float reverse_ratio = 1.0 / _ratio;

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				output[i] = (input[i] > 0.0) - (input[i] < 0.0);
			}

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				output[i] =
					output[i] * input[i] < _threashold ?
					input[i] : input[i] * (_threashold + (((output[i] * input[i]) - _threashold) * reverse_ratio));
			}
		}

		const RectF draw(const Vec2& pos, const Font& font) override
		{
			font(U"Compressor").draw(pos, Palette::Black);
			const auto title_reg = font(U"Compressor").region(pos);

			const auto threshold_reg = drawSlider(U"THREASHOLD", _ui_threashold, title_reg.bl() + pad, 0.0, 1.0);
			const auto ratio_reg = drawSlider(U"RATIO", _ui_ratio, threshold_reg.bl() + pad, 1.0, 8.0);

			{
				std::scoped_lock mutex{ muthreshold, muratio };
				_threashold = (float)_ui_threashold;
				_ratio = (float)_ui_ratio;
			}

			return region(title_reg, ratio_reg);
		}

		void load(INIData& data, const String& path) override
		{
			if (!data.hasSection(U"Compressor"))
			{
				data.addSection(U"Compressor");
				data[U"Compressor.threashold"] = Format(0.0);
				data[U"Compressor.ratio"] = Format(0.0);
				data.save(path);
			}
			
			const auto& sec = data.getSection(U"Compressor");
			for (const auto& key : sec.keys)
			{
				if (key.name == U"ratio")
				{
					_ui_ratio = Parse<double>(key.value);
				}
				else if (key.name == U"threashold")
				{
					_ui_threashold = Parse<double>(key.value);
				}
			}
			
		}

		void save(INIData& data, const String& path) override
		{
			data[U"Compressor.threashold"] = Format(_ui_threashold);
			data[U"Compressor.ratio"] = Format(_ui_ratio);
			data.save(path);
		}
	};
}