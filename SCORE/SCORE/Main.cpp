
# include <Siv3D.hpp> // OpenSiv3D v0.4.3

struct ScoreData
{
	double score = 0.0;
	double highscore = 0.0;
};

using App = SceneManager<String, ScoreData>;

// タイトルシーン
class Title : public App::Scene
{
private:

	Texture textureBackGround1;
public:

	// コンストラクタ（必ず実装）
	Title(const InitData& init)
		: IScene(init)

		, textureBackGround1(U"Asset/BackGround1.jpg")

	{

	}

	// 更新関数
	void update() override
	{
		// エンターで
		if (KeyEnter.down())
		{
			// ゲームシーンに遷移
			changeScene(U"Game");
		}
	}

	// 描画関数 (const 修飾)
	void draw() const override
	{
		//背景の描写
		textureBackGround1(0, 0, 1280, 640).draw(0, 0, ColorF(1.0));

		Scene::SetBackground(ColorF(0.3, 0.4, 0.5));

		FontAsset(U"TitleFont")(U"SCORE").drawAt(640, 150);

		// エンターキーを押せ
		FontAsset(U"EnterFont")(U"Press Enter").draw(570, 350, ColorF(1.0, Periodic::Sine0_1(2s)));


		// 現在のスコアを表示
		FontAsset(U"ScoreFont")(U"HighScore: {}"_fmt(getData().highscore)).draw(530, 500);


	}
};

// 敵の位置をランダムに作成する関数
Vec2 RandomEnemy()
{
	return RandomVec2(1350, { 10, 630 });
}

// ゲームシーン
class Game : public App::Scene
{
private:

	Texture texturePlayer;
	Texture textureRock;
	Texture textureEnemy;
	Texture textureEnemy2;
	Texture textureBackGround1;
	Texture textureBackGround2;
	Audio audio;
	Font font;

public:

	//自機の情報
	Vec2 PlayerPos = Vec2(90, 320);

	//敵機の情報
	Array<Vec2> Rocks = { RandomEnemy() };
	Array<Vec2> Enemies = { RandomEnemy() };
	Array<Vec2> Enemies2 = { RandomEnemy() };

	//背景のスクロール1
	int Scroll1 = 0;
	//背景のスクロール1
	int Scroll2 = 1280;

	//スピード
	double Speed = 5;

	//敵のスポーン感覚
	double EnemySpawn = 0.0;

	double RockSpawns = 1.5;

	double EnemySpawns = 2.8;

	double Enemy2Spawns = 3.2;

	//音の大きさ
	double volume = 1.0;

	// コンストラクタ（必ず実装）
	Game(const InitData& init)
		: IScene(init)


		//スコアの文字の大きさ
		, font(30)

		//素材の宣言
		, texturePlayer(U"Asset/Player.png")
		, textureRock(U"Asset/Rock.png")
		, textureEnemy(U"Asset/Enemy.png")
		, textureEnemy2(U"Asset/Enemy2.png")
		, textureBackGround1(U"Asset/BackGround1.jpg")
		, textureBackGround2(U"Asset/BackGround2.jpg")
		, audio(U"Asset/Addition.mp3", Arg::loop = true)


	{
	}

	void update() override
	{

		// ゲームオーバー判定
		bool GameOver = false;

		//スコアに時間を代入
		EnemySpawn += Scene::DeltaTime();
		getData().score += ceil(Scene::DeltaTime() * Speed * 100);

		//敵の移動
		for (auto& Rock : Rocks)
		{
			Rock.x -= Speed;
		}

		for (auto& Enemy : Enemies)
		{
			Enemy.x -= Speed;
		}

		for (auto& Enemy2 : Enemies2)
		{
			Enemy2.x -= Speed;
		}


		//敵の発生
		while (EnemySpawn > RockSpawns)
		{
			RockSpawns += 1.0 - (Speed / 35);
			Rocks << RandomEnemy();
		}

		while (EnemySpawn > EnemySpawns)
		{
			EnemySpawns += 1.0 - (Speed / 35);
			Enemies << RandomEnemy();
		}

		while (EnemySpawn > Enemy2Spawns)
		{
			Enemy2Spawns += 1.0 - (Speed / 35);
			Enemies2 << RandomEnemy();
		}

		//背景のスクロール1
		if (Scroll1 <= -1280)
		{
			Scroll1 = 1280 - Speed;
		}
		else
		{
			Scroll1 -= Speed;
		}

		//背景のスクロール2
		if (Scroll2 <= -1280)
		{
			Scroll2 = 1280 - Speed;
			if (Speed <= 20)
			{
				Speed++;
			}
		}
		else
		{
			Scroll2 -= 1 * Speed;
		}

		//上下キーで移動
		if (KeyUp.pressed() && (PlayerPos.y > 0))
		{
			PlayerPos.y -= 10;
		}
		else if (PlayerPos.y == 0)
		{
			PlayerPos.y = 0;
		}

		if (KeyDown.pressed() && (PlayerPos.y < 640))
		{
			PlayerPos.y += 10;
		}
		else if (PlayerPos.y == 630)
		{
			PlayerPos.y = 630;
		}

		//ゲームオーバー処理
		for (const auto& Rock : Rocks)
		{
			if (Rock.distanceFrom(PlayerPos) <= 70)
			{
				GameOver = true;
				break;
			}
		}

		for (const auto& Enemy : Enemies)
		{
			if (Enemy.distanceFrom(PlayerPos) <= 70)
			{
				GameOver = true;
				break;
			}
		}

		for (const auto& Enemy2 : Enemies2)
		{
			if (Enemy2.distanceFrom(PlayerPos) <= 70)
			{
				GameOver = true;
				break;
			}
		}

		//ゲームオーバーになったら
		if (GameOver)
		{
			PlayerPos = Vec2(90, 340);
			Rocks.clear();
			Speed = 5;
			Scroll1 = 0;
			Scroll2 = 1280;
			getData().highscore = Max(getData().highscore, getData().score);
			// ゲームシーンに遷移
			changeScene(U"Result");
		}
		//音量設定
if (SimpleGUI::Slider(volume, Vec2(1150, 20)))
{
	audio.setVolume(volume);
}

		//オーディオを再生
		audio.play();

	}

	void draw() const override
	{

		//背景の描写
		textureBackGround1(0, 0, 1280, 640).draw(Scroll1, 0, ColorF(1.0, 0.5));
		textureBackGround2(0, 0, 1280, 640).draw(Scroll2, 0, ColorF(1.0, 0.5));

		//自機の表示
		texturePlayer.scaled(0.5).drawAt(PlayerPos);

		//敵機の表示
		for (const auto& Rock : Rocks)
		{
			textureRock
				.scaled(0.5)
				.rotated(Scene::Time() * Speed)
				.drawAt(Rock);
		}

		for (const auto& Enemy : Enemies)
		{
			textureEnemy
				.scaled(0.4)
				.drawAt(Enemy);
		}

		for (const auto& Enemy2 : Enemies2)
		{
			textureEnemy2
				.scaled(0.4)
				.drawAt(Enemy2);
		}

		// 現在のスコアを表示
		FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(640, 580);


	}
};

// リザルトシーン
class Result : public App::Scene
{
private:

	Texture textureBackGround1;
public:

	// コンストラクタ（必ず実装）
	Result(const InitData& init)
		: IScene(init)

		, textureBackGround1(U"Asset/BackGround1.jpg")
	{

	}

	// 更新関数
	void update() override
	{
		// エンターで
		if (KeyEnter.down())
		{
			// ゲームシーンに遷移
			changeScene(U"Title");

			getData().score = 0.0;
		}
	}

	// 描画関数 (const 修飾)
	void draw() const override
	{
		//背景の描写
		textureBackGround1(0, 0, 1280, 640).draw(0, 0, ColorF(1.0));

		Scene::SetBackground(ColorF(0.3, 0.4, 0.5));

		FontAsset(U"TitleFont")(U"Result").drawAt(640, 150);


		if (450000 < getData().score)
		{
			FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(320, 440);
			FontAsset(U"ResultFont")(U"人類は宇宙からの脅威に打ち勝った").drawAt(640, 500);
		}
		else if (300000 < getData().score)
		{
			FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(320, 440);
			FontAsset(U"ResultFont")(U"天王星にたどり着いた").drawAt(640, 500);
		}
		else if (150000 < getData().score)
		{
			FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(320, 440);
			FontAsset(U"ResultFont")(U"土星にたどり着いた").drawAt(640, 500);
		}
		else if (81600 < getData().score)
		{
			FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(320, 440);
			FontAsset(U"ResultFont")(U"木星にたどり着いた").drawAt(640, 500);
		}
		else if (27000 < getData().score)
		{
			FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(320, 440);
			FontAsset(U"ResultFont")(U"火星にたどり着いた").drawAt(640, 500);
		}
		else if (26000 < getData().score)
		{
			FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(320, 440);
			FontAsset(U"ResultFont")(U"金星にたどり着いた").drawAt(640, 500);
		}
		else if (15210 < getData().score)
		{
			FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(320, 440);
			FontAsset(U"ResultFont")(U"太陽にたどり着いた").drawAt(640, 500);
		}
		else if (405 <= getData().score)
		{
			FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(320, 440);
			FontAsset(U"ResultFont")(U"月にたどり着いた").drawAt(640, 500);
		}
		else
		{
			FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(320, 440);
			FontAsset(U"ResultFont")(U"人類は無抵抗のまま死んだ").drawAt(640, 500);
		}


		// エンターキーを押せ
		FontAsset(U"EnterFont")(U"Press Enter").draw(570, 350, ColorF(1.0, Periodic::Sine0_1(2s)));
	}
};

void Main()
{

	FontAsset::Register(U"TitleFont", 100, Typeface::Heavy);
	FontAsset::Register(U"ScoreFont", 30, Typeface::Bold);
	FontAsset::Register(U"ResultFont", 40, Typeface::Heavy);
	FontAsset::Register(U"EnterFont", 20, Typeface::Bold);

	//画面の大きさ
	Window::Resize(1280, 640);

	App manager;
	manager
		.add<Title>(U"Title")
		.add<Game>(U"Game")
		.add<Result>(U"Result");

	while (System::Update())
	{
		if (!manager.update())
		{
			break;
		}
	}
}

