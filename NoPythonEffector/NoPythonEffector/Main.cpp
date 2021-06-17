
#include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include "Pulldown.hpp"
#include "TinyASIO/TinyASIO.hpp"

void Main()
{
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	const Font font(60);
	FontAsset::Register(U"UIFont", 14);

	const auto drivers = asio::Registry::GetAsioDriverPathes();
	Array<String> converted_drivers;
	for(size_t i = 0; i < drivers.Count(); ++i)
	{
		converted_drivers.emplace_back(Unicode::FromWString(drivers.Items(i).driverName));
	}

	Pulldown device(converted_drivers, U"UIFont", Point(40, 40));

	while (System::Update())
	{
		device.update();
		device.draw();
	}
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
