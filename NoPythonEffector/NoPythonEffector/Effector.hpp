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
	public:
		virtual void apply(asio::SampleType* input, asio::SampleType* output, const long length) = 0;

		virtual ~Effector() {}
	};

	class Compressor : public Effector
	{
		float _threashold = 0.0;
		float _ratio = 0.0;

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

		const float getThreashold() const { return _threashold; }

		const float getRatio() const { return _ratio; }

		void setThreashold(const float threashold)
		{
			std::scoped_lock mutex{ muthreshold };
			_threashold = threashold;
		}

		void setRatio(const float ratio)
		{
			std::scoped_lock mutex{ muratio };
			_ratio = ratio;
		}
	};

	class DistortionSoft : public Effector
	{
		float _gain = 0.0;
		float _quarity = 0.0;
		float _level = 0.0;

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

		void setGain(const float gain) 
		{
			std::scoped_lock mutex{ mugain };
			_gain = gain; 
		}

		void setQuarity(const float quarity) 
		{ 
			std::scoped_lock mutex{ muquarity };
			_quarity = quarity; 
		}

		void setLevel(const float level) 
		{ 
			std::scoped_lock mutex{ mulevel };
			_level = level; 
		}

		const float getGain() const { return _gain; }

		const float getQuarity() const { return _quarity; }

		const float getLevel() const { return _level; }
	};

	class DistortionHard : public Effector
	{
		float _gain = 0.0;
		float _level = 0.0;

		std::mutex mugain;
		std::mutex mulevel;

	public:
		virtual ~DistortionHard() {}

		void apply(asio::SampleType* input, asio::SampleType* output, const long length) override
		{
			std::scoped_lock mutex{ mugain, mulevel };

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{	// ハードクリッピング
				input[i] *= _gain;
			}

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				// 符号だけを取る
				output[i] = (input[i] > 0.0) - (input[i] < 0.0);

				// [-1, +1] の範囲を超えるなら符号を代入する
				output[i] = (output[i] * input[i]) > 1.0 ? output[i] : input[i];
			}

#pragma omp simd
			for (long i = 0; i < length; ++i)
			{
				output[i] *= _level;
			}
		}

		void setGain(const float gain)
		{
			std::scoped_lock mutex(mugain);
			_gain = gain;
		}

		void setLevel(const float level)
		{
			std::scoped_lock mutex(mulevel);
			_level = level;
		}

		const float getGain() const { return _gain; }

		const float getLevel() const { return _level; }
	};
}