#pragma once
#include <mutex>
#include "TinyASIO/TinyASIO.hpp"

#include "Compressor.hpp"
#include "DistortionHard.hpp"
#include "DistortionSoft.hpp"

class MainController : public asio::ControllerBase
{
	static effector::Compressor comp;
	static effector::DistortionHard hard;
	static effector::DistortionSoft soft;
	static effector::Effector* applyer;

	static std::mutex muapplyer;

	static asio::SampleType* worker;

	static void BufferSwitch(long index, long)
	{
		asio::SampleType* inptr = (asio::SampleType*)bufferManager->Input(0).GetBuffer(index);
		asio::SampleType* outLptr = (asio::SampleType*)bufferManager->Output(0).GetBuffer(index);

		const size_t bufsize = bufferLength * sizeof(asio::SampleType);
		if (comp.enabled())
		{
			comp.apply(inptr, worker, bufferLength);

			std::scoped_lock mutex{ muapplyer };
			if (applyer != nullptr)
				applyer->apply(worker, outLptr, bufferLength);
		}
		else
		{
			memcpy_s(outLptr, bufsize, inptr, bufsize);
		}
	}

public:
	MainController(const std::wstring& name)
		: ControllerBase(name)
	{

	}

	virtual ~MainController() 
	{
		if (worker != nullptr)
			free(worker);
	}

	const long InputCount() const
	{
		return channelManager->NumberOfInputs();
	}

	const long OutputCount() const
	{
		return channelManager->NumberOfOutputs();
	}

	std::vector<std::string> GetInputEntry()
	{
		std::vector<std::string> result;
		for (size_t i = 0; i < channelManager->NumberOfInputs(); ++i)
			result.push_back(channelManager->Inputs(i).name);
		return result;
	}

	std::vector<std::string> GetOutputEntry()
	{
		std::vector<std::string> result;
		for (size_t i = 0; i < channelManager->NumberOfOutputs(); ++i)
			result.push_back(channelManager->Outputs(i).name);
		return result;
	}

	void Initialize(const size_t inputch, const size_t outputLch, const size_t outputRch)
	{
		CreateBuffer({ channelManager->Inputs(inputch), channelManager->Outputs(outputLch), channelManager->Outputs(outputRch) }, &BufferSwitch);
		worker = (asio::SampleType*)malloc(bufferLength * sizeof(asio::SampleType));
	}

	void load(INIData& data, const String& path)
	{
		comp.load(data, path);
		hard.load(data, path);
		soft.load(data, path);
	}

	void draw(const Vec2& pos, const Font& font, const size_t index)
	{
		const Vec2 pad{ 0, 8 };
		const auto compreg = comp.draw(pos, font);

		switch (index)
		{
		case 0:
			break;
		case 1:		// Distortion
			const auto hardreg = hard.draw(compreg.bl() + pad, font);
			break;
		case 2:		// Overdrive
			const auto softreg = soft.draw(compreg.bl() + pad, font);
			break;
		}

		// 描画している時間が長いので、別のところで変数をロックする
		std::scoped_lock mutex{ muapplyer };
		switch (index)
		{
		case 0:
			applyer = nullptr;
			break;
		case 1:
			applyer = &hard;
			break;
		case 2:
			applyer = &soft;
			break;
		}
	}

	void save(INIData& data, const String& path)
	{
		comp.save(data, path);
		hard.save(data, path);
		soft.save(data, path);
	}
};

effector::Compressor MainController::comp;
effector::DistortionHard MainController::hard;
effector::DistortionSoft MainController::soft;
effector::Effector* MainController::applyer = nullptr;
std::mutex MainController::muapplyer;
asio::SampleType* MainController::worker = nullptr;