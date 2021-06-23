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

	static void BufferSwitch(long index, long)
	{
		asio::SampleType* inptr = (asio::SampleType*)bufferManager->Input(0).GetBuffer(index);
		asio::SampleType* outLptr = (asio::SampleType*)bufferManager->Output(0).GetBuffer(index);

		comp.apply(inptr, outLptr, bufferLength);
		
		for (size_t i = 0; i < bufferLength; ++i)
			outLptr[i] = inptr[i];
	}

public:
	MainController(const std::wstring& name)
		: ControllerBase(name)
	{

	}

	virtual ~MainController() 
	{
		
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
	}

	void load(INIData& data, const String& path)
	{
		comp.load(data, path);
		hard.load(data, path);
		soft.load(data, path);
	}

	void draw(const Vec2& pos, const Font& font)
	{
		const Vec2 pad{ 0, 8 };
		const auto compreg = comp.draw(pos, font);
		const auto softreg = soft.draw(compreg.bl() + pad, font);
		const auto hardreg = hard.draw(softreg.bl() + pad, font);
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