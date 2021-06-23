﻿#pragma once
#include <mutex>
#include "TinyASIO/TinyASIO.hpp"
#include "Compressor.hpp"

class MainController : public asio::ControllerBase
{
	static effector::Compressor comp;

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
	}

	void draw(const Vec2& pos, const Font& font)
	{
		comp.draw(pos, font);
	}
};

effector::Compressor MainController::comp;