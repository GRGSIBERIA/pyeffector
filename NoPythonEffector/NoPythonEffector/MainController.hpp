#pragma once
#include <mutex>
#include "TinyASIO/TinyASIO.hpp"

class MainController : public asio::ControllerBase
{
	static void* inputptr[2];
	static void* outputLptr[2];
	static void* outputRptr[2];

	static void BufferSwitch(long index, long)
	{
		asio::SampleType* inptr = (asio::SampleType*)inputptr[index];
		asio::SampleType* outLptr = (asio::SampleType*)outputLptr[index];
		asio::SampleType* outRptr = (asio::SampleType*)outputRptr[index];

		const long size = bufferLength * sizeof(asio::SampleType);
		//memcpy_s(outLptr, size, inptr, size);
		//memcpy_s(outRptr, size, inptr, size);
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

		for (int i = 0; i < 2; ++i)
		{
			inputptr[i] = bufferManager->Input(0).GetBuffer(i);
			outputLptr[i] = bufferManager->Output(0).GetBuffer(i);
			outputRptr[i] = bufferManager->Output(1).GetBuffer(i);
		}
	}
};

void* MainController::inputptr[] = { nullptr, nullptr };
void* MainController::outputLptr[] = { nullptr, nullptr };
void* MainController::outputRptr[] = { nullptr, nullptr };