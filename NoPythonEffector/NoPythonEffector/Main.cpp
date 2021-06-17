
#include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include "Pulldown.hpp"
#include "TinyASIO/TinyASIO.hpp"
#include "MainController.hpp"

void Main()
{
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	const Font font(60);
	FontAsset::Register(U"UIFont", 18);
	INIData data(U"./config.ini");
	Array<String> converted_drivers;
	MainController* controller = nullptr;

	const auto drivers = asio::Registry::GetAsioDriverPathes();

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
		data.save(U"./config.ini");
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
	}
	
	/**
	 * 更新処理
	 */
	while (System::Update())
	{
		const Vec2 pad = { 0, 4 };

		auto btnreg = device.getRect();
		auto query_reg = SimpleGUI::ButtonRegion(U"Query", btnreg.bl() + pad);

		if (query_reg.leftClicked())
		{
			if (controller != nullptr)
			{
				delete controller;
			}
			controller = new MainController(drivers.Items(device.getIndex()).driverName);
			
			controller->SampleRate();
			controller->InputLatency();
			controller->OutputLatency();
			controller->OutputCount();
			controller->InputCount();
		}

		device.update();
		device.draw();
	}

	/**
	 * 終了処理、最後に記憶した情報を設定ファイルに記録する
	 */
	delete controller;
	data.writeGlobal(U"Driver Name", device.getItem());
	data.save(U"./config.ini");
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
