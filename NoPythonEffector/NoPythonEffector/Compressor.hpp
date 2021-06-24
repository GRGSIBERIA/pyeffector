#pragma once
#include <Siv3D.hpp>
#include "Effector.hpp"

namespace effector
{
	class Compressor : public Effector
	{
		float _threashold = 0.0;
		float _ratio = 0.0;
		bool _comp_enabled = true;

		double _ui_threashold = 0.0;
		double _ui_ratio = 0.0;
		bool _ui_enabled = true;

		std::mutex muthreshold;
		std::mutex muratio;
		std::mutex muenabled;

	public:
		virtual ~Compressor() {}

		void apply(asio::SampleType* input, asio::SampleType* output, const long length) override
		{
			std::scoped_lock mutex{ muthreshold, muratio, muenabled };

			const float rv_ratio = 1.0 / _ratio;

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				const float _abs = abs(input[i]);
				if (_abs < _threashold)
				{
					output[i] = input[i];
				}
				else
				{
					const float _sign = (input[i] > 0.0) - (input[i] < 0.0);

					const float _diff = (_abs - _threashold) * rv_ratio;

					input[i] = _sign * (_threashold + _diff);
				}
			}
		}

		const RectF draw(const Vec2& pos, const Font& font) override
		{
			font(U"Compressor").draw(pos, Palette::Black);
			const auto title_reg = font(U"Compressor").region(pos);

			const auto enabled_reg = SimpleGUI::CheckBoxRegion(U"Switch", title_reg.bl() + pad);
			SimpleGUI::CheckBox(_ui_enabled, U"Switch", title_reg.bl() + pad);

			const auto threshold_reg = drawSlider(U"THREASHOLD", _ui_threashold, enabled_reg.bl() + pad, 0.0, 1.0);
			const auto ratio_reg = drawSlider(U"RATIO", _ui_ratio, threshold_reg.bl() + pad, 1.0, 8.0);

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
					_ui_enabled = Parse<bool>(key.value);
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