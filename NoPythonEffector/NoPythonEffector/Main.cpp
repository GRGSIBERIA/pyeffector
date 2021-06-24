
#include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include "Pulldown.hpp"
#include "TinyASIO/TinyASIO.hpp"
#include "MainController.hpp"

#include "Compressor.hpp"

void SaveSuggests(INIData& data, const size_t input, const size_t outputL, const size_t outputR)
{
	data.writeGlobal(U"Input Channel", input);
	data.writeGlobal(U"Output L Channel", outputL);
	data.writeGlobal(U"Output R Channel", outputR);
}

void Main()
{
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	const Font font(18);
	FontAsset::Register(U"UIFont", 18);
	const String inipath = U"./config.ini";
	INIData data(inipath);
	Array<String> converted_drivers;
	MainController* controller = nullptr;
	Pulldown input_ch;
	Pulldown outputL_ch;
	Pulldown outputR_ch;

	const auto drivers = asio::Registry::GetAsioDriverPathes();

	long input_latency = 0;
	long output_latency = 0;
	long input_count = 0;
	long output_count = 0;
	long sample_rate = 0;
	long buffer_length = 0;
	size_t suggest_input = 0;
	size_t suggest_outputL = 0;
	size_t suggest_outputR = 0;

	/**
	 * デバイス一覧を取得してプルダウンに反映させる
	 */
	for (size_t i = 0; i < drivers.Count(); ++i)
	{
		converted_drivers.emplace_back(Unicode::FromWString(drivers.Items(i).driverName));
	}
	Pulldown device(converted_drivers, U"UIFont", Point(40, 40));

	/**
	 * 設定ファイルを読み込んでプルダウンに反映させる
	 */
	if (data.isEmpty())
	{
		data.writeGlobal(U"Driver Name", *converted_drivers.begin());
		SaveSuggests(data, suggest_input, suggest_outputL, suggest_outputR);
		data.save(inipath);
	}
	else
	{
		const auto driver_name = data.getGlobalValue(U"Driver Name");
		for (size_t i = 0; i < drivers.Count(); ++i)
		{
			if (driver_name == Unicode::FromWString(drivers.Items(i).driverName))
			{
				device.setIndex(i);
			}
		}
		if (data.hasGlobalValue(U"Input Channel"))
			suggest_input = Parse<size_t>(data.getGlobalValue(U"Input Channel"));
		else
			data.writeGlobal(U"Input Channel", suggest_input);

		if (data.hasGlobalValue(U"Output L Channel"))
			suggest_outputL = Parse<size_t>(data.getGlobalValue(U"Output L Channel"));
		else
			data.writeGlobal(U"Output L Channel", suggest_outputL);

		if (data.hasGlobalValue(U"Output R Channel"))
			suggest_outputR = Parse<size_t>(data.getGlobalValue(U"Output R Channel"));
		else
			data.writeGlobal(U"Output R Channel", suggest_outputR);

		data.save(inipath);
	}

	controller->load(data, inipath);
	
	/**
	 * 更新処理
	 */
	bool is_start = false;
	bool is_playing = true;
	bool past_playing = true;	// 前フレームの状態

	Pulldown effect_selector({ U"None", U"Distortion", U"Overdrive" }, U"UIFont");

	Graphics::SetTargetFrameRateHz(30);	// 30FPSで動作させる

	while (System::Update())
	{
		if (!is_start)
		{	// Startボタンを押すまでの画面
			const Vec2 pad = { 0, 4 };

			auto btnreg = device.getRect();
			auto register_reg = SimpleGUI::ButtonRegion(U"Regist", btnreg.bl() + pad);
			
			if (SimpleGUI::Button(U"Regist", btnreg.bl() + pad))
			{
				if (controller != nullptr)
				{
					delete controller;
				}
				controller = new MainController(drivers.Items(device.getIndex()).driverName);

				sample_rate = controller->SampleRate();
				input_latency = controller->InputLatency();
				output_latency = controller->OutputLatency();
				output_count = controller->OutputCount();
				input_count = controller->InputCount();
				buffer_length = controller->BufferLength();

				const auto input_entries = controller->GetInputEntry();
				const auto output_entries = controller->GetOutputEntry();

				Array<String> input_string;
				for (size_t i = 0; i < input_entries.size(); ++i)
					input_string.push_back(Unicode::Widen(input_entries[i]));

				Array<String> output_string;
				for (size_t i = 0; i < output_entries.size(); ++i)
					output_string.push_back(Unicode::Widen(output_entries[i]));

				input_ch = Pulldown(input_string, U"UIFont");
				outputL_ch = Pulldown(output_string, U"UIFont");
				outputR_ch = Pulldown(output_string, U"UIFont");

				input_ch.setIndex(suggest_input);
				outputL_ch.setIndex(suggest_outputL);
				outputR_ch.setIndex(suggest_outputR);
			}

			const auto black = Palette::Black;
			const auto ilreg = font(U"Input Latency: {} us"_fmt(input_latency)).draw(register_reg.bl() + pad, black);
			const auto olreg = font(U"Output Latency: {} us"_fmt(output_latency)).draw(ilreg.bl() + pad, black);
			const auto icreg = font(U"Input Channels: {} ch"_fmt(input_count)).draw(olreg.bl() + pad, black);
			const auto ocreg = font(U"Output Channels: {} ch"_fmt(output_count)).draw(icreg.bl() + pad, black);
			const auto srreg = font(U"Sample Rate: {} Hz"_fmt(sample_rate)).draw(ocreg.bl() + pad, black);
			const auto blreg = font(U"Buffer Length: {} Samples"_fmt(buffer_length)).draw(srreg.bl() + pad, black);
			const auto inchreg = font(U"Input Channel: ").draw(blreg.bl() + pad, black);
			const auto oLreg = font(U"Output L Channel: ").draw(inchreg.bl() + pad, black);
			const auto oRreg = font(U"Output R Channel: ").draw(oLreg.bl() + pad, black);

			const Vec2 chpad(4, 0);
			input_ch.setPos(inchreg.tr().movedBy(chpad).asPoint());
			outputL_ch.setPos(oLreg.tr().movedBy(chpad).asPoint());
			outputR_ch.setPos(oRreg.tr().movedBy(chpad).asPoint());

			const auto savereg = SimpleGUI::ButtonRegion(U"Save", oRreg.bl() + pad);
			if (SimpleGUI::Button(U"Save", oRreg.bl() + pad))
			{
				data.writeGlobal(U"Driver Name", device.getItem());
				suggest_input = input_ch.getIndex();
				suggest_outputL = outputL_ch.getIndex();
				suggest_outputR = outputR_ch.getIndex();
				SaveSuggests(data, suggest_input, suggest_outputL, suggest_outputR);
				data.save(inipath);
			}

			const auto startreg = SimpleGUI::ButtonRegion(U"Start", savereg.tr() + chpad);
			if (SimpleGUI::Button(U"Start", savereg.tr() + chpad))
			{
				controller->Initialize(suggest_input, suggest_outputL, suggest_outputR);
				is_start = true;
				is_playing = true;
				controller->Start();
			}

			input_ch.update();
			outputL_ch.update();
			outputR_ch.update();
			device.update();

			// UI座標が左上ほど描画の優先順位が高いから、低い順から描画する
			outputR_ch.draw();
			outputL_ch.draw();
			input_ch.draw();
			device.draw();
		}
		else
		{	// Startボタンが押されてからの画面
			const Vec2 pad{ 0.0, 4.0 };

			const auto togglereg = SimpleGUI::CheckBoxRegion(U"Playing", { 4, 4 });
			SimpleGUI::CheckBox(is_playing, U"Playing", { 4, 4 });

			effect_selector.setPos(togglereg.tr().asPoint() + Point{ 4, 0 });
			effect_selector.update();
			
			if (is_playing && !past_playing)
			{
				controller->Start();
			}
			else if (!is_playing && past_playing)
			{
				controller->Stop();
			}

			controller->draw(togglereg.bl() + pad, font, effect_selector.getIndex());

			effect_selector.draw();	// プルダウンで垂れるので最後に描画する
			past_playing = is_playing;
		}
	}

	/**
	 * 終了処理、最後に記憶した情報を設定ファイルに記録する
	 */
	controller->save(data, inipath);
	delete controller;

	data.writeGlobal(U"Driver Name", device.getItem());
	SaveSuggests(data, suggest_input, suggest_outputL, suggest_outputR);
	data.save(inipath);
}

//
// = アドバイス =
// Debug ビルドではプログラムの最適化がオフになります。
// 実行速度が遅いと感じた場合は Release ビルドを試しましょう。
// アプリをリリースするときにも、Release ビルドにするのを忘れないように！
//
// 思ったように動作しない場合は「デバッグの開始」でプログラムを実行すると、
// 出力ウィンドウに詳細なログが表示されるので、エラーの原因を見つけやすくなります。
//
// = お役立ちリンク =
//
// OpenSiv3D リファレンス
// https://siv3d.github.io/ja-jp/
//
// チュートリアル
// https://siv3d.github.io/ja-jp/tutorial/basic/
//
// よくある間違い
// https://siv3d.github.io/ja-jp/articles/mistakes/
//
// サポートについて
// https://siv3d.github.io/ja-jp/support/support/
//
// Siv3D ユーザコミュニティ Slack への参加
// https://siv3d.github.io/ja-jp/community/community/
//
// 新機能の提案やバグの報告
// https://github.com/Siv3D/OpenSiv3D/issues
//
