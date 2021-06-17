#pragma once
#include "TinyASIO/TinyASIO.hpp"

class MainController : public asio::ControllerBase
{
	static asio::InputBuffer input;
	static asio::OutputBuffer output;

	void BufferSwitch(long index, long)
	{

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
};
