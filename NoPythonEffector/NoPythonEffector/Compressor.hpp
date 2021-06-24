#pragma once
#include <Siv3D.hpp>
#include "Effector.hpp"

namespace effector
{
	class Compressor : public Effector
	{
		double _threashold = 0.0;
		double _ratio = 0.0;
		bool _comp_enabled = true;

		double _ui_threashold = 0.0;
		double _ui_ratio = 0.0;

		std::mutex muthreshold;
		std::mutex muratio;
		std::mutex muenabled;

	public:
		virtual ~Compressor() {}

		void apply(asio::SampleType* input, asio::SampleType* output, const long length) override
		{
			std::scoped_lock mutex{ muthreshold, muratio, muenabled };

			const double rv_ratio = 1.0 / _ratio;
			const double norm_threashold = _threashold * (double)INT_MAX;

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				const double _abs = (double)abs(input[i]);
				if (_abs < norm_threashold)
				{
					output[i] = input[i];
				}
				else
				{
					const double _sign = (input[i] > 0.0) - (input[i] < 0.0);

					const double _diff = (_abs - norm_threashold) * rv_ratio;

					output[i] = (asio::SampleType)(_sign * (norm_threashold + _diff));
				}
			}
		}

		const RectF draw(const Vec2& pos, const Font& font) override
		{
			font(U"Compressor").draw(pos, Palette::Black);
			const auto title_reg = font(U"Compressor").region(pos);

			const auto enabled_reg = SimpleGUI::CheckBoxRegion(U"Switch", title_reg.bl() + pad);
			SimpleGUI::CheckBox(_comp_enabled, U"Switch", title_reg.bl() + pad);

			const auto threshold_reg = drawSlider(U"THREASHOLD", _ui_threashold, enabled_reg.bl() + pad, 0.0, 1.0);
			const auto ratio_reg = drawSlider(U"RATIO", _ui_ratio, threshold_reg.bl() + pad, 0.01, 4.0);

			{
				std::scoped_lock mutex{ muthreshold, muratio };
				_threashold = (float)_ui_threashold;
				_ratio = (float)_ui_ratio;
			}

			return region(title_reg, ratio_reg);
		}

		const bool enabled() const { return _comp_enabled; }

		void load(INIData& data, const String& path) override
		{
			if (!data.hasSection(U"Compressor"))
			{
				data.addSection(U"Compressor");
				data[U"Compressor.enabled"] = Format(_comp_enabled);
				data[U"Compressor.threashold"] = Format(0.1);
				data[U"Compressor.ratio"] = Format(2.0);
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
				else if (key.name == U"enabled")
				{
					_comp_enabled = Parse<bool>(key.value);
				}
			}
		}

		void save(INIData& data, const String& path) override
		{
			data[U"Compressor.enabled"] = Format(_comp_enabled);
			data[U"Compressor.threashold"] = Format(_ui_threashold);
			data[U"Compressor.ratio"] = Format(_ui_ratio);
			data.save(path);
		}
	};
}