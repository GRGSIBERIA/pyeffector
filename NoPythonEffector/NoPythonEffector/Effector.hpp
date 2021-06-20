#pragma once
#include <mutex>
#include "TinyASIO/TinyASIO.hpp"

namespace effector
{
	class Effector
	{
	public:
		virtual void apply(asio::SampleType* input, asio::SampleType* output, const long length) = 0;
	};

	class Distortion : public Effector
	{
		float _gain = 0.0;
		float _level = 0.0;

		std::mutex mugain;
		std::mutex mulevel;

	public:
		Distortion(const float gain, const float level)
			: Effector(), _gain(gain), _level(level)
		{

		}

		void apply(asio::SampleType* input, asio::SampleType* output, const long length) override
		{
			std::scoped_lock mutex{ mugain, mulevel };

#pragma omp simd
			for (size_t i = 0; i < length; ++i)
			{	// ハードクリッピング
				input[i] *= _gain;
			}

#pragma omp simd
			for (size_t i = 0; i < length; ++i)
			{
				// 符号だけを取る
				output[i] = (input[i] > 0.0) - (input[i] < 0.0);

				// [-1, +1] の範囲を超えるなら符号を代入する
				output[i] = (output[i] * input[i]) > 1.0 ? output[i] : input[i];
			}

#pragma omp simd
			for (size_t i = 0; i < length; ++i)
			{
				output[i] += _level;
			}
		}

		void setGain(const float gain)
		{
			std::scoped_lock mutex(mugain);
			_gain = gain;
		}

		const float getGain() const
		{
			return _gain;
		}

		void setLevel(const float level)
		{
			std::scoped_lock mutex(mulevel);
			_level = level;
		}

		const float setLevel() const
		{
			return _level;
		}
	};
}