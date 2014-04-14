/******************************************
*	��Ϸ���ƣ��������
*	���뻷����vc6.0 / vc2010 + EasyX_v20130322(beta)
*	Maker��	  xiongfj(1013534648@qq.com)
*	����޸ģ�2013-3-28
*******************************************/

#include "all.h"
#include "welcome.h"
#include <Mmsystem.h>		// ʹ�øü�ʱ�� timeGetTime()
#pragma comment ( lib, "Winmm.lib" )


/**************/

void Init();
void Control( bool& );				// �������ǵĿ���
void Dispatch_role( int );			// �������ѡȡ�Ľ�ɫ
void Dispatch_emeny();				// ����ÿ�ε�ͼ�ĵ���
void Dispatch_boss();				// ���� boss
void Show_Information();			// ��ʾ��ҵ��˸�����Ϣ
void Deal_Information();			// ������Ϣ
void Check_boss( bool& );			// ��� boss �Ƿ�������� �� ���������
void Go_Next_Lev();					// ���Ž�����һ�ض���
void Lift_up_down();				// �������ݱ�Ե�˶�Ч��

// ȫ�ֺ���ָ��
void ( Role::*f[2] )(int,int);
void ( BASE_CLASS::*mf[EMENY_NUM] )( int,const int& );
void ( BASE_BOSS::*bf[BOSS_NUM] )( int id, const int& player );


void main()
{
	initgraph( 500, 450 );

	mciSendString( "open ./res/Sounds/ѡ��.mp3 alias xuan", NULL, 0, NULL );
	mciSendString( "play xuan repeat", NULL, 0, NULL );

	srand( timeGetTime() );
	bool game = true;
	DWORD game_speed_t1 = timeGetTime(), game_speed_t2;

	L::pn->pP = NULL;		// ���ڼ���ȡ�����Ƿ�β
	L::p_b->p_B = NULL;

	Init();
	int role = Chose_role();	// ������ҵ�ѡ����Ϣ
	Dispatch_role( role );		// �������ѡ��Ľ�ɫ new ��Ӧ�������࣬��ȷ����������������� Init() ֮�󣡣���
	BeginBatchDraw();
	Go_Next_Lev();

	while ( game )
	{
		Sleep( 1 );

		game_speed_t2 = timeGetTime();
		Control( game );

		if ( game_speed_t2 - game_speed_t1 > 33 )
		{
			game_speed_t1 = game_speed_t2;

			if ( !Map.Set_Map() )
			{
				Map.Move_map();
				Lift_up_down();
				Show_Information();

				if ( WEAPON::out )
				{
					WEAPON::x = WEAPON::X + Map.x[Map.map_id];
					putimage(WEAPON::x, WEAPON::y, &WEAPON::weapon[WEAPON::weapon_kind][1], SRCAND );
					putimage(WEAPON::x, WEAPON::y, &WEAPON::weapon[WEAPON::weapon_kind][0], SRCINVERT );

					if ( ++WEAPON::time > 400 )
						WEAPON::out = false;
				}
				if ( FOOD::out )
				{
					FOOD::x = FOOD::X + Map.x[Map.map_id];
					putimage( FOOD::x, FOOD::y, &FOOD::img[1], SRCAND );
					putimage( FOOD::x, FOOD::y, &FOOD::img[0], SRCINVERT );
					if ( ++FOOD::time > 400 )
						FOOD::out = false;
				}

				if ( !OTHER::emeny )
					for ( int i = 0; i < EMENY::emeny_num; i++ )
						if ( Emeny[i].appear[ Map.part[Map.map_id] ] )
							( Emeny[i].Mp->*mf[i] )( i, PLAYER::player_num );

				if ( !OTHER::boss )
					for ( int i = 0; i < BOSS::Boss_num; i++ )
						if ( Boss[i].appear )
							( Boss[i].Bp->*bf[i] )( i, PLAYER::player_num );

				for ( int i = 0; i < PLAYER::player_num; i++ )
					if ( Player[i].appear )
						( Player[i].Rp->*f[i] )( Player[i].dir, i + 1 );

				if ( OTHER::emeny )		// ������ס��ҵ�Ч��
					for ( int i = 0; i < EMENY::emeny_num; i++ )
						if ( Emeny[i].appear[ Map.part[Map.map_id] ] )
							( Emeny[i].Mp->*mf[i] )( i, PLAYER::player_num );

				if ( OTHER::boss )
					for ( int i = 0; i < BOSS::Boss_num; i++ )
						if ( Boss[i].appear )
							( Boss[i].Bp->*bf[i] )( i, PLAYER::player_num );

				/***** ���һ�� boss ******/

				if ( Map.map_id == 4 && Map.part[4] == 7 )
				{
					L::pn->PutImg();
					L::p_b->Kick_down();
				}
			}

			/****** ��ͼ��ÿ���账������� *******/

			if ( !Map.Move_map(true) )
			{
				Dispatch_emeny();
				Dispatch_boss();
				Check_boss( game );
			}
		}
		FlushBatchDraw();
	}
	closegraph();
}


// ��ʼ��һЩ��Ҫ����

void Init()
{
	int i, j, x;		// ����ѭ������

	/***** Ч��ͼ ******/

	loadimage( &PLAYER::light[0], "./res/Images/00.jpg", 32, 32 );
	loadimage( &PLAYER::light[1], "./res/Images/01.jpg", 32, 32 );

	IMAGE img;
	loadimage( &img, "./res/Images/effect.jpg", 600, 1000 );
	SetWorkingImage( &img );

	for ( i = 0; i < 2; i++ )
	{
		for ( x = 0; x < 6; x++ )
		{
			getimage( &PLAYER::blood[1][x][i], x * 80, i * 80, 80, 80 );	// 1
			getimage( &PLAYER::blood[0][x][i], x * 80, 160 + i * 80, 80, 80 );

			if ( x < 3 )// ����Ϊ��С�������ڡ�����(0-1-2)
				getimage( &WEAPON::weapon[x][i], i * 48, 576 + x * 48, 48, 48 );
		}
		getimage( &PLAYER::light[i], i * 32, 400, 32, 32 );
		getimage( &OTHER::lift_up_down[1], 0, 432, 344, 144 );

		getimage( &FOOD::img[i], i * 80, 720, 80, 80 );		// ����
	}
	SetWorkingImage();

	/*******/

	IMAGE eh;
	loadimage( &eh, "./res/Images/other.jpg" );
	SetWorkingImage( &eh );

	for ( i = 0; i < 6; i++ )
		getimage( &EMENY::head[i], i * 20, 0, 20, 20 );
	for ( i = 0; i < 2; i++ )
		getimage( &EMENY::papaw[i], i * 70, 20, 70, 40 );
	for ( i = 0; i < 3; i++ )
		getimage( &PLAYER::head[i], i * 30, 60, 30, 35 );
	for ( i = 0; i < 5; i++ )	// boss ͷ�����
		getimage( &BOSS::head[i], i * 30, 95, 30, 30 );
	getimage( &OTHER::lift_up_down[0], 0, 125, 20, 20 );

	SetWorkingImage();

	/***** ��� **********/

	for ( x = 0; x < 2; x++ )
	{
		Player[x].role_kind			= 0;
		f[x]						= &Role::Stand;
		Player[x].dir				= LEFT;
		Player[x].be_kick_counter	= 0;
		Player[x].catch_emeny		= false;
		Player[x].die				= false;
		Player[x].appear			= true;
		Player[x].lying				= false;
		Player[x].be_kiss			= false;
		Player[x].emeny				= 0;
		Player[x].boss				= 0;
		Player[x].throw_dir			= BACK;
		Player[x].skill				= NONE;
		Player[x].Set_bol( 0x3f );

		Player[x].level				= 0;
		Player[x].hp				= 200;
		Player[x].max_hp			= Player[x].hp;
		Player[x].experience		= 0;
		Player[x].life				= 3;
		Player[x].x					= x * 250;
		Player[x].y					= 0;
	}

	/***** ���� **********/

	Thin_Old_Man::Load();
	Small_Monkey::Load();
	  Big_Savage::Load();
	   Small_Dai::Load();
		 Big_Dai::Load();
			Girl::Load();

	int _x[2] = { 20, 270 };
	int _y[3] = { 375, 400, 425 };
	for ( i = 0; i < EMENY_NUM; i++ )
	{
		int a = rand() % 1+3;
		Emeny[i].phead = &EMENY::head[a];
		switch (a)
		{
			case 0:		Emeny[i].Mp = new Small_Monkey;	break;
			case 1:		Emeny[i].Mp = new Small_Dai;	break;
			case 2:		Emeny[i].Mp = new Big_Dai;		break;
			case 3:		Emeny[i].Mp = new Girl;			break;
			case 4:		Emeny[i].Mp = new Thin_Old_Man;	break;
			case 5:		Emeny[i].Mp = new Big_Savage;	break;

			default:	printf("Dispatch_emeny����\n");	break;
		}

		Emeny[i].x					= _x[i / 3];
		Emeny[i].y					= _y[i % 3];
		Emeny[i].stand				= false;
		Emeny[i].be_catch			= false;
		Emeny[i].attack				= false;
		Emeny[i].lying				= true;		// ��ֹ����δ���־ͱ���ҹ���
		Emeny[i].start_fly			= true;
		Emeny[i].role				= 0;
		Emeny[i].be_kick_counter	= 0;
		Emeny[i].hp					= 0;
		Emeny[i].max_hp				= 0;
		mf[i]						= &BASE_CLASS::Walk;
		Emeny[i].Mp->SetHp(Emeny[i].hp);

		for ( j = 0; j < 8; j++ )
		{
			Emeny[i].appear[j] = false;

			if ( j < 7 )
			{
				for ( int id = 0; id < MAX_MAP; id++ )
				{
					if ( j < 5 )
						Emeny[i].live[id][j] = 4;
					else
						Emeny[i].live[id][j] = 6;
				}
				Emeny[i].live[3][5] = 17;
			}
		}
	}

	/********* BOSS *********/

	int hy[2] = { 375, 415 };

	for ( i = 0; i < BOSS_NUM; i++ )
	{
		Boss[i].lying = true;
		Boss[i].start_fly = true;
		Boss[i].role = 0;
		Boss[i].be_kick_counter = 0;

		Boss[i].die = false;
		Boss[i].appear = false;
		Boss[i].x = 10;
		Boss[i].y = hy[i];
		Boss[i].hp = 300;
		Boss[i].max_hp = 300;
		Boss[i].ph = &BOSS::head[0];

		static int a[3] = { 440, 185, 43 };/////////.......
		static RECT rec = { -40, 185, 440, 280 };
		bf[i] = &BASE_BOSS::Walk;
		Boss[i].Bp = new B_ONE( a, &rec );
	}

	/******* OTHER �ṹ ********/

	int __y[4] = { 160, 255, 350 };
	for ( i = 0; i < 3; i++ )
		OTHER::y[i] = __y[i];	// �����ƶ��ķ������Ͻ�����
}

// ��ҿ���

void Control( bool& game )
{
	/********* ���һ *************************/

		if ( GetAsyncKeyState('A') & 0x8000 && Player[0].bol[0] )
		{
			Player[0].dir = LEFT;
			f[0] = &Role::Walk;
		}

		if ( GetAsyncKeyState('D') & 0x8000 && Player[0].bol[1] )
		{
			Player[0].dir = RIGHT;
			f[0] = &Role::Walk;
		}

		if ( GetAsyncKeyState('W') & 0x8000 && Player[0].bol[2] )	
		{
			if ( GetAsyncKeyState('A') & 0x8000 && Player[0].bol[0] )
				Player[0].dir = L_U;
			else if ( GetAsyncKeyState('D') & 0x8000 && Player[0].bol[1] )
				Player[0].dir = R_U;
			else
				Player[0].dir = UP;
			f[0] = &Role::Walk;
		}

		if ( GetAsyncKeyState('S') & 0x8000 && Player[0].bol[3] )
		{
			if ( GetAsyncKeyState('A') & 0x8000 && Player[0].bol[0] )
				Player[0].dir = L_D;
			else if ( GetAsyncKeyState('D') & 0x8000 && Player[0].bol[1] )
				Player[0].dir = R_D;
			else
				Player[0].dir = DOWN;
			f[0] = &Role::Walk;
		}

		if ( GetAsyncKeyState('J') & 0x8000 && Player[0].bol[4] )	f[0] = &Role::Attack;
		if ( GetAsyncKeyState('K') & 0x8000 && Player[0].bol[5] )	f[0] = &Role::Jump;

		if ( GetAsyncKeyState('K') & 0x8000 && Player[0].bol[5] &&
			 GetAsyncKeyState('J') & 0x8000 && Player[0].bol[4] )
			f[0] = &Role::Big_Blow;

	/********** ��Ҷ� *************************/

		if ( PLAYER::player_num == 2 )
		{
			if ( GetAsyncKeyState(VK_LEFT) & 0x8000 && Player[1].bol[0] )
			{
				Player[1].dir = LEFT;
				f[1] = &Role::Walk;
			}

			if ( GetAsyncKeyState(VK_RIGHT) & 0x8000 && Player[1].bol[1] )
			{
				Player[1].dir = RIGHT;
				f[1] = &Role::Walk;
			}

			if ( GetAsyncKeyState(VK_UP) & 0x8000 && Player[1].bol[2] )
			{
				if ( GetAsyncKeyState(VK_LEFT) & 0x8000 && Player[1].bol[0] )
					Player[1].dir = L_U;
				else if ( GetAsyncKeyState(VK_RIGHT) & 0x8000 && Player[1].bol[1] )
					Player[1].dir = R_U;
				else
					Player[1].dir = UP;
				f[1] = &Role::Walk;
			}

			if ( GetAsyncKeyState(VK_DOWN) & 0x8000 && Player[1].bol[3] )
			{
				if ( GetAsyncKeyState(VK_LEFT) & 0x8000 && Player[1].bol[0] )
					Player[1].dir = L_D;
				else if ( GetAsyncKeyState(VK_RIGHT) & 0x8000 && Player[1].bol[1] )
					Player[1].dir = R_D;
				else
					Player[1].dir = DOWN;
				f[1] = &Role::Walk;
			}

			if ( GetAsyncKeyState(VK_NUMPAD1) & 0x8000 && Player[1].bol[4] )	f[1] = &Role::Attack;
			if ( GetAsyncKeyState(VK_NUMPAD2) & 0x8000 && Player[1].bol[5] )	f[1] = &Role::Jump;

			if ( GetAsyncKeyState(VK_NUMPAD1) & 0x8000 && Player[1].bol[4] &&
				 GetAsyncKeyState(VK_NUMPAD2) & 0x8000 && Player[1].bol[5] )
				 f[1] = &Role::Big_Blow;
		}

	if ( GetAsyncKeyState(VK_ESCAPE) & 0x8000 )
		game = false;
}

// �������ѡȡ�Ľ�ɫ

void Dispatch_role( int role )
{
	if ( role > 22 )
	{
		PLAYER::player_num = 1;
		EMENY::emeny_num = 5;
		BOSS::Boss_num = 1;
		Player[0].ph = &PLAYER::head[role % 10];

		switch ( role % 10 )
		{
			case 0:		Player[0].Rp = new Kady;	Player[0].role_kind = 0;		break;
			case 1:		Player[0].Rp = new Heye;	Player[0].role_kind = 1;		break;
			case 2:		Player[0].Rp = new Jack;	Player[0].role_kind = 2;		break;
			default:	MessageBox( NULL, "����һ�û��������!", "����", MB_OK );	break;
		}
		return;
	}

	if ( role <= 22 )
	{
		PLAYER::player_num = 2;
		EMENY::emeny_num = 6;
		BOSS::Boss_num = 2;
		Player[0].ph = &PLAYER::head[role / 10];
		Player[1].ph = &PLAYER::head[role % 10];

		switch ( role / 10 )
		{
			case 0:		Player[0].Rp = new Kady;	Player[0].role_kind = 0;		break;
			case 1:		Player[0].Rp = new Heye;	Player[0].role_kind = 1;		break;
			case 2:		Player[0].Rp = new Jack;	Player[0].role_kind = 2;		break;
			default:	MessageBox( NULL, "����һ�û��������!", "����", MB_OK );	break;
		}

		switch ( role % 10 )
		{
			case 0:		Player[1].Rp = new Kady;	Player[1].role_kind = 0;		break;
			case 1:		Player[1].Rp = new Heye;	Player[1].role_kind = 1;		break;
			case 2:		Player[1].Rp = new Jack;	Player[1].role_kind = 2;		break;
			default:	MessageBox( NULL, "����һ�û��������!", "����", MB_OK );	break;
		}
	}
}


// ����ÿ�ε�ͼ����ĵ���

void Dispatch_emeny()
{
	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( !Emeny[i].appear[ Map.part[Map.map_id] ] &&
			  Emeny[i].live[Map.map_id][ Map.part[Map.map_id] ] > 0 && Map.part[Map.map_id] < 7 )
		{
			int a = rand() % 6;
			delete Emeny[i].Mp;
			Emeny[i].phead = &EMENY::head[a];

			switch (a)
			{
				case 0:		Emeny[i].Mp = new Small_Monkey;	break;
				case 1:		Emeny[i].Mp = new Small_Dai;	break;
				case 2:		Emeny[i].Mp = new Big_Dai;		break;
				case 3:		Emeny[i].Mp = new Girl;			break;
				case 4:		Emeny[i].Mp = new Thin_Old_Man;	break;
				case 5:		Emeny[i].Mp = new Big_Savage;	break;

				default:	printf("Dispatch_emeny����\n");	break;
			}

			Emeny[i].lying = false;
			Emeny[i].Mp->SetHp(Emeny[i].hp);
			Emeny[i].max_hp = Emeny[i].hp;
			Emeny[i].appear[ Map.part[Map.map_id] ] = true;
		}
	}
}

//

void Dispatch_boss()
{
	int _hp[5] = { 300, 340, 380, 420, 440 };		// ǰ�Ĺ���ͬ
	/***** ��һ�� boss ******/

	if ( Map.part[Map.map_id] == 7 && Map.map_id <= 4 )
	{
		for ( int i = 0; i < BOSS::Boss_num; i++ )
			if ( !Boss[i].die && !Boss[i].appear )
			{
				// ������ҵ�����
				for ( int id = 0; id < PLAYER::player_num; id++ )
				{
					WEAPON::out = false;
					Player[id].Rp->have_weapon = false;
				}

				Boss[i].lying = false;
				Boss[i].appear = true;
				Boss[i].ph = &BOSS::head[Map.map_id];
				Boss[i].hp = _hp[Map.map_id];
				Boss[i].max_hp = Boss[i].hp;

				delete Boss[i].Bp;
				switch (Map.map_id)
				{
					case 0:
						{
							static int a0[3] = { 440, 185, 0 };/////////.......ò��������Բ��Ǿ�̬����
							static RECT rec0 = { -40, 185, 440, 280 };	// ��������Ǿ�̬����Ϊ����ֻ��ָ�������
							Boss[i].Bp = new B_ONE( a0, &rec0 );
						}										break;

					case 1:
						{
							int a1[3] = { 440, 180, 0 };
							static RECT rec1 = { -40, 180, 440, 275 };
							Boss[i].Bp = new B_TWO( a1, &rec1 );
						}										break;

					case 2:
						{
							int a2[3] = { 440, 192, 38 };
							static RECT rec2 = { -40, 182, 440, 282 };
							Boss[i].Bp = new B_THREE( a2, &rec2 );
						}										break;

					case 3:
						{
							int a3[3] = { 420, 274, 20 };
							static RECT rec3 = { -20, 180, 420, 275 };
							Boss[i].Bp = new B_FOUR( a3, &rec3 );
						}										break;
					case 4:
						{
							int a4[3] = { 360, 274, 20 };
							static RECT rec4 = { -20, 175, 420, 290 };
							Boss[i].Bp = new B_FIVE( a4, &rec4 );
						}										break;

					default: printf("Dispatch_boss()����\n");	break;
				}

				/****** ������ boss ���ݺ� *******/

				if ( i + 1 == BOSS::Boss_num )
				{
					setfont( 13, 0, "����" );
					char c[5][111] = { "Hello Kitty, I'll rep%&^# fammy043 to VERB U!",
										"I would't tell you that anyone can hit me from behind",
										"$#%%... ",
										"shibushi sichengxiangshi...",
										"the last ... last..."};
					putimage( 10, 380, &BOSS::head[Map.map_id] );

					mciSendString( "open ./res/Sounds/dada.mp3 alias da", 0, 0, 0 );
					mciSendString( "play da repeat", 0, 0, 0 );

					for ( unsigned int n = 0; n <= strlen(c[Map.map_id]); n++ )
					{
						Sleep( 20 );
						outtextxy( 10 + n * 9, 430, c[Map.map_id][n] );
						FlushBatchDraw();
					}

					mciSendString( "close da", NULL, 0, NULL );
					Sleep( 1000 );
				}
			}
	}
	setfont( 21, 0, "����" );
}



// ��ʾ��ҡ�������Ϣ

void Show_Information()
{
	int i;		// ����ѭ������

	Deal_Information();

	/******* ���� ******/

	if ( Map.part[Map.map_id] < 7 )
		for ( int i = 0; i < EMENY::emeny_num; i++ )
			if ( Emeny[i].appear[ Map.part[Map.map_id] ] )
			{
				setfillcolor( RED );
				bar( Emeny[i].x + 30, Emeny[i].y + 5, Emeny[i].x + 30 + Emeny[i].hp, Emeny[i].y + 9 );
				setcolor( WHITE );
				rectangle( Emeny[i].x + 28, Emeny[i].y + 3, Emeny[i].x + 32 + Emeny[i].max_hp, Emeny[i].y + 11 );
				putimage( Emeny[i].x, Emeny[i].y, Emeny[i].phead );
			}

	/******* boss *******/

	for ( i = 0; i < BOSS::Boss_num; i++ )
	{
		if ( (Map.map_id <= 4 && Map.part[Map.map_id] == 7 ||
			  Map.map_id == 4 && Map.part[4] == 2 ||
			  Map.map_id == 4 && Map.part[4] == 5) && Boss[i].appear )
		{
				bar( Boss[i].x + 35, Boss[i].y + 12, Boss[i].x + 35 + Boss[i].hp, Boss[i].y + 15 );
				rectangle( Boss[i].x + 33, Boss[i].y + 10, Boss[i].x + 37 + Boss[i].max_hp, Boss[i].y + 17 );
				putimage( Boss[i].x, Boss[i].y, Boss[i].ph );
		}
	}

	/******** ��� *******/

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
			/****** hp ******/

			putimage( Player[i].x, Player[i].y, Player[i].ph );
			setfillcolor( LIGHTGREEN );
			if ( !Player[i].die )
				bar( Player[i].x + 40, 10, Player[i].x + 40 + Player[i].hp, 15 );
			setcolor( WHITE );
			rectangle( Player[i].x + 38, 8, Player[i].x + 42 + Player[i].max_hp, 17 );

			/******* ���� ******/

			setfont( 12, 0, "����" );
			outtextxy( Player[i].x + 38, 26, "Exp:" );

			if ( Player[i].level <= 6 )
			{
				setfillcolor( YELLOW );
				bar( Player[i].x + 66, 30, Player[i].x + 64 + Player[i].experience, 31 );
				rectangle( Player[i].x + 64, 28, Player[i].x + 66 + PLAYER::exp_lev[Player[i].level], 33 );
			}
			else
				bar( Player[i].x + 64, 30, Player[i].x + 216, 31 );

			/******* ���� ******/

			outtextxy( Player[i].x, 45, "Lev:" );

			for ( int n = 0; n < 8; n++ )
			{
				circle( Player[i].x + 30 + n * 14, 50, 4 );
				setfillcolor( YELLOW );
				if ( n <= Player[i].level )
					fillcircle( Player[i].x + 30 + n * 14, 50, 2 );
			}

			/******** �ؿ������� ********/

			char c[20];
			sprintf( c, "%d/%d", Player[i].life, Map.map_id + 1 );
			outtextxy( Player[i].x + 180, 44, "��/��:" );
			outtextxy( Player[i].x + 220, 44, c );
	}
	setfont( 21, 0, "����" );
}


// ������ҡ�������Ϣ

void Deal_Information()
{
	/******* ��� *******/

	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( Player[i].hp <= 0 )
		{
			Player[i].die = true;
			Player[i].hp = Player[i].max_hp;
			Player[i].life--;
			Player[i].be_kick_counter = 3;
			Player[i].Set_bol( 0x00 );
			f[i] = &Role::Kick_down;
		}

		if ( Player[i].level <= 6 && Player[i].experience >= PLAYER::exp_lev[Player[i].level] )
		{
			Player[i].experience = 0;
			Player[i].level++;
		}
	}
}

// ���������һ�ε�ͼ�� boss
// �������ǰ������ boss

void Check_boss( bool& game )
{
	int i;			// ����ѭ������

	if ( Map.part[Map.map_id] == 7 && Map.map_id <= 4 ||
		 Map.map_id == 4 && Map.part[4] == 2 ||
		 Map.map_id == 4 && Map.part[4] == 5 )
	{
		int b_sum = 0;
		for ( i = 0; i < BOSS::Boss_num; i++ )
			if ( !Boss[i].die )
				b_sum++;

		if ( b_sum == 0 )
		{
			if ( Map.map_id == 4 && Map.part[4] == 7 )
			{
				MessageBox( GetHWnd(), "лл�ݹˣ�see you", "Hello Kitty", MB_OK );
				game = false;
				return;
			}

			for ( i = 0; i < BOSS::Boss_num; i++ )
				Boss[i].die = false;		// �Ա������ؿ� boss ʹ��

			if ( Map.part[Map.map_id] == 7 && Map.map_id <= 3 )
			{
				LEVEL::Level_Up();
				LEVEL::next_lev = true;

				for ( i = 0; i < PLAYER::player_num; i++ )
					Player[i].Set_bol( 0x00 );

				Sleep( 1000 );
				MessageBox( GetHWnd(), "go next!", "go", MB_OK );
				Go_Next_Lev();
				return;
			}
			else
				Map.die_out[4][ Map.part[Map.map_id] ] = true;
		}
	}

	/******* ���������� *********/

	int p_sum = 0;
	for ( i = 0; i < PLAYER::player_num; i++ )
		if ( Player[i].life >= 0 )
			p_sum++;
	if ( p_sum == 0 && Player[0].appear == false )
	{
		mciSendString( "open ./res/Sounds/�������.mp3 alias ww", 0, 0, 0 );
		mciSendString( "play ww", 0, 0, 0 );

		if ( IDYES == MessageBox(GetHWnd(), "�Ƿ�Ҫ��һëǮ������", "note", MB_YESNO) )
		{
			for ( int i = 0; i < PLAYER::player_num; i++ )
			{
				Player[i].appear = true;
				f[i] = &Role::Revive;
				Player[i].Rp->y = 230;
				Player[i].Rp->temp_y = 0;
				Player[i].life = 0;
			}
		}
		else
			game = false;
	}

	if ( PLAYER::player_num == 2 )
	{
		for ( int i = 0; i < 2; i++ )
			if ( Player[i].life < 0 && GetAsyncKeyState('U') && Player[(i+1) % 2].life >= 1 )
			{
				Player[i].life = 0;
				Player[(i+1) % 2].life--;
				Player[i].appear = true;
				f[i] = &Role::Revive;
				Player[i].Rp->temp_y = 0;
				Player[i].Rp->y = 240;
			}
	}
}


// ���Ž�����һ�ػ���

void Go_Next_Lev()
{
	int i;			// ����ѭ������

	if ( LEVEL::next_lev )
	{
		/****** ����ԭ����ɫ���� ******/

		LOGFONT font;
		COLORREF old;
		COLORREF oldfill;

		getfont( &font );
		old = getcolor();
		oldfill = getfillcolor();

		setcolor( LIGHTGREEN );
		loadimage( NULL, "./res/Images/total.jpg", 500, 450 );

		/***********/

		char c[5];
		sprintf( c, "%d", Map.map_id + 1 );
		setfont( 31, 0, "����" );
		outtextxy( 310, 80, c );

		int x[5] = { 144, 192, 286, 333, 349 };
		int y[5] = { 280, 296, 232, 279, 346 };

		for ( i = 0; i < 500; i++ )
		{
			Sleep( 1 );
			line( i, 135, i, 140 );

			if ( i % 10 == 0 )
			{
				setfillcolor( RGB(rand() % 255, rand() % 255, rand() % 255) );
				fillcircle( x[Map.map_id], y[Map.map_id], 6 );
			}

			FlushBatchDraw();
		}

		/****** ����� *******/

		for ( i = 0; i < PLAYER::player_num; i++ )
		{
			Player[i].Set_bol( 0x3f );
			Player[i].Rp->x = MINX + 1;
			Player[i].Rp->y = (Map.max_y[Map.map_id][0] + Map.min_y[Map.map_id][0]) / 2;
			Player[i].Rp->temp_y = Player[i].Rp->y;
		}

		char music[50];
		mciSendString( "close all", NULL, 0, NULL );
		sprintf( music, "open ./res/Sounds/bk%d.mp3 alias bk", Map.map_id + 1 );
		mciSendString( music, NULL, 0, NULL );
		mciSendString( "play bk repeat", 0, 0, 0 );

		setfont( &font );
		setcolor( old );
		setfillcolor( oldfill );
		LEVEL::next_lev = false;
	}
}

//

void Lift_up_down()
{
	if ( Map.map_id != 3 || Map.part[3] != 5 )
		return;

	for ( int i = 0; i < 3; i++ )
	{
		putimage(  25, OTHER::y[i], &OTHER::lift_up_down[0] );
		putimage( 473, OTHER::y[i], &OTHER::lift_up_down[0] );

		OTHER::y[i] = (OTHER::y[i] - 11 >= 65) ? OTHER::y[i] - 11 : 350;
	}
	if ( rand() % 5 < 4 )
		putimage( 70, 64, &OTHER::lift_up_down[1] );
}











/**** MAP ���ʵ�� ************************************************/

// ��ʼ����ͼ����

MAP::MAP()
{
	int i, n;			// ����ѭ������

	y = 0;
	xx = yy = num = 0;
	map_id = 0;
	move = false;
	int line1[MAX_MAP][9] = {	{0, 676, 1352, 2030, 2736, 3442, 4150, 4715, 5280},
								{0, 531, 1062, 1594, 2301, 3008, 3715, 4422, 5132},
								{0, 819, 1492, 2240, 2892, 3544, 4196, 4848, 5358},
								// ��819		Ϊ�ӵĽ��ߣ���ֹ���˳���ֱ�ӵ�������
								{0, 632, 1264, 1896, 2528, 3162, 3662, 4182, 4702},
								{0, 688, 1376, 2064, 2737, 3410, 4085, 4830, 5576 } };

	int line2[MAX_MAP][8] = {	{176, 852, 1530, 2236, 2942, 3650, 4215, 4780},
								{31,  562, 1094, 1801, 2508, 3215, 3922, 4632},
								{319, 992, 1740, 2392, 3044, 3696, 4348, 4858},
								{132, 764, 1396, 2028, 2662, 3162, 3682, 4202},
								{188, 876, 1564, 2237, 2910, 3585, 4330, 5076} };

	int miny[MAX_MAP][8]  = {	{200, 200, 200, 170, 170, 170, 200, 200},
								{190, 190, 190, 227, 227, 227, 227, 195},
								{203, 203, 203, 203, 203, 203, 203, 203},
								{205, 205, 205, 205, 205, 171, 205, 205},
								{205, 205, 205, 205, 205, 205, 205, 205} };

	int maxy[MAX_MAP][8]  = {	{295, 295, 295, 260, 260, 260, 295, 295},
								{295, 295, 295, 295, 295, 295, 295, 295},
								{293, 293, 293, 293, 293, 293, 293, 293},
								{295, 295, 295, 295, 295, 230, 295, 295},
								{295, 295, 295, 295, 295, 295, 295, 295} };
	int w[MAX_MAP] = { 5280, 5132, 5358, 4702, 5576 };

	for ( n = 0; n < MAX_MAP; n++ )
	{
		for ( i = 0; i < 8; i++ )
		{
			line_2[n][i]	= line2[n][i];
			min_y[n][i]		= miny[n][i];
			max_y[n][i]		= maxy[n][i];
			die_out[n][i]	= false;
			die_out[3][4]	= true;			// �����Ĺص������Ϊ true����Ϊ�пӲ��ܴ�
		}
		x[n]	 = 0;
		part[n]  = 0;
		width[n] = w[n];

		for ( i = 0; i < 9; i++ )
			line_1[n][i] = line1[n][i];

		char c[40];
		sprintf( c, "./res/Images/m%d.jpg", n );
		loadimage( &img[n], c, width[n], 450 );
	}
}

// ���õ�ͼ�ı仯

bool MAP::Set_Map()
{
	int i, ii, n;			// ����ѭ������

	/***** 02��12��22��26��34 ��ͼ�Ľ��� *****/
	for ( i = 0; i < 5; i++ )
	{
		if ( map_id  == i && (
			 part[i] == 2 && (i == 0 || i == 1 || i == 2) && die_out[i][2] ||
			 part[i] == 6 && i == 2 && die_out[2][6] ||
			 (part[i] == 4 || part[i] == 5) && i == 3 && die_out[3][ part[i] ]) ||
			 (part[4] == 2 || part[4] == 5) && i == 4 && die_out[4][ part[4] ] )
		{
			Sleep( 1000 );
			double rgb[3];
			DWORD* p = GetImageBuffer();

			for ( ii = 0; ii < PLAYER::player_num; ii++ )
				Player[ii].Set_bol( 0x00 );

			for ( n = 0; n < 11; n++ )
			{
				Sleep( 80 );
				for ( ii = 0; ii < 500 * 450; ii++ )
				{
					rgb[0] = GetRValue( p[ii] );
					rgb[1] = GetGValue( p[ii] );
					rgb[2] = GetBValue( p[ii] );

					for ( int j = 0; j < 3; j++ )
						if ( rgb[j] > 0 )
							rgb[j] *= 0.7;
					p[ii] = BGR( RGB(rgb[0], rgb[1], rgb[2]) );
				}
				FlushBatchDraw();
			}

			part[i]++;
			x[i] = -line_1[i][part[i]];

			for ( ii = 0; ii < PLAYER::player_num; ii++ )
			{
				Player[ii].Set_bol( 0x3f );
				if ( Map.map_id == 3 && Map.part[Map.map_id] ==5 )		// �������л��Χ��һ��
					Player[ii].Rp->x = (MINX + MAXX) / 2;
				else
					Player[ii].Rp->x = MINX + 1;
				Player[ii].Rp->y = ( Map.max_y[i][part[i]] + Map.min_y[i][part[i]] ) / 2;
			}
			return false;
		}
	}

	/****** 05-׹�� *******/
	if ( map_id == 0 && part[0] == 5 && die_out[0][5] == true )
	{
		if ( num == 0 )
		{
			Sleep( 1000 );
			int _x[2], _y[2];
			xx = 210;
			yy = 0;

			for ( i = 0; i < PLAYER::player_num; i++ )
			{
				if ( Player[i].appear )
				{
					Player[i].Set_bol( 0x00 );
					_x[i] = Player[i].Rp->x;
					_y[i] = Player[i].Rp->y;

					setcolor( BLACK );
					setfillcolor( BLACK );
					fillellipse( _x[i], _y[i] + 80, _x[i] + 80, _y[i] + 100 );
				}
			}
			FlushBatchDraw();
			Sleep( 500 );
			cleardevice();
		}

		num++;
		int b = 0;
		if ( num <= 33 )
		{
			b = 2;
			if ( num <= 20 )				yy += 12;
			if ( num >= 21 && num <= 25 )	yy -= 10;
			if ( num >= 29 )				yy += 10;

			if ( num == 21 || num == 29 )
				SetSound( NULL, "���˵���", "map_hit" );
		}

		if ( num > 33 && num <= 53 )
		{
			if ( num % 8 < 4 )
				b = 3;
			else
				b = 4;
		}

		if ( num > 53 && num <= 60 )
			b = 5;

		for ( i = 0; i < PLAYER::player_num; i++ )
			if ( Player[i].appear )
				putimage( xx - i * 80, yy, Player[i].Rp->GetImg(RIGHT,b,0) );

		if ( num > 60 )
		{
			x[0] = -line_1[0][6];
			part[0] = 6;

			for ( i = 0; i < PLAYER::player_num; i++ )
			{
				Player[i].Rp->x = xx - i * 80;
				Player[i].Rp->y = yy;
				Player[i].Set_bol( 0x3f );
			}
			return false;
		}
		return true;
	}
	return false;
}


// ��ͼ�ƶ������������ƶ�ʱ��
// 1����ͼ��ÿ�α��ƶ���ʣ�µĲ���С�ڴ��ڵĿ�ȣ������㹻��ʾ��
// 2����ÿ�α��ƶ�ֻʣ��һ�����ڵĿ�ȵ�ʱ��ֹͣ�ƶ���ֱ������������öε����е��˺�ſ����ƶ���

bool MAP::Move_map( bool check )
{
	for ( int i = 0; i <= 7; i++ )
		if ( x[map_id] <= (-line_1[map_id][i]) && x[map_id] > (-line_1[map_id][i + 1]) )
			part[map_id] = i;

	if ( (x[map_id] - 7) >= -line_2[map_id][ part[map_id] ] || die_out[map_id][ part[map_id] ] )
	{
		if ( move && x[map_id] > (-width[map_id] + 500) )
		{
			move = false;
			int sum = 0;
			char c[2];
			c[0] = 'D';
			c[1] = VK_RIGHT;

			for ( int i = 0; i < PLAYER::player_num; i++ )
			{
				if ( !GetAsyncKeyState(c[i]) && Player[i].appear )
					sum++;
			}

			if ( !sum )
				x[map_id] -= 6;
		}
		else if ( x[map_id] <= (-width[map_id] + 500) && check )	// ��ͼ�ߵ���󣬷��� false����������ƶ��ٶȳ���
			return false;

		if ( !check )
			putimage( x[map_id], y, &img[map_id] );
	}
	else
	{
		move = false;
		if ( !check )
			putimage( x[map_id], y, &img[map_id] );
		return false;
	}
	return true;
}

// ��������ܷ��ڸø߶�

bool MAP::Check( int y )
{
	if ( y <= max_y[map_id][ part[map_id] ] && y >= min_y[map_id][ part[map_id] ] )
		return true;
	else
		return false;
}



/********** Role ����ʵ�� ***************************************/

Role::Role()
{
	x = temp_x = 0;
	y = temp_y = (Map.max_y[Map.map_id][0] + Map.min_y[Map.map_id][0]) / 2;
	X = Y = 0;
	Drtx = 0;
	Dir = RIGHT;
	big = true;

	have_weapon = false;			// ��ǰ�Ƿ�������
	weapon_counter = 0;				// ���� 15 �κ�������ʧ

	w = a = j = ja = bb = jk = jf = lw = kd = df = tr = cm = ka = r = bk = 0;
	n_w = n_a = n_j = n_ja = n_bb = n_jk = n_jf = n_lw = n_kd = n_df = n_tr = n_cm = n_ka = n_r = n_bk = 0;
}

// ʹ�ö����Ƽ���Ӧ����
// ���λֵΪ 1 �����Ƿ�Ϊ��

bool Role::Check_n_x( int n )
{
	int val[14], sum = 0;
	val[0]  = n_w;			val[7]  =	n_lw;

	val[1]  = n_a;			val[8]  =	n_kd;

	val[2]  = n_j;			val[9]  =	n_df;

	val[3]  = n_ja;			val[10] =	n_tr;

	val[4]  = n_bb;			val[11] =	n_cm;

	val[5]  = n_jk;			val[12] =	n_ka;	// ϥ�ǡ�ͷ����

	val[6]  = n_jf;			val[13] =	n_r;	// ����

	for ( int i = 0; i < 13; i++ )
	{
		if ( n & 0x2000 )
			sum += val[i];

		n <<= 1;
	}

	if ( sum == 0 )
		return true;
	else
		return false;
}

//

void Role::Stand( int dir, int player )
{
	Player[player - 1].be_kiss = false;
	Player[player - 1].lying = false;

	// ���ӡ����µ���
	if ( Map.map_id == 2 && Check_hole(PE_player, player - 1) == _IN
		 || Check_lift(PE_player, player - 1) == _IN )
	{
		if ( Player[player - 1].life >= 0 )
			Player[player - 1].life--;
		temp_y = 0;
		Player[player - 1].hp = Player[player - 1].max_hp;
		Player[player - 1].Set_bol( 0x00 );
		f[player - 1] = &Role::Revive;
		return;
	}

	UNREFERENCED_PARAMETER(dir);
	if ( !Player[player - 1].catch_emeny )
	{
		temp_y = y;			// ��ֹ���temp_y != y ��ʱ�򱻴�ϣ����²���
		Drtx = temp_x = 0;
		n_w = n_a = n_j = n_ja = n_bb = n_jk = n_jf = n_lw = n_df = n_tr = n_cm = n_ka = n_kd = n_r = n_bk = 0;
		w = a = j = ja = bb = jk = jf = lw = df = tr = cm = ka = kd = r = bk = 0;
	}
	else
	{
		n_cm++;
		if ( n_cm > 40 )
		{
			n_ka = ka = 0;	// �����в���
			n_cm = cm = 0;
			f[player - 1] = &Role::Stand;

			if ( Emeny[Player[player - 1].emeny].be_catch && Emeny[Player[player - 1].emeny].role == player - 1 )
			{
				mf[Player[player - 1].emeny] = &BASE_CLASS::Walk;
				Emeny[Player[player - 1].emeny].be_kick_counter = 0;
				Emeny[Player[player - 1].emeny].be_catch = false;
			}
			Player[player - 1].catch_emeny = false;
		}
	}
}

//

void Role::Walk_all( int dir, int player, int* ddx, int* ddy )
{
	int i;			// ����ѭ������

	/******* ��⵱ǰ��λ���Ƿ���µ��� ******/
	/******* ���� ******/

	if ( Map.map_id == 2 && Check_hole(PE_player, player - 1) == _IN
		 || Check_lift(PE_player, player - 1) == _IN )
	{
		if ( n_cm != 0 )
		{
			if ( Emeny[Player[player - 1].emeny].be_catch && Emeny[Player[player - 1].emeny].role == player - 1 )
			{
				mf[Player[player - 1].emeny] = &BASE_CLASS::Die;
				Emeny[Player[player - 1].emeny].be_kick_counter = 0;
				Emeny[Player[player - 1].emeny].be_catch = false;
				Emeny[Player[player - 1].emeny].lying = true;
			}
			n_cm = cm = 0;
			Player[player - 1].catch_emeny = false;
		}

		if ( Player[player - 1].life >= 0 )
			Player[player - 1].life--;
		temp_y = 0;
		Player[player - 1].hp = Player[player - 1].max_hp;
		Player[player - 1].Set_bol( 0x00 );
		f[player - 1] = &Role::Revive;
		return;
	}

	if ( !Player[player - 1].catch_emeny && Player[player - 1].role_kind != 2 || Player[player - 1].role_kind == 2 )
	{
		/********* ��Ծ -> ǰ�� *************/

		if ( dir == LEFT && n_j > 0 && n_j < 10 || n_j > 0 && n_j < 10 && dir == RIGHT )
		{
			int _x[2] = { -6, 6 };
			Dir  = dir;
			Drtx = _x[Dir];
			temp_x = 0;
			n_jf = n_j;
			n_j	 = j = w = 0;

			X = x + _x[Dir] / 6 * ( 60 - 6 * n_jf );
			Y = temp_y - 70 + 7 * n_jf;
			
			Player[player - 1].Set_bol( 0x02 );			// 0000,10
			f[player - 1] = &Role::Jump_forward;
			temp_y = ( x - X ) * ( x - X ) / 62 + Y;	// �����Walk() ����δ�ܼ�ʱ����temp_y,�����е�㲻��

			return;
		}
		else if ( n_j >= 10 )
		{
			Player[player - 1].Set_bol( 0x00 );
			f[player - 1] = &Role::Jump;
			return;
		}

		switch ( Player[player - 1].role_kind )
		{
			case 0:
				/****** ���ɳ���� ***********************/
				if ( dir == Dir && n_a > 0 && n_a < 3 )
				{
					n_a = a = 0;
					n_w = w = 0;
					Player[player - 1].Set_bol( 0x00 );		// 0000,00
					f[player - 1] = &Role::Light_wave;
					return;
				}
				break;

			case 1:
				/****** ������ ***********************/
				if ( dir == Dir && n_a > 0 && n_a < 3 )
				{
					n_a = a = 0;
					n_w = w = 0;
					Player[player - 1].Set_bol( 0x00 );		// 0000,00
					f[player - 1] = &Role::Light_wave;
					return;
				}
				break;

			case 2:
				/****** ���ɷ��� ***********************/
				if ( !Player[player - 1].catch_emeny )
				{
					if ( dir == Dir && n_a > 0 && n_a < 3 )
					{
						int _x[2]  = { -5, 5 };
						Drtx = _x[Dir];
						n_a = a = 0;
						n_w = w = 0;
						Player[player - 1].Set_bol( 0x00 );		// 0000,00
						f[player - 1] = &Role::Light_wave;
						return;
					}
				}
				break;

			default:	printf( "Role::Walk_all����\n" );	break;
		}
	}

	/***** �������¡������¡������� *****/
	int dx[8], dy[8];
	for ( i = 0; i < 8; i++ )
	{
		dx[i] = ddx[i];
		dy[i] = ddy[i];
		if ( Player[player - 1].catch_emeny )
		{
			if ( dx[i] != 0 )		dx[i] = abs(dx[i]) / dx[i];
			if ( dy[i] != 0 )		dy[i] = abs(dy[i]) / dy[i];
		}
	}

	/***** ��ͼ�������ƶ� *****************/
	if ( x + dx[dir] > MINX && x + dx[dir] < MAXX )
	{
		if ( Map.Move_map(true) && (dir == RIGHT || dir == R_U || dir == R_D) )
			x += ( dx[dir] - dx[R_U] );
		else
			x += dx[dir];
	}
	if ( dir == RIGHT || dir == R_U || dir == R_D )
		Map.move = true;

	if ( Map.Check(y + dy[dir]) )
	{
		y += dy[dir];
		temp_y = y;
	}

	/***************************************/
	char c1 = 0, c2 = 0;
	switch ( player )
	{
		case 1:
			switch ( dir )
			{
				case LEFT:		c1 = 'A';	if ( !Player[player - 1].catch_emeny ) Dir = dir;			break;
				case RIGHT:		c1 = 'D';	if ( !Player[player - 1].catch_emeny ) Dir = dir;			break;
				case UP:		c1 = 'W';						break;
				case DOWN:		c1 = 'S';						break;
				case L_U:		c1 = 'A';	c2 = 'W';			break;
				case L_D:		c1 = 'A';	c2 = 'S';			break;
				case R_U:		c1 = 'D';	c2 = 'W';			break;
				case R_D:		c1 = 'D';	c2 = 'S';			break;
				default:										break;
			}													break;

		case 2:
			switch ( dir )
			{
				case LEFT:		c1 = VK_LEFT;	if ( !Player[player - 1].catch_emeny ) Dir = dir;		break;
				case RIGHT:		c1 = VK_RIGHT;	if ( !Player[player - 1].catch_emeny ) Dir = dir;		break;
				case UP:		c1 = VK_UP;						break;
				case DOWN:		c1 = VK_DOWN;					break;
				case L_U:		c1 = VK_LEFT;	c2 = VK_UP;		break;
				case L_D:		c1 = VK_LEFT;	c2 = VK_DOWN;	break;
				case R_U:		c1 = VK_RIGHT;	c2 = VK_UP;		break;
				case R_D:		c1 = VK_RIGHT;	c2 = VK_DOWN;	break;
				default:										break;
			}													break;
		default:												break;
	}

	/*****************************/
	if ( !Player[player - 1].catch_emeny )///////////////////////
	{
		n_cm = cm = 0;
		if ( (n_w + 1) % 5 == 0 )
			w = ( ++w > 3 ) ? 0 : w;
		n_w++;
	}

	/****** ץ�� ******/
	for ( i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( abs(temp_y - Emeny[i].Mp->Gety()) < 10 && abs(x - Emeny[i].Mp->Getx()) < 10 &&
			!Emeny[i].lying && !Player[player - 1].catch_emeny && !Emeny[i].be_catch )
		{
			if ( n_cm != 0 )
				printf("walk��n_cm������ 0\n");

			Emeny[i].be_catch = true;
			Player[player - 1].catch_emeny = true;

			Emeny[i].Mp->SetDir( Dir );
			Emeny[i].role = player - 1;
			Player[player - 1].emeny = i;
			mf[i] = &BASE_CLASS::Be_catch;
		}
	}

	/***** �ÿ�������ץ�˺��棬��ΪҪ��֤ץ�˺����� n_cm++������ J �����Ŀ�Ļ���⵽���� n_cm = 0 *****/
	// �Ӷ�����ץס�ĵ������ˣ������Ȼ���� catch_emeny ״̬
	if ( Player[player - 1].catch_emeny )
	{
		n_cm++;
		if ( n_cm % 5 == 4 )
			cm = ( ++cm > 4 ) ? 1 : cm;

		if ( n_cm > 10000 )
		{
			n_ka = ka = 0;
			n_cm = cm = 0;
			f[player - 1] = &Role::Stand;
			
			if ( Emeny[Player[player - 1].emeny].be_catch && Emeny[Player[player - 1].emeny].role == player - 1 )
			{
				Emeny[Player[player - 1].emeny].lying = false;
				mf[Player[player - 1].emeny] = &BASE_CLASS::Walk;
				Emeny[Player[player - 1].emeny].be_kick_counter = 0;
				Emeny[Player[player - 1].emeny].be_catch = false;
			}
			Player[player - 1].catch_emeny = false;
		}
	}

	if ( !(GetAsyncKeyState(c1) & 0x8000) && dir < 4 ||
	   ( !(GetAsyncKeyState(c1) & 0x8000) || !(GetAsyncKeyState(c2) & 0x8000) ) && dir >= 4 )
	{
		n_w  = w  = 0;
		f[player - 1] = &Role::Stand;
	}
}

// 

void Role::Jump( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	if ( Player[player - 1].role_kind != 2 )
		if ( n_cm != 0 )
		{
			if ( Emeny[Player[player - 1].emeny].be_catch && Emeny[Player[player - 1].emeny].role == player - 1 )
			{
				mf[Player[player - 1].emeny] = &BASE_CLASS::Walk;
				Emeny[Player[player - 1].emeny].be_kick_counter = 0;
				Emeny[Player[player - 1].emeny].be_catch = false;
			}
			n_cm = cm = 0;
			Player[player - 1].catch_emeny = false;
		}

	if ( n_j == 0 )
	{
		Player[player - 1].Set_bol( 0x32 );		// 1100,10
		SetSound( NULL, "����", "s5" );
	}

	/*****************************/
	if ( n_j <= 9 )	
		temp_y -= 7;
	if ( n_j >= 10 && n_j <= 19 )
		temp_y += 7;

	n_j++;

	/*****************************/
	switch ( n_j )
	{
		case 9:		j = 1;	break;
		case 19:
			j = 2;
			SetSound( NULL, "���", "s6" );
			break;
		case 21:
			if ( Player[player - 1].role_kind == 2 )
				if ( Player[player - 1].catch_emeny )
				{
					n_cm = cm = 0;
					n_ka = ka = 0;
					Emeny[Player[player - 1].emeny].be_kick_counter = 0;
					mf[Player[player - 1].emeny] = &BASE_CLASS::Walk;
					Emeny[Player[player - 1].emeny].be_catch = false;
					Player[player - 1].catch_emeny = false;
				}
			n_j = j = 0;
			Player[player - 1].Set_bol( 0x3f );		// 1111,11
			f[player - 1] = &Role::Stand;
							break;
		default:			break;
	}
}

//

void Role::Attack( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);

	/***** ץ�˺��� ****/
	if ( n_cm > 0 )
	{
		char c[2] = { '\0', '\0' };
		switch ( player )
		{
			case 1:	c[0] = 'A';		c[1] = 'D';			break;
			case 2: c[0] = VK_LEFT;	c[1] = VK_RIGHT;	break;
			default: printf( "role �� Attack ����" );	break;
		}

		if ( GetAsyncKeyState(c[0]) || GetAsyncKeyState(c[1]) )
		{
			if ( GetAsyncKeyState(c[0]) )
			{
				if ( Dir == LEFT )
					Player[player - 1].throw_dir = FORWARD;
				else
					Player[player - 1].throw_dir = BACK;
				Emeny[Player[player - 1].emeny].Mp->SetDir( LEFT );
			}

			if ( GetAsyncKeyState(c[1]) )
			{
				if ( Dir == RIGHT )
					Player[player - 1].throw_dir = FORWARD;
				else
					Player[player - 1].throw_dir = BACK;
				Emeny[Player[player - 1].emeny].Mp->SetDir( RIGHT );
			}

			n_ka = ka = 0;
			n_cm = cm = 0;
			Player[player - 1].Set_bol( 0x00 );
			Emeny[Player[player - 1].emeny].be_kick_counter = 0;
			mf[Player[player - 1].emeny] = &BASE_CLASS::Throw_away;
			f[player - 1] = &Role::Throw_role;
			return;
		}

		/**** ϥ�ǹ��� ***/
		Player[player - 1].Set_bol( 0x00 );
		n_ka = 0;
		ka = 5;
		f[player - 1] = &Role::Knee_attack;
		return;
	}

	/***** ���ߡ���ϥ�ǹ��� ***************/
	if ( n_j > 0 )
	{
		if ( GetAsyncKeyState('S') & 0x8000 && player == 1 ||
			 GetAsyncKeyState(VK_DOWN) & 0x8000 && player == 2 )
		{
			n_jk = n_j;
			jk	 = 1;
			f[player - 1] = &Role::Jump_knee;			
		}
		else
		{
			n_ja = n_j;
			f[player - 1] = &Role::Jump_attack;
		}

		Drtx = 0;
		n_a = a = 0;
		n_j = j = 0;
		Player[player - 1].Set_bol( 0x00 );		// 0000,00
		return;
	}

	/****** ��ǰ�� --> ��ǰ�ߡ���ǰϥ�ǹ��� ********************/
	if ( n_jf > 0 )
	{
		int _x[2] = { -6, 6 };
		Drtx = _x[Dir];

		if ( GetAsyncKeyState('S') & 0x8000 && player == 1 || 
			 GetAsyncKeyState(VK_DOWN) & 0x8000 && player == 2 )
		{
			jk	 = 1;
			n_jk = n_jf;
			f[player - 1] = &Role::Jump_knee;
		}
		else
		{
			ja	 = 1;
			n_ja = n_jf;
			f[player - 1] = &Role::Jump_attack;
		}
		n_a = a = 0;
		n_jf = jf = 0;
		Player[player - 1].Set_bol( 0x00 );		// 0000,00
		return;
	}

	/****** ����Ƿ��ȡ�ļ��� *****/
	if ( FOOD::out && abs(x-FOOD::x) < 20 && abs(FOOD::y - y) < 20 )
	{
		Player[player - 1].hp = Player[player - 1].max_hp;
		FOOD::out = false;
	}

	/*****************************/
	int _a = 1;
	if ( Player[player - 1].role_kind == 1 )
		_a = 5;

	if ( n_a % 2 == 0 )
		a = ( ++a > _a ) ? 0 : a;

	n_a++;

	char L = '\0', R = '\0';
	switch ( player )
	{
		case 1:		L = 'A'; R = 'D';			break;
		case 2:		L = VK_LEFT; R = VK_RIGHT;	break;
		default:	printf("������������");		break;
	}

	if ( GetAsyncKeyState(L) & 0x8000 )	Dir = LEFT;
	if ( GetAsyncKeyState(R) & 0x8000 ) Dir = RIGHT;

	if ( n_a % 4 == 1 )
		SetSound( NULL, "��ȭ", "s" );

	if ( !GetAsyncKeyState('J') && player == 1 || !GetAsyncKeyState(VK_NUMPAD1) && player == 2 )
	{
		n_a = a = 0;
		f[player - 1] = &Role::Stand;
	}

	/***** ɨ���Ƿ���е��� **************/
	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( abs(y - Emeny[i].Mp->Gety()) < 18 )
		{
			if ( abs(x - Emeny[i].Mp->Getx()) < 50 && Check_position(Dir, x, Emeny[i].Mp->Getx()) )
			{
				if ( !Emeny[i].lying )
				{
					if ( Emeny[i].be_catch )
						Player[player % 2].catch_emeny = false;

					Emeny[i].be_catch = false;
					Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
					Emeny[i].role = player - 1;
					mf[i] = &BASE_CLASS::Kick_away;
				}
			}
		}
	}

	/******** �Ƿ���� boss ********/
	// ��ҹ�����Χ����С�� boss �Ĺ�����Χ
	if ( Map.part[Map.map_id] == 7 ||
		 Map.map_id == 4 && Map.part[4] == 2 ||
		 Map.map_id == 4 && Map.part[4] == 5 )///////// ��Щ��ͼ�м�Ҳ���� boss
		for ( int i = 0; i < BOSS::Boss_num; i++ )
			if ( abs(y - Boss[i].Bp->y - 20) < 15 )
				if ( abs(x - Boss[i].Bp->x - 10) < 45 &&
					Check_position(Dir, x, Boss[i].Bp->x+10) && !Boss[i].lying )
				{
					Boss[i].role = (byte)( player - 1 );
					bf[i] = &BASE_BOSS::Kick_away;
					Boss[i].Bp->Dir = ( Dir + 1 ) % 2;
				}
}

//

void Role::Big_Blow( int dir, int player )
{
	int i;			// ����ѭ������

	if ( n_bb == 0 )
		SetSound( NULL, "������", "s2" );

	UNREFERENCED_PARAMETER(dir);
	Player[player - 1].Set_bol( 0x00 );		// 0000,00

	for ( i = 0; i < EMENY::emeny_num; i++ )
		if ( abs(y - Emeny[i].Mp->Gety()) < 28 && abs(x - Emeny[i].Mp->Getx()) < 70 )
				if ( !Emeny[i].lying )
				{
					SetSound( NULL, "����", "s3" );

					if ( Emeny[i].be_catch )
						Player[player % 2].catch_emeny = false;

					Emeny[i].be_catch = false;
					Emeny[i].role = player - 1;
					Emeny[i].be_kick_counter = 5;
					Emeny[i].start_fly = true;
					mf[i] = &BASE_CLASS::Kick_away;
					Emeny[i].Mp->SetDir( RIGHT );

					int _x = Emeny[i].Mp->Getx();
					int _y = Emeny[i].Mp->Gety();
					int dx = 40, dy = 30;

					if ( x <= _x )
					{
						Emeny[i].Mp->SetDir( LEFT );
						dx = 0;
					}
					putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
					putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

					/*************/

					Player[player - 1].experience += 1;
					Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 5 / 2;

					if ( big )
					{
						big = false;
						Player[player - 1].hp -= 10;
					}
				}
	/******* boss *******/
	for ( i = 0; i < BOSS::Boss_num; i++ )
		if ( abs(y - Boss[i].Bp->y-20) < 15 && abs(x - Boss[i].Bp->x) < 60 && !Boss[i].lying )
		{
			SetSound( NULL, "����", "ss3" );

			Boss[i].role = (byte)( player - 1 );
			Boss[i].start_fly = true;
			Boss[i].be_kick_counter = 5;
			bf[i] = &BASE_BOSS::Kick_away;
			Boss[i].Bp->Dir = RIGHT;

			int _x = Boss[i].Bp->x;
			int _y = Boss[i].Bp->y;
			int dx = 40, dy = 30;

			if ( x <= _x )
			{
				Boss[i].Bp->Dir = LEFT;
				dx = 0;
			}
			putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
			putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

			Player[player - 1].experience += 1;
			Boss[i].hp -= LEVEL::harm[Player[player - 1].level] * 4;

			if ( big )
			{
				big = false;
				Player[player - 1].hp -= 20;
			}
		}

	n_bb++;
	if ( (n_bb + 1) % 2 == 0 )
		bb = (++bb > 3 ) ? 0 : bb;
	else if ( n_bb >= 30 )
	{
		SetSound( "s2", "���", "s4" );

		big = true;
		bb = n_bb = 0;
		j  = n_j  = 0;		// ������
		cm = n_cm = 0;
		ka = n_ka = 0;
		a  = n_a  = 0;
		Player[player - 1].Set_bol( 0x3f );		// 1111,11
		Player[player - 1].catch_emeny = false;
		f[player - 1] = &Role::Stand;
	}
}

//

void Role::Throw_role( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	if ( n_tr == 0 )
	{
		Dir++;
		Player[player - 1].experience += 1;
		if ( Player[player - 1].throw_dir == FORWARD )
			Dir++;
	}

	if ( n_tr == 7 )
		SetSound( NULL, "˦��", "s12" );

	n_tr++;

	if ( Player[player - 1].role_kind == 0 )
	{
		if ( n_tr == 4 )	tr = 2;
		if ( n_tr == 7 )	tr = 3;
		if ( n_tr == 10 )	tr = 4;
		if ( n_tr > 20 )
		{
			Player[player - 1].throw_dir = BACK;
			Player[player - 1].Set_bol( 0x3f );
			tr = n_tr = 0;
			Dir++;
			f[player - 1] = &Role::Stand;
			Player[player - 1].catch_emeny = false;
		}
	}
	else if ( Player[player - 1].role_kind == 1 )
	{
		int dx[2] = { 20, -20 };
		switch ( n_tr )
		{
			case 2:		tr = 1;					break;
			case 6:		tr = 2;					break;
			case 10:	tr = 3; x -= dx[Dir];	break;
			case 14:	tr = 4;	x -= dx[Dir];	break;
			case 22:	tr = 5; x += dx[Dir];	break;
			case 26:	tr = 6;	x += dx[Dir];	break;
			case 32:
				Player[player - 1].throw_dir = BACK;
				Player[player - 1].Set_bol( 0x3f );
				tr = n_tr = 0;
				Dir++;
				f[player - 1] = &Role::Stand;
				Player[player - 1].catch_emeny = false;
				break;
			default:			break;
		}
	}
}

//

void Role::Jump_forward( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);

	if ( Player[player - 1].role_kind != 2 )
	{
		if ( n_jf >= 7  && n_jf <= 9 )	jf = 1;
		if ( n_jf >= 10 && n_jf <= 12 )	jf = 2;
		if ( n_jf >= 13 && n_jf <= 15 )	jf = 4;
		if ( n_jf >= 16 && n_jf <= 18 )	jf = 5;
		if ( n_jf > 18 )				jf = 6;
	}
	else
	{
		if ( n_jf < 10 )
			jf = 0;
		else
			jf = 1;
	}

	/***** �ƶ���Խ���¼ ***************/
	if ( x + Drtx > MINX && x + Drtx < MAXX )
		x += Drtx;
	else
		temp_x++;
	temp_y = ( x + temp_x * Drtx - X ) * ( x + temp_x * Drtx - X ) / 62 + Y;
	n_jf++;

	/***** ***************/
	if ( n_jf == 18 )
		SetSound( NULL, "���", "s13" );

	if ( n_jf > 20 )
	{
		if ( Player[player - 1].role_kind == 2 )
			if ( Player[player - 1].catch_emeny )
			{
				n_cm = cm = 0;
				n_ka = ka = 0;
				Emeny[Player[player - 1].emeny].be_kick_counter = 0;
				mf[Player[player - 1].emeny] = &BASE_CLASS::Walk;
				Emeny[Player[player - 1].emeny].be_catch = false;
				Player[player - 1].catch_emeny = false;
			}

		n_jf	= 0;
		jf		= 0;
		Drtx	= 0;
		temp_x  = 0;
		Player[player - 1].Set_bol( 0x3f );
		f[player - 1] = &Role::Stand;
	}
}

//

void Role::Jump_attack( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	/***** ֱ�ߵ� y ֵ **************/
	if ( n_ja <= 9 )
		temp_y -= 7;
	if ( n_ja >= 10 && n_ja <= 19 )
		temp_y += 7;

	/***** ֱ��ǰ�ߡ�Խ����� ********/
	if ( x + Drtx > MINX && x + Drtx < MAXX )
		x += Drtx;
	else
		temp_x++;

	/****** ǰ�ߵ� y ֵ **************/
	if ( Drtx != 0 )
		temp_y = ( x + temp_x * Drtx - X ) * ( x + temp_x * Drtx - X ) / 62 + Y;

	/****** ����Ƿ����е��� ********/
	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( abs(y - Emeny[i].Mp->Gety()) < 18 && Emeny[i].lying == false &&
			(Dir == LEFT &&  x - Emeny[i].Mp->Getx() < 50 && x > Emeny[i].Mp->Getx() ||
			 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 50 && x < Emeny[i].Mp->Getx()) )
		{
			SetSound( NULL, "����", "h12" );

			int dx[2] = { -20, 50 };
			putimage( x + dx[Dir], temp_y + 40, &PLAYER::light[1], SRCAND );
			putimage( x + dx[Dir], temp_y + 40, &PLAYER::light[0], SRCINVERT );

			if ( Emeny[i].be_catch )
				Player[player % 2].catch_emeny = false;

			Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
			Emeny[i].start_fly = true;
			Emeny[i].lying = true;
			Emeny[i].be_catch = false;	//
			Emeny[i].role = player - 1;	//
			Emeny[i].be_kick_counter = 5;
			mf[i] = &BASE_CLASS::Kick_away;

			Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 2;
		}
	}

	/*******************/
	if ( Map.part[Map.map_id] == 7 ||
		 Map.map_id == 4 && Map.part[4] == 2 ||
		 Map.map_id == 4 && Map.part[4] == 5 )
	{
		for ( int i = 0; i < BOSS::Boss_num; i++ )
		{
			if ( abs(y - Boss[i].Bp->y-20) < 15 && !Boss[i].lying &&
				 Check_position(Dir, x, Boss[i].Bp->x) && abs(x - Boss[i].Bp->x) < 50 )
			{
				SetSound( NULL, "����", "hh12" );
				int dx[2] = { -20, 50 };
				putimage( x + dx[Dir], temp_y + 40, &PLAYER::light[1], SRCAND );
				putimage( x + dx[Dir], temp_y + 40, &PLAYER::light[0], SRCINVERT );

				Boss[i].Bp->Dir = ( Dir + 1 ) % 2;
				Boss[i].start_fly = true;
				Boss[i].lying = true;
				Boss[i].role = (byte)( player - 1 );
				Boss[i].be_kick_counter = 5;
				bf[i] = &BASE_BOSS::Kick_away;

				Boss[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
			}
		}
	}
	n_ja++;

	/********************/
	if ( n_ja >= 8 && n_ja <= 15 )	ja = 1;
	if ( n_ja > 15 && n_ja <= 19 )	ja = 2;
	if ( n_ja == 18 )
		SetSound( NULL, "���", "h13" );
	if ( n_ja >= 21 )
	{
		Drtx	= 0;
		temp_x = 0;
		n_ja	= 0;
		ja		= 0;
		Player[player - 1].Set_bol( 0x3f );		// 1111,11
		f[player - 1] = &Role::Stand;
	}
}

//

void Role::Jump_knee( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	if ( n_jk <= 9 )
		temp_y -= 7;

	if ( n_jk >= 10 && n_jk <= 19 )
		temp_y += 7;

	/********************/
	if ( x + Drtx > MINX && x + Drtx < MAXX )
		x += Drtx;
	else
		temp_x++;

	/********************/
	if ( Drtx != 0 )
		temp_y = ( x + temp_x * Drtx - X ) * ( x + temp_x * Drtx - X ) / 62 + Y;
	n_jk++;

	/***********/
	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( abs(y - Emeny[i].Mp->Gety()) < 18 && Emeny[i].lying == false && !Emeny[i].stand &&
			(Dir == LEFT &&  x - Emeny[i].Mp->Getx() < 50 && x > Emeny[i].Mp->Getx() ||
			 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 50 && x < Emeny[i].Mp->Getx()) )
		{
			SetSound( NULL, "����", "s16" );
			int dx[2] = { 0, 40 };
			putimage( x + dx[Dir], temp_y + 60, &PLAYER::light[1], SRCAND );
			putimage( x + dx[Dir], temp_y + 60, &PLAYER::light[0], SRCINVERT );

			if ( Emeny[i].be_catch )
				Player[player % 2].catch_emeny = false;

			mf[i] = &BASE_CLASS::Stand;
			Emeny[i].be_catch = false;
			Emeny[i].stand = true;
			Emeny[i].role = player - 1;

			Player[player - 1].experience += 1;
			Emeny[i].hp -= LEVEL::harm[Player[player - 1].level];

			if ( Emeny[i].hp <= 0 )
			{
				Emeny[i].Mp->SetDir( (Dir+1) % 2 );
				Emeny[i].start_fly = true;
				Emeny[i].be_catch = false;
				Emeny[i].be_kick_counter = 5;
				mf[i] = &BASE_CLASS::Kick_away;
				Emeny[i].lying = true;
			}
		}
	}

	/********************/
	switch ( n_jk )
	{
		case 9:		jk = 1;	break;
		case 19:	jk = 2;	SetSound( NULL, "���", "s17" );	break;
		case 21:
			Drtx	= 0;
			temp_x = 0;
			n_jk	= 0;
			jk		= 0;
			Player[player - 1].Set_bol( 0x3f );
			f[player - 1] = &Role::Stand;
			break;

		default:			break;
	}
}

//

void Role::Knee_attack( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);

	int dx[2] = { -2, 40 };
	putimage( x + dx[Dir], y + 30, &PLAYER::light[1], SRCAND );
	putimage( x + dx[Dir], y + 30, &PLAYER::light[0], SRCINVERT );

	if ( n_ka == 0 )
	{
		Player[player - 1].experience += 1;
		Emeny[Player[player - 1].emeny].hp -= LEVEL::harm[Player[player - 1].level] * 2;

		// ���� �� hp < 0 ��ɱ
		if ( ++Emeny[Player[player - 1].emeny].be_kick_counter >= 4 || Emeny[Player[player - 1].emeny].hp <= 0 )
		{
			Emeny[Player[player - 1].emeny].Mp->SetDir( (Dir+1) % 2 );
			Emeny[Player[player - 1].emeny].start_fly = true;
			Emeny[Player[player - 1].emeny].be_catch = false;
			Emeny[Player[player - 1].emeny].be_kick_counter = 5;
			mf[Player[player - 1].emeny] = &BASE_CLASS::Kick_away;
		}
		Emeny[Player[player - 1].emeny].lying = true;
		SetSound( NULL, "����", "s18" );
	}

	if ( n_ka == 3 )
		ka = 0;
	n_ka++;

	if ( n_ka >= 6 )
	{
		n_ka = ka = 0;
		f[player - 1] = &Role::Stand;
		Player[player - 1].Set_bol( 0x3f );

		if ( Emeny[Player[player - 1].emeny].be_kick_counter >= 4 )
		{
			n_cm = cm = 0;
			f[player - 1] = &Role::Stand;
			Player[player - 1].catch_emeny = false;
		}
	}
}

//

void Role::Kick_down( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	if ( n_kd == 0 )
	{
		have_weapon = false;
		if ( Player[player - 1].role_kind == 0 )
			Player[player - 1].skill = NONE;	// ��ֹ�����������̱��ɵ����湥������������ʧ����Ȼ���õ�
		if ( Player[player - 1].catch_emeny )
		{
			// �� if ��Ϊ��ȷ��˫��ģʽ��������
			// ����� a ������� b ס�ŵĵ��ˣ�������ץס������ʱ b �����Ȼ����ץ��״̬
			// �������ͷŵ���֮ǰ�����⵱ǰ�Ƿ�ץ�ŵ���!!!
			if ( Emeny[Player[player - 1].emeny].be_catch && Emeny[Player[player - 1].emeny].role == player - 1 )
			{
				Player[player - 1].skill = NONE;
				Emeny[Player[player - 1].emeny].lying = false;
				mf[Player[player - 1].emeny] = &BASE_CLASS::Walk;
				Emeny[Player[player - 1].emeny].be_kick_counter = 0;
				Emeny[Player[player - 1].emeny].be_catch = false;
			}
			Player[player - 1].catch_emeny = false;
		}
		Player[player - 1].be_kick_counter++;

		if ( !Player[player - 1].die )
			Player[player - 1].hp -= 20;
	}

	if ( Player[player - 1].be_kick_counter < 3 )
	{
		if ( n_kd == 0 )
			SetSound( NULL, "����", "s19" );

		if ( ++n_kd < 9 )
			kd = 1;
		else
		{
			w = a = j = ja = jk = jf = lw = kd = df = tr = cm = ka = 0;
			n_w = n_a = n_j = n_ja = n_jk = n_jf = n_lw = n_kd = n_df = n_tr = n_cm = n_ka = 0;

			Player[player - 1].lying = false;
			Player[player - 1].Set_bol( 0x3f );
			f[player - 1] = &Role::Stand;
		}
	}
	else
	{
		if ( n_kd == 0 )
		{
			SetSound( NULL, "��ұ�����", "s20" );
			int Dx[2] = { 40, -40 };
			X = x + Dx[Dir];
			Y = y - 88;
			temp_x = 0;
			Drtx = Dx[Dir] / 10;
		}
		n_kd++;

		if ( n_kd <= 20 )
		{
			kd = 2;
			if ( x + Drtx > MINX && x + Drtx < MAXX )
				x += Drtx;
			else
				temp_x++;
			temp_y = (x + temp_x * Drtx - X) * (x + temp_x * Drtx - X) / 18 + Y;
		}
		else if ( Map.map_id == 2 && Check_hole(PE_player, player - 1) == _IN
				  || Check_lift(PE_player, player - 1) == _IN )
		{
			if ( Player[player - 1].life >= 0 )
				Player[player - 1].life--;
			temp_y = 0;
			Player[player - 1].hp = Player[player - 1].max_hp;
			Player[player - 1].Set_bol( 0x00 );
			f[player - 1] = &Role::Revive;
			return;
		}

		/*****************/
		int pa[3] = { 21, 49, 60 };
		int pb[3] = { 21, 70, 70 };
		int* p = 0;

		if ( !Player[player - 1].die )
			p = pa;
		else
			p = pb;

		if ( n_kd >= *p && n_kd <= *(++p) )
		{
			if ( n_kd % 4 == 0 )	kd = 3;
			if ( n_kd % 8 == 0 )	kd = 4;
		}

		if ( n_kd > *p && n_kd <= *(++p) )
			kd = 5;

		if ( n_kd > *p )
		{
			Drtx = temp_x = 0;
			Player[player - 1].lying = false;
			Player[player - 1].Set_bol( 0x3f );
			f[player - 1] = &Role::Stand;
			Player[player - 1].be_kick_counter = 0;

			if ( Player[player - 1].die )
			{
				temp_y = 0;
				f[player - 1] = &Role::Revive;
				Player[player - 1].Set_bol( 0x00 );
			}
			w = a = j = ja = jk = jf = lw = kd = df = tr = cm = ka = 0;
			n_w = n_a = n_j = n_ja = n_jk = n_jf = n_lw = n_kd = n_df = n_tr = n_cm = n_ka = 0;
		}
	}
	if ( Player[player - 1].hp <= 0 )	// main �ı����ĺ������� hp <= 0 �����
		n_kd = 0;
}

//

void Role::Revive( int dir, int player )
{
	int n;			// ����ѭ������

	UNREFERENCED_PARAMETER(dir);
	x = 230;		// ��ֹ��������������ȥ
	Player[player - 1].Set_bol( 0x00 );
	Player[player - 1].lying = true;		// �����������̱�����..

	if ( Player[player - 1].life >= 0 )
	{
		for ( n = 0; n < EMENY::emeny_num; n++ )
		{
			// ע�ⲻ�ܷŵ�������һ��ҹ����š�ץ�ŵĵ���!!!!
			if ( Emeny[n].role == player - 1 && Emeny[n].lying == false )
			{
				if ( Emeny[n].be_catch == false )
				{
					mf[n] = &BASE_CLASS::Kick_away;
					Emeny[n].be_kick_counter = 5;
					Emeny[n].start_fly = true;
					Emeny[n].lying = true;
				}
			}
		}

		/****** ֻҪ���� lying �Ķ��ŵ����᲻�� bug ����*******/
		for ( n = 0; n < BOSS::Boss_num; n++ )
			if ( !Boss[n].lying )
			{
				bf[n] = &BASE_BOSS::Kick_away;
				Boss[n].be_kick_counter = 5;
				Boss[n].start_fly = true;
				Boss[n].lying = true;
			}
	}
	else
	{
		y = 0;
		Player[player - 1].appear = false;
		f[player - 1] = &Role::Stand;
		return;
	}

	int miny = Map.min_y[Map.map_id][ Map.part[Map.map_id] ];
	UNREFERENCED_PARAMETER( player );

	if ( temp_y + 15 < miny )
		temp_y += 15;
	else
	{
		n_r = 0;
		y = temp_y = miny;
		Player[player - 1].lying = false;
		Player[player - 1].die = false;
		f[player - 1] = &Role::Stand;
		Player[player - 1].Set_bol( 0x3f );
	}
}

// �漰 x ��λ�仯���������� boss �� kiss ��Χ
// ��ֹ��ұ� kiss Խ��

void Role::Be_kiss( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	int n = player - 1;
	int dy[6]	 = { -20, -10, -20, -10, -20, -10 };
	int dx[2][6] = {{50, 40, 50, 40, 50, 40 },
					{-20, -10, -20, -10, -20, -10 } };

	if ( n_bk % 8 == 0 && n_bk < 48 )
	{
		if ( Player[player - 1].hp >= 10 )
			Player[player - 1].hp -= 2;
		temp_y	= Boss[Player[n].boss].Bp->y + dy[n_bk / 8];
		x		= Boss[Player[n].boss].Bp->x + dx[Dir][n_bk / 8];
	}
	else if ( n_bk >= 48 )
	{
		Player[player - 1].be_kiss = false;
		n_bk = bk = 0;
		f[n] = &Role::Kick_down;
		Player[n].be_kick_counter = 3;
		return;
	}
	n_bk++;
}



/***** Kady ��ʵ�� ********************************************************************************************************/

Kady::Kady()
{
	shoot_knife = false;
	wave_x = wave_y = 0;
	
	/**** �������ֽ�ɫ�����ߣ�ֱ�������У���ȭ *****/
	IMAGE img;
	loadimage( &img, "./res/Images/kady.jpg", 696, 3756 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )
	{
		for ( int x = 0; x < 10; x++ )// [1][][]Ϊ�ҡ� [0][][]Ϊ��
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 80, i * 80, 80, 80 );			// 1
				getimage( &walk[0][x][i], x * 80, 160 + i * 80, 80, 80 );	

				getimage( &jump_attack[1][x][i], x * 80, 960  + i * 80, 80, 80 );	// 4
				getimage( &jump_attack[0][x][i], x * 80, 1120 + i * 80, 80, 80 );

				getimage( &b_blow[1][x][i], x * 80, 1280 + i * 80, 80, 80 );	// ���в�������
				getimage( &b_blow[0][x][i], x * 80, 1280 + i * 80, 80, 80 );	// 5
			}

			if ( x < 2 )
			{
				getimage( &attack[1][x][i], x * 80, 320 + i * 80, 80, 80 ); // 2
				getimage( &attack[0][x][i], x * 80, 480 + i * 80, 80, 80 );
			}

			if ( x < 3 )
			{
				getimage( &knife[1][x][i], 160 + x * 80, 320 + i * 80, 80, 80 );		// 2
				getimage( &knife[0][x][i], 160 + x * 80, 480 + i * 80, 80, 80 );

				getimage( &jump[1][x][i], x * 80, 640 + i * 80, 80, 80 );	// 3
				getimage( &jump[0][x][i], x * 80, 800 + i * 80, 80, 80 );

				getimage( &jump_knee[1][x][i], x * 80, 1440 + i * 80, 80, 80 );	// 6
				getimage( &jump_knee[0][x][i], x * 80, 1600 + i * 80, 80, 80 );
			}

			if ( x < 5 )
			{
				getimage( &throw_role[1][x][i], x * 80, 3056 + i * 80, 80, 80 );	// 11
				getimage( &throw_role[0][x][i], x * 80, 3216 + i * 80, 80, 80 );
			}

			if ( x < 7 )
			{
				getimage( &jump_forward[1][x][i], x * 80, 1760 + i * 80, 80, 80 );	// 7
				getimage( &jump_forward[0][x][i], x * 80, 1920 + i * 80, 80, 80 );

				getimage( &light_wave[1][x][i], x * 80, 2080 + i * 80, 80, 80 );	// 8
				getimage( &light_wave[0][x][i], x * 80, 2240 + i * 80, 80, 80 );
			}

			if ( x < 6 )	// 8��9֮�䲻���ӣ���
			{
				getimage( &kick_down[0][x][i], x * 80, 2416 + i * 80, 80, 80 );	// 9
				getimage( &kick_down[1][x][i], x * 80, 2576 + i * 80, 80, 80 );		// �����е㲻һ��

				getimage( &catch_attack[1][x][i], x * 80, 3376 + i * 80, 80, 80 );	// 12
				getimage( &catch_attack[0][x][i], x * 80, 3536 + i * 80, 80, 80 );
			}

			if ( x < 8 )
			{
				getimage( &dragon_fist[1][x][i], x * 80, 2736 + i * 80, 80, 80 );	// 10
				getimage( &dragon_fist[0][x][i], x * 80, 2896 + i * 80, 80, 80 );
			}
		}
	}
	SetWorkingImage();
}


//

void Kady::Stand( int dir, int player )
{
	if ( !Player[player - 1].catch_emeny )
	{
		putimage( x, y, &jump[Dir][2][1], SRCAND );
		putimage( x, y, &jump[Dir][2][0], SRCINVERT );
	}
	else
	{
		putimage( x, temp_y, &catch_attack[Dir][0][1], SRCAND );
		putimage( x, temp_y, &catch_attack[Dir][0][0], SRCINVERT );
	}
	Role::Stand( dir, player );
}

// ���ຯ�����𴫵��βΡ���� img

void Kady::Walk( int dir, int player )
{
	int dx[8] = { -5, 5,  0, 0,	 4, 4, -4, -4 };
	int dy[8] = {  0, 0, -4, 4,	-3, 3, -3,  3 };

	if ( !Player[player - 1].catch_emeny )
	{
		if ( dir == LEFT && n_j > 0 && n_j < 10 || n_j > 0 && n_j < 10 && dir == RIGHT )
		{
			putimage( x, temp_y, &jump_forward[Dir][jf][1], SRCAND );
			putimage( x, temp_y, &jump_forward[Dir][jf][0], SRCINVERT );
			Role::Walk_all( dir, player, dx, dy );
			return;
		}
		else if ( n_j >= 10 )
		{
			putimage( x, temp_y, &jump[Dir][j][1], SRCAND );
			putimage( x, temp_y, &jump[Dir][j][0], SRCINVERT );
			Role::Walk_all( dir, player, dx, dy );
			return;
		}

		/****** ���ɳ���� ***********************/
		if ( dir == Dir && n_a > 0 && n_a < 4 )
		{
			putimage( x, temp_y, &light_wave[Dir][lw][1], SRCAND );
			putimage( x, temp_y, &light_wave[Dir][lw][0], SRCINVERT );
			Role::Walk_all( dir, player, dx, dy );
			return;
		}
	}

	/*****************************/
	if ( !Player[player - 1].catch_emeny )
	{
		putimage( x, temp_y, &walk[Dir][w][1], SRCAND );
		putimage( x, temp_y, &walk[Dir][w][0], SRCINVERT );
	}
	else
	{
		putimage( x, temp_y, &catch_attack[Dir][cm][1], SRCAND );
		putimage( x, temp_y, &catch_attack[Dir][cm][0], SRCINVERT );
	}
	Role::Walk_all( dir, player, dx, dy );
}

// �ڹ���ǰ��⵱ǰ�Ƿ����ϳɣ����ߡ���Ծϥ�ǹ�����������ǰ��
// ת�����ߵ�ʱ��ֱ��ʹ img �±��Ӧ���ߡ�ͼ
// ��Ϻ�ǵù��㱻��ϵİ�������!!!

void Kady::Attack( int dir, int player )
{
	if ( n_cm > 0 )
	{
		char c[2] = { '\0', '\0' };
		switch ( player )
		{
			case 1:	c[0] = 'A';		c[1] = 'D';			break;
			case 2: c[0] = VK_LEFT;	c[1] = VK_RIGHT;	break;
			default: printf( "kady �� Attack ����" );	break;
		}
		if ( GetAsyncKeyState(c[0]) || GetAsyncKeyState(c[1]) )
		{
			putimage( x, temp_y, &catch_attack[Dir][5][1], SRCAND );
			putimage( x, temp_y, &catch_attack[Dir][5][0], SRCINVERT );
			Role::Attack( dir, player );
			return;
		}

		putimage( x, temp_y, &catch_attack[Dir][5][1], SRCAND );
		putimage( x, temp_y, &catch_attack[Dir][5][0], SRCINVERT );
		Role::Attack( dir, player );
		return;
	}

	/***** ���ߡ���ϥ�ǹ��� ***************/
	if ( n_j > 0 )
	{
		if ( GetAsyncKeyState('S') & 0x8000 && player == 1 ||
			 GetAsyncKeyState(VK_DOWN) & 0x8000 && player == 2 )
		{
			putimage( x, temp_y, &jump_knee[Dir][jk][1], SRCAND );
			putimage( x, temp_y, &jump_knee[Dir][jk][0], SRCINVERT );
		}
		else
		{
			putimage( x, temp_y, &jump_attack[Dir][ja][1], SRCAND );
			putimage( x, temp_y, &jump_attack[Dir][ja][0], SRCINVERT );
		}
		Role::Attack( dir, player );
		return;
	}

	/****** ��ǰ�� --> ��ǰ�ߡ���ǰϥ�ǹ��� ********************/
	if ( n_jf > 0 )
	{
		if ( GetAsyncKeyState('S') & 0x8000 && player == 1 || 
			 GetAsyncKeyState(VK_DOWN) & 0x8000 && player == 2 )
		{
			putimage( x, temp_y, &jump_knee[Dir][jk][1], SRCAND );
			putimage( x, temp_y, &jump_knee[Dir][jk][0], SRCINVERT );
		}
		else
		{
			putimage( x, temp_y, &jump_attack[Dir][ja][1], SRCAND );
			putimage( x, temp_y, &jump_attack[Dir][ja][0], SRCINVERT );
		}
		Role::Attack( dir, player );
		return;
	}

	/****** ����Ƿ��ȡ������ *******/
	if ( WEAPON::out && WEAPON::weapon_kind == Player[player - 1].role_kind &&
		abs(x - WEAPON::x) < 20 && abs(y - WEAPON::y+40) < 20 )
	{
		have_weapon = true;
		WEAPON::out = false;
		weapon_counter = 0;
	}
	if ( have_weapon )
	{
		Player[player - 1].Set_bol( 0x00 );
		f[player - 1] = &Role::Weapon_attack;
		n_a = a = 0;

		int dx[2] = { -20, 20 };
		putimage( x+dx[Dir], y, &knife[Dir][0][1], SRCAND );
		putimage( x+dx[Dir], y, &knife[Dir][0][0], SRCINVERT );
		return;
	}

	/*****************************/
	putimage( x, temp_y, &attack[Dir][a][1], SRCAND );
	putimage( x, temp_y, &attack[Dir][a][0], SRCINVERT );
	Role::Attack( dir, player );
}

//

void Kady::Weapon_attack( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	int dx[2] = { -20, 20 };
	putimage( x+dx[Dir], y, &knife[Dir][a][1], SRCAND );
	putimage( x+dx[Dir], y, &knife[Dir][a][0], SRCINVERT );

	/***** ����Ƿ��ȡ�ļ��� *****/
	if ( FOOD::out && abs(x-FOOD::x) < 20 && abs(FOOD::y - y) < 20 )
	{
		Player[player - 1].hp = Player[player - 1].max_hp;
		FOOD::out = false;
	}

	if ( n_a <= 3 )		a = 0;
	if ( n_a >= 4 )		a = 1;
	if ( n_a == 4 )
		SetSound( NULL, "����", "wa2" );

	if ( !shoot_knife )
	{
		if ( ++n_a > 9 )
		{
			n_a = a = 0;
			Player[player - 1].Set_bol( 0x3f );
			f[player - 1] = &Role::Stand;

			if (++weapon_counter >= 15)
			{
				wave_x = x;	// ���ùⲨ����
				wave_y = y;
				shoot_knife = true;
				Player[player-  1].Set_bol( 0x00 );
				f[player - 1] = &Role::Weapon_attack;

				weapon_counter = 0;
				have_weapon = false;
			}
			return;
		}
	}

	if ( shoot_knife )
	{
		putimage( wave_x, wave_y, &knife[Dir][2][1], SRCAND );
		putimage( wave_x, wave_y, &knife[Dir][2][0], SRCINVERT );

		int dx[2] = { -19, 19 };
		if ( wave_x + dx[Dir] >= MINX && wave_x + dx[Dir] <= MAXX )
			wave_x += dx[Dir];
		else
		{
			shoot_knife = false;
			n_a = a = 0;
			f[player - 1] = &Role::Stand;
			Player[player- 1].Set_bol( 0x3f );
			return;
		}
	}

	/******* ���ɵ������� *******/
	int tx = x;
	int d_x = 80;
	if ( shoot_knife )
	{
		d_x = 30;
		tx = wave_x;
	}

	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( abs(y - Emeny[i].Mp->Gety()) < 18 )
		{
			if ( Dir == LEFT && tx - Emeny[i].Mp->Getx() < d_x && tx > Emeny[i].Mp->Getx() ||
				 Dir ==	RIGHT && Emeny[i].Mp->Getx() - tx < d_x && tx < Emeny[i].Mp->Getx() )
			{
				if ( !Emeny[i].lying && (n_a > 3 || shoot_knife) )
				{
					if ( shoot_knife )
					{
						shoot_knife = false;
						n_a = a = 0;
						f[player - 1] = &Role::Stand;
						Player[player- 1].Set_bol( 0x3f );
					}

					if ( Emeny[i].be_catch )
						Player[player % 2].catch_emeny = false;

					// һ�����֣�ò�Ʋ��ð� Player[..].emeny = i
					Emeny[i].be_catch = false;
					Emeny[i].role = player - 1;
					Emeny[i].be_kick_counter = 5;
					Emeny[i].start_fly = true;
					mf[i] = &BASE_CLASS::Kick_away;
					Emeny[i].Mp->SetDir( RIGHT );

					int _x = Emeny[i].Mp->Getx();
					int _y = Emeny[i].Mp->Gety();
					int dx = 40, dy = 30;

					if ( tx <= _x )
					{
						Emeny[i].Mp->SetDir( LEFT );
						dx = 0;
					}
					putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
					putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

					/***********/

					Player[player - 1].experience += 1;
					Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 7;
				}
			}
		}
	}
}

//

void Kady::Big_Blow( int dir, int player )
{
	if ( Player[player - 1].hp <= 20 )
	{
		putimage( x, y, &jump[Dir][2][1], SRCAND );
		putimage( x, y, &jump[Dir][2][0], SRCINVERT );

		big = true;
		bb = n_bb = 0;
		j  = n_j  = 0;		// ������
		cm = n_cm = 0;
		ka = n_ka = 0;
		a  = n_a  = 0;
		Player[player - 1].Set_bol( 0x3f );		// 1111,11
		Player[player - 1].catch_emeny = false;
		f[player - 1] = &Role::Stand;
		return;
	}

	putimage( x, temp_y - 56, &b_blow[Dir][bb][1], SRCAND );
	putimage( x, temp_y - 56, &b_blow[Dir][bb][0], SRCINVERT );
	Role::Big_Blow( dir, player );
}

// ��Ծ�������������¡�J �ĸ�������
// ���� 10 �����棬�½� 10 �����棬�� 3 ��վ����ͼ
// �����ĸ߶ȱ��� == �½��ĸ߶�

void Kady::Jump( int dir, int player )
{
	putimage( x, temp_y, &jump[Dir][j][1], SRCAND );
	putimage( x, temp_y, &jump[Dir][j][0], SRCINVERT );
	Role::Jump( dir, player );
}

// �����

void Kady::Light_wave( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	putimage( x, temp_y, &light_wave[Dir][lw][1], SRCAND );
	putimage( x, temp_y, &light_wave[Dir][lw][0], SRCINVERT );

	int _lw = 5;
	int dx[2] = { -4, 4 };
	Drtx = dx[Dir];

	switch ( n_lw )
	{
		case 0:	
			lw = 0;
			SetSound( NULL, "���۹Ⲩ", "s7" );
			break;

		case 4:		lw = 1;	break;
		case 9:		lw = 2;	break;
		case 14:	lw = 3;	break;
		case 19:	lw = 4;	break;
		case 10:
			SetSound( NULL, "�����Ⲩ", "s8" );
			break;

		case 15:
			wave_x = x + dx[Dir] * 10;
			wave_y = y;
			break;

		case 30:
			n_lw = lw = 0;
			Player[player - 1].Set_bol( 0x3f );		// 1111,11
			f[player - 1] = &Role::Stand;
			Player[player - 1].skill = NONE;
			return;

		default:
			break;
	}

	/******* ��ʼ�������� ********/
	if ( n_lw >= 15 )
	{
		Player[player - 1].skill = LIGHT_WAVE;
		if ( n_lw % 6 == 0 || n_lw % 6 == 1 || n_lw % 6 == 2 )
			_lw = 5;
		else
			_lw = 6;
		putimage( wave_x, wave_y, &light_wave[Dir][_lw][1], SRCAND );
		putimage( wave_x, wave_y, &light_wave[Dir][_lw][0], SRCINVERT );
		wave_x += int(dx[Dir] * LEVEL::wave[Player[player - 1].level]);

		/********************/
		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( abs(y - Emeny[i].Mp->Gety()) < 18 &&
				 abs(wave_x - Emeny[i].Mp->Getx()) < 50 &&
				 Emeny[i].lying == false )
			{
				SetSound( NULL, "����", "s9" );

				/**** ��ײЧ�� ****/
				int dx[2] = { -10, 50 };
				putimage( wave_x + dx[Dir], y + 20, &PLAYER::light[1], SRCAND );
				putimage( wave_x + dx[Dir], y + 20, &PLAYER::light[0], SRCINVERT );

				/***** �Ϳջ��� *****/
				if ( Emeny[i].be_catch )
					Player[player % 2].catch_emeny = false;

				Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
				Emeny[i].start_fly = true;		// ��ʼ��һ�ַ���
				Emeny[i].be_catch = false;		// ˫��ģʽ���豣֤
				Emeny[i].role = player - 1;
				Emeny[i].be_kick_counter = 5;
				mf[i] = &BASE_CLASS::Kick_away;

				/***** �黹��ɫ����Ȩ ******/
				n_lw = lw = 0;
				Player[player - 1].Set_bol( 0x3f );		// 1111,11
				Player[player - 1].skill = NONE;
				f[player - 1] = &Role::Stand;

				Player[player - 1].experience += 1;
				Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
				return;
			}
		}

		/**********************/
		// boss �� temp_y �� y ��ֵС�� 10 ���ܳ�����˺�
		if ( Map.part[Map.map_id] == 7 ||
			 Map.map_id == 4 && Map.part[4] == 2 ||
			 Map.map_id == 4 && Map.part[4] == 5 )
			for ( int i = 0; i < BOSS::Boss_num; i++ )
				if ( abs(wave_x - Boss[i].Bp->x) < 24 &&
					 abs(y - Boss[i].Bp->y - 20) < 15 &&
					 abs(Boss[i].Bp->y - Boss[i].Bp->temp_y) < 10 && !Boss[i].lying )
				{
					SetSound( NULL, "����", "s9" );
					/**** ��ײЧ�� ****/

					int dx[2] = { -10, 50 };
					putimage( wave_x + dx[Dir], y + 20, &PLAYER::light[1], SRCAND );
					putimage( wave_x + dx[Dir], y + 20, &PLAYER::light[0], SRCINVERT );

					Boss[i].Bp->Dir = ( Dir + 1 ) % 2;
					Boss[i].start_fly = true;
					Boss[i].role = (byte)( player - 1 );
					Boss[i].be_kick_counter = 5;
					bf[i] = &BASE_BOSS::Kick_away;

					n_lw = lw = 0;
					Player[player - 1].Set_bol( 0x3f );		// 1111,11
					Player[player - 1].skill = NONE;
					f[player - 1] = &Role::Stand;

					Player[player - 1].experience += 1;
					Boss[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
					return;
				}
	}
	n_lw++;
}

// ����ȭ

void Kady::Dragon_fist( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	putimage( x, temp_y, &dragon_fist[Dir][df][1], SRCAND );
	putimage( x, temp_y, &dragon_fist[Dir][df][0], SRCINVERT );

	/***** ȷ������ ******/
	if ( n_df == 0 )
	{
		X = x;
		Y = temp_y - 12;
		temp_x = 0;
		Player[player - 1].experience += 1;
	}
	n_df++;

	int dx[2] = { -1, 1 };

	/***** ����ǰ�μ���Ƿ�ɻ��ɵ��� *******/
	if ( n_df < 6 )
		for ( int i = 0; i < EMENY::emeny_num; i++ )
			if ( abs(temp_y - Emeny[i].Mp->Gety()) < 18 && 
				(Dir == LEFT && x - Emeny[i].Mp->Getx() < 70 && x > Emeny[i].Mp->Getx() ||
				 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 70 && x < Emeny[i].Mp->Getx()) &&
				 Emeny[i].lying == false )
			{
				if ( Emeny[i].be_catch )
					Player[player % 2].catch_emeny = false;

				Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
				Emeny[i].start_fly = true;
				Emeny[i].be_catch = false;
				Emeny[i].role = player - 1;
				Emeny[i].be_kick_counter = 4;
				mf[i] = &BASE_CLASS::Kick_away;

				Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
			}

	/******* ��� boss *********/
	if ( n_df < 6 && Map.part[Map.map_id] == 7 ||
		 Map.map_id == 4 && Map.part[4] == 2 ||
		 Map.map_id == 4 && Map.part[4] == 5 )
		for ( int i = 0; i < BOSS::Boss_num; i++ )
			if ( abs(temp_y - Boss[i].Bp->y-20) < 15 && abs(x - Boss[i].Bp->x) < 70 &&
				 Check_position(Dir, x, Boss[i].Bp->x) && !Boss[i].lying )
			{
				Boss[i].Bp->Dir = ( Dir + 1 ) % 2;
				Boss[i].start_fly = true;
				Boss[i].role = (byte)( player - 1 );
				Boss[i].be_kick_counter = 4;
				bf[i] = &BASE_BOSS::Kick_away;
				Boss[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
			}

	/***** �������½�·�� *******/
	if ( n_df <= 16 )
	{
		if ( x + dx[Dir] >= MINX && x + dx[Dir] <= MAXX )
			x += dx[Dir];
		else
			temp_x++;

		temp_y = -(x + temp_x * dx[Dir] - X) * (x + temp_x * dx[Dir] - X) / 2 + Y;
	}
	if ( n_df == 2  )	df = 2;
	if ( n_df == 4  )	df = 3;
	if ( n_df == 6  )	df = 4;
	if ( n_df == 14 )	df = 5;
	if ( n_df == 19 )	df = 6;
	if ( n_df == 25 )	df = 7;
	if ( n_df == 4 )
		SetSound( NULL, "dragon_fist��", "s10" );

	if ( n_df >= 17 && n_df <= 26 )
		temp_y += 14;

	if ( n_df >= 27 )
	{
		SetSound( NULL, "���", "s11" );

		temp_x = 0;
		Player[player - 1].Set_bol( 0x3f );
		f[player - 1] = &Role::Stand;
		n_df = df = 0;
		n_a = a = 0;
	}
}

// ����

void Kady::Throw_role( int dir, int player )
{
	putimage( x, y, &throw_role[Dir][tr][1], SRCAND );
	putimage( x, y, &throw_role[Dir][tr][0], SRCINVERT );
	Role::Throw_role( dir, player );
}

// ����������ͬ�������߹켣�˶���ע�����׼ȷ����ƽ�� ������ == ��ص�
// Խ���Ҫ���ּ�������

void Kady::Jump_forward( int dir, int player )
{
	Role::Jump_forward( dir, player );
	putimage( x, temp_y, &jump_forward[Dir][jf][1], SRCAND );
	putimage( x, temp_y, &jump_forward[Dir][jf][0], SRCINVERT );
}

// ֱ�� + ǰ��

void Kady::Jump_attack( int dir, int player )
{
	Role::Jump_attack( dir, player );
	putimage( x, temp_y, &jump_attack[Dir][ja][1], SRCAND );
	putimage( x, temp_y, &jump_attack[Dir][ja][0], SRCINVERT );
}

// ����ϥ�ǹ���(��ֱ����ǰ)
// ���е���Ծ�����ı� y ֵ

void Kady::Jump_knee( int dir, int player )
{
	Role::Jump_knee( dir, player );
	putimage( x, temp_y, &jump_knee[Dir][jk][1], SRCAND );
	putimage( x, temp_y, &jump_knee[Dir][jk][0], SRCINVERT );
}

// ���� catch_attack
// ץ�˺�ϥ�ǹ���

void Kady::Knee_attack( int dir, int player )
{
	putimage( x, temp_y, &catch_attack[Dir][ka][1], SRCAND );
	putimage( x, temp_y, &catch_attack[Dir][ka][0], SRCINVERT );
	Role::Knee_attack( dir, player );
}

// �����С�����

void Kady::Kick_down( int dir, int player )
{
	shoot_knife = false;
	int k = 1;
	if ( Player[player - 1].die )
		k = 2;

	if ( Player[player - 1].be_kick_counter < 3 )
	{
		putimage( x, temp_y, &kick_down[Dir][kd][1], SRCAND );
		putimage( x, temp_y, &kick_down[Dir][kd][0], SRCINVERT );
	}
	else if ( n_kd % k == 0 )
	{
		putimage( x, temp_y, &kick_down[Dir][kd][1], SRCAND );
		putimage( x, temp_y, &kick_down[Dir][kd][0], SRCINVERT );
	}
	Role::Kick_down( dir, player );
}

//

void Kady::Revive( int dir, int player )
{
	if ( ++n_r % 2 == 0 )
	{
		putimage( x, temp_y, &jump[Dir][1][1], SRCAND );
		putimage( x, temp_y, &jump[Dir][1][0], SRCINVERT );
	}
	Role::Revive( dir, player );
}

//

void Kady::Be_kiss( int dir, int player )
{
	Role::Be_kiss( dir, player );
	putimage( x, temp_y, &kick_down[Dir][1][1], SRCAND );
	putimage( x, temp_y, &kick_down[Dir][1][0], SRCINVERT );
}



/***** Heye ���ʵ�� ************************************************************************************************/

Heye::Heye()
{
	weapx = weapy = 0;
	IMAGE img;
	loadimage( &img, "./res/Images/heye.jpg", 648, 3804 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )
	{
		for ( int x = 0; x < 7; x++ )
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 80, i * 80, 80, 80 );			// 1
				getimage( &walk[0][x][i], x * 80, 160 + i * 80, 80, 80 );

				getimage( &b_blow[1][x][i], x * 80, 1280 + i * 80, 80, 80 );		// 5
				getimage( &b_blow[0][x][i], x * 80, 1280 + i * 80, 80, 80 );
			}

			if ( x < 6 )
			{
				getimage( &kick_down[0][x][i], x * 80, 2400 + i * 80, 80, 80 );	// 9
				getimage( &kick_down[1][x][i], x * 80, 2560 + i * 80, 80, 80 );

				getimage( &catch_attack[1][x][i], x * 80, 3360 + i * 80, 80, 80 );	// 12
				getimage( &catch_attack[0][x][i], x * 80, 3520 + i * 80, 80, 80 );
			}

			if ( x < 3 )
			{
				getimage( &jump[1][x][i], x * 80, 640 + i * 80, 80, 80 );		// 3
				getimage( &jump[0][x][i], x * 80, 800 + i * 80, 80, 80 );

				getimage( &jump_attack[1][x][i], x * 80, 960  + i * 80, 80, 80 );		// 4
				getimage( &jump_attack[0][x][i], x * 80, 1120 + i * 80, 80, 80 );

				getimage( &jump_knee[1][x][i], x * 80, 1440 + i * 80, 80, 80 );		// 6
				getimage( &jump_knee[0][x][i], x * 80, 1600 + i * 80, 80, 80 );

				getimage( &weapon[1][x][i], 240 + x * 80, 640 + i * 80, 80, 80 );		// 7
				getimage( &weapon[0][x][i], 240 + x * 80, 800 + i * 80, 80, 80 );
			}

			if ( x < 7 )
			{
				getimage( &attack[1][x][i], x * 80, 320 + i * 80, 80, 80 );	// 2
				getimage( &attack[0][x][i], x * 80, 480 + i * 80, 80, 80 );

				getimage( &jump_forward[1][x][i], x * 80, 1760 + i * 80, 80, 80 );		// 7
				getimage( &jump_forward[0][x][i], x * 80, 1920 + i * 80, 80, 80 );

				getimage( &light_wave[1][x][i], x * 80, 2080 + i * 80, 80, 80 );		// 8
				getimage( &light_wave[0][x][i], x * 80, 2240 + i * 80, 80, 80 );

				getimage( &dragon_fist[1][x][i], x * 80, 2720 + i * 80, 80, 80 );	// 10
				getimage( &dragon_fist[0][x][i], x * 80, 2880 + i * 80, 80, 80 );

				getimage( &throw_role[1][x][i], x * 80, 3040 + i * 80, 80, 80 );	// 11
				getimage( &throw_role[0][x][i], x * 80, 3200 + i * 80, 80, 80 );
			}
		}
	}
	SetWorkingImage();
}

//

void Heye::Stand( int dir, int player )
{
	if ( !Player[player - 1].catch_emeny )
	{
		putimage( x, y, &jump[Dir][2][1], SRCAND );
		putimage( x, y, &jump[Dir][2][0], SRCINVERT );
	}
	else
	{
		putimage( x, temp_y, &catch_attack[Dir][0][1], SRCAND );
		putimage( x, temp_y, &catch_attack[Dir][0][0], SRCINVERT );
	}

	Role::Stand( dir, player );
}

//

void Heye::Walk( int dir, int player )
{
	int dx[8] = { -5, 5,  0, 0,	 4, 4, -4, -4 };
	int dy[8] = {  0, 0, -4, 4,	-3, 3, -3,  3 };

	if ( !Player[player - 1].catch_emeny )
	{
		if ( dir == LEFT && n_j > 0 && n_j < 10 || n_j > 0 && n_j < 10 && dir == RIGHT )
		{
			putimage( x, temp_y, &jump_forward[Dir][jf][1], SRCAND );
			putimage( x, temp_y, &jump_forward[Dir][jf][0], SRCINVERT );
			Role::Walk_all( dir, player, dx, dy );	// �������ú��ı��ж����������޷� if��Ӧ����
			return;
		}
		else if ( n_j >= 10 )
		{
			putimage( x, temp_y, &jump[Dir][j][1], SRCAND );
			putimage( x, temp_y, &jump[Dir][j][0], SRCINVERT );
			Role::Walk_all( dir, player, dx, dy );
			return;
		}

		/****** ���ɵ����� ***********************/
		if ( dir == Dir && n_a > 0 && n_a < 4 )
		{
			putimage( x, temp_y, &light_wave[Dir][lw][1], SRCAND );
			putimage( x, temp_y, &light_wave[Dir][lw][0], SRCINVERT );
			Role::Walk_all( dir, player, dx, dy );
			return;
		}
	}

	/*****************************/
	if ( !Player[player - 1].catch_emeny )
	{
		putimage( x, temp_y, &walk[Dir][w][1], SRCAND );
		putimage( x, temp_y, &walk[Dir][w][0], SRCINVERT );
	}
	else
	{
		putimage( x, temp_y, &catch_attack[Dir][cm][1], SRCAND );
		putimage( x, temp_y, &catch_attack[Dir][cm][0], SRCINVERT );
	}
	Role::Walk_all( dir, player, dx, dy );
}

//

void Heye::Attack( int dir, int player )
{
	if ( n_cm > 0 )
	{
		char c[2] = { '\0', '\0' };
		switch ( player )
		{
			case 1:	c[0] = 'A';		c[1] = 'D';			break;
			case 2: c[0] = VK_LEFT;	c[1] = VK_RIGHT;	break;
			default: printf( "kady �� Attack ����" );	break;
		}
		if ( GetAsyncKeyState(c[0]) || GetAsyncKeyState(c[1]) )
		{
			putimage( x, temp_y, &catch_attack[Dir][5][1], SRCAND );
			putimage( x, temp_y, &catch_attack[Dir][5][0], SRCINVERT );
			Role::Attack( dir, player );
			return;
		}
		putimage( x, temp_y, &catch_attack[Dir][5][1], SRCAND );
		putimage( x, temp_y, &catch_attack[Dir][5][0], SRCINVERT );
		Role::Attack( dir, player );
		return;
	}

	/***** ���ߡ���ϥ�ǹ��� ***************/
	if ( n_j > 0 )
	{
		if ( GetAsyncKeyState('S') & 0x8000 && player == 1 ||
			 GetAsyncKeyState(VK_DOWN) & 0x8000 && player == 2 )
		{
			putimage( x, temp_y, &jump_knee[Dir][jk][1], SRCAND );
			putimage( x, temp_y, &jump_knee[Dir][jk][0], SRCINVERT );	
		}
		else
		{
			putimage( x, temp_y, &jump_attack[Dir][ja][1], SRCAND );
			putimage( x, temp_y, &jump_attack[Dir][ja][0], SRCINVERT );
		}
		Role::Attack( dir, player );
		return;
	}

	/****** ��ǰ�� --> ��ǰ�ߡ���ǰϥ�ǹ��� ********************/
	if ( n_jf > 0 )
	{
		if ( GetAsyncKeyState('S') & 0x8000 && player == 1 || 
			 GetAsyncKeyState(VK_DOWN) & 0x8000 && player == 2 )
		{
			putimage( x, temp_y, &jump_knee[Dir][jk][1], SRCAND );
			putimage( x, temp_y, &jump_knee[Dir][jk][0], SRCINVERT );
		}
		else
		{
			putimage( x, temp_y, &jump_attack[Dir][ja][1], SRCAND );
			putimage( x, temp_y, &jump_attack[Dir][ja][0], SRCINVERT );
		}
		Role::Attack( dir, player );
		return;
	}
	putimage( x, temp_y, &attack[Dir][a][1], SRCAND );
	putimage( x, temp_y, &attack[Dir][a][0], SRCINVERT );

	/****** ����Ƿ��ȡ������ *******/
	if ( WEAPON::out && WEAPON::weapon_kind == Player[player - 1].role_kind &&
		abs(x - WEAPON::x) < 20 && abs(y - WEAPON::y+40) < 20 )
	{
		have_weapon = true;
		WEAPON::out = false;
		weapon_counter = 0;
	}

	if ( have_weapon )
	{
		Player[player - 1].Set_bol( 0x00 );
		f[player - 1] = &Role::Weapon_attack;
		n_a = a = 0;
		return;
	}

	Role::Attack( dir, player );
}

//

void Heye::Weapon_attack( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	putimage( x, y, &weapon[Dir][a][1], SRCAND );
	putimage( x, y, &weapon[Dir][a][0], SRCINVERT );

	/****** ����Ƿ��ȡ�ļ��� *****/
	if ( FOOD::out && abs(x-FOOD::x) < 20 && abs(FOOD::y - y) < 20 )
	{
		Player[player - 1].hp = Player[player - 1].max_hp;
		FOOD::out = false;
	}

	if ( n_a <= 3 )		a = 0;
	if ( n_a >= 4 )		a = 1;

	if ( n_a == 4 )
	{
		SetSound( NULL, "����", "wa12" );
		weapx = x;
		weapy = y;
	}

	/******* ������� *******/
	if ( ++n_a > 4 )
	{
		putimage( weapx, weapy, &weapon[Dir][2][1], SRCAND );
		putimage( weapx, weapy, &weapon[Dir][2][0], SRCINVERT );

		int dx[2] = { -19, 19 };
		if ( weapx + dx[Dir] >= MINX && weapx + dx[Dir] <= MAXX )
			weapx += dx[Dir];
		else
		{
			n_a = a = 0;
			f[player - 1] = &Role::Stand;
			Player[player- 1].Set_bol( 0x3f );

			if (++weapon_counter >= 15)
			{
				weapon_counter = 0;
				have_weapon = false;
			}
			return;
		}

		// ���е������ò���
		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( abs(weapy - Emeny[i].Mp->Gety()) < 18 )
			{
				if ( Dir == LEFT && weapx - Emeny[i].Mp->Getx() < 30 && weapx > Emeny[i].Mp->Getx() ||
					 Dir ==	RIGHT && Emeny[i].Mp->Getx() - weapx < 30 && weapx < Emeny[i].Mp->Getx() )
				{
					if ( !Emeny[i].lying && n_a > 4 )
					{
						n_a = a = 0;
						f[player - 1] = &Role::Stand;
						Player[player- 1].Set_bol( 0x3f );
						if (++weapon_counter >= 15)
						{
							weapon_counter = 0;
							have_weapon = false;
						}

						if ( Emeny[i].be_catch )
							Player[player % 2].catch_emeny = false;

						// һ�����֣�ò�Ʋ��ð� Player[..].emeny = i
						Emeny[i].be_catch = false;
						Emeny[i].role = player - 1;
						Emeny[i].be_kick_counter = 5;
						Emeny[i].start_fly = true;
						mf[i] = &BASE_CLASS::Kick_away;
						Emeny[i].Mp->SetDir( RIGHT );

						int _x = Emeny[i].Mp->Getx();
						int _y = Emeny[i].Mp->Gety();
						int dx = 40, dy = 30;

						if ( weapx <= _x )
						{
							Emeny[i].Mp->SetDir( LEFT );
							dx = 0;
						}
						putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
						putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

						/***********/
						Player[player - 1].experience += 1;
						Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 7;
					}
				}
			}
		}
	}
}

//

void Heye::Big_Blow( int dir, int player )
{
	if ( Player[player - 1].hp <= 20 )
	{
		putimage( x, y, &jump[Dir][2][1], SRCAND );
		putimage( x, y, &jump[Dir][2][0], SRCINVERT );

		big = true;
		bb = n_bb = 0;
		j  = n_j  = 0;		// ������
		cm = n_cm = 0;
		ka = n_ka = 0;
		a  = n_a  = 0;
		Player[player - 1].Set_bol( 0x3f );		// 1111,11
		Player[player - 1].catch_emeny = false;
		f[player - 1] = &Role::Stand;
		return;
	}

	putimage( x, temp_y - 56, &b_blow[Dir][bb][1], SRCAND );
	putimage( x, temp_y - 56, &b_blow[Dir][bb][0], SRCINVERT );
	Role::Big_Blow( dir, player );
}

//

void Heye::Jump( int dir, int player )
{
	putimage( x, temp_y, &jump[Dir][j][1], SRCAND );
	putimage( x, temp_y, &jump[Dir][j][0], SRCINVERT );
	Role::Jump( dir, player );
}

// ������

void Heye::Light_wave( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	putimage( x, y, &light_wave[Dir][lw][1], SRCAND );
	putimage( x, y, &light_wave[Dir][lw][0], SRCINVERT );

	int counter = 4;

	if ( lw == 5 )
		counter = 10;
	else
		counter = 4;

	if ( n_lw == 4 )
		SetSound( NULL, "������1", "h6" );
	if ( n_lw == 16 )
		SetSound( NULL, "������2", "h7" );

	n_lw++;
	if ( (n_lw + 1) % counter == 0 )
		lw++;

	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( Dir == LEFT &&  x - Emeny[i].Mp->Getx() < 70 && x > Emeny[i].Mp->Getx() ||
			 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 70 && x < Emeny[i].Mp->Getx() )
			if ( abs(y - Emeny[i].Mp->Gety()) < 18 && Emeny[i].lying == false )
			{
				SetSound( NULL, "����", "h8" );
				/**** ��ײЧ�� ****/
				int _x = Emeny[i].Mp->Getx();
				int _y = Emeny[i].Mp->Gety();
				int dx = 40, dy = 30;

				if ( x <= _x )
					dx = 0;
				putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
				putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

				/***** �Ϳջ��� *****/
				if ( Emeny[i].be_catch )
					Player[player % 2].catch_emeny = false;

				Emeny[i].start_fly = true;		// ��ʼ��һ�ַ���
				Emeny[i].be_catch = false;		// ˫��ģʽ���豣֤
				Emeny[i].role = player - 1;
				Emeny[i].be_kick_counter = 5;
				Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
				mf[i] = &BASE_CLASS::Kick_away;

				Player[player - 1].experience += 1;
				Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
			}
	}

	/**********************/
	// ���� boss ��Ծ���ͨͨ��Ч
///// ò�������Ĺ����������е� boss �����Ա���������������������
	if ( Map.part[Map.map_id] == 7 ||
		 Map.map_id == 4 && Map.part[4] == 2 ||
		 Map.map_id == 4 && Map.part[4] == 5 )
		for ( int i = 0; i < BOSS::Boss_num; i++ )
			if ( abs(x - Boss[i].Bp->x) < 60 && Check_position(Dir, x, Boss[i].Bp->x) &&
				 abs(y - Boss[i].Bp->y-20) < 15 && !Boss[i].lying )
			{
				SetSound( NULL, "����", "h8" );

				int _x = Boss[i].Bp->x;
				int _y = Boss[i].Bp->y;
				int dx = 50, dy = 44;
				if ( x <= _x )
					dx = 0;

				putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
				putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

				Boss[i].start_fly = true;
				Boss[i].role = (byte)( player - 1 );
				Boss[i].be_kick_counter = 5;
				Boss[i].Bp->Dir = (Dir + 1) % 2;
				bf[i] = &BASE_BOSS::Kick_away;

				Player[player - 1].experience += 1;
				Boss[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
			}

	if ( lw > 6 )
	{
		n_lw = lw = 0;
		Player[player - 1].Set_bol( 0x3f );		// 1111,11
		f[player - 1] = &Role::Stand;
	}
}

// ������

void Heye::Dragon_fist( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	if ( n_df <= 9 )	
		temp_y -= 7;

	if ( n_df >= 10 && n_df <= 19 )
		temp_y += 7;

	if ( n_df == 0 )
		Player[player - 1].experience += 1;

	putimage( x, temp_y, &dragon_fist[Dir][df][1], SRCAND );
	putimage( x, temp_y, &dragon_fist[Dir][df][0], SRCINVERT );
	n_df++;

	if ( n_df < 8 )
		for ( int i = 0; i < EMENY::emeny_num; i++ )
			if ( abs(temp_y - Emeny[i].Mp->Gety()) < 18 && 
				(Dir == LEFT &&  x - Emeny[i].Mp->Getx() < 70 && x > Emeny[i].Mp->Getx() ||
				 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 70 && x < Emeny[i].Mp->Getx()) &&
				 Emeny[i].lying == false )
			{
				if ( Emeny[i].be_catch )
					Player[player % 2].catch_emeny = false;

				Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
				Emeny[i].start_fly = true;
				Emeny[i].be_catch = false;
				Emeny[i].role = player - 1;
				Emeny[i].be_kick_counter = 4;
				mf[i] = &BASE_CLASS::Kick_away;

				Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
			}

	/******* ��� boss *********/
	if ( n_df < 8 && Map.part[Map.map_id] == 7 ||
		 Map.map_id == 4 && Map.part[4] == 2 ||
		 Map.map_id == 4 && Map.part[4] == 5 )
		for ( int i = 0; i < BOSS::Boss_num; i++ )
			if ( abs(temp_y - Boss[i].Bp->y-20) < 15 && abs(x - Boss[i].Bp->x) < 70 &&
				 Check_position(Dir, x, Boss[i].Bp->x) && !Boss[i].lying )
			{
				Boss[i].Bp->Dir = ( Dir + 1 ) % 2;
				Boss[i].start_fly = true;
				Boss[i].role = (byte)( player - 1 );
				Boss[i].be_kick_counter = 4;
				bf[i] = &BASE_BOSS::Kick_away;
				Boss[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
			}

	switch ( n_df )
	{
		case 2:		df = 1;		break;
		case 4:		df = 2;	SetSound( NULL, "������", "h9" );	break;
		case 6:		df = 3;		break;
		case 12:	df = 4;		break;
		case 15:	df = 5;		break;
		case 18:	df = 6;		break;
		case 21:
			n_a = a = 0;
			n_df = df = 0;
			f[player - 1] = &Role::Stand;
			Player[player - 1].Set_bol( 0x3f );
			break;
		default:
			break;
	}
}

//

void Heye::Throw_role( int dir, int player )
{
	putimage( x, y, &throw_role[Dir][tr][1], SRCAND );
	putimage( x, y, &throw_role[Dir][tr][0], SRCINVERT );
	Role::Throw_role( dir, player );
}

// ǰ��

void Heye::Jump_forward(int dir, int player )
{
	Role::Jump_forward( dir, player );
	putimage( x, temp_y, &jump_forward[Dir][jf][1], SRCAND );
	putimage( x, temp_y, &jump_forward[Dir][jf][0], SRCINVERT );
}

//

void Heye::Jump_attack( int dir, int player )
{
	putimage( x, temp_y, &jump_attack[Dir][ja][1], SRCAND );
	putimage( x, temp_y, &jump_attack[Dir][ja][0], SRCINVERT );
	Role::Jump_attack( dir, player );
}

//

void Heye::Jump_knee( int dir, int player )
{
	Role::Jump_knee( dir, player );
	putimage( x, temp_y, &jump_knee[Dir][jk][1], SRCAND );
	putimage( x, temp_y, &jump_knee[Dir][jk][0], SRCINVERT );
}

//

void Heye::Knee_attack( int dir, int player )
{
	putimage( x, temp_y, &catch_attack[Dir][ka][1], SRCAND );
	putimage( x, temp_y, &catch_attack[Dir][ka][0], SRCINVERT );
	Role::Knee_attack( dir, player );
}

//

void Heye::Kick_down( int dir, int player )
{
	int k = 1;
	if ( Player[player - 1].die )
		k = 2;

	if ( Player[player - 1].be_kick_counter < 3 )
	{
		putimage( x, temp_y, &kick_down[Dir][kd][1], SRCAND );
		putimage( x, temp_y, &kick_down[Dir][kd][0], SRCINVERT );
	}
	else if ( n_kd % k == 0 )
	{
		putimage( x, temp_y, &kick_down[Dir][kd][1], SRCAND );
		putimage( x, temp_y, &kick_down[Dir][kd][0], SRCINVERT );
	}
	Role::Kick_down( dir, player );
}


//

void Heye::Revive( int dir, int player )
{
	if ( ++n_r % 2 == 0 )
	{
		putimage( x, temp_y, &jump[Dir][1][1], SRCAND );
		putimage( x, temp_y, &jump[Dir][1][0], SRCINVERT );
	}
	Role::Revive( dir, player );
}

//

void Heye::Be_kiss( int dir, int player )
{
	Role::Be_kiss( dir, player );
	putimage( x, temp_y, &kick_down[Dir][1][1], SRCAND );
	putimage( x, temp_y, &kick_down[Dir][1][0], SRCINVERT );
}



/****** Jack ��ʵ�� *******************************************************************************/

Jack::Jack()
{
	thump = false;

	IMAGE img;
	loadimage( &img, "./res/Images/jack.jpg", 540, 3396 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )
	{
		for ( int x = 0; x < 6; x++ )
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 80, i * 80, 80, 80 );			// 1
				getimage( &walk[0][x][i], x * 80, 160 + i * 80, 80, 80 );

				getimage( &b_blow[1][x][i], x * 80, 1280 + i * 80, 80, 80 );		// 5
				getimage( &b_blow[0][x][i], x * 80, 1280 + i * 80, 80, 80 );

				getimage( &throw_role[0][x][i], x * 80, 2720 + i * 80, 80, 80 );	// 11
				getimage( &throw_role[1][x][i], x * 80, 2880 + i * 80, 80, 80 );	// �ϳ�ʱ�㷴��
			}

			if ( x < 3 )
			{
				getimage( &attack[1][x][i], x * 80, 320 + i * 80, 80, 80 );	// 2
				getimage( &attack[0][x][i], x * 80, 480 + i * 80, 80, 80 );

				getimage( &hammer[1][x][i], 240 + x * 100, 320 + i * 100, 100, 100 );	// 2
				getimage( &hammer[0][x][i], 240 + x * 100, 520 + i * 100, 100, 100 );

				getimage( &jump[1][x][i], x * 80, 640 + i * 80, 80, 80 );	// 3
				getimage( &jump[0][x][i], x * 80, 800 + i * 80, 80, 80 );

				getimage( &jump_attack[1][x][i], x * 80, 960  + i * 80, 80, 80 );	// 4
				getimage( &jump_attack[0][x][i], x * 80, 1120 + i * 80, 80, 80 );

				getimage( &jump_knee[1][x][i], x * 80, 1440 + i * 80, 80, 80 );		// 6
				getimage( &jump_knee[0][x][i], x * 80, 1600 + i * 80, 80, 80 );

				getimage( &jump_forward[1][x][i], x * 80, 640 + i * 80, 80, 80 );		// 7
				getimage( &jump_forward[0][x][i], x * 80, 800 + i * 80, 80, 80 );
			}

			if ( x < 2 )
			{
				getimage( &light_wave[1][x][i], x * 80, 1760 + i * 80, 80, 80 );	// 8
				getimage( &light_wave[0][x][i], x * 80, 1920 + i * 80, 80, 80 );
			}

			getimage( &kick_down[0][x][i], x * 80, 2080 + i * 80, 80, 80 );		// 9 �����е㲻һ��
			getimage( &kick_down[1][x][i], x * 80, 2240 + i * 80, 80, 80 );

			getimage( &catch_attack[1][x][i], x * 80, 3040 + i * 80, 80, 80 );	// 12
			getimage( &catch_attack[0][x][i], x * 80, 3200 + i * 80, 80, 80 );

			if ( x < 5 )
			{
				getimage( &dragon_fist[1][x][i], x * 80, 2400 + i * 80, 80, 80 );	// 10
				getimage( &dragon_fist[0][x][i], x * 80, 2560 + i * 80, 80, 80 );
			}
		}
	}
	SetWorkingImage();
}

//

void Jack::Stand( int dir, int player )
{
	if ( !Player[player - 1].catch_emeny )
	{
		putimage( x, y, &jump[Dir][2][1], SRCAND );
		putimage( x, y, &jump[Dir][2][0], SRCINVERT );
		Drtx = 0;
	}
	else
	{
		putimage( x, temp_y, &catch_attack[Dir][0][1], SRCAND );
		putimage( x, temp_y, &catch_attack[Dir][0][0], SRCINVERT );
	}
	Role::Stand( dir, player );
}

//

void Jack::Walk( int dir, int player )
{
	int dx[8] = { -4, 4,  0, 0,	 3, 3, -3, -3 };
	int dy[8] = {  0, 0, -3, 3,	-2, 2, -2,  2 };

		if ( dir == LEFT && n_j > 0 && n_j < 10 || n_j > 0 && n_j < 10 && dir == RIGHT )
		{
			putimage( x, temp_y, &jump_forward[Dir][jf][1], SRCAND );
			putimage( x, temp_y, &jump_forward[Dir][jf][0], SRCINVERT );
			Walk_all( dir, player, dx, dy );
			return;
		}
		else if ( n_j >= 10 )
		{
			putimage( x, temp_y, &jump[Dir][j][1], SRCAND );
			putimage( x, temp_y, &jump[Dir][j][0], SRCINVERT );
			Walk_all( dir, player, dx, dy );
			return;
		}

		/****** ���ɷ��� ***********************/
		if ( !Player[player - 1].catch_emeny )
		{
			if ( dir == Dir && n_a > 0 && n_a < 4 )
			{
				putimage( x, temp_y, &light_wave[Dir][lw][1], SRCAND );
				putimage( x, temp_y, &light_wave[Dir][lw][0], SRCINVERT );
				Walk_all( dir, player, dx, dy );
				return;
			}
		}
	
	/*****************************/
	if ( !Player[player - 1].catch_emeny )
	{
		putimage( x, temp_y, &walk[Dir][w][1], SRCAND );
		putimage( x, temp_y, &walk[Dir][w][0], SRCINVERT );
	}
	else
	{
		putimage( x, temp_y, &catch_attack[Dir][cm][1], SRCAND );
		putimage( x, temp_y, &catch_attack[Dir][cm][0], SRCINVERT );
	}
	Walk_all( dir, player, dx, dy );
}

//

void Jack::Attack( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	/***** ץ�˺��� ****/
	if ( n_cm > 0 )
	{
		if ( n_j + n_jf != 0 )
		{
			int _x[2] = { -6, 6 };
			if ( n_j > 0 )
			{
				Drtx = 0;
				n_jk = n_j;
				n_j = j = 0;
			}
			else
			{
				n_jk = n_jf;
				Drtx = _x[Dir];
				n_jf = jf = 0;
			}
			n_a = a = 0;

			// ò����ת���� Jump_knee() ��ʱ����Ҳ��ᱻ�ж�
			// ��Ϊ n_jk = n_j/n_jf;
			// ͬʱʹ���� lying = true���ɱ�����˱��ж�
			thump = true;
			Player[player - 1].skill = THUMP;
			Emeny[Player[player - 1].emeny].lying = true;

			Player[player - 1].Set_bol( 0x00 );		// 0000,00
			f[player - 1] = &Role::Jump_knee;

			putimage( x, temp_y, &jump_knee[Dir][jk][1], SRCAND );
			putimage( x, temp_y, &jump_knee[Dir][jk][0], SRCINVERT );
			return;
		}

		/*************/
		char c[2] = { '\0', '\0' };
		switch ( player )
		{
			case 1:	c[0] = 'A';		c[1] = 'D';			break;
			case 2: c[0] = VK_LEFT;	c[1] = VK_RIGHT;	break;
			default: printf( "kady �� Attack ����" );	break;
		}

		if ( n_j + n_jf == 0 )
		{
			if ( GetAsyncKeyState(c[0]) || GetAsyncKeyState(c[1]) )
			{
				if ( GetAsyncKeyState(c[0]) )
				{
					if ( Dir == LEFT )
						Player[player - 1].throw_dir = FORWARD;
					else
						Player[player - 1].throw_dir = BACK;
					Emeny[Player[player - 1].emeny].Mp->SetDir( LEFT );
				}

				if ( GetAsyncKeyState(c[1]) )
				{
					if ( Dir == RIGHT )
						Player[player - 1].throw_dir = FORWARD;
					else
						Player[player - 1].throw_dir = BACK;
					Emeny[Player[player - 1].emeny].Mp->SetDir( RIGHT );
				}

				n_ka = ka = 0;
				n_cm = cm = 0;
				Player[player - 1].skill = THROW_BACK;
				Player[player - 1].Set_bol( 0x00 );
				Emeny[Player[player - 1].emeny].lying = true;
				f[player - 1] = &Role::Throw_role;
				tr = 1;

				putimage( x, temp_y, &catch_attack[Dir][5][1], SRCAND );
				putimage( x, temp_y, &catch_attack[Dir][5][0], SRCINVERT );
				return;
			}
			else
			{
				/**** ͷ���� ***/
				Player[player - 1].Set_bol( 0x00 );
				n_ka = 0;
				ka = 5;
				f[player - 1] = &Role::Knee_attack;

			//	�����ڴ����ø�ֵ����Ϊ��ִ�� Knee_attack() ֮ǰ
			//	�п��ܱ����˹�����ת�� Kick_down()�����µ��˵� lying ���ࡣ
			//	Emeny[Player[player - 1].emeny].lying = true;

				putimage( x, temp_y, &catch_attack[Dir][5][1], SRCAND );
				putimage( x, temp_y, &catch_attack[Dir][5][0], SRCINVERT );
				return;
			}
		}
	}

	/***** ���� ***************/
	if ( n_j > 0 || n_jf > 0 )
	{
		int _x[2] = { -6, 6 };
		if ( n_j > 0 )
		{
			Drtx = 0;
			n_ja = n_j;
			n_j = j = 0;
		}
		else
		{
			n_ja = n_jf;
			Drtx = _x[Dir];
			n_jf = jf = 0;
		}
		ja = 1;
		n_a = a = 0;

		f[player - 1] = &Role::Jump_attack;
		Player[player - 1].Set_bol( 0x00 );		// 0000,00
		putimage( x, temp_y, &jump_attack[Dir][ja][1], SRCAND );
		putimage( x, temp_y, &jump_attack[Dir][ja][0], SRCINVERT );
		return;
	}

	/*****************************/
	putimage( x, temp_y, &attack[Dir][a][1], SRCAND );
	putimage( x, temp_y, &attack[Dir][a][0], SRCINVERT );

	/****** ����Ƿ��ȡ�ļ��� *****/
	if ( FOOD::out && abs(x-FOOD::x) < 20 && abs(FOOD::y - y) < 20 )
	{
		Player[player - 1].hp = Player[player - 1].max_hp;
		FOOD::out = false;
	}

	/****** ����Ƿ��ȡ������ *******/
	if ( WEAPON::out && WEAPON::weapon_kind == Player[player - 1].role_kind &&
		abs(x - WEAPON::x) < 20 && abs(y - WEAPON::y+40) < 20 )
	{
		have_weapon = true;
		WEAPON::out = false;
		weapon_counter = 0;
	}

	if ( have_weapon )
	{
		Player[player - 1].Set_bol( 0x00 );
		f[player - 1] = &Role::Weapon_attack;
		n_a = a = 0;
		return;
	}

	if ( n_a % 4 == 0 )
		a = (++a > 1) ? 0 : a;
	n_a++;

	/*************/
	char L = '\0', R = '\0';
	switch ( player )
	{
		case 1:		L = 'A'; R = 'D';			break;
		case 2:		L = VK_LEFT; R = VK_RIGHT;	break;
		default:	printf("������������");		break;
	}

	if ( GetAsyncKeyState(L) & 0x8000 )	Dir = LEFT;
	if ( GetAsyncKeyState(R) & 0x8000 ) Dir = RIGHT;

	if ( n_a % 9 == 3 )
		SetSound( NULL, "��ȭ", "j" );

	/*************/
	if ( !GetAsyncKeyState('J') && player == 1 || !GetAsyncKeyState(VK_NUMPAD1) && player == 2 )
	{
		n_a = a = 0;
		f[player - 1] = &Role::Stand;
	}

	/***** ɨ���Ƿ���е��� **************/
	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( abs(y - Emeny[i].Mp->Gety()) < 18 )
		{
			if ( Dir == LEFT &&  x - Emeny[i].Mp->Getx() < 50 && x > Emeny[i].Mp->Getx() ||
				 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 50 && x < Emeny[i].Mp->Getx() )
			{
				if ( !Emeny[i].lying && n_a % 4 > 1 )
				{
					if ( Emeny[i].be_catch )
						Player[player % 2].catch_emeny = false;

					Emeny[i].be_catch = false;
					Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
					Emeny[i].role = player - 1;
					mf[i] = &BASE_CLASS::Kick_away;
				}
			}
		}
	}

	/******** ��� Boss ********/
	if ( Map.part[Map.map_id] == 7 ||
		 Map.map_id == 4 && Map.part[4] == 2 ||
		 Map.map_id == 4 && Map.part[4] == 5 )
		for ( int i = 0; i < BOSS::Boss_num; i++ )
			if ( abs(y - Boss[i].Bp->y - 20) < 15 )
				if ( abs(x - Boss[i].Bp->x - 10) < 45 &&
					Check_position(Dir, x, Boss[i].Bp->x+10) && !Boss[i].lying )
				{
					Boss[i].role = (byte)( player - 1 );
					bf[i] = &BASE_BOSS::Kick_away;
					Boss[i].Bp->Dir = ( Dir + 1 ) % 2;
				}
}

//

void Jack::Weapon_attack( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	int _x[2] = { -15, 0 };

	putimage( x+_x[Dir], y-15, &hammer[Dir][a][1], SRCAND );
	putimage( x+_x[Dir], y-15, &hammer[Dir][a][0], SRCINVERT );

	/****** ����Ƿ��ȡ�ļ��� *****/
	if ( FOOD::out && abs(x-FOOD::x) < 20 && abs(FOOD::y - y) < 20 )
	{
		Player[player - 1].hp = Player[player - 1].max_hp;
		FOOD::out = false;
	}

	int n = n_a % 12;
	if ( n <= 2 )			a = 0;
	if ( n >= 3 && n <= 5 )	a = 1;
	if ( n >= 6 )			a = 2;
	if ( n_a == 6 )
		SetSound( NULL, "����", "jj2" );
	if ( ++n_a > 11 )
	{
		n_a = a = 0;
		Player[player - 1].Set_bol( 0x3f );
		f[player - 1] = &Role::Stand;

		if (++weapon_counter >= 15)
		{
			weapon_counter = 0;
			have_weapon = false;
		}
		return;
	}

	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( abs(y - Emeny[i].Mp->Gety()) < 18 )
		{
			if ( Dir == LEFT && x - Emeny[i].Mp->Getx() < 80 && x > Emeny[i].Mp->Getx() ||
				 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 80 && x < Emeny[i].Mp->Getx() )
			{
				if ( !Emeny[i].lying && n_a > 9 )
				{
					if ( Emeny[i].be_catch )
						Player[player % 2].catch_emeny = false;

					// һ�����֣�ò�Ʋ��ð� Player[..].emeny = i
					Emeny[i].be_catch = false;
					Emeny[i].role = player - 1;
					Emeny[i].be_kick_counter = 5;
					Emeny[i].start_fly = true;
					mf[i] = &BASE_CLASS::Kick_away;
					Emeny[i].Mp->SetDir( RIGHT );

					int _x = Emeny[i].Mp->Getx();
					int _y = Emeny[i].Mp->Gety();
					int dx = 40, dy = 30;

					if ( x <= _x )
					{
						Emeny[i].Mp->SetDir( LEFT );
						dx = 0;
					}
					putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
					putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

					/***********/
					Player[player - 1].experience += 1;
					Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 9;
				}
			}
		}
	}
}

//

void Jack::Big_Blow( int dir, int player )
{
	int i;			// ����ѭ������

	// ����
	if ( Map.map_id == 2 && Check_hole(PE_player, player - 1) == _IN
		 || Check_lift(PE_player, player - 1) == _IN )
	{
		if ( Player[player - 1].life >= 0 )
			Player[player - 1].life--;
		temp_y = 0;
		Player[player - 1].hp = Player[player - 1].max_hp;
		Player[player - 1].Set_bol( 0x00 );
		f[player - 1] = &Role::Revive;
		return;
	}

	if ( Player[player - 1].hp <= 20 )
	{
		putimage( x, y, &walk[Dir][1][1], SRCAND );
		putimage( x, y, &walk[Dir][1][0], SRCINVERT );

		// ���й����� hp <= 10 ���ᵼ�²���
		big = true;
		bb = n_bb = 0;
		j  = n_j  = 0;
		a  = n_a  = 0;
		cm = n_cm = 0;
		ka = n_ka = 0;
		Player[player - 1].Set_bol( 0x3f );		// 1111,11
		f[player - 1] = &Role::Stand;
		Player[player - 1].catch_emeny = false;
		return;
	}

	if ( n_bb == 0 )
		SetSound( NULL, "������", "j1" );

	UNREFERENCED_PARAMETER(dir);
	Player[player - 1].Set_bol( 0x00 );		// 0000,00

	if ( x - 10 > MINX )
		if ( GetAsyncKeyState('A') & 0x8000 && player == 1 || GetAsyncKeyState(VK_LEFT) & 0x8000 && player == 2 )
			x -= 10;

	if ( x + 10 < MAXX )
		if ( GetAsyncKeyState('D') & 0x8000 && player == 1 || GetAsyncKeyState(VK_RIGHT) & 0x8000 && player == 2 )
		{
			if ( Map.Move_map(true) )
				x += 5;
			else
				x += 10;
			Map.move = true;
		}

	putimage( x, temp_y, &b_blow[Dir][bb][1], SRCAND );
	putimage( x, temp_y, &b_blow[Dir][bb][0], SRCINVERT );

	for ( i = 0; i < EMENY::emeny_num; i++ )
		if ( abs(y - Emeny[i].Mp->Gety()) < 28 && abs(x - Emeny[i].Mp->Getx()) < 70 )
				if ( !Emeny[i].lying )
				{
					SetSound( NULL, "����", "j2" );

					if ( Emeny[i].be_catch )
						Player[player % 2].catch_emeny = false;

					// һ�����֣�ò�Ʋ��ð� Player[..].emeny = i
					Emeny[i].be_catch = false;
					Emeny[i].role = player - 1;
					Emeny[i].be_kick_counter = 5;
					Emeny[i].start_fly = true;
					mf[i] = &BASE_CLASS::Kick_away;
					Emeny[i].Mp->SetDir( RIGHT );

					int _x = Emeny[i].Mp->Getx();
					int _y = Emeny[i].Mp->Gety();
					int dx = 40, dy = 30;

					if ( x <= _x )
					{
						Emeny[i].Mp->SetDir( LEFT );
						dx = 0;
					}
					putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
					putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

					/***********/
					Player[player - 1].experience += 1;
					Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 4;

					if ( big )
					{
						big = false;
						Player[player - 1].hp -= 10;
					}
				}

	for ( i = 0; i < BOSS::Boss_num; i++ )
		if ( abs(y - Boss[i].Bp->y-20) < 15 && abs(x - Boss[i].Bp->x) < 60 && !Boss[i].lying )
		{
			SetSound( NULL, "����", "jj2" );

			Boss[i].role = (byte)( player - 1 );
			Boss[i].start_fly = true;
			Boss[i].be_kick_counter = 5;
			bf[i] = &BASE_BOSS::Kick_away;
			Boss[i].Bp->Dir = RIGHT;

			int _x = Boss[i].Bp->x;
			int _y = Boss[i].Bp->y;
			int dx = 40, dy = 30;

			if ( x <= _x )
			{
				Boss[i].Bp->Dir = LEFT;
				dx = 0;
			}
			putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
			putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

			Player[player - 1].experience += 1;
			Boss[i].hp -= LEVEL::harm[Player[player - 1].level] * 5 / 2;

			if ( big )
			{
				big = false;
				Player[player - 1].hp -= 20;
			}
		}

	n_bb++;
	if ( (n_bb + 1) % 2 == 0 )
		bb = (++bb > 3 ) ? 0 : bb;
	else if ( n_bb >= 45 )
	{
		mciSendString( "close j1", 0, 0, 0 );

		big = true;
		bb = n_bb = 0;
		j  = n_j  = 0;		// ������
		a  = n_a  = 0;
		cm = n_cm = 0;
		ka = n_ka = 0;
		Player[player - 1].Set_bol( 0x3f );		// 1111,11
		f[player - 1] = &Role::Stand;
		Player[player - 1].catch_emeny = false;
	}
}

//

void Jack::Jump( int dir, int player )
{
	if ( Player[player - 1].catch_emeny )
	{
		putimage( x, temp_y, &jump_knee[Dir][0][1], SRCAND );
		putimage( x, temp_y, &jump_knee[Dir][0][0], SRCINVERT );
	}
	else
	{
		putimage( x, temp_y, &jump[Dir][j][1], SRCAND );
		putimage( x, temp_y, &jump[Dir][j][0], SRCINVERT );
	}
	Role::Jump( dir, player );
}

// ����

void Jack::Light_wave( int dir, int player )
{
	// ����
	if ( Map.map_id == 2 && Check_hole(PE_player, player - 1) == _IN
		 || Check_lift(PE_player, player - 1) == _IN )
	{
		if ( Player[player - 1].life >= 0 )
			Player[player - 1].life--;
		temp_y = 0;
		Player[player - 1].hp = Player[player - 1].max_hp;
		Player[player - 1].Set_bol( 0x00 );
		f[player - 1] = &Role::Revive;
		return;
	}

	UNREFERENCED_PARAMETER(dir);
	if ( n_lw > 5 )
	{
		lw = 1;
		if ( Dir == RIGHT )
			Map.move = true;	// ��ͼ���ƶ����뱣���� putimage ǰ��

		int _drx = (int)( Drtx * LEVEL::run_fly[Player[player - 1].level] );
		if ( x + _drx > MINX && x + _drx < MAXX )
			x += _drx;
	}
	if ( n_lw == 6 )
		SetSound( NULL, "����", "j5" );

	n_lw++;

	putimage( x, temp_y, &light_wave[Dir][lw][1], SRCAND );
	putimage( x, temp_y, &light_wave[Dir][lw][0], SRCINVERT );

	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( Dir == LEFT &&  x - Emeny[i].Mp->Getx() < 50 && x > Emeny[i].Mp->Getx() ||
			 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 50 && x < Emeny[i].Mp->Getx() )
			if ( abs(y - Emeny[i].Mp->Gety()) < 18 && Emeny[i].lying == false )
			{
				SetSound( NULL, "����", "j6" );

				/**** ��ײЧ�� ****/
				int _x = Emeny[i].Mp->Getx();
				int _y = Emeny[i].Mp->Gety();
				int dx = 40, dy = 30;

				if ( x <= _x )
					dx = 0;
				putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
				putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

				/**********/
				if ( Emeny[i].be_catch )
					Player[player % 2].catch_emeny = false;

				Emeny[i].be_catch = false;		// ˫��ģʽ���豣֤
				Emeny[i].role = player - 1;
				Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
				mf[i] = &BASE_CLASS::Kick_away;

				n_lw = lw = 0;
				Drtx = 0;
				Player[player - 1].Set_bol( 0x3f );		// 1111,11
				f[player - 1] = &Role::Stand;

				Player[player - 1].experience += 1;
				Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 2;
				return;
			}
	}

	/***********************/
	// ���˹�����Χ��΢С�� boss �ģ��������������ܱ�������

	if ( Map.part[Map.map_id] == 7 ||
		 Map.map_id == 4 && Map.part[4] == 2 ||
		 Map.map_id == 4 && Map.part[4] == 5 )
		for ( int i = 0; i < BOSS::Boss_num; i++ )
			if ( abs(x - Boss[i].Bp->x -10) < 45 && Check_position(Dir, x, Boss[i].Bp->x+10) &&
				 abs(y - Boss[i].Bp->temp_y-10) < 11 && !Boss[i].lying )
			{
				SetSound( NULL, "����", "j6" );

				int _x = Boss[i].Bp->x;
				int _y = Boss[i].Bp->temp_y;
				int dx = 50, dy = 44;
				if ( x <= _x )
					dx = 0;

				putimage( _x + dx, _y + dy, &PLAYER::light[1], SRCAND );
				putimage( _x + dx, _y + dy, &PLAYER::light[0], SRCINVERT );

				Boss[i].role = (byte)( player - 1 );
				Boss[i].Bp->Dir = ( Dir + 1 ) % 2;
				bf[i] = &BASE_BOSS::Kick_away;

				n_lw = lw = 0;
				Drtx = 0;
				Player[player - 1].Set_bol( 0x3f );
				f[player - 1] = &Role::Stand;

				Player[player - 1].experience += 1;
				Boss[i].hp -= LEVEL::harm[Player[player - 1].level] * 2;
				return;
			}

	if ( n_lw > 25 )
	{
		n_lw = lw = 0;
		Drtx = 0;
		Player[player - 1].Set_bol( 0x3f );		// 1111,11
		f[player - 1] = &Role::Stand;
	}
}

// �ö�����ɵ��˺���һ��������ת��������

void Jack::Dragon_fist( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);

	if ( n_df == 0 )
		Player[player - 1].experience += 1;

	putimage( x, temp_y, &dragon_fist[Dir][df][1], SRCAND );
	putimage( x, temp_y, &dragon_fist[Dir][df][0], SRCINVERT );
	n_df++;

	if ( n_df < 17 && n_df > 5 )
		for ( int i = 0; i < EMENY::emeny_num; i++ )
			if ( abs(temp_y - Emeny[i].Mp->Gety()) < 18 && 
				(Dir == LEFT &&  x - Emeny[i].Mp->Getx() < 70 && x > Emeny[i].Mp->Getx() ||
				 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 70 && x < Emeny[i].Mp->Getx()) &&
				 Emeny[i].lying == false )
			//	if ( Emeny[i].role != player - 1 )���ɣ���Ϊ������˱��������Emeny[i] = player - 1 ��
				{
					if ( Emeny[i].be_catch )
						Player[player % 2].catch_emeny = false;

					Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
					Emeny[i].start_fly = true;
					Emeny[i].be_catch = false;
					Emeny[i].role = player - 1;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
				}

	/******* ��� boss *********/
	if ( n_df > 5 && n_df < 17 && Map.part[Map.map_id] == 7 ||
		 Map.map_id == 4 && Map.part[4] == 2 ||
		 Map.map_id == 4 && Map.part[4] == 5 )
		for ( int i = 0; i < BOSS::Boss_num; i++ )
			if ( abs(temp_y - Boss[i].Bp->y-20) < 15 && abs(x - Boss[i].Bp->x) < 70 &&
				 Check_position(Dir, x, Boss[i].Bp->x) && !Boss[i].lying )
			{
				Boss[i].Bp->Dir = ( Dir + 1 ) % 2;
				Boss[i].start_fly = true;
				Boss[i].role = (byte)( player - 1 );
				Boss[i].be_kick_counter = 4;
				bf[i] = &BASE_BOSS::Kick_away;
				Boss[i].hp -= LEVEL::harm[Player[player - 1].level] * 3;
			}

	switch ( n_df )
	{
		case 2:		df = 1;	temp_y -= 10;	break;
		case 5:		df = 2;	temp_y -= 20;	break;
		case 11:	df = 3;	temp_y += 30;	SetSound( NULL, "˫������", "j7" );	break;
		case 18:	df = 4;					break;
		case 21:
			n_df = df = 0;
			n_a = a = 0;
			f[player - 1] = &Role::Stand;
			Player[player - 1].Set_bol( 0x3f );
			break;
		default:
			break;
	}
}

//

void Jack::Throw_role( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	if ( n_tr == 0 )
	{
		Dir++;
		Player[player - 1].experience += 1;
		if ( Player[player - 1].throw_dir == FORWARD )
			Dir++;
	}

	putimage( x, temp_y, &throw_role[Dir][tr][1], SRCAND );
	putimage( x, temp_y, &throw_role[Dir][tr][0], SRCINVERT );

	n_tr++;
	int dx[2] = { 15, -15 };

	switch ( n_tr )
	{
		case 4:		tr = 2;	SetSound( NULL, "˦��", "j8" );	break;
		case 8:		tr = 3;	x -= dx[Dir];	break;
		case 19:
			SetSound( NULL, "����", "j9" );
			Player[player - 1].throw_dir = BACK;
			Player[player - 1].Set_bol( 0x3f );
			tr = n_tr = 0;
			x += dx[Dir];
			Dir++;
			f[player - 1] = &Role::Stand;
			Player[player - 1].skill = NONE;
			Player[player - 1].catch_emeny = false;
			break;
		default:			break;
	}
}

//

void Jack::Jump_forward(int dir, int player )
{
	Role::Jump_forward( dir, player );
	if ( Player[player - 1].catch_emeny )
	{
		putimage( x, temp_y, &jump_knee[Dir][0][1], SRCAND );
		putimage( x, temp_y, &jump_knee[Dir][0][0], SRCINVERT );
	}
	else
	{
		putimage( x, temp_y, &jump_forward[Dir][jf][1], SRCAND );
		putimage( x, temp_y, &jump_forward[Dir][jf][0], SRCINVERT );
	}
}

//

void Jack::Jump_attack( int dir, int player )
{
	putimage( x, temp_y, &jump_attack[Dir][ja][1], SRCAND );
	putimage( x, temp_y, &jump_attack[Dir][ja][0], SRCINVERT );
	Role::Jump_attack( dir, player );
}

// ̩ɽѹ��

void Jack::Jump_knee( int dir, int player )
{
	UNREFERENCED_PARAMETER(dir);
	if ( n_jk <= 9 )
		temp_y -= 7;

	if ( n_jk >= 10 && n_jk <= 19 )
		temp_y += 7;

	/********************/
	if ( Dir == RIGHT && Drtx != 0 )
		Map.move = true;

	if ( x + Drtx > MINX && x + Drtx < MAXX )
		x += Drtx;
	else
		temp_x++;

	/********************/
	if ( Drtx != 0 && n_jk <= 20 )
		temp_y = ( x + temp_x * Drtx - X ) * ( x + temp_x * Drtx - X ) / 62 + Y;
	if ( Drtx != 0 && n_jk >= 21 )
		temp_y = ( x + temp_x * Drtx - X ) * ( x + temp_x * Drtx - X ) / 19 + Y;

	putimage( x, temp_y, &jump_knee[Dir][jk][1], SRCAND );
	putimage( x, temp_y, &jump_knee[Dir][jk][0], SRCINVERT );
	n_jk++;

	/***********/
	for ( int i = 0; i < EMENY::emeny_num; i++ )
	{
		if ( i != Player[player - 1].emeny && n_jk > 3 && n_jk < 20 && thump )
			if ( abs(y - Emeny[i].Mp->Gety()) < 18 && Emeny[i].lying == false &&
				(Dir == LEFT &&  x - Emeny[i].Mp->Getx() < 70 && x > Emeny[i].Mp->Getx() ||
				 Dir ==	RIGHT && Emeny[i].Mp->Getx() - x < 70 && x < Emeny[i].Mp->Getx()) )
			{
				if ( Emeny[i].be_catch )
					Player[player % 2].catch_emeny = false;

				Emeny[i].Mp->SetDir( (Dir + 1) % 2 );
				Emeny[i].start_fly = true;
				Emeny[i].lying = true;
				mf[i] = &BASE_CLASS::Kick_away;
				Emeny[i].be_catch = false;
				Emeny[i].be_kick_counter = 5;
				Emeny[i].role = player - 1;

				Player[player - 1].experience += 1;
				Emeny[i].hp -= LEVEL::harm[Player[player - 1].level] * 2;
			}
	}

	/********************/
	if ( n_jk > 5 && n_jk < 21 )
		jk = 1;

	if ( n_jk == 21 )
	{
		// ����
		if ( Map.map_id == 2 && Check_hole(PE_player, player - 1) == _IN
			 || Check_lift(PE_player, player - 1) == _IN )
		{
			if ( Player[player - 1].life >= 0 )
				Player[player - 1].life--;
			Player[player - 1].hp = Player[player - 1].max_hp;
			Player[player - 1].Set_bol( 0x00 );
			f[player - 1] = &Role::Revive;

			Drtx = 0;
			n_jk = jk = 0;
			n_cm = cm = 0;
			n_ka = ka = 0;
			temp_x = temp_y = 0;
			Player[player - 1].skill = NONE;
			Player[player - 1].catch_emeny = false;
			thump = false;

			mf[Player[player - 1].emeny] = &BASE_CLASS::Die;
			Emeny[Player[player - 1].emeny].hp = 0;
			Emeny[Player[player - 1].emeny].be_catch = false;
			return;
		}

		SetSound( NULL, "����", "j13" );
		jk = 2;
		int dx[2] = { 20, -20 };
		Drtx = dx[Dir] / 5;
		X = x + dx[Dir];
		Y = temp_y - 30;
		temp_x = 0;
	}

	if ( n_jk == 22 )
	{
		Map.y += 3;
		Emeny[Player[player - 1].emeny].be_catch = false;
		Emeny[Player[player - 1].emeny].be_kick_counter = 5;
		Emeny[Player[player - 1].emeny].start_fly = true;
		mf[Player[player - 1].emeny] = &BASE_CLASS::Kick_away;
		Emeny[Player[player - 1].emeny].Mp->SetDir( (Dir + 1) % 2 );

		int _x = Emeny[Player[player - 1].emeny].Mp->Getx();
		int _y = Emeny[Player[player - 1].emeny].Mp->Gety();
		putimage( _x + 20, _y + 65, &PLAYER::light[1], SRCAND );
		putimage( _x + 20, _y + 65, &PLAYER::light[0], SRCINVERT );

		Player[player - 1].experience += 3;
		Emeny[Player[player - 1].emeny].hp -= LEVEL::harm[Player[player - 1].level] * 6;
	}
	if ( n_jk == 23 )
		Map.y -= 3;

	if ( n_jk > 31 )
	{
		Drtx	= 0;
		temp_x = 0;
		n_jk = jk = 0;
		n_cm = cm = 0;
		n_ka = ka = 0;

		Player[player - 1].skill = NONE;
		Player[player - 1].Set_bol( 0x3f );
		Player[player - 1].catch_emeny = false;
		f[player - 1] = &Role::Stand;
		thump = false;
	}
}

//

void Jack::Knee_attack( int dir, int player )
{
	putimage( x, temp_y, &catch_attack[Dir][ka][1], SRCAND );
	putimage( x, temp_y, &catch_attack[Dir][ka][0], SRCINVERT );
	Role::Knee_attack( dir, player );
}

//

void Jack::Kick_down( int dir, int player )
{
	int k = 1;
	if ( Player[player - 1].die )
		k = 2;

	if ( Player[player - 1].be_kick_counter < 3 )
	{
		putimage( x, temp_y, &kick_down[Dir][kd][1], SRCAND );
		putimage( x, temp_y, &kick_down[Dir][kd][0], SRCINVERT );
	}
	else if ( n_kd % k == 0 )
	{
		putimage( x, temp_y, &kick_down[Dir][kd][1], SRCAND );
		putimage( x, temp_y, &kick_down[Dir][kd][0], SRCINVERT );
	}
	Role::Kick_down( dir, player );
}

// ��Ҹ���

void Jack::Revive( int dir, int player )
{
	if ( ++n_r % 2 == 0 )
	{
		putimage( x, temp_y, &jump[Dir][1][1], SRCAND );
		putimage( x, temp_y, &jump[Dir][1][0], SRCINVERT );
	}
	Role::Revive( dir, player );
}

//

void Jack::Be_kiss( int dir, int player )
{
	Role::Be_kiss( dir, player );
	putimage( x, temp_y, &kick_down[Dir][1][1], SRCAND );
	putimage( x, temp_y, &kick_down[Dir][1][0], SRCINVERT );
}

///////
///////		������ʵ��
///////



#define MIN_X	-80
#define MAX_X	520

#define WEAP	3	// �������ָ���
#define FOD		3	// ���ȳ��ָ���


/***** Small_Monkey ***************/

Small_Monkey::Small_Monkey()
{
	X = Y = temp_x = 0;
	Drtx[0] = -4;
	Drtx[1] = 4;

	int _x[2] = { -110, 550 };
	x = _x[rand() % 2];
	Revise_dispatch(x);		// ���������ڵ����ڳ��ֵ�λ��

	int min = Map.min_y[Map.map_id][Map.part[Map.map_id]];
	int max = Map.max_y[Map.map_id][Map.part[Map.map_id]];
	y = rand() % (max - min) + min;
	temp_y = y;		// temp_y �� y ����

	step = 0;
	_a = 0;
	dx1 = rand() % 3 + 3;
	dy1 = rand() % 3 + 3;
	dx2 = rand() % 3 + 7;
	Dir = LEFT;
	role = 0;
	leave = false;
	start = true;

	w = a = ka = ta = s = bc = d = 0;
	n_w = n_a = n_ka = n_ta = n_s = n_bc = n_d = 0;

	w_t1 = w_t2 = timeGetTime();

	int _level[6] = { 3,3,2,2,1,1 };
	int _dt[6] = { 80,70,60,50,45,37 };
	for ( int i = 0; i < 6; i++ )
	{
		w_dt[i] = _dt[i];
		attack_level[i] = _level[i];
	}
}

//

void Small_Monkey::Load()
{
	IMAGE img;
	loadimage( &img, "./res/Images/����.jpg", 828, 2748 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )		// 0 ��/����ͼ��1 ��/����ͼ
	{
		for ( int x = 0; x < 14; x++ )
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 80, i * 80, 80, 80 );		// 0
				getimage( &walk[0][x][i], x * 80, 160 + i * 80, 80, 80 );
			}

			if ( x < 5 )
			{
				getimage( &throw_away[1][x][i], x * 80, 960  + i * 80, 80, 80 );	// 3
				getimage( &throw_away[0][x][i], x * 80, 1120 + i * 80, 80, 80 );

				getimage( &be_catch[1][x][i], x * 80, 1280 + i * 80, 80, 80 );	// 4
				getimage( &be_catch[0][x][i], x * 80, 1440 + i * 80, 80, 80 );
			}

			if ( x < 2 )
			{
				getimage( &attack[1][x][i], x * 80, 320 + i * 80, 80, 80 );		// 1
				getimage( &attack[0][x][i], x * 80, 480 + i * 80, 80, 80 );
			}

			if ( x < 6 )
			{
				getimage( &kick_away[1][x][i], x * 80, 640 + i * 80, 80, 80 );	// 2
				getimage( &kick_away[0][x][i], x * 80, 800 + i * 80, 80, 80 );
			}
		}
	}
	SetWorkingImage();
}

void Small_Monkey::SetHp( int& hp )
{
	int a = Map.map_id;
	int b = Map.part[Map.map_id];

	hp = Hp[a][b];
}

//

void Small_Monkey::Stand( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &walk[Dir][0][1], SRCAND );
	putimage( x, y, &walk[Dir][0][0], SRCINVERT );

	if ( ++n_s > 35 )
	{
		w = a = ka = ta = s = bc = 0;
		n_w = n_a = n_ka = n_ta = n_s = n_bc = 0;
		Emeny[id].stand = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

// ������������������ĳһ���
// �˶��������� y �᷽����б�������� �� x ���򿿽����

void Small_Monkey::Walk( int id, const int& player )
{
	// ����ҵ��ڵ����÷����������ڸ�
	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y > Player[i].Rp->y )
			OTHER::emeny = true;
		else
			OTHER::emeny = false;
	}

	// ����
	if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
		 || Check_lift(PE_emeny, id) == _IN )
	{
		Emeny[id].hp = 0;
		mf[id] = &BASE_CLASS::Die;
		return;
	}

	Emeny[id].stand = false;
	Emeny[id].lying = false;
	if ( !Emeny[id].appear[ Map.part[Map.map_id] ] )
		return;

	/**** ���δ���˶�ʱ�� *************/
	w_t2 = timeGetTime();
	if ( w_t2 - w_t1 < w_dt[LEVEL::walk_dt] )//|| Player[Emeny[id].role].be_kick_counter >= 3 )
	{
		putimage( x, y, &walk[Dir][w][1], SRCAND );
		putimage( x, y, &walk[Dir][w][0], SRCINVERT );
		return;
	}
	else
		w_t1 = w_t2;

	/**** �������׷���Ķ��� ************/
	if ( rand() % 20 == 0 )
		start = true;

	if ( start )
	{
		int num = rand() % player;
		if ( Player[num].life >= 0 )
			Emeny[id].role = num;
		start = false;
	}
	
	int dx = 0, dy = 0;

	/***** ������� *************/

	if ( !leave )
	{
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x );

		/**** б���˶� ***********/

		if ( abs(y - Player[Emeny[id].role].Rp->y) > 5 )
		{
			if ( cos(_a) < 0 )	dx1 = -abs(dx1);
			if ( cos(_a) > 0 )	dx1 =  abs(dx1);
			if ( sin(_a) < 0 )	dy1 =  abs(dy1);
			if ( sin(_a) > 0 )	dy1 = -abs(dy1);

			dx = dx1;
			dy = dy1;
		}

		/**** ˮƽ�˶� ****/

		if ( abs(y - Player[Emeny[id].role].Rp->y) <= 5 )
		{
			if ( cos(_a) < 0 )	dx2 = -abs(dx2);
			if ( cos(_a) > 0 )	dx2 =  abs(dx2);

			dx = dx2;
			dy = 0;
		}
	}

	/**** ����ʼ��������� ****/

	if ( x < Player[Emeny[id].role].Rp->x )	Dir = RIGHT;
	if ( x > Player[Emeny[id].role].Rp->x )	Dir = LEFT;

	/**** �ﵽ���������������\Զ�� *****/

	if ( abs( x + dx - Player[Emeny[id].role].Rp->x) < 40 &&
		 abs( y + dy - Player[Emeny[id].role].Rp->y) < 10 )
	{
		
		leave = true;

		if ( abs( x + dx - Player[Emeny[id].role].Rp->x) > 30 )
		{
			n_w = 0;
			n_a = a = 0;
			Emeny[id].attack = false;

			if ( rand() % attack_level[LEVEL::a_level] == 0 )
			{
				Emeny[id].attack = true;
				mf[id] = &BASE_CLASS::Attack;

				putimage( x, y, &walk[Dir][w][1], SRCAND );
				putimage( x, y, &walk[Dir][w][0], SRCINVERT );

				return;
			}
		}
	}

	if ( leave && step == 0 )
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x ) + (double)(rand() % 314) / 100 + 1.57;

	if ( leave )
	{
		if ( ++step > 30 )
		{
			step = 0;
			leave = false;
		}

		dx = (int)( 6 * cos(_a) );
		dy = (int)(-6 * sin(_a) );
	}

	/**** һ��ͼƬ������ ****/

	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	if ( rand() % 68 == 0 )
	{
		n_s = 0;
		n_w = w = 0;
		mf[id] = &BASE_CLASS::Stand;
		return;
	}

	if ( ++n_w % 6 == 0 )
		w = ( ++w > 3 ) ? 0 : w;

	/**** ���Խ�� ****/

	if ( !Map.Check( y + dy ) || x + dx < -120 || x + dx > 560 || !Revise_lift(id, dx) )
	{
		step = 0;
		leave = false;
	}
	else
	{
		Revise_xy( id, x, y, dx, dy );		// ���������ƶ�����
		x += dx;
		y += dy;
		temp_y = y;
	}

	/******/

	if ( x + dx > -120 && x + dx < -110 || x + dx > 550 && x + dx < 560 )
	{
		int _x[2] = { -110, 550 };
		int id = rand() % 2;

		x = _x[id]; 
	}
}

// �ﵽ����������������� ��ֹһС��ʱ��

void Small_Monkey::Attack( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &attack[Dir][1][1], SRCAND );
	putimage( x, y, &attack[Dir][1][0], SRCINVERT );

	if ( id < 3 )
	{
		putimage( x + 40, y - 20, &EMENY::papaw[1], SRCAND );
		putimage( x + 40, y - 20, &EMENY::papaw[0], SRCINVERT );

		setbkmode( TRANSPARENT );
		setcolor( LIGHTGREEN );
		setfont( 20, 0, "����" );

		char* c[3];
		c[0] = "��";
		c[1] = "��";
		c[2] = "��";
		outtextxy( x + 45, y - 15, c[id] );
	}

	if ( ++n_a > 11 )
	{
		Emeny[id].attack = false;
		n_a = a = 0;
		mf[id] = &BASE_CLASS::Walk;
	}

	int cx = 0;

	/***** ������Ч *************/

	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( Emeny[id].attack && n_a < 4 && Player[i].Rp->Check_n_x( 0XFBB ) &&
			 abs( x - Player[i].Rp->x ) < 50 && abs( y - Player[i].Rp->y ) < 18 &&
			 (Dir == Player[i].Rp->Dir || Player[i].Rp->Getn_a() == 0) && Player[i].die == false )
		{
			cx = Player[i].Rp->x;
			if ( Check_position(Dir, x, cx) )
			{
				Player[i].Set_bol( 0x00 );
				f[i] = &Role::Kick_down;

				if ( x > Player[i].Rp->x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;

				// ��ֹ��ҹ����ŵ��˱�͵Ϯ����� be_kick_counter
				// ֻ�ܹ�����Щ��ȫվ�ŵĵ��� (.start_fly == true) �ģ�

				for ( int jj = 0; jj < EMENY::emeny_num; jj++ )
					if ( Emeny[jj].start_fly && !Emeny[jj].lying )
						Emeny[jj].be_kick_counter = 0;
			}
		}
	}
}

//

void Small_Monkey::Kick_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int _r = ( Dir + 1 ) % 2;	// �����ɷ����������෴

	if ( n_ka == 3 && Emeny[id].be_kick_counter < 4 )
		ka = 2;

	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	/****** ������ײЧ�� *******/

	if ( n_ka <= 7 && rand() % 6 == 0 && Emeny[id].be_kick_counter < 4 )
	{
		setbkmode( TRANSPARENT );
		setcolor( LIGHTGREEN );
		outtextxy( x + 20, y - 20, "��!" );
		int dx = rand() % 20 + 10;
		int dy = rand() % 10 + 20;
		putimage( x + dx, y + dy, &PLAYER::light[1], SRCAND );
		putimage( x + dx, y + dy, &PLAYER::light[0], SRCINVERT );
	}

	/***** ������ҹ�����Ч ****/

	if ( n_ka == 0 && Emeny[id].be_kick_counter == 3 )
	{
		Player[Emeny[id].role].Set_bol( 0x00 );
		f[Emeny[id].role] = &Role::Dragon_fist;

		/**** ˦�� ****/

		if ( Emeny[id].role == 0 && GetAsyncKeyState('S') & 0x8000 ||
			 Emeny[id].role == 1 && GetAsyncKeyState(VK_DOWN) & 0x8000 )	
		{
			n_ta = ta = 0;
			n_ka = ka = 0;
			f[Emeny[id].role] = &Role::Throw_role;

			if ( Player[Emeny[id].role].role_kind == 2 )
			{
				mf[id] = &BASE_CLASS::Be_catch;

				// �ڵ��˽��� Be_catch() �п��ܱ���ϣ���������������࣬������ҵ� Kick_down() ������
				// �Ҹ�������������Ӧ������
				Emeny[id].lying = true;
				Player[Emeny[id].role].skill = THROW_BACK;
			}
			else
			{
				Emeny[id].be_kick_counter = 0;
				mf[id] = &BASE_CLASS::Throw_away;
			}
			return;
		}
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 2;
	}

	/****** ���ι������� ******/

	if ( n_ka == 7 && Emeny[id].be_kick_counter < 4 )
	{
		n_ka = ka = 0;
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level];

		/****/

		if ( Emeny[id].hp <= 0 )
		{
			Emeny[id].start_fly = true;
			Emeny[id].lying = true;
			Emeny[id].be_kick_counter = 5;
			return;
		}

		/*****/

		if ( GetAsyncKeyState('J') & 0x8000 && Emeny[id].role == 0 ||
			 GetAsyncKeyState(VK_NUMPAD1) & 0x8000 && Emeny[id].role == 1 )
			 Emeny[id].be_kick_counter++;

		if ( Emeny[id].be_kick_counter < 4 )
		{
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/***** ���ø߷�·�� ********/

	if ( Emeny[id].be_kick_counter == 4 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 42, -42 };
			X = x + Dx[Dir];
			Y = y - 160;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}

		if ( n_ka == 42 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 87 )
		{
			if ( n_ka < 30 )
			{
				int dx[2] = { -30, 30 };
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][0], SRCINVERT );
			}

			if ( n_ka <= 7  )					ka = 0;
			if ( n_ka >= 8 && n_ka <= 30 )		ka = 3;
			if ( n_ka >= 31 )					ka = 4;
			if ( n_ka >= 76 )					ka = 5;

			if ( n_ka >= 0 && n_ka <= 41 )
			{
				if ( x - Drtx[Dir] / 2 >= MIN_X && x - Drtx[Dir] / 2 <= MAX_X )
					x -= Drtx[Dir] / 2;
				else
					temp_x++;
				temp_y = (x - temp_x * Drtx[Dir] / 2 - X) * (x - temp_x * Drtx[Dir] / 2 - X) / 11 + Y;
			}
			if ( n_ka >= 42 && n_ka <= 51 )
			{
				int dx = Drtx[Dir] / 4;
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
		}

		if ( n_ka == 44 || n_ka == 51 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 51;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 51 && Emeny[id].hp <= 0 )
		{
			temp_x = 0;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Die;
			return;
		}

		if ( n_ka > 87 )
		{
			temp_x = 0;
			Emeny[id].lying = false;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/****** ���õͷ�·�� ******/

	if ( Emeny[id].be_kick_counter == 5 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 10, -10 };
			X = x + Dx[Dir];
			Y = y - 40;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}
		if ( n_ka == 20 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}
		if ( n_ka <= 64 )
		{
			if ( n_ka <= 15  )					ka = 3;
			if ( n_ka >= 16 && n_ka <= 40 )		ka = 4;
			if ( n_ka >= 41 )					ka = 5;

			int dx = Drtx[Dir] / 4;
			if ( n_ka <= 19 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;

				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/2.5 ) + Y;
			}

			if ( n_ka >= 0 && n_ka <= 11 )
			{
				int dx[2] = { -25, 25 };
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][0], SRCINVERT );
			}

			if ( n_ka > 19 && n_ka <= 29 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
		}

		if ( n_ka == 20 || n_ka == 29 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 29;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 29 && Emeny[id].hp <= 0 )
		{
			mf[id] = &BASE_CLASS::Die;
			Emeny[id].be_catch = false;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			n_ka = ka = 0;
			n_bc = bc = 0;
			Emeny[id].be_kick_counter = 0;
			temp_x = 0;
			return;
		}

		if ( n_ka > 64 )
		{
			temp_x = 0;
			n_bc = bc = 0;
			n_ka = ka = 0;
			mf[id] = &BASE_CLASS::Walk;
			Emeny[id].lying = false;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			Emeny[id].be_kick_counter = 0;
			return;
		}
	}
	n_ka++;
}

//

void Small_Monkey::Throw_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -60, 60 };

	int _x = Player[Emeny[id].role].Rp->x;
	int _y = Player[Emeny[id].role].Rp->y;

	if ( n_ta == 0 )
		Emeny[id].lying = true;

	if ( n_ta <= 3 )
	{
		ta = 0;
		temp_y = _y;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 2;
		else
			x = _x - dx[Dir] * 2 / 3;
	}
	if ( n_ta >= 4 && n_ta <= 6 )
	{
		temp_y = _y - 20;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 3;
		else
			x = _x - dx[Dir] / 3;
	}
	if ( n_ta >= 7 && n_ta <= 9 )
	{
		temp_y = _y - 30;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 5;
		else
			x = _x + dx[Dir] / 6;
	}

	/************/

	if ( n_ta == 10 )
	{
		ta = 1;
		X = x + dx[Dir] / 3;
		Y = temp_y - 22;
		temp_x = 0;
	}

	if ( n_ta == 29 )
	{
		X = x + dx[Dir] / 12;
		Y = temp_y - 20;
		temp_x = 0;
	}

	/*******/

		int x1 = dx[Dir] / 10;
		if ( n_ta <= 38 && n_ta >= 10 )
		{
			if ( n_ta >= 29 )		x1 /= 6;
			if ( x + x1 >= MIN_X && x + x1 <= MAX_X )
				x += x1;
			else
				temp_x++;

			double k = 148;		// ע�⣬������뱣֤ k != 0
			if ( n_ta <= 19 )				ta = 1;
			if ( n_ta >= 20 && n_ta <= 28 )	ta = 2;
			if ( n_ta >= 29 && n_ta <= 38 )
			{
				if ( n_ta == 29 )
					Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 3;
				ta = 3;
				k = 1.2;
			}
			temp_y = int( (x+temp_x*x1-X)*(x+temp_x*x1-X)/k ) + Y;
		}

		if ( n_ta == 29 || n_ta == 39 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ta = 38;
				Emeny[id].hp = 0;	// ��������ļ��ʹ���˽�����
			}
		}

		if ( n_ta >= 60 && n_ta <= 82 )
			ta = 4;

	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );

	if ( n_ta == 38 && Emeny[id].hp <= 0 )
	{
		mf[id] = &BASE_CLASS::Die;
		Emeny[id].be_catch = false;
		Emeny[id].lying = true;
		n_ta = ta = 0;
		return;
	}

	/****** ��� 0-25 �׶��Ƿ����е��� *******/

	if ( n_ta >= 0 && n_ta <= 24 )
	{
		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id && abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;

					Emeny[i].hp -= LEVEL::harm[ Player[Emeny[id].role].level ];
				}
			}
		}
	}

	n_ta++;
	if ( n_ta >= 83)
	{
		n_ta = ta = 0;
		Emeny[id].be_catch = false;
		Emeny[id].lying = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

// ����ҵ����긳ֵ�����ˣ�ʵ�ֵ��˱����ץ���ƶ�

void Small_Monkey::Be_catch( int id, const int& player )
{
	UNREFERENCED_PARAMETER( player );
	int dx[2] = { -18, 21 };
	if ( Player[Emeny[id].role].skill != THROW_BACK )
		Dir = Player[Emeny[id].role].Rp->Dir;

	y = Player[Emeny[id].role].Rp->y;
	int _y = Player[Emeny[id].role].Rp->temp_y;
	int dy = 0;

	/*****/

	if ( Player[Emeny[id].role].skill == THUMP )
	{
		dy = 15;
		bc = 2;
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill != THROW_BACK && Player[Emeny[id].role].skill != THUMP )
	{
		bc = 1;
		if ( ++n_bc >= 6 )
		{
			Emeny[id].lying = false;
			n_bc = bc = 0;
		}
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill == THROW_BACK )
	{
			if ( n_bc >= 0 && n_bc <= 4 )
			{
				dx[0] = 24;
				dx[1] = -24;
				bc = 3;
			}
			if ( n_bc >= 5 && n_bc <= 8 )
			{
				dx[0] = 10;
				dx[1] = -10;
				dy = -40;
			}
			if ( n_bc >= 9 && n_bc <= 16 )
			{
				bc = 4;
				dx[0] = -40;
				dx[1] = 45;
				dy = 20;
			}
			if ( n_bc > 16 )
			{
				Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 4;
				n_bc = bc = 0;
				Dir++;
				mf[id] = &BASE_CLASS::Kick_away;
				Emeny[id].be_kick_counter = 5;
				Emeny[id].start_fly = true;

				putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
				putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );
				return;
			}
		n_bc++;

		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;

					Emeny[i].hp -= LEVEL::harm[Player[Emeny[id].role].level];
				}
			}
		}
	}
	x = Player[Emeny[id].role].Rp->x + dx[Dir];

	putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
	putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );

	if ( n_bc >= 10 && n_bc <= 14 )
	{
		putimage( x + 20, _y + 65, &PLAYER::light[1], SRCAND );
		putimage( x + 20, _y + 65, &PLAYER::light[0], SRCINVERT );
	}
}

//

void Small_Monkey::Die( int id, const int& player )
{
	UNREFERENCED_PARAMETER( player );
	if ( ++n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}

	if ( n_d > 50 )
	{
		if ( rand() % WEAP == 0 && !WEAPON::out )
			Weapon_Out( x, y, id, player );

		if ( rand() % FOD == 0 && !FOOD::out )
		{
			FOOD::out = true;
			FOOD::X = x - Map.x[Map.map_id];
			FOOD::y = y;
			FOOD::time = 0;
		}

		x = y = 0;			// �����˶�����Ϸ��
		n_d = d = 0;
		mf[id] = &BASE_CLASS::Walk;
		Emeny[id].Mp->SetHp( Emeny[id].hp );
		Emeny[id].appear[ Map.part[Map.map_id] ] = false;

		if ( Map.part[Map.map_id] < 7 )
		{
			if ( Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ] > 0 )
				Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ]--;

			int sum = 0;
			for ( int i = 0; i < EMENY::emeny_num; i++ )
				sum += Emeny[i].live[Map.map_id][ Map.part[Map.map_id] ];

			/****** �����������ǰ���� boss ********/

			if ( sum <= 0 )
			{
				if ( Map.map_id == 4 && (Map.part[4] == 5 || Map.part[4] == 2) )
					Dispatch_five_boss( Map.part[4] );
				else
					Map.die_out[Map.map_id][ Map.part[Map.map_id] ] = true;
			}
		}
	}
}


////// С�� ////////////////////////////////

Small_Dai::Small_Dai()
{
	X = Y = temp_x = 0;
	Drtx[0] = -4;
	Drtx[1] = 4;

	int _x[2] = { -110, 550 };
	x = _x[rand() % 2];
	Revise_dispatch(x);		// ���������ڵ����ڳ��ֵ�λ��

	int min = Map.min_y[Map.map_id][Map.part[Map.map_id]];
	int max = Map.max_y[Map.map_id][Map.part[Map.map_id]];
	y = rand() % (max - min) + min;
	temp_y = y;

	step = 0;
	_a = 0;
	dx1 = rand() % 3 + 3;
	dy1 = rand() % 3 + 3;
	dx2 = rand() % 3 + 5;
	Dir = LEFT;
	role = 0;
	leave = false;
	start = true;

	w = a = ka = ta = s = bc = d = 0;
	n_w = n_a = n_ka = n_ta = n_s = n_bc = n_d = 0;

	w_t1 = w_t2 = timeGetTime();

	int _level[6] = { 4,4,3,3,2,1 };
	int _dt[6] = { 125,110,95,90,85,85 };
	for ( int i = 0; i < 6; i++ )
	{
		w_dt[i] = _dt[i];
		attack_level[i] = _level[i];
	}
}

//

void Small_Dai::Load()
{
	IMAGE img;
	loadimage( &img, "./res/Images/С��.jpg", 800, 3000 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )		// 0 ��/����ͼ��1 ��/����ͼ
	{
		for ( int x = 0; x < 14; x++ )
		{
			if ( x < 5 )
			{
				getimage( &walk[1][x][i], x * 80, i * 80, 80, 80 );		// 0
				getimage( &walk[0][x][i], x * 80, 160 + i * 80, 80, 80 );

				getimage( &be_catch[1][x][i], x * 80, 1280 + i * 80, 80, 80 );	// 4
				getimage( &be_catch[0][x][i], x * 80, 1440 + i * 80, 80, 80 );
			}

			if ( x < 2 )
			{
				getimage( &attack[1][x][i], x * 80, 320 + i * 80, 80, 80 );		// 1
				getimage( &attack[0][x][i], x * 80, 480 + i * 80, 80, 80 );
			}

			if ( x < 5 )
			{
				getimage( &kick_away[1][x][i], x * 80, 640 + i * 80, 80, 80 );	// 2
				getimage( &kick_away[0][x][i], x * 80, 800 + i * 80, 80, 80 );

				getimage( &throw_away[1][x][i], x * 80, 960  + i * 80, 80, 80 );	// 3
				getimage( &throw_away[0][x][i], x * 80, 1120 + i * 80, 80, 80 );
			}
		}
	}
	SetWorkingImage();
}

void Small_Dai::SetHp( int& hp )
{
	int a = Map.map_id;
	int b = Map.part[Map.map_id];

	hp = Hp[a][b];
}

//

void Small_Dai::Stand( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &walk[Dir][4][1], SRCAND );
	putimage( x, y, &walk[Dir][4][0], SRCINVERT );

	if ( ++n_s > 45 )
	{
		w = a = ka = ta = s = bc = 0;
		n_w = n_a = n_ka = n_ta = n_s = n_bc = 0;
		Emeny[id].stand = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}
//

void Small_Dai::Walk( int id, const int& player )
{
	// ����ҵ��ڵ����÷����������ڸ�
	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y > Player[i].Rp->y )
			OTHER::emeny = true;
		else
			OTHER::emeny = false;
	}

	// ����
	if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
		 || Check_lift(PE_emeny, id) == _IN )
	{
		Emeny[id].hp = 0;
		mf[id] = &BASE_CLASS::Die;
		return;
	}

	Emeny[id].stand = false;
	Emeny[id].lying = false;
	if ( !Emeny[id].appear[ Map.part[Map.map_id] ] )
		return;

	/**** ���δ���˶�ʱ�� *************/

	w_t2 = timeGetTime();
	if ( w_t2 - w_t1 < w_dt[LEVEL::walk_dt] )
	{
		putimage( x, y, &walk[Dir][w][1], SRCAND );
		putimage( x, y, &walk[Dir][w][0], SRCINVERT );
		return;
	}
	else
		w_t1 = w_t2;

	/**** �������׷���Ķ��� ************/

	if ( start )
	{
		int num = rand() % player;
		if ( Player[num].life >= 0 )
			Emeny[id].role = num;
		start = false;
	}
	if ( rand() % 20 == 0 )
		start = true;

	int dx = 0, dy = 0;

	/***** ������� *************/

	if ( !leave )
	{
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x );

		/**** б���˶� ***********/

		if ( abs(y - Player[Emeny[id].role].Rp->y) > 5 )
		{
			if ( cos(_a) < 0 )	dx1 = -abs(dx1);
			if ( cos(_a) > 0 )	dx1 =  abs(dx1);
			if ( sin(_a) < 0 )	dy1 =  abs(dy1);
			if ( sin(_a) > 0 )	dy1 = -abs(dy1);

			dx = dx1;
			dy = dy1;
		}

		/**** ˮƽ�˶� ****/

		if ( abs(y - Player[Emeny[id].role].Rp->y) <= 5 )
		{
			if ( cos(_a) < 0 )	dx2 = -abs(dx2);
			if ( cos(_a) > 0 )	dx2 =  abs(dx2);

			dx = dx2;
			dy = 0;
		}
	}

	/**** ����ʼ��������� ****/

	if ( x < Player[Emeny[id].role].Rp->x )	Dir = RIGHT;
	if ( x > Player[Emeny[id].role].Rp->x )	Dir = LEFT;

	/**** �ﵽ���������������\Զ�� *****/

	if ( abs( x + dx - Player[Emeny[id].role].Rp->x) < 40 &&
		 abs( y + dy - Player[Emeny[id].role].Rp->y) < 10 )
	{
		leave = true;

		if ( abs( x + dx - Player[Emeny[id].role].Rp->x) > 10 )
		{
			n_w = 0;
			n_a = a = 0;
			Emeny[id].attack = false;		// ��ֹ�������жϣ������в���

			if ( rand() % attack_level[LEVEL::a_level] == 0 )
			{
				Emeny[id].attack = true;
				mf[id] = &BASE_CLASS::Attack;
				putimage( x, y, &walk[Dir][w][1], SRCAND );
				putimage( x, y, &walk[Dir][w][0], SRCINVERT );
				return;
			}
		}
	}

	if ( leave && step == 0 )
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x ) + (double)(rand() % 314) / 100 + 1.57;

	if ( leave )
	{
		if ( ++step > 30 )
		{
			step = 0;
			leave = false;
		}

		dx = (int)( 6 * cos(_a) );
		dy = (int)(-6 * sin(_a) );
	}

	/**** һ��ͼƬ������ ****/

	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	if ( rand() % 68 == 0 )
	{
		n_s = 0;
		n_w = w = 0;
		mf[id] = &BASE_CLASS::Stand;
		return;
	}

	if ( ++n_w % 2 == 0 )
		w = ( ++w > 3 ) ? 0 : w;

	/**** ���Խ�� ****/
	
	if ( !Map.Check( y + dy ) || x + dx < -120 || x + dx > 560 || !Revise_lift(id, dx) )
	{
		step = 0;
		leave = false;
	}
	else
	{
		Revise_xy( id, x, y, dx, dy );		// ���������ƶ�����
		x += dx;
		y += dy;
		temp_y = y;
	}

	/******/

	if ( x + dx > -120 && x + dx < -110 || x + dx > 550 && x + dx < 560 )
	{
		int _x[2] = { -110, 550 };
		int id = rand() % 2;

		x = _x[id]; 
	}
}

//

void Small_Dai::Attack( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &attack[Dir][1][1], SRCAND );
	putimage( x, y, &attack[Dir][1][0], SRCINVERT );

	if ( id < 3 )
	{
		putimage( x + 40, y - 20, &EMENY::papaw[1], SRCAND );
		putimage( x + 40, y - 20, &EMENY::papaw[0], SRCINVERT );

		setbkmode( TRANSPARENT );
		setcolor( LIGHTGREEN );
		setfont( 20, 0, "����" );

		char* c[3];
		c[0] = "��";
		c[1] = "��";
		c[2] = "��";
		outtextxy( x + 48, y - 15, c[id] );
	}

	if ( ++n_a > 11 )
	{
		Emeny[id].attack = false;
		n_a = a = 0;
		mf[id] = &BASE_CLASS::Walk;
	}

	int cx = 0;

	/***** ������Ч *************/

	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( Emeny[id].attack && n_a < 4 && Player[i].Rp->Check_n_x( 0XFBB ) &&
			 abs( x - Player[i].Rp->x ) < 50 &&
			 abs( y - Player[i].Rp->y ) < 18 &&
			 (Dir == Player[i].Rp->Dir ||
			 Player[i].Rp->Getn_a() == 0) && Player[i].die == false )
		{
			cx = Player[i].Rp->x;
			if ( Check_position(Dir, x, cx) )
			{
				Player[i].Set_bol( 0x00 );
				f[i] = &Role::Kick_down;

				if ( x > Player[i].Rp->x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;

				for ( int jj = 0; jj < EMENY::emeny_num; jj++ )
					if ( Emeny[jj].start_fly && !Emeny[jj].lying )
						Emeny[jj].be_kick_counter = 0;
			}
		}
	}
}

//

void Small_Dai::Kick_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int _r = ( Dir + 1 ) % 2;

	if ( n_ka == 3 && Emeny[id].be_kick_counter < 4 )
		ka = 1;

	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	/****** ������ײЧ�� *******/

	if ( n_ka <= 7 && rand() % 6 == 0 && Emeny[id].be_kick_counter < 4 )
	{
		setbkmode( TRANSPARENT );
		setcolor( LIGHTGREEN );
		outtextxy( x + 20, y - 20, "��!" );
		int dx = rand() % 20 + 10;
		int dy = rand() % 10 + 20;
		putimage( x + dx, y + dy, &PLAYER::light[1], SRCAND );
		putimage( x + dx, y + dy, &PLAYER::light[0], SRCINVERT );
	}

	/***** ������ҹ�����Ч ****/

	if ( n_ka == 0 && Emeny[id].be_kick_counter == 3 )
	{
		Player[Emeny[id].role].Set_bol( 0x00 );
		f[Emeny[id].role] = &Role::Dragon_fist;

		/**** ˦�� ****/

		if ( Emeny[id].role == 0 && GetAsyncKeyState('S') & 0x8000 ||
			 Emeny[id].role == 1 && GetAsyncKeyState(VK_DOWN) & 0x8000 )	
		{
			n_ta = ta = 0;
			n_ka = ka = 0;
			f[Emeny[id].role] = &Role::Throw_role;

			if ( Player[Emeny[id].role].role_kind == 2 )
			{
				Emeny[id].lying = true;
				mf[id] = &BASE_CLASS::Be_catch;
				Player[Emeny[id].role].skill = THROW_BACK;
			}
			else
			{
				Emeny[id].be_kick_counter = 0;
				mf[id] = &BASE_CLASS::Throw_away;
			}
			return;
		}
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 2;
	}

	/****** ���ι������� ******/

	if ( n_ka == 7 && Emeny[id].be_kick_counter < 4 )
	{
		n_ka = ka = 0;
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level];

		/****/

		if ( Emeny[id].hp <= 0 )
		{
			Emeny[id].start_fly = true;
			Emeny[id].lying = true;
			Emeny[id].be_kick_counter = 5;
			return;
		}

		/*****/

		if ( GetAsyncKeyState('J') & 0x8000 && Emeny[id].role == 0 ||
			 GetAsyncKeyState(VK_NUMPAD1) & 0x8000 && Emeny[id].role == 1 )
			 Emeny[id].be_kick_counter++;

		if ( Emeny[id].be_kick_counter < 4 )
		{
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/***** ���ø߷�·�� ********/

	if ( Emeny[id].be_kick_counter == 4 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 42, -42 };
			X = x + Dx[Dir];
			Y = y - 160;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}

		if ( n_ka == 42 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 87 )
		{
			if ( n_ka < 30 )
			{
				int dx[2] = { -30, 30 };
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][0], SRCINVERT );
			}

			if ( n_ka <= 7  )					ka = 0;
			if ( n_ka >= 8 && n_ka <= 30 )		ka = 2;
			if ( n_ka >= 31 )					ka = 3;
			if ( n_ka >= 76 )					ka = 4;

			if ( n_ka >= 0 && n_ka <= 41 )
			{
				if ( x - Drtx[Dir] / 2 >= MIN_X && x - Drtx[Dir] / 2 <= MAX_X )
					x -= Drtx[Dir] / 2;
				else
					temp_x++;
				temp_y = (x - temp_x * Drtx[Dir] / 2 - X) * (x - temp_x * Drtx[Dir] / 2 - X) / 11 + Y;
			}

			if ( n_ka >= 42 && n_ka <= 51 )
			{
				int dx = Drtx[Dir] / 4;
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
		}

		if ( n_ka == 44 || n_ka == 51 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 51;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 51 && Emeny[id].hp <= 0 )
		{
			temp_x = 0;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Die;
			return;
		}

		if ( n_ka > 87 )
		{
			temp_x = 0;
			Emeny[id].lying = false;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/****** ���õͷ�·�� ******/

	if ( Emeny[id].be_kick_counter == 5 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 10, -10 };
			X = x + Dx[Dir];
			Y = y - 40;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}

		if ( n_ka == 20 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;//////////////Ҫ��Ҫ�� temp_y ��??
			temp_x = 0;
		}

		if ( n_ka <= 64 )
		{
			if ( n_ka <= 15  )					ka = 2;
			if ( n_ka >= 16 && n_ka <= 40 )		ka = 3;
			if ( n_ka >= 41 )					ka = 4;

			int dx = Drtx[Dir] / 4;
			if ( n_ka <= 19 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;

				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/2.5 ) + Y;
			}

			if ( n_ka >= 0 && n_ka <= 11 )
			{
				int dx[2] = { -20, 20 };
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][0], SRCINVERT );
			}
			
			if ( n_ka > 19 && n_ka <= 29 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
		}

		if ( n_ka == 20 || n_ka == 29 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 29;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 29 && Emeny[id].hp <= 0 )
		{
			mf[id] = &BASE_CLASS::Die;
			Emeny[id].be_catch = false;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			n_ka = ka = 0;
			n_bc = bc = 0;
			Emeny[id].be_kick_counter = 0;
			temp_x = 0;
			return;
		}

		if ( n_ka > 64 )
		{
			n_bc = bc = 0;
			n_ka = ka = 0;
			mf[id] = &BASE_CLASS::Walk;
			Emeny[id].lying = false;
			temp_x = 0;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			Emeny[id].be_kick_counter = 0;
			return;
		}
	}
	n_ka++;
}

//

void Small_Dai::Throw_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -60, 60 };

	int _x = Player[Emeny[id].role].Rp->x;
	int _y = Player[Emeny[id].role].Rp->y;

	if ( n_ta == 0 )
		Emeny[id].lying = true;

	if ( n_ta <= 3 )
	{
		ta = 0;
		temp_y = _y;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 2;
		else
			x = _x - dx[Dir] * 2 / 3;
	}
	if ( n_ta >= 4 && n_ta <= 6 )
	{
		temp_y = _y - 20;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 3;
		else
			x = _x - dx[Dir] / 3;
	}
	if ( n_ta >= 7 && n_ta <= 9 )
	{
		temp_y = _y - 30;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 5;
		else
			x = _x + dx[Dir] / 6;
	}

	/************/

	if ( n_ta == 10 )
	{
		ta = 1;
		X = x + dx[Dir] / 3;
		Y = temp_y - 22;
		temp_x = 0;
	}

	if ( n_ta == 29 )
	{
		X = x + dx[Dir] / 12;
		Y = temp_y - 20;
		temp_x = 0;
	}

	/*******/

		int x1 = dx[Dir] / 10;
		if ( n_ta <= 38 && n_ta >= 10 )
		{
			if ( n_ta >= 29 )		x1 /= 6;
			if ( x + x1 >= MIN_X && x + x1 <= MAX_X )
				x += x1;
			else
				temp_x++;

			double k = 148;		// ע�⣬������뱣֤ k != 0
			if ( n_ta <= 19 )				ta = 1;
			if ( n_ta >= 20 && n_ta <= 28 )	ta = 2;
			if ( n_ta >= 29 && n_ta <= 38 )
			{
				if ( n_ta == 29 )
					Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 3;
				ta = 3;
				k = 1.2;
			}
			temp_y = int( (x+temp_x*x1-X)*(x+temp_x*x1-X)/k ) + Y;
		}

		if ( n_ta == 29 || n_ta == 39 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ta = 38;
				Emeny[id].hp = 0;	// ��������ļ��ʹ���˽�����
			}
		}

		if ( n_ta >= 60 && n_ta <= 82 )
			ta = 4;

	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );

	if ( n_ta == 38 && Emeny[id].hp <= 0 )
	{
		mf[id] = &BASE_CLASS::Die;
		Emeny[id].be_catch = false;
		Emeny[id].lying = true;
		n_ta = ta = 0;
		return;
	}

	/****** ��� 0-25 �׶��Ƿ����е��� *******/

	if ( n_ta >= 0 && n_ta <= 24 )
	{
		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;

					Emeny[i].hp -= LEVEL::harm[ Player[Emeny[id].role].level ];
				}
			}
		}
	}

	n_ta++;
	if ( n_ta >= 83)
	{
		n_ta = ta = 0;
		Emeny[id].be_catch = false;
		Emeny[id].lying = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

//

void Small_Dai::Be_catch( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -23, 25 };
	if ( Player[Emeny[id].role].skill != THROW_BACK )
		Dir = Player[Emeny[id].role].Rp->Dir;

	y = Player[Emeny[id].role].Rp->y;
	int _y = Player[Emeny[id].role].Rp->temp_y;
	int dy = 0;

	/*****/

	if ( Player[Emeny[id].role].skill == THUMP )
	{
		dy = 15;
		bc = 2;
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill != THROW_BACK && Player[Emeny[id].role].skill != THUMP )
	{
		bc = 1;
		if ( ++n_bc >= 6 )
		{
			Emeny[id].lying = false;
			n_bc = bc = 0;
		}
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill == THROW_BACK )
	{
			if ( n_bc >= 0 && n_bc <= 4 )
			{
				dx[0] = 24;
				dx[1] = -24;
				bc = 3;
			}
			if ( n_bc >= 5 && n_bc <= 8 )
			{
				dx[0] = 10;
				dx[1] = -10;
				dy = -40;
			}
			if ( n_bc >= 9 && n_bc <= 16 )
			{
				bc = 4;
				dx[0] = -40;
				dx[1] = 45;
				dy = 20;
			}
			if ( n_bc > 16 )
			{
				Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 4;
				n_bc = bc = 0;
				Dir++;
				mf[id] = &BASE_CLASS::Kick_away;
				Emeny[id].be_kick_counter = 5;
				Emeny[id].start_fly = true;

				putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
				putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );
				return;
			}
		n_bc++;

		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;

					Emeny[i].hp -= LEVEL::harm[Player[Emeny[id].role].level];
				}
			}
		}
	}

	x = Player[Emeny[id].role].Rp->x + dx[Dir];

	putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
	putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );

	if ( n_bc >= 10 && n_bc <= 14 )
	{
		putimage( x + 20, _y + 65, &PLAYER::light[1], SRCAND );
		putimage( x + 20, _y + 65, &PLAYER::light[0], SRCINVERT );
	}
}

//

void Small_Dai::Die( int id, const int& player )
{
	UNREFERENCED_PARAMETER( player );
	if ( ++n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}

	if ( n_d > 50 )
	{
		if ( rand() % WEAP == 0 && !WEAPON::out )
			Weapon_Out( x, y, id, player );

		if ( rand() % FOD == 0 && !FOOD::out )
		{
			FOOD::out = true;
			FOOD::X = x - Map.x[Map.map_id];
			FOOD::y = y;
			FOOD::time = 0;
		}

		x = y = 0;			// �����˶�����Ϸ��
		n_d = d = 0;
		mf[id] = &BASE_CLASS::Walk;
		Emeny[id].Mp->SetHp( Emeny[id].hp );
		Emeny[id].appear[ Map.part[Map.map_id] ] = false;

		if ( Map.part[Map.map_id] < 7 )
		{
			if ( Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ] > 0 )
				Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ]--;

			int sum = 0;
			for ( int i = 0; i < EMENY::emeny_num; i++ )
				sum += Emeny[i].live[Map.map_id][ Map.part[Map.map_id] ];

			/****** �����������ǰ���� boss ********/

			if ( sum <= 0 )
			{
				if ( Map.map_id == 4 && (Map.part[4] == 5 || Map.part[4] == 2) )
					Dispatch_five_boss( Map.part[4] );
				else
					Map.die_out[Map.map_id][ Map.part[Map.map_id] ] = true;
			}
		}
	}
}



////// ��� //////////////////////////////////////////////////

Big_Dai::Big_Dai()
{
	X = Y = temp_x = 0;
	Drtx[0] = -4;
	Drtx[1] = 4;

	int _x[2] = { -110, 550 };
	x = _x[rand() % 2];
	Revise_dispatch(x);		// ���������ڵ����ڳ��ֵ�λ��

	int min = Map.min_y[Map.map_id][Map.part[Map.map_id]];
	int max = Map.max_y[Map.map_id][Map.part[Map.map_id]];
	y = rand() % (max - min) + min;
	temp_y = y;

	step = 0;
	_a = 0;
	dx1 = rand() % 3 + 3;
	dy1 = rand() % 3 + 3;
	dx2 = rand() % 3 + 6;
	Dir = LEFT;
	role = 0;
	leave = false;
	start = true;
	resist = false;

	w = a = ka = ta = s = bc = d = 0;
	n_w = n_a = n_ka = n_ta = n_s = n_bc = n_d = 0;

	w_t1 = w_t2 = timeGetTime();

	int _level[6] = { 4,4,3,2,2,1 };
	int _dt[6] = { 107,100,95,95,90,90 };
	for ( int i = 0; i < 6; i++ )
	{
		w_dt[i] = _dt[i];
		attack_level[i] = _level[i];
	}
}

//

void Big_Dai::Load()
{
	IMAGE img;
	loadimage( &img, "./res/Images/���.jpg", 800, 2000 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )		// 0 ��/����ͼ��1 ��/����ͼ
	{
		for ( int x = 0; x < 14; x++ )
		{
			if ( x < 5 )
			{
				getimage( &walk[1][x][i], x * 80, i * 80, 80, 80 );		// 0
				getimage( &walk[0][x][i], x * 80, 160 + i * 80, 80, 80 );

				getimage( &throw_away[1][x][i], x * 80, 960  + i * 80, 80, 80 );	// 3
				getimage( &throw_away[0][x][i], x * 80, 1120 + i * 80, 80, 80 );
			}

			if ( x < 6 )
			{
				getimage( &kick_away[1][x][i], x * 80, 640 + i * 80, 80, 80 );	// 2
				getimage( &kick_away[0][x][i], x * 80, 800 + i * 80, 80, 80 );
			}
			if ( x < 2 )
			{
				getimage( &attack[1][x][i], x * 80, 320 + i * 80, 80, 80 );		// 1
				getimage( &attack[0][x][i], x * 80, 480 + i * 80, 80, 80 );
			}

			if ( x < 5 )
			{
				getimage( &be_catch[1][x][i], x * 80, 1280 + i * 80, 80, 80 );		// 4
				getimage( &be_catch[0][x][i], x * 80, 1440 + i * 80, 80, 80 );
			}
		}
	}
	SetWorkingImage();
}

void Big_Dai::SetHp( int& hp )
{
	int a = Map.map_id;
	int b = Map.part[Map.map_id];

	hp = Hp[a][b];
}

//

void Big_Dai::Stand( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &walk[Dir][4][1], SRCAND );
	putimage( x, y, &walk[Dir][4][0], SRCINVERT );

	if ( ++n_s > 45 )
	{
		w = a = ka = ta = s = bc = 0;
		n_w = n_a = n_ka = n_ta = n_s = n_bc = 0;
		Emeny[id].stand = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}
//

void Big_Dai::Walk( int id, const int& player )
{
	// ����ҵ��ڵ����÷����������ڸ�
	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y > Player[i].Rp->y )
			OTHER::emeny = true;
		else
			OTHER::emeny = false;
	}

	// ����
	if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
		 || Check_lift(PE_emeny, id) == _IN )
	{
		Emeny[id].hp = 0;
		mf[id] = &BASE_CLASS::Die;
		return;
	}

	Emeny[id].stand = false;
	Emeny[id].lying = false;
	if ( !Emeny[id].appear[ Map.part[Map.map_id] ] )
		return;

	/**** ���δ���˶�ʱ�� *************/

	w_t2 = timeGetTime();
	if ( w_t2 - w_t1 < w_dt[LEVEL::walk_dt] )
	{
		putimage( x, y, &walk[Dir][w][1], SRCAND );
		putimage( x, y, &walk[Dir][w][0], SRCINVERT );
		return;
	}
	else
		w_t1 = w_t2;

	/**** �������׷���Ķ��� ************/

	if ( start )
	{
		int num = rand() % player;
		if ( Player[num].life >= 0 )
			Emeny[id].role = num;
		start = false;
	}
	if ( rand() % 20 == 0 )
		start = true;

	int dx = 0, dy = 0;

	/***** ������� *************/

	if ( !leave )
	{
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x );

		/**** б���˶� ***********/

		if ( abs(y - Player[Emeny[id].role].Rp->y) > 5 )
		{
			if ( cos(_a) < 0 )	dx1 = -abs(dx1);
			if ( cos(_a) > 0 )	dx1 =  abs(dx1);
			if ( sin(_a) < 0 )	dy1 =  abs(dy1);
			if ( sin(_a) > 0 )	dy1 = -abs(dy1);

			dx = dx1;
			dy = dy1;
		}

		/**** ˮƽ�˶� ****/

		if ( abs(y - Player[Emeny[id].role].Rp->y) <= 5 )
		{
			if ( cos(_a) < 0 )	dx2 = -abs(dx2);
			if ( cos(_a) > 0 )	dx2 =  abs(dx2);

			dx = dx2;
			dy = 0;
		}
	}

	/**** ����ʼ��������� ****/

	if ( x < Player[Emeny[id].role].Rp->x )	Dir = RIGHT;
	if ( x > Player[Emeny[id].role].Rp->x )	Dir = LEFT;

	/**** �ﵽ���������������\Զ�� *****/

	if ( abs( x + dx - Player[Emeny[id].role].Rp->x) < 40 && abs( y + dy - Player[Emeny[id].role].Rp->y) < 10 )
	{
		leave = true;
		if ( abs( x + dx - Player[Emeny[id].role].Rp->x) > 10 )
		{
			n_w = 0;
			n_a = a = 0;
			Emeny[id].attack = false;

			if ( rand() % attack_level[LEVEL::a_level] == 0 )
			{
				Emeny[id].attack = true;
				mf[id] = &BASE_CLASS::Attack;
				putimage( x, y, &walk[Dir][w][1], SRCAND );
				putimage( x, y, &walk[Dir][w][0], SRCINVERT );
				return;
			}
		}
	}

	if ( leave && step == 0 )
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x ) + (double)(rand() % 314) / 100 + 1.57;

	if ( leave )
	{
		if ( ++step > 30 )
		{
			step = 0;
			leave = false;
		}

		dx = (int)( 6 * cos(_a) );
		dy = (int)(-6 * sin(_a) );
	}

	/**** һ��ͼƬ������ ****/

	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	if ( rand() % 68 == 0 )
	{
		n_s = 0;
		n_w = w = 0;
		mf[id] = &BASE_CLASS::Stand;
		return;
	}

	if ( ++n_w % 2 == 0 )
		w = ( ++w > 3 ) ? 0 : w;

	/**** ���Խ�� ****/
	
	if ( !Map.Check( y + dy ) || x + dx < -120 || x + dx > 560 || !Revise_lift(id, dx) )
	{
		step = 0;
		leave = false;
	}
	else
	{
		Revise_xy( id, x, y, dx, dy );		// ���������ƶ�����
		x += dx;
		y += dy;
		temp_y = y;
	}

	/******/

	if ( x + dx > -120 && x + dx < -110 || x + dx > 550 && x + dx < 560 )
	{
		int _x[2] = { -110, 550 };
		int id = rand() % 2;

		x = _x[id]; 
	}
}

void Big_Dai::Attack( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &attack[Dir][a][1], SRCAND );
	putimage( x, y, &attack[Dir][a][0], SRCINVERT );

	if ( id < 3 )
	{
		putimage( x + 40, y - 20, &EMENY::papaw[1], SRCAND );
		putimage( x + 40, y - 20, &EMENY::papaw[0], SRCINVERT );

		setbkmode( TRANSPARENT );
		setcolor( LIGHTGREEN );
		setfont( 20, 0, "����" );

		char* c[3];
		c[0] = "����";
		c[1] = "�߱�";
		c[2] = "��PP";
		outtextxy( x + 48, y - 15, c[id] );
	}

	if ( n_a > 5 )
		a = 1;

	if ( ++n_a > 11 )
	{
		Emeny[id].attack = false;
		n_a = a = 0;
		mf[id] = &BASE_CLASS::Walk;
	}

	int cx = 0;

	/***** ������Ч *************/

	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( Emeny[id].attack && n_a > 3 && Player[i].Rp->Check_n_x( 0XFBB ) &&
			 abs( x - Player[i].Rp->x ) < 50 && abs( y - Player[i].Rp->y ) < 18 &&
			 (Dir == Player[i].Rp->Dir ||
			 Player[i].Rp->Getn_a() == 0) && Player[i].die == false )///////////�������ų�����ֱ�ӻ�����ң�����Ҳ�����ˣ�
		{
			cx = Player[i].Rp->x;
			if ( Check_position(Dir, x, cx) )
			{
				//Emeny[id].attack = false;
				Player[i].Set_bol( 0x00 );
				f[i] = &Role::Kick_down;

				if ( x > Player[i].Rp->x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;

				for ( int jj = 0; jj < EMENY::emeny_num; jj++ )
					if ( Emeny[jj].start_fly && !Emeny[jj].lying )
						Emeny[jj].be_kick_counter = 0;
			}
		}
	}
}

//

void Big_Dai::Kick_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int _r = ( Dir + 1 ) % 2;

	if ( n_ka == 3 && Emeny[id].be_kick_counter < 4 )
		ka = 1;

	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	/****** ������ײЧ�� *******/

	if ( n_ka <= 7 && rand() % 6 == 0 && Emeny[id].be_kick_counter < 4 )
	{
		setbkmode( TRANSPARENT );
		setcolor( LIGHTGREEN );
		outtextxy( x + 20, y - 20, "��!" );
		int dx = rand() % 20 + 10;
		int dy = rand() % 10 + 20;
		putimage( x + dx, y + dy, &PLAYER::light[1], SRCAND );
		putimage( x + dx, y + dy, &PLAYER::light[0], SRCINVERT );
	}

	/***** ������ҹ�����Ч ****/

	if ( n_ka == 0 && Emeny[id].be_kick_counter == 3 )
	{
		Player[Emeny[id].role].Set_bol( 0x00 );
		f[Emeny[id].role] = &Role::Dragon_fist;

		/**** ˦�� ****/

		if ( Emeny[id].role == 0 && GetAsyncKeyState('S') & 0x8000 ||
			 Emeny[id].role == 1 && GetAsyncKeyState(VK_DOWN) & 0x8000 )	
		{
			n_ta = ta = 0;
			n_ka = ka = 0;
			f[Emeny[id].role] = &Role::Throw_role;

			if ( Player[Emeny[id].role].role_kind == 2 )
			{
				Emeny[id].lying = true;
				mf[id] = &BASE_CLASS::Be_catch;
				Player[Emeny[id].role].skill = THROW_BACK;
			}
			else
			{
				Emeny[id].be_kick_counter = 0;
				mf[id] = &BASE_CLASS::Throw_away;
			}
			return;
		}
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 2;
	}

	/****** ���ι������� ******/

	if ( n_ka == 7 && Emeny[id].be_kick_counter < 4 )
	{
		n_ka = ka = 0;
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level];

		/****/

		if ( Emeny[id].hp <= 0 )
		{
			Emeny[id].start_fly = true;
			Emeny[id].lying = true;
			Emeny[id].be_kick_counter = 5;
			return;
		}

		/*****/

		if ( GetAsyncKeyState('J') & 0x8000 && Emeny[id].role == 0 ||
			 GetAsyncKeyState(VK_NUMPAD1) & 0x8000 && Emeny[id].role == 1 )
			 Emeny[id].be_kick_counter++;

		if ( Emeny[id].be_kick_counter < 4 )
		{
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/***** ���ø߷�·�� ********/

	if ( Emeny[id].be_kick_counter == 4 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 42, -42 };
			X = x + Dx[Dir];
			Y = y - 160;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}

		if ( n_ka == 42 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 87 )
		{
			if ( n_ka < 30 )
			{
				int dx[2] = { -30, 30 };
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][0], SRCINVERT );
			}

			if ( n_ka <= 7  )					ka = 0;
			if ( n_ka >= 8 && n_ka <= 30 )		ka = 2;
			if ( n_ka >= 31 )					ka = 3;
			if ( n_ka >= 76 )					ka = 4;

			if ( n_ka >= 0 && n_ka <= 41 )
			{
				if ( x - Drtx[Dir] / 2 >= MIN_X && x - Drtx[Dir] / 2 <= MAX_X )
					x -= Drtx[Dir] / 2;
				else
					temp_x++;
				temp_y = (x - temp_x * Drtx[Dir] / 2 - X) * (x - temp_x * Drtx[Dir] / 2 - X) / 11 + Y;
			}

			if ( n_ka >= 42 && n_ka <= 51 )
			{
				int dx = Drtx[Dir] / 4;
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
		}

		if ( n_ka == 44 || n_ka == 51 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 51;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 51 && Emeny[id].hp <= 0 )
		{
			temp_x = 0;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Die;
			return;
		}

		if ( n_ka > 87 )
		{
			temp_x = 0;
			Emeny[id].lying = false;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/****** ���õͷ�·�� ******/

	if ( Emeny[id].be_kick_counter == 5 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 10, -10 };
			X = x + Dx[Dir];
			Y = y - 40;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}

		if ( n_ka == 20 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 64 )
		{
			if ( n_ka <= 15  )					ka = 2;
			if ( n_ka >= 16 && n_ka <= 40 )		ka = 3;
			if ( n_ka >= 41 )					ka = 4;

			int dx = Drtx[Dir] / 4;
			if ( n_ka <= 19 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;

				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/2.5 ) + Y;
			}

			if ( n_ka >= 0 && n_ka <= 11 )
			{
				int dx[2] = { -25, 25 };
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][0], SRCINVERT );
			}

			if ( n_ka > 19 && n_ka <= 29 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
		}

		if ( n_ka == 20 || n_ka == 29 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 29;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 29 && Emeny[id].hp <= 0 )
		{
			mf[id] = &BASE_CLASS::Die;
			Emeny[id].be_catch = false;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			n_ka = ka = 0;
			n_bc = bc = 0;
			Emeny[id].be_kick_counter = 0;
			temp_x = 0;
			return;
		}

		if ( n_ka > 64 )
		{
			n_bc = bc = 0;
			n_ka = ka = 0;
			mf[id] = &BASE_CLASS::Walk;
			Emeny[id].lying = false;
			temp_x = 0;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			Emeny[id].be_kick_counter = 0;
			return;
		}
	}
	n_ka++;
}

//

void Big_Dai::Throw_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -60, 60 };

	int _x = Player[Emeny[id].role].Rp->x;
	int _y = Player[Emeny[id].role].Rp->y;

	if ( n_ta == 0 )
		Emeny[id].lying = true;

	if ( n_ta <= 3 )
	{
		ta = 0;
		temp_y = _y;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 2;
		else
			x = _x - dx[Dir] * 2 / 3;
	}
	if ( n_ta >= 4 && n_ta <= 6 )
	{
		temp_y = _y - 20;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 3;
		else
			x = _x - dx[Dir] / 3;
	}
	if ( n_ta >= 7 && n_ta <= 9 )
	{
		temp_y = _y - 30;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 5;
		else
			x = _x + dx[Dir] / 6;
	}

	/************/

	if ( n_ta == 10 )
	{
		ta = 1;
		X = x + dx[Dir] / 3;
		Y = temp_y - 22;
		temp_x = 0;
	}

	if ( n_ta == 29 )
	{
		X = x + dx[Dir] / 12;
		Y = temp_y - 20;
		temp_x = 0;
	}

	/*******/

		int x1 = dx[Dir] / 10;
		if ( n_ta <= 38 && n_ta >= 10 )
		{
			if ( n_ta >= 29 )		x1 /= 6;
			if ( x + x1 >= MIN_X && x + x1 <= MAX_X )
				x += x1;
			else
				temp_x++;

			double k = 148;		// ע�⣬������뱣֤ k != 0
			if ( n_ta <= 19 )				ta = 1;
			if ( n_ta >= 20 && n_ta <= 28 )	ta = 2;
			if ( n_ta >= 29 && n_ta <= 38 )
			{
				if ( n_ta == 29 )
					Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 3;
				ta = 3;
				k = 1.2;
			}
			temp_y = int( (x+temp_x*x1-X)*(x+temp_x*x1-X)/k ) + Y;
		}

		if ( n_ta == 29 || n_ta == 39 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ta = 38;
				Emeny[id].hp = 0;	// ��������ļ��ʹ���˽�����
			}
		}

		if ( n_ta >= 60 && n_ta <= 82 )
			ta = 4;

	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );

	if ( n_ta == 38 && Emeny[id].hp <= 0 )
	{
		mf[id] = &BASE_CLASS::Die;
		Emeny[id].be_catch = false;
		Emeny[id].lying = true;
		n_ta = ta = 0;
		return;
	}

	/****** ��� 0-25 �׶��Ƿ����е��� *******/

	if ( n_ta >= 0 && n_ta <= 24 )
	{
		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;
					
					Emeny[i].hp -= LEVEL::harm[ Player[Emeny[id].role].level ];
				}
			}
		}
	}

	n_ta++;
	if ( n_ta >= 83)
	{
		n_ta = ta = 0;
		Emeny[id].be_catch = false;
		Emeny[id].lying = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

//

void Big_Dai::Be_catch( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -23, 25 };
	if ( Player[Emeny[id].role].skill != THROW_BACK )
		Dir = Player[Emeny[id].role].Rp->Dir;
	y = Player[Emeny[id].role].Rp->y;
	int _y = Player[Emeny[id].role].Rp->temp_y;
	int dy = 0;

	/*****/

	if ( Player[Emeny[id].role].skill == THUMP )
	{
		dy = 15;
		bc = 2;
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill != THROW_BACK && Player[Emeny[id].role].skill != THUMP )
	{
		bc = 1;
		if ( ++n_bc >= 6 )
		{
			Emeny[id].lying = false;
			n_bc = bc = 0;
		}
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill == THROW_BACK )
	{
			if ( n_bc >= 0 && n_bc <= 4 )
			{
				dx[0] = 24;
				dx[1] = -24;
				bc = 3;
			}
			if ( n_bc >= 5 && n_bc <= 8 )
			{
				dx[0] = 10;
				dx[1] = -10;
				dy = -40;
			}
			if ( n_bc >= 9 && n_bc <= 16 )
			{
				bc = 4;
				dx[0] = -40;
				dx[1] = 45;
				dy = 20;
			}
			if ( n_bc > 16 )
			{
				Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 4;
				n_bc = bc = 0;
				Dir++;
				mf[id] = &BASE_CLASS::Kick_away;
				Emeny[id].be_kick_counter = 5;
				Emeny[id].start_fly = true;

				putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
				putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );
				return;
			}
		n_bc++;

		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;

					Emeny[i].hp -= LEVEL::harm[Player[Emeny[id].role].level];
				}
			}
		}
	}

	x = Player[Emeny[id].role].Rp->x + dx[Dir];

	putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
	putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );

	if ( n_bc >= 10 && n_bc <= 14 )
	{
		putimage( x + 20, _y + 65, &PLAYER::light[1], SRCAND );
		putimage( x + 20, _y + 65, &PLAYER::light[0], SRCINVERT );
	}
}

//

void Big_Dai::Die( int id, const int& player )
{
	UNREFERENCED_PARAMETER( player );
	if ( ++n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}

	if ( n_d > 50 )
	{
		if ( rand() % WEAP == 0 && !WEAPON::out )
			Weapon_Out( x, y, id, player );

		if ( rand() % FOD == 0 && !FOOD::out )
		{
			FOOD::out = true;
			FOOD::X = x - Map.x[Map.map_id];
			FOOD::y = y;
			FOOD::time = 0;
		}

		x = y = 0;			// �����˶�����Ϸ��
		n_d = d = 0;
		mf[id] = &BASE_CLASS::Walk;
		Emeny[id].Mp->SetHp( Emeny[id].hp );
		Emeny[id].appear[ Map.part[Map.map_id] ] = false;

		if ( Map.part[Map.map_id] < 7 )
		{
			if ( Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ] > 0 )
				Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ]--;

			int sum = 0;
			for ( int i = 0; i < EMENY::emeny_num; i++ )
				sum += Emeny[i].live[Map.map_id][ Map.part[Map.map_id] ];

			/****** �����������ǰ���� boss ********/

			if ( sum <= 0 )
			{
				if ( Map.map_id == 4 && (Map.part[4] == 5 || Map.part[4] == 2) )
					Dispatch_five_boss( Map.part[4] );
				else
					Map.die_out[Map.map_id][ Map.part[Map.map_id] ] = true;
			}
		}
	}
}



///// ���� //////////////////////////////

Girl::Girl()
{
	int _x[2] = { -110, 550 };
	x = _x[rand() % 2];
	Revise_dispatch(x);		// ���������ڵ����ڳ��ֵ�λ��

	int min = Map.min_y[Map.map_id][Map.part[Map.map_id]];
	int max = Map.max_y[Map.map_id][Map.part[Map.map_id]];
	y = rand() % (max - min) + min;
	temp_y = y;

	Drtx[0] = -4;
	Drtx[1] = 4;
	X = Y = 0;
	temp_x = 0;

	step = 0;
	_a = 0;
	dx1 = rand() % 3 + 3;
	dy1 = rand() % 3 + 3;
	dx2 = rand() % 3 + 6;
	Dir = LEFT;
	attack_kind = 0;
	role = 0;
	leave = false;
	start = true;

	w = a = ka = ta = s = bc = d = 0;
	n_w = n_a = n_ka = n_ta = n_s = n_bc = n_d = 0;

	w_t1 = w_t2 = timeGetTime();

	int _level[6] = { 3,3,2,2,1,1 };
	int _dt[6] = { 120,90,85,70,65,66 };
	for ( int i = 0; i < 6; i++ )
	{
		w_dt[i] = _dt[i];
		attack_level[i] = _level[i];
	}
}

//

void Girl::Load()
{
	IMAGE img;
	loadimage( &img, "./res/Images/����.jpg", 800, 2000 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )		// 0 ��/����ͼ��1 ��/����ͼ
	{
		for ( int x = 0; x < 14; x++ )
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 80, i * 80, 80, 80 );		// 0
				getimage( &walk[0][x][i], x * 80, 160 + i * 80, 80, 80 );
			}

			if ( x < 5 )
			{
				getimage( &kick_away[1][x][i], x * 80, 640 + i * 80, 80, 80 );	// 2
				getimage( &kick_away[0][x][i], x * 80, 800 + i * 80, 80, 80 );

				getimage( &throw_away[1][x][i], x * 80, 960  + i * 80, 80, 80 );	// 3
				getimage( &throw_away[0][x][i], x * 80, 1120 + i * 80, 80, 80 );
			}

			if ( x < 5 )
			{
				getimage( &be_catch[1][x][i], x * 80, 1280 + i * 80, 80, 80 );	// 4
				getimage( &be_catch[0][x][i], x * 80, 1440 + i * 80, 80, 80 );
			}

			if ( x < 8 )
			{
				getimage( &attack[1][x][i], x * 80, 320 + i * 80, 80, 80 );		// 1
				getimage( &attack[0][x][i], x * 80, 480 + i * 80, 80, 80 );
			}
		}
	}
	SetWorkingImage();
}

//

void Girl::Stand( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &kick_away[(Dir + 1) % 2][1][1], SRCAND );
	putimage( x, y, &kick_away[(Dir + 1) % 2][1][0], SRCINVERT );

	if ( ++n_s > 45 )
	{
		w = a = ka = ta = s = bc = 0;
		n_w = n_a = n_ka = n_ta = n_s = n_bc = 0;
		Emeny[id].stand = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

void Girl::SetHp( int& hp )
{
	int a = Map.map_id;
	int b = Map.part[Map.map_id];

	hp = Hp[a][b];
}

//

void Girl::Walk( int id, const int& player )
{
	// ����ҵ��ڵ����÷����������ڸ�
	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y > Player[i].Rp->y )
			OTHER::emeny = true;
		else
			OTHER::emeny = false;
	}

	// ����
	if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
		 || Check_lift(PE_emeny, id) == _IN )
	{
		Emeny[id].hp = 0;
		mf[id] = &BASE_CLASS::Die;
		return;
	}

	Emeny[id].stand = false;
	Emeny[id].lying = false;
	if ( !Emeny[id].appear[ Map.part[Map.map_id] ] )
		return;

	/**** ���δ���˶�ʱ�� *************/

	w_t2 = timeGetTime();
	if ( w_t2 - w_t1 < w_dt[LEVEL::walk_dt] )
	{
		putimage( x, y, &walk[Dir][w][1], SRCAND );
		putimage( x, y, &walk[Dir][w][0], SRCINVERT );
		return;
	}
	else
		w_t1 = w_t2;

	/**** �������׷���Ķ��� ************/

	if ( start )
	{
		int num = rand() % player;
		if ( Player[num].life >= 0 )
			Emeny[id].role = num;
		start = false;
	}
	if ( rand() % 20 == 0 )
		start = true;

	int dx = 0, dy = 0;

	/***** ������� *************/

	if ( !leave )
	{
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x );

		/**** б���˶� ***********/

		if ( abs(y - Player[Emeny[id].role].Rp->y) > 5 )
		{
			if ( cos(_a) < 0 )	dx1 = -abs(dx1);
			if ( cos(_a) > 0 )	dx1 =  abs(dx1);
			if ( sin(_a) < 0 )	dy1 =  abs(dy1);
			if ( sin(_a) > 0 )	dy1 = -abs(dy1);

			dx = dx1;
			dy = dy1;
		}

		/**** ˮƽ�˶� ****/

		if ( abs(y - Player[Emeny[id].role].Rp->y) <= 5 )
		{
			if ( cos(_a) < 0 )	dx2 = -abs(dx2);
			if ( cos(_a) > 0 )	dx2 =  abs(dx2);

			dx = dx2;
			dy = 0;
		}
	}

	/**** ����ʼ��������� ****/

	if ( x < Player[Emeny[id].role].Rp->x )	Dir = RIGHT;
	if ( x > Player[Emeny[id].role].Rp->x )	Dir = LEFT;

	/**** �ﵽ���������������\Զ�� *****/

	if ( abs( x + dx - Player[Emeny[id].role].Rp->x) < 40 &&
		 abs( y + dy - Player[Emeny[id].role].Rp->y) < 10 )
	{
		leave = true;
		if ( abs( x + dx - Player[Emeny[id].role].Rp->x) > 10 )
		{
			n_a = n_w = 0;
			Emeny[id].attack = false;

			if ( rand() % attack_level[LEVEL::a_level] == 0 )
			{
				attack_kind = rand() % 2;
				switch ( attack_kind )
				{
					case 0:
						a = 0;
						break;

					case 1:
						X = x - Drtx[Dir] * 10;
						Y = y - 88;
						temp_y = y;
						a = 2;
						temp_x = 0;
						break;

					default:
						break;
				}
				Emeny[id].attack = true;
				mf[id] = &BASE_CLASS::Attack;
				putimage( x, y, &walk[Dir][w][1], SRCAND );
				putimage( x, y, &walk[Dir][w][0], SRCINVERT );
				return;
			}
		}
	}

	if ( leave && step == 0 )
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x ) + (double)(rand() % 314) / 100 + 1.57;

	if ( leave )
	{
		if ( ++step > 30 )
		{
			step = 0;
			leave = false;
		}

		dx = (int)( 6 * cos(_a) );
		dy = (int)(-6 * sin(_a) );
	}

	/**** һ��ͼƬ������ ****/

	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	if ( rand() % 68 == 0 )
	{
		n_s = 0;
		n_w = w = 0;
		mf[id] = &BASE_CLASS::Stand;
		return;
	}

	if ( ++n_w % 3 == 0 )
		w = ( ++w > 3 ) ? 0 : w;

	/**** ���Խ�� ****/
	
	if ( !Map.Check( y + dy ) || x + dx < -120 || x + dx > 560 || !Revise_lift(id, dx) )
	{
		step = 0;
		leave = false;
	}
	else
	{
		Revise_xy( id, x, y, dx, dy );		// ���������ƶ�����
		x += dx;
		y += dy;
		temp_y = y;
	}

	if ( x + dx > -120 && x + dx < -110 || x + dx > 550 && x + dx < 560 )
	{
		int _x[2] = { -110, 550 };
		int id = rand() % 2;

		x = _x[id]; 
	}
}

//

void Girl::Attack( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
		switch ( attack_kind )
		{
			case 0:
				putimage( x, y, &attack[Dir][a][1], SRCAND );
				putimage( x, y, &attack[Dir][a][0], SRCINVERT );

				if ( id < 3 )
				{
					putimage( x + 40, y - 20, &EMENY::papaw[1], SRCAND );
					putimage( x + 40, y - 20, &EMENY::papaw[0], SRCINVERT );

					setbkmode( TRANSPARENT );
					setcolor( LIGHTGREEN );
					setfont( 20, 0, "����" );

					char* c[3];
					c[0] = "����";
					c[1] = "������";
					c[2] = "fuck";
					outtextxy( x + 48, y - 15, c[id] );
				}

				if ( ++n_a == 5 )	a = 1;
				if ( n_a > 11 )
				{
					Emeny[id].attack = false;
					n_a = a = 0;
					mf[id] = &BASE_CLASS::Walk;
				}
				break;

			case 1:
				putimage( x, temp_y, &attack[Dir][a][1], SRCAND );
				putimage( x, temp_y, &attack[Dir][a][0], SRCINVERT );

				if ( x - Drtx[Dir] > MIN_X && x - Drtx[Dir] < MAX_X )
					x -= Drtx[Dir];
				else
					temp_x++;

				temp_y = (x - temp_x * Drtx[Dir] - X) * (x - temp_x * Drtx[Dir] - X) / 18 + Y;

				if ( ++n_a % 4 == 0 )	a++;
				if ( n_a >= 20 )
				{
					Emeny[id].attack = false;
					n_a = a = 0;
					temp_x = 0;
					mf[id] = &BASE_CLASS::Walk;
				}
				break;

			default:
				printf("���ӳ���\n");
				break;
		}

	int cx = 0;

	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( Emeny[id].attack && n_a < 3 && Player[i].Rp->Check_n_x( 0XFBB ) &&
			 abs( x - Player[i].Rp->x ) < 50 &&
			 abs( y - Player[i].Rp->y ) < 22 &&
			 (Dir == Player[i].Rp->Dir ||
			 Player[i].Rp->Getn_a() == 0) && Player[i].die == false )
		{
			cx = Player[i].Rp->x;
			if ( Check_position(Dir, x, cx) )
			{
				Emeny[id].attack = false;
				if ( attack_kind == 1 )
					Player[i].be_kick_counter = 3;

				Player[i].Set_bol( 0x00 );
				f[i] = &Role::Kick_down;

				if ( x > Player[i].Rp->x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;

				for ( int jj = 0; jj < EMENY::emeny_num; jj++ )
					if ( Emeny[jj].start_fly && !Emeny[jj].lying )
						Emeny[jj].be_kick_counter = 0;
			}
		}
	}
}

//

void Girl::Kick_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int _r = ( Dir + 1 ) % 2;

	if ( n_ka == 3 && Emeny[id].be_kick_counter < 4 )
		ka = 1;

	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	/****** ������ײЧ�� *******/

	if ( n_ka <= 7 && rand() % 6 == 0 && Emeny[id].be_kick_counter < 4 )
	{
		setbkmode( TRANSPARENT );
		setcolor( LIGHTGREEN );
		outtextxy( x + 20, y - 20, "��!" );
		int dx = rand() % 20 + 10;
		int dy = rand() % 10 + 20;
		putimage( x + dx, y + dy, &PLAYER::light[1], SRCAND );
		putimage( x + dx, y + dy, &PLAYER::light[0], SRCINVERT );
	}

	/***** ������ҹ�����Ч ****/

	if ( n_ka == 0 && Emeny[id].be_kick_counter == 3 )
	{
		Player[Emeny[id].role].Set_bol( 0x00 );
		f[Emeny[id].role] = &Role::Dragon_fist;

		/**** ˦�� ****/

		if ( Emeny[id].role == 0 && GetAsyncKeyState('S') & 0x8000 ||
			 Emeny[id].role == 1 && GetAsyncKeyState(VK_DOWN) & 0x8000 )	
		{
			n_ta = ta = 0;
			n_ka = ka = 0;
			f[Emeny[id].role] = &Role::Throw_role;

			if ( Player[Emeny[id].role].role_kind == 2 )
			{
				Emeny[id].lying = true;
				mf[id] = &BASE_CLASS::Be_catch;
				Player[Emeny[id].role].skill = THROW_BACK;
			}
			else
			{
				Emeny[id].be_kick_counter = 0;
				mf[id] = &BASE_CLASS::Throw_away;
			}
			return;
		}
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 2;
	}

	/****** ���ι������� ******/

	if ( n_ka == 7 && Emeny[id].be_kick_counter < 4 )
	{
		n_ka = ka = 0;
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level];

		/****/

		if ( Emeny[id].hp <= 0 )
		{
			Emeny[id].start_fly = true;
			Emeny[id].lying = true;
			Emeny[id].be_kick_counter = 5;
			return;
		}

		/*****/

		if ( GetAsyncKeyState('J') & 0x8000 && Emeny[id].role == 0 ||
			 GetAsyncKeyState(VK_NUMPAD1) & 0x8000 && Emeny[id].role == 1 )
			 Emeny[id].be_kick_counter++;

		if ( Emeny[id].be_kick_counter < 4 )
		{
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/***** ���ø߷�·�� ********/

	if ( Emeny[id].be_kick_counter == 4 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 42, -42 };
			X = x + Dx[Dir];
			Y = y - 160;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}

		if ( n_ka == 42 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 87 )
		{
			if ( n_ka < 30 )
			{
				int dx[2] = { -30, 30 };
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][0], SRCINVERT );
			}

			if ( n_ka <= 7  )					ka = 0;
			if ( n_ka >= 8 && n_ka <= 30 )		ka = 2;
			if ( n_ka >= 31 )					ka = 3;
			if ( n_ka >= 76 )					ka = 4;

			if ( n_ka >= 0 && n_ka <= 41 )
			{
				if ( x - Drtx[Dir] / 2 >= MIN_X && x - Drtx[Dir] / 2 <= MAX_X )
					x -= Drtx[Dir] / 2;
				else
					temp_x++;
				temp_y = (x - temp_x * Drtx[Dir] / 2 - X) * (x - temp_x * Drtx[Dir] / 2 - X) / 11 + Y;
			}

			if ( n_ka >= 42 && n_ka <= 51 )
			{
				int dx = Drtx[Dir] / 4;
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
		}

		if ( n_ka == 44 || n_ka == 51 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 51;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 51 && Emeny[id].hp <= 0 )
		{
			temp_x = 0;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Die;
			return;
		}

		if ( n_ka > 87 )
		{
			temp_x = 0;
			Emeny[id].lying = false;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/****** ���õͷ�·�� ******/

	if ( Emeny[id].be_kick_counter == 5 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 10, -10 };
			X = x + Dx[Dir];
			Y = y - 40;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}
		if ( n_ka == 20 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 64 )
		{
			if ( n_ka <= 15  )					ka = 2;
			if ( n_ka >= 16 && n_ka <= 40 )		ka = 3;
			if ( n_ka >= 41 )					ka = 4;

			int dx = Drtx[Dir] / 4;
			if ( n_ka <= 19 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;

				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/2.5 ) + Y;
			}

			if ( n_ka > 19 && n_ka <= 29 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
			if ( n_ka >= 0 && n_ka <= 11 )
			{
				int dx[2] = { -25, 25 };
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][0], SRCINVERT );
			}
		}

		if ( n_ka == 20 || n_ka == 29 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 29;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 29 && Emeny[id].hp <= 0 )
		{
			mf[id] = &BASE_CLASS::Die;
			Emeny[id].be_catch = false;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			n_ka = ka = 0;
			n_bc = bc = 0;
			Emeny[id].be_kick_counter = 0;
			temp_x = 0;
			return;
		}

		if ( n_ka > 64 )
		{
			n_bc = bc = 0;
			n_ka = ka = 0;
			mf[id] = &BASE_CLASS::Walk;
			Emeny[id].lying = false;
			temp_x = 0;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			Emeny[id].be_kick_counter = 0;
			return;
		}
	}
	n_ka++;
}

//

void Girl::Throw_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -60, 60 };

	int _x = Player[Emeny[id].role].Rp->x;
	int _y = Player[Emeny[id].role].Rp->y;

	if ( n_ta == 0 )
		Emeny[id].lying = true;

	if ( n_ta <= 3 )
	{
		ta = 0;
		temp_y = _y;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 2;
		else
			x = _x - dx[Dir] * 2 / 3;
	}
	if ( n_ta >= 4 && n_ta <= 6 )
	{
		temp_y = _y - 20;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 3;
		else
			x = _x - dx[Dir] / 3;
	}
	if ( n_ta >= 7 && n_ta <= 9 )
	{
		temp_y = _y - 30;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 5;
		else
			x = _x + dx[Dir] / 6;
	}

	/************/

	if ( n_ta == 10 )
	{
		ta = 1;
		X = x + dx[Dir] / 3;
		Y = temp_y - 22;
		temp_x = 0;
	}

	if ( n_ta == 29 )
	{
		X = x + dx[Dir] / 12;
		Y = temp_y - 20;
		temp_x = 0;
	}

	/*******/

		int x1 = dx[Dir] / 10;
		if ( n_ta <= 38 && n_ta >= 10 )
		{
			if ( n_ta >= 29 )		x1 /= 6;
			if ( x + x1 >= MIN_X && x + x1 <= MAX_X )
				x += x1;
			else
				temp_x++;

			double k = 148;		// ע�⣬������뱣֤ k != 0
			if ( n_ta <= 19 )				ta = 1;
			if ( n_ta >= 20 && n_ta <= 28 )	ta = 2;
			if ( n_ta >= 29 && n_ta <= 38 )
			{
				if ( n_ta == 29 )
					Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 3;
				ta = 3;
				k = 1.2;
			}
			temp_y = int( (x+temp_x*x1-X)*(x+temp_x*x1-X)/k ) + Y;
		}

		if ( n_ta == 29 || n_ta == 39 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ta = 38;
				Emeny[id].hp = 0;	// ��������ļ��ʹ���˽�����
			}
		}

		if ( n_ta >= 60 && n_ta <= 82 )
			ta = 4;

	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );

	if ( n_ta == 38 && Emeny[id].hp <= 0 )
	{
		mf[id] = &BASE_CLASS::Die;
		Emeny[id].be_catch = false;
		Emeny[id].lying = true;
		n_ta = ta = 0;
		return;
	}

	/****** ��� 0-25 �׶��Ƿ����е��� *******/

	if ( n_ta >= 0 && n_ta <= 24 )
	{
		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;
					
					Emeny[i].hp -= LEVEL::harm[ Player[Emeny[id].role].level ];
				}
			}
		}
	}

	n_ta++;
	if ( n_ta >= 83)
	{
		n_ta = ta = 0;
		Emeny[id].be_catch = false;
		Emeny[id].lying = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

void Girl::Be_catch( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -23, 25 };
	if ( Player[Emeny[id].role].skill != THROW_BACK )
		Dir = Player[Emeny[id].role].Rp->Dir;

	y = Player[Emeny[id].role].Rp->y;
	int _y = Player[Emeny[id].role].Rp->temp_y;
	int dy = 0;

	/*****/

	if ( Player[Emeny[id].role].skill == THUMP )
	{
		dy = 15;
		bc = 2;
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill != THROW_BACK && Player[Emeny[id].role].skill != THUMP )
	{
		bc = 1;
		if ( ++n_bc >= 6 )
		{
			Emeny[id].lying = false;
			n_bc = bc = 0;
		}
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill == THROW_BACK )
	{
			if ( n_bc >= 0 && n_bc <= 4 )
			{
				dx[0] = 24;
				dx[1] = -24;
				bc = 3;
			}
			if ( n_bc >= 5 && n_bc <= 8 )
			{
				dx[0] = 10;
				dx[1] = -10;
				dy = -40;
			}
			if ( n_bc >= 9 && n_bc <= 16 )
			{
				bc = 4;
				dx[0] = -40;
				dx[1] = 45;
				dy = 20;
			}
			if ( n_bc > 16 )
			{
				Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 4;
				n_bc = bc = 0;
				Dir++;
				mf[id] = &BASE_CLASS::Kick_away;
				Emeny[id].be_kick_counter = 5;
				Emeny[id].start_fly = true;
				putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
				putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );
				return;
			}
		n_bc++;

		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;

					Emeny[i].hp -= LEVEL::harm[Player[Emeny[id].role].level];
				}
			}
		}
	}

	x = Player[Emeny[id].role].Rp->x + dx[Dir];

	putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
	putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );

	if ( n_bc >= 10 && n_bc <= 14 )
	{
		putimage( x + 20, _y + 65, &PLAYER::light[1], SRCAND );
		putimage( x + 20, _y + 65, &PLAYER::light[0], SRCINVERT );
	}
}

//

void Girl::Die( int id, const int& player )
{
	UNREFERENCED_PARAMETER( player );
	if ( ++n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}

	if ( n_d > 50 )
	{
		if ( rand() % WEAP == 0 && !WEAPON::out )
			Weapon_Out( x, y, id, player );

		if ( rand() % FOD == 0 && !FOOD::out )
		{
			FOOD::out = true;
			FOOD::X = x - Map.x[Map.map_id];
			FOOD::y = y;
			FOOD::time = 0;
		}

		x = y = 0;			// �����˶�����Ϸ��
		n_d = d = 0;
		mf[id] = &BASE_CLASS::Walk;
		Emeny[id].Mp->SetHp( Emeny[id].hp );
		Emeny[id].appear[ Map.part[Map.map_id] ] = false;

		if ( Map.part[Map.map_id] < 7 )
		{
			if ( Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ] > 0 )
				Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ]--;

			int sum = 0;
			for ( int i = 0; i < EMENY::emeny_num; i++ )
				sum += Emeny[i].live[Map.map_id][ Map.part[Map.map_id] ];

			/****** �����������ǰ���� boss ********/

			if ( sum <= 0 )
			{
				if ( Map.map_id == 4 && (Map.part[4] == 5 || Map.part[4] == 2) )
					Dispatch_five_boss( Map.part[4] );
				else
					Map.die_out[Map.map_id][ Map.part[Map.map_id] ] = true;
			}
		}
	}
}



///// �C��ͷ ////////////////////////////////////////////////////////////

Thin_Old_Man::Thin_Old_Man()
{
	int _x[2] = { -110, 550 };
	x = _x[rand() % 2];
	Revise_dispatch(x);		// ���������ڵ����ڳ��ֵ�λ��

	int min = Map.min_y[Map.map_id][Map.part[Map.map_id]];
	int max = Map.max_y[Map.map_id][Map.part[Map.map_id]];
	y = rand() % (max - min) + min;
	temp_y = y;

	knife_x = knife_y = 0;
	k_dir = LEFT;

	Drtx[0] = -4;
	Drtx[1] = 4;
	X = Y = 0;
	temp_x = 0;

	step = 0;
	_a = 0;
	dx1 = rand() % 3 + 3;
	dy1 = rand() % 3 + 3;
	dx2 = rand() % 3 + 6;
	Dir = LEFT;
	attack_kind = 0;
	role = 0;
	leave = false;
	start = true;

	w = a = ka = ta = s = bc = d = 0;
	n_w = n_a = n_ka = n_ta = n_s = n_bc = n_d = 0;

	w_t1 = w_t2 = timeGetTime();

	int _level[6] = { 4,4,3,3,2,2 };
	int _dt[6] = { 90,85,75,60,50,45 };
	for ( int i = 0; i < 6; i++ )
	{
		w_dt[i] = _dt[i];
		attack_level[i] = _level[i];
	}
}

//

void Thin_Old_Man::Load()
{
	IMAGE img;
	loadimage( &img, "./res/Images/�C��ͷ.jpg", 800, 2000 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )		// 0 ��/����ͼ��1 ��/����ͼ
	{
		for ( int x = 0; x < 14; x++ )
		{
			if ( x < 5 )
			{
				getimage( &walk[1][x][i], x * 80, i * 80, 80, 80 );		// 0
				getimage( &walk[0][x][i], x * 80, 160 + i * 80, 80, 80 );
			}

			if ( x < 10 )
			{
				getimage( &attack[1][x][i], x * 80, 320 + i * 80, 80, 80 );		// 1
				getimage( &attack[0][x][i], x * 80, 480 + i * 80, 80, 80 );
			}

			if ( x < 5 )
			{
				getimage( &kick_away[1][x][i], x * 80, 640 + i * 80, 80, 80 );	// 2
				getimage( &kick_away[0][x][i], x * 80, 800 + i * 80, 80, 80 );

				getimage( &throw_away[1][x][i], x * 80, 960  + i * 80, 80, 80 );	// 3
				getimage( &throw_away[0][x][i], x * 80, 1120 + i * 80, 80, 80 );
			}

			if ( x < 5 )
			{
				getimage( &be_catch[1][x][i], x * 80, 1280 + i * 80, 80, 80 );	// 4
				getimage( &be_catch[0][x][i], x * 80, 1440 + i * 80, 80, 80 );
			}
		}
	}
	SetWorkingImage();
}

void Thin_Old_Man::SetHp( int& hp )
{
	int a = Map.map_id;
	int b = Map.part[Map.map_id];

	hp = Hp[a][b];
}

//

void Thin_Old_Man::Stand( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &walk[Dir][4][1], SRCAND );
	putimage( x, y, &walk[Dir][4][0], SRCINVERT );

	if ( ++n_s > 45 )
	{
		w = a = ka = ta = s = bc = 0;
		n_w = n_a = n_ka = n_ta = n_s = n_bc = 0;
		Emeny[id].stand = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

//

void Thin_Old_Man::Walk( int id, const int& player )
{
	// ����ҵ��ڵ����÷����������ڸ�
	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y > Player[i].Rp->y )
			OTHER::emeny = true;
		else
			OTHER::emeny = false;
	}

	// ����
	if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
		 || Check_lift(PE_emeny, id) == _IN )
	{
		Emeny[id].hp = 0;
		mf[id] = &BASE_CLASS::Die;
		return;
	}

	Emeny[id].stand = false;
	Emeny[id].lying = false;
	if ( !Emeny[id].appear[ Map.part[Map.map_id] ] )
		return;

	/**** ���δ���˶�ʱ�� *************/

	w_t2 = timeGetTime();
	if ( w_t2 - w_t1 < w_dt[LEVEL::walk_dt] )
	{
		putimage( x, y, &walk[Dir][w][1], SRCAND );
		putimage( x, y, &walk[Dir][w][0], SRCINVERT );
		return;
	}
	else
		w_t1 = w_t2;

	/**** �������׷���Ķ��� ************/

	if ( start )
	{
		int num = rand() % player;
		if ( Player[num].life >= 0 )
			Emeny[id].role = num;
		start = false;
	}
	if ( rand() % 30 == 0 )
		start = true;

	int dx = 0, dy = 0;

	/***** ������� *************/

	if ( !leave )
	{
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x );

		/**** б���˶� ***********/

		if ( abs(y - Player[Emeny[id].role].Rp->y) > 5 )
		{
			if ( cos(_a) < 0 )	dx1 = -abs(dx1);
			if ( cos(_a) > 0 )	dx1 =  abs(dx1);
			if ( sin(_a) < 0 )	dy1 =  abs(dy1);
			if ( sin(_a) > 0 )	dy1 = -abs(dy1);

			dx = dx1;
			dy = dy1;
		}

		/**** ˮƽ�˶� ****/

		if ( abs(y - Player[Emeny[id].role].Rp->y) <= 5 )
		{
			if ( cos(_a) < 0 )	dx2 = -abs(dx2);
			if ( cos(_a) > 0 )	dx2 =  abs(dx2);

			dx = dx2;
			dy = 0;
		}
	}

	/**** ����ʼ��������� ****/

	if ( x < Player[Emeny[id].role].Rp->x )	Dir = RIGHT;
	if ( x > Player[Emeny[id].role].Rp->x )	Dir = LEFT;

	/**** �ﵽ�������Զ�� *****/

	if ( abs( x + dx - Player[Emeny[id].role].Rp->x) < 40 &&
		 abs( y + dy - Player[Emeny[id].role].Rp->y) < 10 )
		leave = true;

	/***** ����ɵ� *********/

	if ( abs(x + dx - Player[Emeny[id].role].Rp->x) >= 140 &&
		 abs(x + dx - Player[Emeny[id].role].Rp->x) <= 150 &&
		 abs(y + dy - Player[Emeny[id].role].Rp->y) < 18 )
	{
		n_a = a = n_w = 0;
		Emeny[id].attack = false;

		if ( rand() % attack_level[LEVEL::a_level] == 0 )
		{
			int _x[2] = { -20, 20 };
			knife_x = x + _x[Dir];
			knife_y = y;
			k_dir = Dir;

			a = 7;
			attack_kind = 2;
			turn_dir = false;
			Emeny[id].attack = true;
			mf[id] = &BASE_CLASS::Attack;
			putimage( x, y, &walk[Dir][w][1], SRCAND );
			putimage( x, y, &walk[Dir][w][0], SRCINVERT );
			return;
		}
	}

	/***** ���ؽš��������� *********/

	if ( abs(x + dx - Player[Emeny[id].role].Rp->x) >= 60 &&
		 abs(x + dx - Player[Emeny[id].role].Rp->x) <= 70 &&
		 abs(y + dy - Player[Emeny[id].role].Rp->y) < 18 )
	{
		n_a = n_w = 0;
		Emeny[id].attack = false;

		if ( rand() % attack_level[LEVEL::a_level] == 0 )
		{
			Emeny[id].attack = true;
			attack_kind = rand() % 2;
			switch ( attack_kind )
			{
				case 0:
					a = 0;
					mf[id] = &BASE_CLASS::Attack;
					putimage( x, y, &walk[Dir][w][1], SRCAND );
					putimage( x, y, &walk[Dir][w][0], SRCINVERT );
					return;

				case 1:
					a = 2;
					X = x + Drtx[Dir] * 25;
					Y = y - 50;
					temp_x = 0;
					temp_y = y;
					mf[id] = &BASE_CLASS::Attack;

					putimage( x, y, &walk[Dir][w][1], SRCAND );
					putimage( x, y, &walk[Dir][w][0], SRCINVERT );
					return;

				default:
					printf("�C��ͷ����\n");
					break;
			}
		}
	}

	if ( leave && step == 0 )
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x ) + (double)(rand() % 314) / 100 + 1.57;

	if ( leave )
	{
		if ( ++step > 30 )
		{
			step = 0;
			leave = false;
		}

		dx = (int)( 6 * cos(_a) );
		dy = (int)(-6 * sin(_a) );
	}

	/**** һ��ͼƬ������ ****/

	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	if ( rand() % 68 == 0 )
	{
		n_s = 0;
		n_w = w = 0;
		mf[id] = &BASE_CLASS::Stand;
		return;
	}

	if ( ++n_w % 2 == 0 )
		w = ( ++w > 3 ) ? 0 : w;

	/**** ���Խ�� ****/
	
	if ( !Map.Check( y + dy ) || x + dx < -120 || x + dx > 560 || !Revise_lift(id, dx) )
	{
		step = 0;
		leave = false;
	}
	else
	{
		Revise_xy( id, x, y, dx, dy );		// ���������ƶ�����
		x += dx;
		y += dy;
		temp_y = y;
	}

	if ( x + dx > -120 && x + dx < -110 || x + dx > 550 && x + dx < 560 )
	{
		int _x[2] = { -110, 550 };
		int id = rand() % 2;

		x = _x[id]; 
	}
}

//

void Thin_Old_Man::Attack( int id, const int& player )
{
	if ( n_a < 11 )
	{
		if ( id < 3 )
		{
			putimage( x + 40, y - 20, &EMENY::papaw[1], SRCAND );
			putimage( x + 40, y - 20, &EMENY::papaw[0], SRCINVERT );

			setbkmode( TRANSPARENT );
			setcolor( LIGHTGREEN );
			setfont( 20, 0, "����" );

			char* c[3];
			c[0] = "Ԥ��";
			c[1] = "������";
			c[2] = "������";
			outtextxy( x + 44, y - 15, c[id] );
		}
	}

	UNREFERENCED_PARAMETER(player);
	int i = 0;
	switch ( attack_kind )
	{
		/***** ���ؽ� ********/

		case 0:
			putimage( x, y, &attack[Dir][a][1], SRCAND );
			putimage( x, y, &attack[Dir][a][0], SRCINVERT );

			if ( ++n_a >= 14 )
			{
				a = 1;
				int dx = Drtx[Dir] * 5 / 2;
				if ( x + dx >= MIN_X && x + dx <= MAX_X && Check_lift_xy(x+dx) == _UNIN )
					x += dx;
				if ( Check_lift_xy(x+dx) == _IN )
					n_a = 36;
			}
			if ( n_a > 35 )
			{
				n_a = a = 0;
				Emeny[id].attack = false;
				mf[id] = &BASE_CLASS::Walk;
			}
			break;

		/***** �������� ********/

		case 1:
			putimage( x, temp_y, &attack[Dir][a][1], SRCAND );
			putimage( x, temp_y, &attack[Dir][a][0], SRCINVERT );

			if ( ++n_a == 16 )	a = 3;

			if ( n_a > 16 )
			{
				int dx = Drtx[Dir] * 5 / 2;
				if ( x + dx > MIN_X && x + dx < MAX_X )
					x += dx;
				else
					temp_x++;

				if ( n_a % 5 == 0 && a < 6 )
					a++;
////////////////////////// �ô� bug���������ͷ�����������׶α������������ܱ��� y �����ϵ�ƽ��
				temp_y = (x + temp_x * dx - X) * (x + temp_x * dx - X) / 200 + Y;
			}

			if ( n_a >= 36 )
			{
				n_a = a = 0;
				Emeny[id].attack = false;
				mf[id] = &BASE_CLASS::Walk;
			}
			break;

		/**** �ɵ� *******/

		case 2:
			putimage( x, y, &attack[Dir][a][1], SRCAND );
			putimage( x, y, &attack[Dir][a][0], SRCINVERT );

			if ( ++n_a >= 10 )
			{
				a = 8;
				int dx = Drtx[k_dir] * 5 / 2;
				if ( knife_x + dx >= MIN_X && knife_x + dx <= MAX_X )
					knife_x += dx;
				else
				{
					n_a = a = 0;
					Emeny[id].attack = false;
					mf[id] = &BASE_CLASS::Walk;
				}

				putimage( knife_x, knife_y, &attack[k_dir][9][1], SRCAND );
				putimage( knife_x, knife_y, &attack[k_dir][9][0], SRCINVERT );
			}

			for ( i = 0; i < PLAYER::player_num; i++ )
			{
				/***** �ɵ�������Ч �� ������ *********/

				if ( n_a >= 10 && Player[i].Rp->Check_n_x( 0X9BB ) &&
					abs( knife_x - Player[i].Rp->x ) < 20 && 
					abs( knife_y - Player[i].Rp->y ) < 18 )
				{
					/***** �������Ҳ���Ըı�ɵ����򣬴��� ******/

					if ( k_dir != Player[i].Rp->Dir &&
						 Player[i].Rp->Getn_a() != 0 || Player[i].Rp->n_bb != 0 )
					{
						turn_dir = true;
						k_dir = ( ++k_dir > 1 ) ? 0 : k_dir;
					}

					if ( !turn_dir )
					{
						if ( Player[i].die == false )
							if ( k_dir == Player[i].Rp->Dir || Player[i].Rp->Getn_a() == 0 )
							{
								Player[i].be_kick_counter = 3;
								Player[i].Set_bol( 0x00 );
								f[i] = &Role::Kick_down;

								if ( knife_x > Player[i].Rp->x )
									Player[i].Rp->Dir = RIGHT;
								else
									Player[i].Rp->Dir = LEFT;
		///////////////////////////////////////////////////////////���ܻ������һ������ڹ����ĵ���
								for ( int jj = 0; jj < EMENY::emeny_num; jj++ )
									if ( Emeny[jj].start_fly && !Emeny[jj].lying )
										Emeny[jj].be_kick_counter = 0;
							}
					}
				}

				/***************/

				if ( Player[i].skill == LIGHT_WAVE && !turn_dir )
				{
					if ( abs( knife_x - Player[i].Rp->Get_wavex() ) < 20 && 
						 abs( knife_y - Player[i].Rp->Get_wavey() ) < 18 && n_a >= 10 )
					{
						turn_dir = true;
						k_dir = ( ++k_dir > 1 ) ? 0 : k_dir;
					}
				}

				/******* �����ǰ�Ƿ��� ********/

				if ( turn_dir )
				{
					for ( int i = 0; i < EMENY::emeny_num; i++ )
					{
						if ( abs(knife_x - Emeny[i].Mp->Getx()) < 20 &&
							 abs(knife_y - Emeny[i].Mp->Gety()) < 18 && !Emeny[i].lying )
						{
							if ( Emeny[i].be_catch )
								Player[(Emeny[i].role + 1) % 2].catch_emeny = false;

							Emeny[i].be_catch = false;
							Emeny[i].be_kick_counter = 5;
							Emeny[i].start_fly = true;
							mf[i] = &BASE_CLASS::Kick_away;

							Emeny[i].Mp->SetDir( RIGHT );
							if ( knife_x <= x )
								Emeny[i].Mp->SetDir( LEFT );

							Emeny[i].hp -= 20;
						}
					}
				}
			}
			return;

		default:
			printf("����ͷ����");
			break;
	}

	int cx = 0;

	/***** �Ƿɵ��Ĺ�����Ч *********/

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
		if ( Emeny[id].attack && n_a > 17 && Player[i].Rp->Check_n_x( 0XFBB ) &&
			 abs( y - Player[i].Rp->y ) < 18 && Player[i].die == false &&
			 abs( x - Player[i].Rp->x ) < 50 && attack_kind != 2 )
		{
			cx = Player[i].Rp->x;
			if ( Check_position(Dir, x, cx) )
			{
				if ( attack_kind == 0 || attack_kind == 1 )
					Player[i].be_kick_counter = 3;

				Player[i].Set_bol( 0x00 );
				f[i] = &Role::Kick_down;

				if ( x > Player[i].Rp->x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;

				for ( int jj = 0; jj < EMENY::emeny_num; jj++ )
					if ( Emeny[jj].start_fly && !Emeny[jj].lying )
						Emeny[jj].be_kick_counter = 0;
			}
		}
	}
}

//

void Thin_Old_Man::Kick_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int _r = ( Dir + 1 ) % 2;	// �����ɷ����������෴

	if ( n_ka == 3 && Emeny[id].be_kick_counter < 4 )
		ka = 1;

	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	/****** ������ײЧ�� *******/

	if ( n_ka <= 7 && rand() % 6 == 0 && Emeny[id].be_kick_counter < 4 )
	{
		setbkmode( TRANSPARENT );
		setcolor( LIGHTGREEN );
		outtextxy( x + 20, y - 20, "��!" );
		int dx = rand() % 20 + 10;
		int dy = rand() % 10 + 20;
		putimage( x + dx, y + dy, &PLAYER::light[1], SRCAND );
		putimage( x + dx, y + dy, &PLAYER::light[0], SRCINVERT );
	}

	/***** ������ҹ�����Ч ****/

	if ( n_ka == 0 && Emeny[id].be_kick_counter == 3 )
	{
		Player[Emeny[id].role].Set_bol( 0x00 );
		f[Emeny[id].role] = &Role::Dragon_fist;

		/**** ˦�� ****/

		if ( Emeny[id].role == 0 && GetAsyncKeyState('S') & 0x8000 ||
			 Emeny[id].role == 1 && GetAsyncKeyState(VK_DOWN) & 0x8000 )
		{
			n_ta = ta = 0;
			n_ka = ka = 0;
			f[Emeny[id].role] = &Role::Throw_role;

			if ( Player[Emeny[id].role].role_kind == 2 )
			{
				Emeny[id].lying = true;
				mf[id] = &BASE_CLASS::Be_catch;
				Player[Emeny[id].role].skill = THROW_BACK;
			}
			else
			{
				Emeny[id].be_kick_counter = 0;
				mf[id] = &BASE_CLASS::Throw_away;
			}
			return;
		}

		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 2;
	}

	/****** ���ι������� ******/
	// �п��ܱ�����ȭ���

	if ( n_ka == 7 && Emeny[id].be_kick_counter < 4 )
	{
		n_ka = ka = 0;
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level];

		/****/

		if ( Emeny[id].hp <= 0 )
		{
			Emeny[id].start_fly = true;
			Emeny[id].lying = true;
			Emeny[id].be_kick_counter = 5;
			return;
		}

		/*****/

		if ( GetAsyncKeyState('J') & 0x8000 && Emeny[id].role == 0 ||
			 GetAsyncKeyState(VK_NUMPAD1) & 0x8000 && Emeny[id].role == 1 )
			 Emeny[id].be_kick_counter++;

		if ( Emeny[id].be_kick_counter < 4 )
		{
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/***** ���ø߷�·�� ********/

	if ( Emeny[id].be_kick_counter == 4 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 42, -42 };
			X = x + Dx[Dir];
			Y = y - 160;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}

		if ( n_ka == 42 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 87 )
		{
			if ( n_ka < 30 )
			{
				int dx[2] = { -30, 30 };
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][0], SRCINVERT );
			}

			if ( n_ka <= 7  )					ka = 0;
			if ( n_ka >= 8 && n_ka <= 30 )		ka = 2;
			if ( n_ka >= 31 )					ka = 3;
			if ( n_ka >= 76 )					ka = 4;

			if ( n_ka >= 0 && n_ka <= 41 )
			{
				if ( x - Drtx[Dir] / 2 >= MIN_X && x - Drtx[Dir] / 2 <= MAX_X )
					x -= Drtx[Dir] / 2;
				else
					temp_x++;
				temp_y = (x - temp_x * Drtx[Dir] / 2 - X) * (x - temp_x * Drtx[Dir] / 2 - X) / 11 + Y;
			}

			if ( n_ka >= 42 && n_ka <= 51 )
			{
				int dx = Drtx[Dir] / 4;
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
		}

		if ( n_ka == 44 || n_ka == 51 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 51;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 51 && Emeny[id].hp <= 0 )
		{
			temp_x = 0;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Die;

			return;
		}

		if ( n_ka > 87 )
		{
			temp_x = 0;
			Emeny[id].lying = false;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/****** ���õͷ�·�� ******/

	if ( Emeny[id].be_kick_counter == 5 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 10, -10 };
			X = x + Dx[Dir];
			Y = y - 40;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}
		if ( n_ka == 20 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 64 )
		{
			setbkmode( TRANSPARENT );
			setcolor( BLUE );
			if ( n_ka < 30 )
				outtextxy( x + 40, y + 25, "����.." );

			if ( n_ka <= 15  )					ka = 2;
			if ( n_ka >= 16 && n_ka <= 40 )		ka = 3;
			if ( n_ka >= 41 )					ka = 4;

			int dx = Drtx[Dir] / 4;
			if ( n_ka <= 19 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/2.5 ) + Y;
			}

			if ( n_ka > 19 && n_ka <= 29 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
			if ( n_ka >= 0 && n_ka <= 11 )
			{
				int dx[2] = { -25, 25 };
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 35, &PLAYER::blood[Dir][n_ka/2][0], SRCINVERT );
			}
		}

		if ( n_ka == 20 || n_ka == 29 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 29;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 29 && Emeny[id].hp <= 0 )
		{
			mf[id] = &BASE_CLASS::Die;
			Emeny[id].be_catch = false;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			n_ka = ka = 0;
			n_bc = bc = 0;
			Emeny[id].be_kick_counter = 0;
			temp_x = 0;
			return;
		}

		if ( n_ka > 64 )
		{
			n_bc = bc = 0;	// С�ֿ��ܻ�ʹ�������
			n_ka = ka = 0;
			mf[id] = &BASE_CLASS::Walk;
			Emeny[id].lying = false;
			temp_x = 0;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			Emeny[id].be_kick_counter = 0;
			return;
		}
	}
	n_ka++;
}

void Thin_Old_Man::Throw_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -60, 60 };

	int _x = Player[Emeny[id].role].Rp->x;
	int _y = Player[Emeny[id].role].Rp->y;

	if ( n_ta == 0 )
		Emeny[id].lying = true;

	if ( n_ta <= 3 )
	{
		ta = 0;
		temp_y = _y;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 2;
		else
			x = _x - dx[Dir] * 2 / 3;
	}
	if ( n_ta >= 4 && n_ta <= 6 )
	{
		temp_y = _y - 20;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 3;
		else
			x = _x - dx[Dir] / 3;
	}
	if ( n_ta >= 7 && n_ta <= 9 )
	{
		temp_y = _y - 30;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 5;
		else
			x = _x + dx[Dir] / 6;
	}

	/************/

	if ( n_ta == 10 )
	{
		ta = 1;
		X = x + dx[Dir] / 3;
		Y = temp_y - 22;
		temp_x = 0;
	}

	if ( n_ta == 29 )
	{
		X = x + dx[Dir] / 12;
		Y = temp_y - 20;
		temp_x = 0;
	}

	/*******/

		int x1 = dx[Dir] / 10;
		if ( n_ta <= 38 && n_ta >= 10 )
		{
			if ( n_ta >= 29 )		x1 /= 6;
			if ( x + x1 >= MIN_X && x + x1 <= MAX_X )
				x += x1;
			else
				temp_x++;

			double k = 148;		// ע�⣬������뱣֤ k != 0
			if ( n_ta <= 19 )				ta = 1;
			if ( n_ta >= 20 && n_ta <= 28 )	ta = 2;
			if ( n_ta >= 29 && n_ta <= 38 )
			{
				if ( n_ta == 29 )
					Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 3;
				ta = 3;
				k = 1.2;
			}
			temp_y = int( (x+temp_x*x1-X)*(x+temp_x*x1-X)/k ) + Y;
		}

		if ( n_ta == 29 || n_ta == 39 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ta = 38;
				Emeny[id].hp = 0;	// ��������ļ��ʹ���˽�����
			}
		}

		if ( n_ta >= 60 && n_ta <= 82 )
			ta = 4;

	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );

	if ( n_ta == 38 && Emeny[id].hp <= 0 )
	{
		mf[id] = &BASE_CLASS::Die;
		Emeny[id].be_catch = false;
		Emeny[id].lying = true;
		n_ta = ta = 0;
		return;
	}

	/****** ��� 0-25 �׶��Ƿ����е��� *******/

	if ( n_ta >= 0 && n_ta <= 24 )
	{
		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;
					
					Emeny[i].hp -= LEVEL::harm[ Player[Emeny[id].role].level ];
				}
			}
		}
	}

	n_ta++;
	if ( n_ta >= 83)
	{
		n_ta = ta = 0;
		Emeny[id].be_catch = false;
		Emeny[id].lying = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

// �ú�������������������ץ�����𵹲塢ץס�����������ӵı�ˤ

void Thin_Old_Man::Be_catch( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -23, 25 };
	if ( Player[Emeny[id].role].skill != THROW_BACK )
		Dir = Player[Emeny[id].role].Rp->Dir;

	y = Player[Emeny[id].role].Rp->y;
	int _y = Player[Emeny[id].role].Rp->temp_y;
	int dy = 0;

	/*****/

	if ( Player[Emeny[id].role].skill == THUMP )
	{
		dy = 15;
		bc = 2;
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill != THROW_BACK && Player[Emeny[id].role].skill != THUMP )
	{
		bc = 1;
		if ( ++n_bc >= 6 )
		{
			Emeny[id].lying = false;
			n_bc = bc = 0;
		}
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill == THROW_BACK )
	{
			if ( n_bc >= 0 && n_bc <= 4 )
			{
				dx[0] = 24;
				dx[1] = -24;
				bc = 3;
			}
			if ( n_bc >= 5 && n_bc <= 8 )
			{
				dx[0] = 10;
				dx[1] = -10;
				dy = -40;
			}
			if ( n_bc >= 9 && n_bc <= 16 )
			{
				bc = 4;
				dx[0] = -40;
				dx[1] = 45;
				dy = 20;
			}
			if ( n_bc > 16 )	// ���������ɳ��� 20 ���� Jack �� Throw_role() ������ .skill
			{
				Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 4;
				n_bc = bc = 0;
				Dir++;
				mf[id] = &BASE_CLASS::Kick_away;
				Emeny[id].be_kick_counter = 5;
				Emeny[id].start_fly = true;

				putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
				putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );
				return;
			}
		n_bc++;

		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;

					Emeny[i].hp -= LEVEL::harm[Player[Emeny[id].role].level];
				}
			}
		}
	}

	x = Player[Emeny[id].role].Rp->x + dx[Dir];

	putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
	putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );

	if ( n_bc >= 10 && n_bc <= 14 )
	{
		putimage( x + 20, _y + 65, &PLAYER::light[1], SRCAND );
		putimage( x + 20, _y + 65, &PLAYER::light[0], SRCINVERT );
	}
}

//

void Thin_Old_Man::Die( int id, const int& player )
{
	UNREFERENCED_PARAMETER( player );
	if ( ++n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}

	if ( n_d > 50 )
	{
		if ( rand() % WEAP == 0 && !WEAPON::out )
			Weapon_Out( x, y, id, player );

		if ( rand() % FOD == 0 && !FOOD::out )
		{
			FOOD::out = true;
			FOOD::X = x - Map.x[Map.map_id];
			FOOD::y = y;
			FOOD::time = 0;
		}

		x = y = 0;			// �����˶�����Ϸ��
		n_d = d = 0;
		mf[id] = &BASE_CLASS::Walk;
		Emeny[id].Mp->SetHp( Emeny[id].hp );
		Emeny[id].appear[ Map.part[Map.map_id] ] = false;

		if ( Map.part[Map.map_id] < 7 )
		{
			if ( Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ] > 0 )
				Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ]--;

			int sum = 0;
			for ( int i = 0; i < EMENY::emeny_num; i++ )
				sum += Emeny[i].live[Map.map_id][ Map.part[Map.map_id] ];

			/****** �����������ǰ���� boss ********/

			if ( sum <= 0 )
			{
				if ( Map.map_id == 4 && (Map.part[4] == 5 || Map.part[4] == 2) )
					Dispatch_five_boss( Map.part[4] );
				else
					Map.die_out[Map.map_id][ Map.part[Map.map_id] ] = true;
			}
		}
	}
}

// ����һЩ bug ��Ϣ

void Thin_Old_Man::Out_Put(int id)
{
	printf( "x: %d, y: %d \n", x, y );

	if ( Emeny[id].lying )
		printf( "lying: true\n" );
	else
		printf( "lying: false\n" );

	if ( Emeny[id].start_fly )
		printf( "start_fly: true\n" );
	else
		printf( "start_fly: false\n" );

	if ( Emeny[id].appear[ Map.part[Map.map_id] ] )
		printf( "appear[i]: true\n" );
	else
		printf( "appear[i]: false\n" );

	if ( Emeny[id].be_catch )
		printf( "be_catch: true\n" );
	else
		printf( "be_catch: false\n" );

//	cout << n_w << endl << n_a << endl << n_ka << endl << n_ta << endl
//		<< n_s << endl << n_bc << endl << n_d << endl;
}


//// �����Ұ�� ///////////////////////////////////////

Big_Savage::Big_Savage()
{
	int _x[2] = { -110, 550 };
	x = _x[rand() % 2];
	Revise_dispatch(x);		// ���������ڵ����ڳ��ֵ�λ��

	X = Y = temp_x = 0;
	int min = Map.min_y[Map.map_id][Map.part[Map.map_id]];
	int max = Map.max_y[Map.map_id][Map.part[Map.map_id]];
	y = rand() % (max - min) + min;
	temp_y = y;

	Drtx[0] = -4;
	Drtx[1] = 4;
	step = 0;
	_a = 0;
	dx1 = rand() % 3 + 3;
	dy1 = rand() % 3 + 3;
	dx2 = rand() % 3 + 6;
	Dir = LEFT;
	attack_kind = 1;
	role = 0;
	leave = false;
	start = true;

	w = a = ka = ta = s = bc = d = 0;
	n_w = n_a = n_ka = n_ta = n_s = n_bc = n_d = 0;

	w_t1 = w_t2 = timeGetTime();

	int _level[6] = { 4,4,3,3,3,1 };
	int _dt[6] = { 125,115,95,90,85,80 };
	for ( int i = 0; i < 6; i++ )
	{
		w_dt[i] = _dt[i];
		attack_level[i] = _level[i];
	}
}

//

void Big_Savage::Load()
{
	IMAGE img;
	loadimage( &img, "./res/Images/�����Ұ��.jpg", 800, 2000 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )		// 0 ��/����ͼ��1 ��/����ͼ
	{
		for ( int x = 0; x < 14; x++ )
		{
			if ( x < 5 )
			{
				getimage( &walk[1][x][i], x * 80, i * 80, 80, 80 );		// 0
				getimage( &walk[0][x][i], x * 80, 160 + i * 80, 80, 80 );

				getimage( &kick_away[1][x][i], x * 80, 640 + i * 80, 80, 80 );	// 2
				getimage( &kick_away[0][x][i], x * 80, 800 + i * 80, 80, 80 );

				getimage( &throw_away[1][x][i], x * 80, 960  + i * 80, 80, 80 );	// 3
				getimage( &throw_away[0][x][i], x * 80, 1120 + i * 80, 80, 80 );
			}

			if ( x < 4 )
			{
				getimage( &attack[1][x][i], x * 80, 320 + i * 80, 80, 80 );		// 1
				getimage( &attack[0][x][i], x * 80, 480 + i * 80, 80, 80 );
			}

			if ( x < 5 )
			{
				getimage( &be_catch[1][x][i], x * 80, 1280 + i * 80, 80, 80 );	// 4
				getimage( &be_catch[0][x][i], x * 80, 1440 + i * 80, 80, 80 );
			}
		}
	}
	SetWorkingImage();
}

//

void Big_Savage::SetHp( int& hp )
{
	int a = Map.map_id;
	int b = Map.part[Map.map_id];

	hp = Hp[a][b];
}

//

void Big_Savage::Stand( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &walk[Dir][4][1], SRCAND );
	putimage( x, y, &walk[Dir][4][0], SRCINVERT );

	if ( ++n_s > 43 )
	{
		w = a = ka = ta = s = bc = 0;
		n_w = n_a = n_ka = n_ta = n_s = n_bc = 0;
		Emeny[id].stand = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

//

void Big_Savage::Walk( int id, const int& player )
{
	// ����ҵ��ڵ����÷����������ڸ�
	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y > Player[i].Rp->y )
			OTHER::emeny = true;
		else
			OTHER::emeny = false;
	}

	// ����
	if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
		 || Check_lift(PE_emeny, id) == _IN )
	{
		Emeny[id].hp = 0;
		mf[id] = &BASE_CLASS::Die;
		return;
	}

	Emeny[id].stand = false;
	Emeny[id].lying = false;
	if ( !Emeny[id].appear[ Map.part[Map.map_id] ] )
		return;

	/**** ���δ���˶����� *************/

	w_t2 = timeGetTime();
	if ( w_t2 - w_t1 < w_dt[LEVEL::walk_dt] )
	{
		putimage( x, y, &walk[Dir][w][1], SRCAND );
		putimage( x, y, &walk[Dir][w][0], SRCINVERT );
		return;
	}
	else
		w_t1 = w_t2;

	/**** �������׷���Ķ��� ************/

	if ( start )
	{
		int num = rand() % player;
		if ( Player[num].life >= 0 )
			Emeny[id].role = num;
		start = false;
	}
	if ( rand() % 20 == 0 )
		start = true;

	int dx = 0, dy = 0;

	/***** ������� *************/

	if ( !leave )
	{
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x );

		/**** б���˶� ***********/

		if ( abs(y - Player[Emeny[id].role].Rp->y) > 5 )
		{
			if ( cos(_a) < 0 )	dx1 = -abs(dx1);
			if ( cos(_a) > 0 )	dx1 =  abs(dx1);
			if ( sin(_a) < 0 )	dy1 =  abs(dy1);
			if ( sin(_a) > 0 )	dy1 = -abs(dy1);

			dx = dx1;
			dy = dy1;
		}

		/**** ˮƽ�˶� ****/

		if ( abs(y - Player[Emeny[id].role].Rp->y) <= 5 )
		{
			if ( cos(_a) < 0 )	dx2 = -abs(dx2);
			if ( cos(_a) > 0 )	dx2 =  abs(dx2);

			dx = dx2;
			dy = 0;
		}
	}

	/**** ����ʼ��������� ****/

	if ( x < Player[Emeny[id].role].Rp->x )	Dir = RIGHT;
	if ( x > Player[Emeny[id].role].Rp->x )	Dir = LEFT;

	/**** �ﵽ���������������\Զ�� *****/

	if ( abs( x + dx - Player[Emeny[id].role].Rp->x) < 90 && 
		 abs( y + dy - Player[Emeny[id].role].Rp->y) < 10 )
	{
		if ( attack_kind == 1 && abs( x + dx - Player[Emeny[id].role].Rp->x) > 80 )
		{
			n_w = n_a = a = 0;
			Emeny[id].attack = false;

			if ( rand() % attack_level[LEVEL::a_level] == 0 )
			{
				a = 2;
				attack_kind = 1;
				Emeny[id].attack = true;
				mf[id] = &BASE_CLASS::Attack;

				putimage( x, y, &walk[Dir][w][1], SRCAND );
				putimage( x, y, &walk[Dir][w][0], SRCINVERT );
				return;
			}
		}

		if ( abs( x + dx - Player[Emeny[id].role].Rp->x) < 40 &&
			 abs( x + dx - Player[Emeny[id].role].Rp->x) > 10 )
		{
			leave = true;
			n_w = n_a = a = 0;
			Emeny[id].attack = false;

			if ( rand() % attack_level[LEVEL::a_level] == 0 )
			{
				a = 0;
				attack_kind = 0;
				Emeny[id].attack = true;
				mf[id] = &BASE_CLASS::Attack;

				putimage( x, y, &walk[Dir][w][1], SRCAND );
				putimage( x, y, &walk[Dir][w][0], SRCINVERT );
				return;
			}
		}
	}

	if ( leave && step == 0 )
		_a = atan2( (double)(y - Player[Emeny[id].role].Rp->y), Player[Emeny[id].role].Rp->x - x ) + (double)(rand() % 314) / 100 + 1.57;

	if ( leave )
	{
		if ( ++step > 30 )
		{
			step = 0;
			leave = false;
		}

		dx = (int)( 6 * cos(_a) );
		dy = (int)(-6 * sin(_a) );
	}

	/**** һ��ͼƬ������ ****/

	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	if ( rand() % 68 == 0 )
	{
		n_s = 0;
		n_w = w = 0;
		mf[id] = &BASE_CLASS::Stand;
		return;
	}

	if ( ++n_w % 2 == 0 )
		w = ( ++w > 3 ) ? 0 : w;

	/**** ���Խ�� ****/

	if ( !Map.Check( y + dy ) || x + dx < -120 || x + dx > 560 || !Revise_lift(id, dx) )
	{
		step = 0;
		leave = false;
	}
	else
	{
		Revise_xy( id, x, y, dx, dy );		// ���������ƶ�����
		x += dx;
		y += dy;
		temp_y = y;
	}

	if ( x + dx > -120 && x + dx < -110 || x + dx > 550 && x + dx < 560 )
	{
		int _x[2] = { -110, 550 };
		int id = rand() % 2;

		x = _x[id]; 
	}
}

//

void Big_Savage::Attack( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	switch ( attack_kind )
	{
		case 0:
			putimage( x, y, &attack[Dir][a][1], SRCAND );
			putimage( x, y, &attack[Dir][a][0], SRCINVERT );

			if ( id < 4 )
			{
				putimage( x + 40, y - 20, &EMENY::papaw[1], SRCAND );
				putimage( x + 40, y - 20, &EMENY::papaw[0], SRCINVERT );

				setbkmode( TRANSPARENT );
				setcolor( LIGHTGREEN );
				setfont( 20, 0, "����" );

				char* c[4];
				c[0] = "����";
				c[1] = "ȥ��";
				c[2] = "xxxxx";
				c[3] = "����";
				outtextxy( x + 48, y - 15, c[id] );
			}

			if ( ++n_a == 5 )	a = 1;
			if ( n_a > 11 )
			{
				Emeny[id].attack = false;
				n_a = a = 0;
				mf[id] = &BASE_CLASS::Walk;
				return;
			}
			break;

		case 1:			// ���ֱײ
			if ( n_a % 9 == 0 )
				SetSound( NULL, "Ұ�˽Ų�", "f3" );
			putimage( x, y, &attack[Dir][a][1], SRCAND );
			putimage( x, y, &attack[Dir][a][0], SRCINVERT );

			if ( x + Drtx[Dir] * 2 > MIN_X && x + Drtx[Dir] * 2 < MAX_X &&
				 Check_lift_xy(x + Drtx[Dir] * 2) == _UNIN )
				x += Drtx[Dir] * 2;
			if ( Check_lift_xy(x + Drtx[Dir] * 2) == _IN )
				n_a = 40;

			if ( ++n_a % 4 == 0 )	a = 2;
			if ( n_a % 8 == 0 )		a = 3;
			if ( n_a >= 40 )
			{
				Emeny[id].attack = false;
				n_a = a = 0;
				mf[id] = &BASE_CLASS::Walk;
			}
			break;

		default:
			printf("Ұ�˳���\n");
			break;
	}

	int cx = 0;

	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( Player[i].Rp->Check_n_x( 0XFBB ) &&
			 abs( x - Player[i].Rp->x ) < 50 &&
			 abs( y - Player[i].Rp->y ) < 18 &&
			 (Dir == Player[i].Rp->Dir ||
			 Player[i].Rp->Getn_a() == 0) && Player[i].die == false )
		{
			cx = Player[i].Rp->x;
			if ( Check_position(Dir, x, cx) )
			{
				if ( attack_kind == 1 )
					Player[i].be_kick_counter = 3;

				Player[i].Set_bol( 0x00 );
				f[i] = &Role::Kick_down;

				if ( x > Player[i].Rp->x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;

				for ( int jj = 0; jj < EMENY::emeny_num; jj++ )
					if ( Emeny[jj].start_fly && !Emeny[jj].lying )
						Emeny[jj].be_kick_counter = 0;
			}
		}
	}
}

//

void Big_Savage::Kick_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int _r = ( Dir + 1 ) % 2;

	if ( n_ka == 3 && Emeny[id].be_kick_counter < 4 )
		ka = 1;

	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	/****** ������ײЧ�� *******/

	if ( n_ka <= 7 && rand() % 6 == 0 && Emeny[id].be_kick_counter < 4 )
	{
		setbkmode( TRANSPARENT );
		setcolor( LIGHTGREEN );
		outtextxy( x + 20, y - 20, "��!" );
		int dx = rand() % 20 + 10;
		int dy = rand() % 10 + 20;
		putimage( x + dx, y + dy, &PLAYER::light[1], SRCAND );
		putimage( x + dx, y + dy, &PLAYER::light[0], SRCINVERT );
	}

	/***** ������ҹ�����Ч ****/

	if ( n_ka == 0 && Emeny[id].be_kick_counter == 3 )
	{
		Player[Emeny[id].role].Set_bol( 0x00 );
		f[Emeny[id].role] = &Role::Dragon_fist;

		/**** ˦�� ****/

		if ( Emeny[id].role == 0 && GetAsyncKeyState('S') & 0x8000 ||
			 Emeny[id].role == 1 && GetAsyncKeyState(VK_DOWN) & 0x8000 )	
		{
			n_ta = ta = 0;
			n_ka = ka = 0;
			f[Emeny[id].role] = &Role::Throw_role;

			if ( Player[Emeny[id].role].role_kind == 2 )
			{
				Emeny[id].lying = true;
				mf[id] = &BASE_CLASS::Be_catch;
				Player[Emeny[id].role].skill = THROW_BACK;
			}
			else
			{
				Emeny[id].be_kick_counter = 0;
				mf[id] = &BASE_CLASS::Throw_away;
			}
			return;
		}
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 2;
	}

	/****** ���ι������� ******/

	if ( n_ka == 7 && Emeny[id].be_kick_counter < 4 )
	{
		n_ka = ka = 0;
		Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level];

		/****/

		if ( Emeny[id].hp <= 0 )
		{
			Emeny[id].start_fly = true;
			Emeny[id].lying = true;
			Emeny[id].be_kick_counter = 5;
			return;
		}

		/*****/

		if ( GetAsyncKeyState('J') & 0x8000 && Emeny[id].role == 0 ||
			 GetAsyncKeyState(VK_NUMPAD1) & 0x8000 && Emeny[id].role == 1 )
			 Emeny[id].be_kick_counter++;

		if ( Emeny[id].be_kick_counter < 4 )
		{
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/***** ���ø߷�·�� ********/

	if ( Emeny[id].be_kick_counter == 4 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 42, -42 };
			X = x + Dx[Dir];
			Y = y - 160;
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}

		if ( n_ka == 42 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 87 )
		{
			if ( n_ka < 30 )
			{
				int dx[2] = { -30, 30 };
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 40, &PLAYER::blood[Dir][n_ka/5][0], SRCINVERT );
			}

			if ( n_ka <= 7  )					ka = 0;
			if ( n_ka >= 8 && n_ka <= 30 )		ka = 2;
			if ( n_ka >= 31 )					ka = 3;
			if ( n_ka >= 76 )					ka = 4;

			if ( n_ka >= 0 && n_ka <= 41 )
			{
				if ( x - Drtx[Dir] / 2 >= MIN_X && x - Drtx[Dir] / 2 <= MAX_X )
					x -= Drtx[Dir] / 2;
				else
					temp_x++;
				temp_y = (x - temp_x * Drtx[Dir] / 2 - X) * (x - temp_x * Drtx[Dir] / 2 - X) / 11 + Y;
			}

			if ( n_ka >= 42 && n_ka <= 51 )
			{
				int dx = Drtx[Dir] / 4;
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
		}

		if ( n_ka == 44 || n_ka == 51 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 51;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 51 && Emeny[id].hp <= 0 )
		{
			temp_x = 0;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Die;
			return;
		}

		if ( n_ka > 87 )
		{
			temp_x = 0;
			Emeny[id].lying = false;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			n_ka = ka = 0;
			Emeny[id].be_kick_counter = 0;
			mf[id] = &BASE_CLASS::Walk;
			return;
		}
	}

	/****** ���õͷ�·�� ******/

	if ( Emeny[id].be_kick_counter == 5 )
	{
		if ( Emeny[id].start_fly )
		{
			int Dx[2] = { 10, -10 };
			X = x + Dx[Dir];
			Y = y - 40;		//////////////////////////Ҫ��Ҫ temp_y = y �ء�������
			temp_x = n_ka = 0;
			Emeny[id].start_fly = false;
			Emeny[id].lying = true;
		}
		if ( n_ka == 20 )
		{
			int dx[2] = { 5, -5 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}
		if ( n_ka <= 64 )
		{
			if ( n_ka <= 15  )					ka = 2;
			if ( n_ka >= 16 && n_ka <= 40 )		ka = 3;
			if ( n_ka >= 41 )					ka = 4;

			int dx = Drtx[Dir] / 4;
			if ( n_ka <= 19 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/2.5 ) + Y;
			}

			if ( n_ka > 19 && n_ka <= 29 )
			{
				if ( x - dx >= MIN_X && x - dx <= MAX_X )
					x -= dx;
				else
					temp_x++;
				temp_y = int( (x-temp_x*dx-X)*(x-temp_x*dx-X)/1.2 ) + Y;
			}
			if ( n_ka >= 0 && n_ka <= 11 )
			{
				int dx[2] = { -15, 15 };
				putimage( x + dx[Dir], temp_y - 55, &PLAYER::blood[Dir][n_ka/2][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 55, &PLAYER::blood[Dir][n_ka/2][0], SRCINVERT );
			}
		}

		if ( n_ka == 20 || n_ka == 29 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ka = 29;
				Emeny[id].hp = 0;
			}
		}

		if ( n_ka == 29 && Emeny[id].hp <= 0 )
		{
			mf[id] = &BASE_CLASS::Die;
			Emeny[id].be_catch = false;
			Emeny[id].lying = true;
			Emeny[id].start_fly = true;
			n_ka = ka = 0;
			n_bc = bc = 0;
			Emeny[id].be_kick_counter = 0;
			temp_x = 0;
			return;
		}

		if ( n_ka > 64 )
		{
			n_bc = bc = 0;
			n_ka = ka = 0;
			mf[id] = &BASE_CLASS::Walk;
			Emeny[id].lying = false;
			temp_x = 0;
			Emeny[id].start_fly = true;
			Emeny[id].be_catch = false;
			Emeny[id].be_kick_counter = 0;
			return;
		}
	}
	n_ka++;
}

//

void Big_Savage::Throw_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -60, 60 };

	int _x = Player[Emeny[id].role].Rp->x;
	int _y = Player[Emeny[id].role].Rp->y;

	if ( n_ta == 0 )
		Emeny[id].lying = true;

	if ( n_ta <= 3 )
	{
		ta = 0;
		temp_y = _y;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 2;
		else
			x = _x - dx[Dir] * 2 / 3;
	}
	if ( n_ta >= 4 && n_ta <= 6 )
	{
		temp_y = _y - 20;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 3;
		else
			x = _x - dx[Dir] / 3;
	}
	if ( n_ta >= 7 && n_ta <= 9 )
	{
		temp_y = _y - 30;
		if ( Player[Emeny[id].role].throw_dir == FORWARD )
			x = _x - dx[Dir] / 5;
		else
			x = _x + dx[Dir] / 6;
	}

	/************/

	if ( n_ta == 10 )
	{
		ta = 1;
		X = x + dx[Dir] / 3;
		Y = temp_y - 22;
		temp_x = 0;
	}

	if ( n_ta == 29 )
	{
		X = x + dx[Dir] / 12;
		Y = temp_y - 20;
		temp_x = 0;
	}

	/*******/

		int x1 = dx[Dir] / 10;
		if ( n_ta <= 38 && n_ta >= 10 )
		{
			if ( n_ta >= 29 )		x1 /= 6;
			if ( x + x1 >= MIN_X && x + x1 <= MAX_X )
				x += x1;
			else
				temp_x++;

			double k = 148;		// ע�⣬������뱣֤ k != 0
			if ( n_ta <= 19 )				ta = 1;
			if ( n_ta >= 20 && n_ta <= 28 )	ta = 2;
			if ( n_ta >= 29 && n_ta <= 38 )
			{
				if ( n_ta == 29 )
					Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 3;
				ta = 3;
				k = 1.2;
			}
			temp_y = int( (x+temp_x*x1-X)*(x+temp_x*x1-X)/k ) + Y;
		}

		if ( n_ta == 29 || n_ta == 39 )
		{
			// ����
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );
			if ( Map.map_id == 2 && Check_hole(PE_emeny, id) == _IN
				 || Check_lift(PE_emeny, id) == _IN )
			{
				n_ta = 38;
				Emeny[id].hp = 0;	// ��������ļ��ʹ���˽�����
			}
		}

		if ( n_ta >= 60 && n_ta <= 82 )
			ta = 4;

	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );

	if ( n_ta == 38 && Emeny[id].hp <= 0 )
	{
		mf[id] = &BASE_CLASS::Die;
		Emeny[id].be_catch = false;
		Emeny[id].lying = true;
		n_ta = ta = 0;
		return;
	}

	/****** ��� 0-25 �׶��Ƿ����е��� *******/

	if ( n_ta >= 0 && n_ta <= 24 )
	{
		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id && abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;

					Emeny[i].hp -= LEVEL::harm[ Player[Emeny[id].role].level ];
				}
			}
		}
	}

	n_ta++;
	if ( n_ta >= 83)
	{
		n_ta = ta = 0;
		Emeny[id].be_catch = false;
		Emeny[id].lying = false;
		mf[id] = &BASE_CLASS::Walk;
	}
}

// ʹ���� _y ���� y �����˶�����ֹ bug �����Խ��

void Big_Savage::Be_catch( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -23, 25 };
	if ( Player[Emeny[id].role].skill != THROW_BACK )
		Dir = Player[Emeny[id].role].Rp->Dir;
	y = Player[Emeny[id].role].Rp->y;
	int _y = Player[Emeny[id].role].Rp->temp_y;
	int dy = 0;

	/*****/

	if ( Player[Emeny[id].role].skill == THUMP )
	{
		dy = 15;
		bc = 2;
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill != THROW_BACK && Player[Emeny[id].role].skill != THUMP )
	{
		bc = 1;
		if ( ++n_bc >= 6 )
		{
			Emeny[id].lying = false;
			n_bc = bc = 0;
		}
	}

	/*****/

	if ( Emeny[id].lying && Player[Emeny[id].role].skill == THROW_BACK )
	{
			if ( n_bc >= 0 && n_bc <= 4 )
			{
				dx[0] = 24;
				dx[1] = -24;
				bc = 3;
			}
			if ( n_bc >= 5 && n_bc <= 8 )
			{
				dx[0] = 10;
				dx[1] = -10;
				dy = -40;
			}
			if ( n_bc >= 9 && n_bc <= 16 )
			{
				bc = 4;
				dx[0] = -40;
				dx[1] = 45;
				dy = 20;
			}
			if ( n_bc > 16 )
			{
				Emeny[id].hp -= LEVEL::harm[Player[Emeny[id].role].level] * 4;
				n_bc = bc = 0;
				Dir++;
				mf[id] = &BASE_CLASS::Kick_away;
				Emeny[id].be_kick_counter = 5;
				Emeny[id].start_fly = true;

				putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
				putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );
				return;
			}
		n_bc++;

		for ( int i = 0; i < EMENY::emeny_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Emeny[i].Mp->Gety() - Player[Emeny[id].role].Rp->y) < 18 &&
					 abs(x - Emeny[i].Mp->Getx()) < 50 && Emeny[i].lying == false )
				{
					Emeny[i].start_fly = true;
					Emeny[i].role = Emeny[id].role;
					Emeny[i].be_kick_counter = 5;
					mf[i] = &BASE_CLASS::Kick_away;

					if ( Emeny[i].be_catch )
						Player[(Emeny[i].role+1) % 2].catch_emeny = false;
					Emeny[i].be_catch = false;

					Emeny[i].hp -= LEVEL::harm[Player[Emeny[id].role].level];
				}
			}
		}
	}

	x = Player[Emeny[id].role].Rp->x + dx[Dir];
	putimage( x, _y + dy, &be_catch[Dir][bc][1], SRCAND );
	putimage( x, _y + dy, &be_catch[Dir][bc][0], SRCINVERT );

	if ( n_bc >= 10 && n_bc <= 14 )
	{
		putimage( x + 20, _y + 65, &PLAYER::light[1], SRCAND );
		putimage( x + 20, _y + 65, &PLAYER::light[0], SRCINVERT );
	}
}

//

void Big_Savage::Die( int id, const int& player )
{
	UNREFERENCED_PARAMETER( player );
	if ( ++n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}

	if ( n_d > 50 )
	{
		if ( rand() % WEAP == 0 && !WEAPON::out )
			Weapon_Out( x, y, id, player );

		if ( rand() % FOD == 0 && !FOOD::out )
		{
			FOOD::out = true;
			FOOD::X = x - Map.x[Map.map_id];
			FOOD::y = y;
			FOOD::time = 0;
		}

		x = y = 0;			// �����˶�����Ϸ��
		n_d = d = 0;
		mf[id] = &BASE_CLASS::Walk;
		Emeny[id].Mp->SetHp( Emeny[id].hp );
		Emeny[id].appear[ Map.part[Map.map_id] ] = false;

		/*********/

		if ( Map.part[Map.map_id] < 7 )
		{
			if ( Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ] > 0 )
				Emeny[id].live[Map.map_id][ Map.part[Map.map_id] ]--;

			int sum = 0;
			for ( int i = 0; i < EMENY::emeny_num; i++ )
				sum += Emeny[i].live[Map.map_id][ Map.part[Map.map_id] ];

			/****** �����������ǰ���� boss ********/

			if ( sum <= 0 )
			{
				if ( Map.map_id == 4 && (Map.part[4] == 5 || Map.part[4] == 2) )
					Dispatch_five_boss( Map.part[4] );
				else
					Map.die_out[Map.map_id][ Map.part[Map.map_id] ] = true;
			}
		}
	}
}

////
////	BOSS ��ʵ��
////
////


/************* BASE_BOSS **************/

BASE_BOSS::BASE_BOSS( int* p, RECT* r )
{
	x = p[0] - rand() % 150;
	y = p[1];
	X = Y = 0;
	Drtx[0] = -4;
	Drtx[1] = 4;
	temp_x = temp_y = 0;

	a_kind = 0;

	w = a = ka = ta = tb = d = 0;
	n_w = n_a = n_ka = n_ta = n_tb = n_d = 0;

	start = false;

	rec = r;
	Dir = LEFT;
	move_dir = LEFT;
	w_t1 = w_t2 = timeGetTime();
	w_dt = p[2];	// ��ͬ�����ݸ�ֵ����
}

// ��� boss �˶�����������������ڸ�������� Walk() �е����������
// ����������д

void BASE_BOSS::Walk( int id, const int& player )
{
	Boss[id].lying = false;
	w_t2 = timeGetTime();
	int dx[4] = { -5, 5, 0, 0 };
	int dy[4] = { 0, 0, -3, 3 };

	if ( w_t2 - w_t1 < w_dt )
		return;
	else
		w_t1 = w_t2;

	/***** ����˫��ģʽ ******/

	if ( rand() % 40 == 0 )		start = true;
	if ( start )
	{
		start = false;
		int num = rand() % player;

		if ( Player[num].life >= 0 )
			Boss[id].role = (byte)num;
	}
	Dir = SetDir( x, Player[Boss[id].role].Rp->x );	// boss ��·ʼ�ճ������

	/******* x/y ����Խ���ػ��� ********/

	if ( x + dx[move_dir] >= rec->left && x + dx[move_dir] <= rec->right &&
		 y + dy[move_dir] >= rec->top && y + dy[move_dir] <= rec->bottom )
	{
		x += dx[move_dir];
		y += dy[move_dir];
		temp_y = y;
	}
	else
	{
		int a_k = rand() % 4;
		if ( a_k < 2 )
		{
			a_kind = a_k;
			n_w = w = 0;
			n_a = a = 0;	// ��ֹ�������ж��в���,�����޷����㿪ʼ��Attack() �޷���ʼ����ʼֵ!!
			bf[id] = &BASE_BOSS::Attack;
		}
		else
			move_dir = Change_move_dir( x, y );
	}

	if ( n_w++ % 4 == 0 )
		w = ( ++w > 3 ) ? 0 : w;
}

//

void BASE_BOSS::Kick_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	if ( n_ka == 3 && Boss[id].be_kick_counter < 4 )
		ka = 1;

	/****** ������ײЧ�� *******/

	if ( n_ka <= 7 && rand() % 6 == 0 && Boss[id].be_kick_counter < 4 )
	{
		int dx = rand() % 20 + 10;
		int dy = rand() % 10 + 20;
		putimage( x + dx, y + dy, &PLAYER::light[1], SRCAND );
		putimage( x + dx, y + dy, &PLAYER::light[0], SRCINVERT );
	}

	/***** ������ҹ�����Ч ****/

	if ( n_ka == 0 && Boss[id].be_kick_counter == 3 )
	{
		Player[Boss[id].role].Set_bol( 0x00 );
		f[Boss[id].role] = &Role::Dragon_fist;

		/**** ˦�� ****/

		if ( Boss[id].role == 0 && GetAsyncKeyState('S') & 0x8000 ||
			 Boss[id].role == 1 && GetAsyncKeyState(VK_DOWN) & 0x8000 )	
		{
			n_tb = tb = 0;		// ��ֹ���ܵ� bug ����
			n_ta = ta = 0;
			n_ka = ka = 0;
			f[Boss[id].role] = &Role::Throw_role;

			if ( Player[Boss[id].role].role_kind == 2 )
			{
				Boss[id].lying = true;
				bf[id] = &BASE_BOSS::Throw_back;
			}
			else
			{
				Boss[id].be_kick_counter = 0;
				bf[id] = &BASE_BOSS::Throw_away;
			}
			return;
		}
		Boss[id].hp -= LEVEL::harm[Player[Boss[id].role].level] * 2;
	}

	/****** ���ι������� ******/

	if ( n_ka == 7 && Boss[id].be_kick_counter < 4 )
	{
		n_ka = ka = 0;
		Boss[id].hp -= LEVEL::harm[Player[Boss[id].role].level];

		/****/

		if ( Boss[id].hp <= 0 )
		{
			Boss[id].start_fly = true;
			Boss[id].lying = true;
			Boss[id].be_kick_counter = 5;
			return;
		}

		/*****/

		if ( GetAsyncKeyState('J') & 0x8000 && Boss[id].role == 0 ||
			 GetAsyncKeyState(VK_NUMPAD1) & 0x8000 && Boss[id].role == 1 )
			 Boss[id].be_kick_counter++;

		if ( Boss[id].be_kick_counter < 4 )
		{
			bf[id] = &BASE_BOSS::Walk;
			return;
		}
	}

	/***** ���ø߷�·�� ********/

	if ( Boss[id].be_kick_counter == 4 )
	{
		if ( Boss[id].start_fly )
		{
			int Dx[2] = { 42, -42 };
			X = x + Dx[Dir];
			Y = y - 160;
			temp_x = n_ka = 0;
			Boss[id].start_fly = false;
			Boss[id].lying = true;
		}

		if ( n_ka == 42 )
		{
			int dx[2] = { 20, -20 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 87 )
		{
			if ( n_ka < 30 )
			{
				int dx[2] = { -10, 30 };
				putimage( x + dx[Dir], temp_y - 30, &PLAYER::blood[Dir][n_ka/5][1], SRCAND );
				putimage( x + dx[Dir], temp_y - 30, &PLAYER::blood[Dir][n_ka/5][0], SRCINVERT );
			}

			if ( n_ka <= 7  )					ka = 0;
			if ( n_ka >= 8 && n_ka <= 30 )		ka = 2;
			if ( n_ka >= 31 )					ka = 3;
			if ( n_ka >= 76 )					ka = 4;

			if ( n_ka >= 0 && n_ka <= 41 )
			{
				if ( x - Drtx[Dir] / 2 >= rec->left && x - Drtx[Dir] / 2 <= rec->right )
					x -= Drtx[Dir] / 2;
				else
					temp_x++;
				temp_y = (x - temp_x * Drtx[Dir] / 2 - X) * (x - temp_x * Drtx[Dir] / 2 - X) / 11 + Y;
			}

			if ( n_ka >= 42 && n_ka <= 51 )
			{
				if ( x - Drtx[Dir] >= rec->left && x - Drtx[Dir] <= rec->right )
					x -= Drtx[Dir];
				else
					temp_x++;
				temp_y = (x - temp_x * Drtx[Dir] - X) * (x - temp_x * Drtx[Dir] - X) / 20 + Y;
			}
		}

		if ( n_ka == 44 || n_ka == 51 )
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );

		if ( n_ka == 51 && Boss[id].hp <= 0 )
		{
			temp_x = 0;
			Boss[id].lying = true;
			Boss[id].start_fly = true;
			n_ka = ka = 0;
			Boss[id].be_kick_counter = 0;
			bf[id] = &BASE_BOSS::Die;
			return;
		}

		if ( n_ka > 87 )
		{
			temp_x = 0;
			Boss[id].start_fly = true;
			n_ka = ka = 0;
			Boss[id].be_kick_counter = 0;

			/***** ������/����صڶ��� boss Ѫ����һ���ʱ��һֱ���ڹ������� *****/

			if ( (Map.map_id == 3 || Map.map_id == 4 && Map.part[4] == 5) && Boss[id].hp*2 < Boss[id].max_hp )
			{
				n_a = 11;
				a_kind = 2;
				bf[id] = &BASE_BOSS::Attack;
			}
			else
				bf[id] = &BASE_BOSS::Walk;
			return;
		}
	}

	/****** ���õͷ�·�� ******/

	if ( Boss[id].be_kick_counter == 5 )
	{
		if ( Boss[id].start_fly )
		{
			int Dx[2] = { 40, -40 };
			X = x + Dx[Dir];
			Y = y - 40;
			temp_x = n_ka = 0;
			Boss[id].start_fly = false;
			Boss[id].lying = true;
		}

		if ( n_ka == 20 )
		{
			int dx[2] = { 20, -20 };
			X = x + dx[Dir];
			Y = y - 20;
			temp_x = 0;
		}

		if ( n_ka <= 64 )
		{
			if ( n_ka <= 15  )					ka = 2;
			if ( n_ka >= 16 && n_ka <= 40 )		ka = 3;
			if ( n_ka >= 41 )					ka = 4;

			if ( n_ka <= 19 )
			{
				if ( x - Drtx[Dir] >= rec->left && x - Drtx[Dir] <= rec->right )
					x -= Drtx[Dir];
				else
					temp_x++;

				temp_y = (x - temp_x * Drtx[Dir] - X) * (x - temp_x * Drtx[Dir] - X) / 40 + Y;
			}
			
			if ( n_ka > 19 && n_ka <= 29 )
			{
				if ( x - Drtx[Dir] >= rec->left && x - Drtx[Dir] <= rec->right )
					x -= Drtx[Dir];
				else
					temp_x++;
				temp_y = (x - temp_x * Drtx[Dir] - X) * (x - temp_x * Drtx[Dir] - X) / 20 + Y;
			}
		}

		if ( n_ka == 20 || n_ka == 29 )
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );

		if ( n_ka == 29 && Boss[id].hp <= 0 )
		{
			bf[id] = &BASE_BOSS::Die;
			Boss[id].lying = true;
			Boss[id].start_fly = true;
			n_ka = ka = 0;
			Boss[id].be_kick_counter = 0;
			temp_x = 0;
			return;
		}

		if ( n_ka > 64 )
		{
			n_ka = ka = 0;
			temp_x = 0;
			Boss[id].start_fly = true;
			Boss[id].be_kick_counter = 0;
			
			/***** ������/����صڶ��� boss Ѫ����һ���ʱ��һֱ���ڹ������� *****/

			if ( (Map.map_id == 3 || Map.map_id == 4 && Map.part[4] == 5) && Boss[id].hp*2 < Boss[id].max_hp )
			{
				n_a = 11;
				a_kind = 2;
				bf[id] = &BASE_BOSS::Attack;
			}
			else
				bf[id] = &BASE_BOSS::Walk;
			return;
		}
	}
	n_ka++;
}

// temp_y ����ҵ� y ��Լ

void BASE_BOSS::Throw_away( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx[2] = { -60, 60 };

	int _x = Player[Boss[id].role].Rp->x;
	int _y = Player[Boss[id].role].Rp->y;

	if ( n_ta == 0 )
		Boss[id].lying = true;

	if ( n_ta <= 3 )
	{
		ta = 0;
		if ( Player[Boss[id].role].throw_dir == FORWARD )
		{// �����Ų���ʡ�ԣ����� else ��֪���ĸ� if ��ԣ���
			if ( _x - dx[Dir] / 2 >= rec->left && _x - dx[Dir] / 2 <= rec->right )
				x = _x - dx[Dir] / 2;
		}
		else if ( _x - dx[Dir] * 2 / 3 >= rec->left && _x - dx[Dir] * 2 / 3 <= rec->right )
			x = _x - dx[Dir] * 2 / 3;
	}
	if ( n_ta >= 4 && n_ta <= 6 )
	{
		temp_y = _y - 20;
		if ( Player[Boss[id].role].throw_dir == FORWARD )
		{
			if ( _x - dx[Dir] / 3 >= rec->left && _x - dx[Dir] / 3 <= rec->right )
				x = _x - dx[Dir] / 3;
		}
		else if ( _x - dx[Dir] / 3 >= rec->left && _x - dx[Dir] / 3 <= rec->right )
			x = _x - dx[Dir] / 3;
	}
	if ( n_ta >= 7 && n_ta <= 9 )
	{
		temp_y = _y - 40;
		if ( Player[Boss[id].role].throw_dir == FORWARD )
		{
			if ( _x - dx[Dir] / 5 >= rec->left && _x - dx[Dir] / 5 <= rec->right )
				x = _x - dx[Dir] / 5;
		}
		else if ( _x + dx[Dir] / 6 >= rec->left && _x + dx[Dir] / 6 <= rec->right )
			x = _x + dx[Dir] / 6;
	}

	/************/

	if ( n_ta == 10 )
	{
		ta = 1;
		X = x + dx[Dir] * 2 / 3;
		Y = temp_y - 19;
		temp_x = 0;
	}

	if ( n_ta == 32 )
	{
		X = x + dx[Dir] / 3;
		Y = temp_y - 26;
		temp_x = 0;
	}

	/*******/

		int x1 = dx[Dir] / 10;
		if ( n_ta <= 38 && n_ta >= 10 )
		{
			if ( x + x1 >= rec->left && x + x1 <= rec->right )
				x += x1;
			else
				temp_x++;

			int k = 0;		// ע�⣬������뱣֤ k != 0
			if ( n_ta <= 19 )
			{
				ta = 1;
				k = 34;
			}
			if ( n_ta >= 20 && n_ta <= 31 )
			{
				ta = 2;
				k = 160;
			}
			if ( n_ta >= 32 && n_ta <= 38 )
			{
				if ( n_ta == 32 )
					Boss[id].hp -= LEVEL::harm[Player[Boss[id].role].level] * 3;
				ta = 3;
				k = 31;
			}
			temp_y = ( x + temp_x * x1 - X ) * ( x + temp_x * x1 - X ) / k + Y;
		}

		if ( n_ta == 29 || n_ta == 39 )
			PlaySound( "./res/Sounds/���˵���.wav", NULL, SND_ASYNC );

		if ( n_ta >= 60 && n_ta <= 82 )
			ta = 4;

	if ( n_ta == 38 && Boss[id].hp <= 0 )
	{
		bf[id] = &BASE_BOSS::Die;
		Boss[id].lying = true;
		n_ta = ta = 0;
		return;
	}

	/*************/

	if ( n_ta >= 0 && n_ta <= 24 && BOSS::Boss_num == 2 )
	{
		for ( int i = 0; i < BOSS::Boss_num; i++ )
		{
			if ( i != id && abs(x - Boss[i].Bp->x) < 50 && Boss[i].lying == false )
			{
				if ( abs(Boss[i].Bp->y - y) < 18 )
				{
					Boss[i].start_fly = true;
					Boss[i].role = Boss[id].role;
					Boss[i].be_kick_counter = 5;
					bf[i] = &BASE_BOSS::Kick_away;
					Boss[i].hp -= LEVEL::harm[ Player[Boss[id].role].level ];
				}
			}
		}
	}

	n_ta++;
	if ( n_ta >= 83)
	{
		n_ta = ta = 0;
		Boss[id].lying = false;

		/***** ������/����صڶ��� boss Ѫ����һ���ʱ��һֱ���ڹ������� *****/

		if ( (Map.map_id == 3 || Map.map_id == 4 && Map.part[4] == 5) && Boss[id].hp*2 < Boss[id].max_hp )
		{
			n_a = 11;
			a_kind = 2;
			bf[id] = &BASE_BOSS::Attack;
		}
		else
			bf[id] = &BASE_BOSS::Walk;
	}
}

// ���ӱ�ˤʹ�� throw_away IMAGE �� 0��1 �±�ͼƬ

void BASE_BOSS::Throw_back( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int _y = Player[Emeny[id].role].Rp->temp_y;
	int dy = 0;
	int dx[2] = { -23, 25 };

	if ( n_tb >= 0 && n_tb <= 4 )
	{
		dx[0] = 24;
		dx[1] = -24;
		tb = 0;
	}
	if ( n_tb >= 5 && n_tb <= 8 )
	{
		dx[0] = 10;
		dx[1] = -10;
		dy = -40;
	}
	if ( n_tb >= 9 && n_tb <= 16 )
	{
		tb = 1;
		dx[0] = -45;
		dx[1] = 40;
		dy = 10;
	}
	if ( n_tb > 16 )
	{
		Boss[id].hp -= LEVEL::harm[Player[Boss[id].role].level] * 4;
		n_tb = tb = 0;
		ka = 2;			// ���� Kick_away() �������ͼƬ����û��ຯ�������µ�һ������
		Dir++;
		bf[id] = &BASE_BOSS::Kick_away;
		Boss[id].be_kick_counter = 5;
		Boss[id].start_fly = true;

		return;
	}
	n_tb++;

	if ( BOSS::Boss_num == 2 )
		for ( int i = 0; i < BOSS::Boss_num; i++ )
		{
			if ( i != id )
			{
				if ( abs(Boss[i].Bp->y - y) < 20 &&
					 abs(Boss[i].Bp->x - x) < 50 && Boss[i].lying == false )
				{
					Boss[i].start_fly = true;
					Boss[i].role = Boss[id].role;
					Boss[i].be_kick_counter = 5;
					bf[i] = &BASE_BOSS::Kick_away;

					Boss[i].hp -= LEVEL::harm[Player[Boss[id].role].level];
				}
			}
		}

	/****** ��¼�����Ա�����ʹ�� ******/

	temp_y = _y + dy;

	int cx = Player[Boss[id].role].Rp->x + dx[Dir];	// С�� boss Խ��
	if ( cx >= rec->left && cx <= rec->right )
		x = cx;

	if ( n_tb >= 10 && n_tb <= 14 )
	{
		putimage( x + 20, _y + 65, &PLAYER::light[1], SRCAND );
		putimage( x + 20, _y + 65, &PLAYER::light[0], SRCINVERT );
	}
}

//

void BASE_BOSS::Die( int id, const int& player )
{
	UNREFERENCED_PARAMETER( player );

	if ( n_d++ > 50 )
	{
		x = y = 0;
		n_d = d = 0;
		bf[id] = &BASE_BOSS::Walk;
		Boss[id].appear = false;
		Boss[id].die = true;
	}
}




/************ ONE ****************/

// ͬʱ��ʼ�����๹�캯��

B_ONE::B_ONE( int* p, RECT* rec ):
BASE_BOSS( p, rec )					// ������һ����Ĭ�Ϲ��캯��������ϵͳ�����Զ�����Ĭ�Ϲ��캯��
{								// ��ʱ��ʼ��һ��Ҫд�������ĸ����캯������Ϊû��Ĭ�ϵĿɵ��ã�
	IMAGE img;
	loadimage( &img, "./res/Images/boss0.jpg", 800, 2400 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )
		for ( int x = 0; x < 6; x++ )
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 100, i * 100, 100, 100 );			// 1
				getimage( &walk[0][x][i], x * 100, 200 + i * 100, 100, 100 );
			}

			getimage( &attack[1][x][i], x * 100, 400 + i * 100, 100, 100 );	// 2
			getimage( &attack[0][x][i], x * 100, 600 + i * 100, 100, 100 );

			if ( x < 5 )
			{
				getimage( &kick_away[1][x][i], x * 100, 800 + i * 100, 100, 100 );	// 3
				getimage( &kick_away[0][x][i], x * 100, 1000 + i * 100, 100, 100 );

				getimage( &throw_away[1][x][i], x * 100, 1200 + i * 100, 100, 100 );	// 4
				getimage( &throw_away[0][x][i], x * 100, 1400 + i * 100, 100, 100 );
			}
		}
	SetWorkingImage();
}

// ���û���� Walk() �����˶�

void B_ONE::Walk( int id, const int& player )
{
	int i;			// ����ѭ������

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y + 20 > Player[i].Rp->y )
			OTHER::boss = true;
		else
			OTHER::boss = false;
	}

	BASE_BOSS::Walk( id, player );
	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	/****** boss ͼƬ��С��һ�� *****/
	/****** ͬʱ�������� ********/

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
		int c_x = Player[i].Rp->x;
		int c_y = Player[i].Rp->y;
		int bx = x + 10;	// ȡboss���ĵ���������ĵ�Ƚ�
		int by = y + 20;

		// ֱ���ǹ�����Ч����ֹ boss �����������

		if ( abs(bx - c_x) < 50 && abs(by - c_y) < 15 && Check_position(Dir, bx, c_x) &&
			!Player[i].die && !Player[i].lying && Player[i].Rp->Check_n_x(0XFBB) )
		{
			a_kind = 2;
			n_w = w = 0;
			n_a = a = 0;		// ��ֹ�������ж��в���
			Player[i].Set_bol( 0x00 );
			Player[i].lying = true;
			f[i] = &Role::Kick_down;
			Player[i].be_kick_counter++;
			bf[id] = &BASE_BOSS::Attack;

			if ( bx > c_x )
				Player[i].Rp->Dir = RIGHT;
			else
				Player[i].Rp->Dir = LEFT;
		}
	}
}

// ���� id ( 0-ֱ�ߣ�1-ǰ�ߣ�2-�ֻ� )

void B_ONE::Attack( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	int dx;
	int _x[2] = { -60, 60 };
	putimage( x, temp_y, &attack[Dir][a][1], SRCAND );
	putimage( x, temp_y, &attack[Dir][a][0], SRCINVERT );

	switch ( a_kind )
	{
		case 2:		// ȭ��
			if ( n_a++ % 3 == 0 )
				a = ( ++a > 3 ) ? 0 : a;

			if ( n_a > 15 )
			{
				int turn_dir[4] = { (Dir + 1) % 2, (Dir + 1) % 2, DOWN, UP };	// ������� boss ����
				move_dir = turn_dir[move_dir];
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
			}
			break;

		case 1:				// ֱ������
			if ( n_a % 20 < 10 )
			{
				a = 4;
				temp_y -= 9;
			}
			if ( n_a % 20 >= 10 )
			{
				a = 5;
				temp_y += 9;
			}
			n_a++;
			if ( n_a >= 60 )
			{
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
			}
			break;

		case 0:			// ǰ��
			if ( n_a % 20 == 0 )	// ��ر�֤ case ��ʼʱ�����ܽ� if ��ʼ��
			{
				move_dir = Dir;//////��������ȷ�� move_dir ���ܸı�!!1
				X = x + _x[Dir];
				Y = y - 90;
				temp_x = 0;
			}
			n_a++;
			if ( n_a % 20 < 10 )
				a = 4;
			else
				a = 5;

			dx = _x[move_dir] / 10;
			if ( x + dx >= rec->left && x + dx <= rec->right )
				x += dx;
			else
				temp_x++;
			temp_y = (x + temp_x * dx - X) * (x + temp_x * dx - X) / 40 + Y;

			if ( n_a >= 80 )
			{
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
			}
			break;

		default: printf( "B_ONE::Attack ����\n" );	break;
	}

	// ���в��� boss ����

	if ( a_kind < 2 )
	{
		for ( int i = 0; i < PLAYER::player_num; i++ )
		{
			int bx = x + 10;
			int by = y + 20;
			int c_x = Player[i].Rp->x;
			int c_y = Player[i].Rp->y;

			if ( Player[i].Rp->Check_n_x( 0x239 ) && !Player[i].lying &&
				 !Player[i].die && Check_position(Dir, bx, c_x) &&
				 abs(bx - c_x) < 50 && abs(by - c_y) < 15 )
				{
					Player[i].lying = true;
					f[i] = &Role::Kick_down;
					Player[i].Set_bol( 0x00 );
					Player[i].be_kick_counter = 3;

					if ( bx > c_x )
						Player[i].Rp->Dir = RIGHT;
					else
						Player[i].Rp->Dir = LEFT;
				}
		}
	}
}

//

void B_ONE::Kick_away( int id, const int& player )
{
	int _r = ( Dir + 1 ) % 2;
	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	BASE_BOSS::Kick_away( id, player );
}

//

void B_ONE::Throw_away( int id, const int& player )
{
	BASE_BOSS::Throw_away( id, player );
	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );
}

//

void B_ONE::Throw_back( int id, const int& player )
{
	BASE_BOSS::Throw_back( id, player );	// �ȵ��ö�λ temp_y�������ͼƬ
	putimage( x, temp_y, &throw_away[Dir][tb][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][tb][0], SRCINVERT );
}

//

void B_ONE::Die( int id, const int& player )
{
	if ( n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}
	BASE_BOSS::Die( id, player );
}


/********** B_TWO ********************/

B_TWO::B_TWO( int* p, RECT* rec ):
BASE_BOSS( p, rec )
{
	IMAGE img;
	loadimage( &img, "./res/Images/boss1.jpg", 900, 1600 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )
	{
		for ( int x = 0; x < 5; x++ )
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 100, i * 100, 100, 100 );		// 1
				getimage( &walk[0][x][i], x * 100, 200 + i * 100, 100, 100 );
			}
			if ( x < 3 )
			{
				getimage( &attack[1][x][i], x * 100, 400 + i * 100, 100, 100 );		// 2
				getimage( &attack[0][x][i], x * 100, 600 + i * 100, 100, 100 );
			}

			getimage( &kick_away[1][x][i], x * 100, 800 + i * 100, 100, 100 );	// 3
			getimage( &kick_away[0][x][i], x * 100, 1000 + i * 100, 100, 100 );

			getimage( &throw_away[1][x][i], x * 100, 1200 + i * 100, 100, 100 );	// 4
			getimage( &throw_away[0][x][i], x * 100, 1400 + i * 100, 100, 100 );
		}
	}
	SetWorkingImage();
}

//

void B_TWO::Walk( int id, const int& player )
{
	int i;			// ����ѭ������

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y + 20 > Player[i].Rp->y )
			OTHER::boss = true;
		else
			OTHER::boss = false;
	}

	BASE_BOSS::Walk( id, player );
	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
		int c_x = Player[i].Rp->x;
		int c_y = Player[i].Rp->y;
		int bx = x + 10;	// ȡboss���ĵ���������ĵ�Ƚ�
		int by = y + 20;

		// ֱ���ǹ�����Ч����ֹ boss �����������
		// ���� boss �Ѷȣ������ Check_position()��ǰ�󶼿ɹ���
		// Check_n_x() ���һ�� boss ��ͬ

		if ( abs(bx - c_x) < 50 && abs(by - c_y) < 15 && /*Check_position(Dir, bx, c_x) &&*/
			!Player[i].die && !Player[i].lying && Player[i].Rp->Check_n_x(0XFBB) )
		{
			a_kind = 2;
			n_w = w = 0;
			n_a = a = 0;		// ��ֹ�������ж��в���
			Player[i].Set_bol( 0x00 );
			Player[i].lying = true;
			f[i] = &Role::Kick_down;
			Player[i].be_kick_counter++;
			bf[id] = &BASE_BOSS::Attack;

			if ( bx > c_x )
				Player[i].Rp->Dir = RIGHT;
			else
				Player[i].Rp->Dir = LEFT;
		}
	}
}

//

void B_TWO::Attack( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	putimage( x, temp_y, &attack[Dir][a][1], SRCAND );
	putimage( x, temp_y, &attack[Dir][a][0], SRCINVERT );

	if ( a_kind == 2 )
	{
		if ( ++n_a > 9 )
		{
			int turn_dir[4] = { (Dir + 1) % 2, (Dir + 1) % 2, DOWN, UP };	// ������� boss ����
			move_dir = turn_dir[move_dir];
			n_a = a = 0;
			bf[id] = &BASE_BOSS::Walk;
			return;
		}
	}

	/***** ���й��� ******/

	if ( a_kind < 2 )
	{
		if ( n_a == 0 )		a = 1;
		if ( n_a == 11 )	a = 2;

		int _x[2] = { -9, 9 };
		if ( x + _x[Dir] >= rec->left && x + _x[Dir] <= rec->right && n_a < 33 )
			x += _x[Dir];

		/***** x ��ⷶΧ����������߼�ⷶΧ ******/

		for ( int i = 0 ;i < PLAYER::player_num; i++ )
		{
			int bx = x + 10;
			int by = y + 20;
			int c_x = Player[i].Rp->x;
			int c_y = Player[i].Rp->y;

			if ( Player[i].Rp->Check_n_x(0x239) && !Player[i].lying &&
				!Player[i].die && Check_position(Dir, x, c_x) && n_a < 33 &&
				abs( x - c_x ) < 60 && abs(by - c_y) < 15 )
			{
				Player[i].lying = true;
				f[i] = &Role::Kick_down;
				Player[i].Set_bol( 0x00 );
				Player[i].be_kick_counter = 3;

				if ( bx > c_x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;
			}
		}

		if ( ++n_a > 50 )
		{
			n_a = a = 0;
			bf[id] = &BASE_BOSS::Walk;
		}
	}
}

//

void B_TWO::Kick_away( int id, const int& player )
{
	int _r = ( Dir + 1 ) % 2;
	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	BASE_BOSS::Kick_away( id, player );
}

//

void B_TWO::Throw_away( int id, const int& player )
{
	BASE_BOSS::Throw_away( id, player );
	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );
}

//

void B_TWO::Throw_back( int id, const int& player )
{
	BASE_BOSS::Throw_back( id, player );	// �ȵ��ö�λ temp_y�������ͼƬ
	putimage( x, temp_y, &throw_away[Dir][tb][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][tb][0], SRCINVERT );
}

//

void B_TWO::Die( int id, const int& player )
{
	if ( n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}
	BASE_BOSS::Die( id, player );
}

/************* B_THREE ***************************/

B_THREE::B_THREE( int* p, RECT* rec ):
BASE_BOSS( p, rec )
{
	_a = 0;
	dx1 = 4;
	dy1 = 4;
	dx2 = 5;
	leave = false;
	kiss = false;
	step = 0;

	IMAGE img;
	loadimage( &img, "./res/Images/boss2.jpg", 1200, 1600 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )
	{
		for ( int x = 0; x < 12; x++ )
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 100, i * 100, 100, 100 );		// 1
				getimage( &walk[0][x][i], x * 100, 200 + i * 100, 100, 100 );
			}
			getimage( &attack[1][x][i], x * 100, 400 + i * 100, 100, 100 );		// 2
			getimage( &attack[0][x][i], x * 100, 600 + i * 100, 100, 100 );

			if ( x < 5 )
			{
				getimage( &kick_away[1][x][i], x * 100, 800 + i * 100, 100, 100 );	// 3
				getimage( &kick_away[0][x][i], x * 100, 1000 + i * 100, 100, 100 );

				getimage( &throw_away[1][x][i], x * 100, 1200 + i * 100, 100, 100 );	// 4
				getimage( &throw_away[0][x][i], x * 100, 1400 + i * 100, 100, 100 );
			}
		}
	}
	SetWorkingImage();
}

//

void B_THREE::Walk( int id, const int& player )
{
	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y + 20 > Player[i].Rp->y )
			OTHER::boss = true;
		else
			OTHER::boss = false;
	}

	Boss[id].lying = false;		// Kick_away() �� Throw_away() û�����ã�������룡
	w_t2 = timeGetTime();
	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	if ( w_t2 - w_t1 < w_dt )
		return;
	else
		w_t1 = w_t2;

	/***********************/

	if ( rand() % 40 == 0 )		start = true;
	if ( start )
	{
		start = false;
		int num = rand() % player;

		if ( Player[num].life >= 0 )
			Boss[id].role = (byte)num;
	}

	int p_y = Player[Boss[id].role].Rp->y;
	int p_x = Player[Boss[id].role].Rp->x;
	Dir = SetDir( x, p_x );	// boss ��·ʼ�ճ������

	/***** ������� *************/

	int dx = 0, dy = 0;
	if ( !leave )
	{
		int bx = x + 10;
		int by = y + 20;
		_a = atan2( (double)(by - p_y), p_x - bx );

		/**** б���˶� ***********/

		if ( abs(by - p_y) >= 15 )
		{
			if ( cos(_a) < 0 )	dx1 = -abs(dx1);
			if ( cos(_a) > 0 )	dx1 =  abs(dx1);
			if ( sin(_a) < 0 )	dy1 =  abs(dy1);
			if ( sin(_a) > 0 )	dy1 = -abs(dy1);

			dx = dx1;
			dy = dy1;
		}
		else
		{
			if ( cos(_a) < 0 )	dx2 = -abs(dx2);
			if ( cos(_a) > 0 )	dx2 =  abs(dx2);

			dx = dx2;
			dy = 0;
		}
	}

	/******* ������ĳһ��Χ�ڣ����Զ����� ********/

	if ( abs(x - p_x) < 100 && abs(x - p_x) > 90 && rand() % 3 == 0 )
		leave = true;

	/****** ��⹥������ ******/

	int bx = x + 10;	// �޲�boss�����Ա�Ƚ�
	int by = y + 20;

	if ( abs(by - p_y) < 15 && Check_position(Dir, bx, p_x) &&
		 !Player[Boss[id].role].lying && !Player[Boss[id].role].die )
	{
		bool attack = false;
		if ( abs(bx - p_x) < 50 )	leave = true;

		if ( abs(bx - p_x) < 50 && Player[Boss[id].role].Rp->Check_n_x(0x6B9) )
		{
			if ( Check_kiss_x(x, rec->left+50, rec->right - 60) )	// ȷ��kiss����Ҳ��ᱻԽ��
				a_kind = rand() % 2;	// 0 - kiss��1 - ��ȭ
			else
				a_kind = 1;

			attack = true;
			Player[Boss[id].role].Set_bol( 0x00 );
			f[Boss[id].role] = &Role::Stand;		// ������ҹ���������boss������
		}

		if ( abs(bx - p_x) < 20 && Player[Boss[id].role].Rp->Check_n_x(0x239) )
		{
			if ( p_y != Player[Boss[id].role].Rp->temp_y && rand() % 2 == 0 )
			{
				a_kind = 2;			// boss ������ȭ
				attack = true;
			}
		}

		if ( attack )
		{
			n_w = w = 0;
			n_a = a = 0;
			bf[id] = &BASE_BOSS::Attack;
			return;
		}
	}

	/******** boss ���˲���⹥��ģʽ 3 ********/

	if ( leave )
	{
		if ( ++step > 20 )
		{
			step = 0;
			leave = false;		// ����������� leave = true
		}
		if ( rand() % 3 == 1 && step > 15 )
		{
			a_kind = 3;
			step = 0;
			n_w = w = 0;
			n_a = a = 0;
			bf[id] = &BASE_BOSS::Attack;
			return;
		}

		int _x[2] = { 6, -6 };
		dx = _x[Dir];
		dy = 0;
	}

	if ( x + dx > rec->left && x + dx < rec->right &&
		 y + dy > rec->top && y + dy < rec->bottom )
	{
		x += dx;
		y += dy;
		temp_y = y;
	}
	else
	{
		step = 0;
		leave = false;
	}

	if ( n_w++ % 4 == 0 )
		w = ( ++w > 3 ) ? 0 : w;
}

//

void B_THREE::Attack( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);

	switch (a_kind)
	{
		case 0:			// kiss
			if ( n_a == 0 )
			{
				kiss = true;
				Player[Boss[id].role].be_kiss = true;
					int bx = x + 10;
					int c_x = Player[Boss[id].role].Rp->x;

					Player[Boss[id].role].boss = id;
					Player[Boss[id].role].lying = true;
					f[Boss[id].role] = &Role::Be_kiss;

					if ( bx > c_x )
						Player[Boss[id].role].Rp->Dir = RIGHT;
					else
						Player[Boss[id].role].Rp->Dir = LEFT;
			}
			if ( n_a % 8 == 0 )		a = 10;
			if ( n_a % 16 == 0 )	a = 11;
			if ( ++n_a > 50 )
			{
				kiss = false;
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
			}
			break;

		case 1:			// ��ȭ
			a = 6;
			if ( n_a == 0 )
			{
				for ( int i = 0; i < PLAYER::player_num; i++ )
				{
					int bx = x + 10;	// ȡboss���ĵ���������ĵ�Ƚ�
					int by = y + 20;	// ���е�boss��ȭ�����붼����һ����������һ�boss������!
					int c_x = Player[i].Rp->x;
					int c_y = Player[i].Rp->y;

					if ( Player[i].Rp->Check_n_x(0x6B9) && !Player[i].lying &&
						!Player[i].die && Check_position(Dir, bx, c_x) &&
						abs(by - c_y) < 15 && abs(bx - c_x) < 50 )
					{
						Player[i].lying = true;
						f[i] = &Role::Kick_down;
						Player[i].Set_bol( 0x00 );

						if ( bx > c_x )
							Player[i].Rp->Dir = RIGHT;
						else
							Player[i].Rp->Dir = LEFT;
					}
				}
			}

			if ( ++n_a > 10 )
			{
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
			}
			break;

		case 2:			// ����ȭ
			if ( n_a == 0 )		a = 7;
			if ( n_a == 4 )		a = 8;
			if ( n_a == 9 )		a = 9;
			if ( ++n_a > 18 )
			{
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
			}
			break;

		case 3:			// ���
			if ( n_a < 20 )
			{
				if ( n_a % 4 == 0 )		a = 0;
				if ( n_a % 8 == 0 )		a = 1;
				_a = atan2( (double)(y+20 - Player[Boss[id].role].Rp->y), Player[Boss[id].role].Rp->x - x-20 );
			}
			if ( n_a >= 20 && n_a <= 45 )
			{
				int _x = x + (int)( 11 * cos(_a) );
				int _y = y - (int)( 11 * sin(_a) );
				if ( _x > rec->left && _x < rec->right && _y > rec->top && _y < rec->bottom )
					x = _x, y = _y;

				if ( n_a % 4 == 0 )
					a = (++a > 5 ) ? 2 : a;

			}
			if ( n_a > 45 )	 a = 6;
			if ( ++n_a > 60 )
			{
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
			}
			break;
		default:printf( "B_THREE::Attack()����\n" );	break;
	}
	putimage( x, y, &attack[Dir][a][1], SRCAND );
	putimage( x, y, &attack[Dir][a][0], SRCINVERT );

	/******* ��� 2\3 *******/

	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		int bx = x + 10;
		int by = y + 20;
		int c_x = Player[i].Rp->x;
		int c_y = Player[i].Rp->y;

		if ( a_kind == 3 && Player[i].Rp->Check_n_x(0x239) && n_a > 20 && n_a < 45 ||
			 a_kind == 2 && Player[i].Rp->Check_n_x(0x639) )
		{
			if ( Check_position(Dir, bx, c_x) && !Player[i].die && !Player[i].lying &&
				 abs(by - c_y) < 15 && abs(bx - c_x) < 50 )
			{
				Player[i].lying = true;
				f[i] = &Role::Kick_down;
				Player[i].Set_bol( 0x00 );
				Player[i].be_kick_counter = 3;

				if ( bx > c_x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;
			}
		}
	}
}

//

void B_THREE::Kick_away( int id, const int& player )
{
	if ( kiss )
	{
		for ( int i = 0; i < PLAYER::player_num; i++ )
		{
			if ( Player[i].be_kiss )
			{
				Player[i].be_kiss = false;
				f[i] = &Role::Stand;
			}
		}
		kiss = false;
	}

	int _r = ( Dir + 1 ) % 2;
	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	BASE_BOSS::Kick_away( id, player );
}

//

void B_THREE::Throw_away( int id, const int& player )
{
	if ( kiss )
	{
		for ( int i = 0; i < PLAYER::player_num; i++ )
		{
			if ( Player[i].be_kiss )
			{
				Player[i].be_kiss = false;
				f[i] = &Role::Stand;
			}
		}
		kiss = false;
	}

	BASE_BOSS::Throw_away( id, player );
	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );
}

//

void B_THREE::Throw_back( int id, const int& player )
{
	if ( kiss )
	{
		for ( int i = 0; i < PLAYER::player_num; i++ )
		{
			if ( Player[i].be_kiss )
			{
				Player[i].be_kiss = false;
				f[i] = &Role::Stand;
			}
		}
		kiss = false;
	}

	BASE_BOSS::Throw_back( id, player );	// �ȵ��ö�λ temp_y�������ͼƬ
	putimage( x, temp_y, &throw_away[Dir][tb][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][tb][0], SRCINVERT );
}

//

void B_THREE::Die( int id, const int& player )
{
	if ( n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}
	BASE_BOSS::Die( id, player );
}

/*********** B_FOUR *****************/

B_FOUR::B_FOUR( int* p, RECT* rec ):
BASE_BOSS( p, rec )
{
	_a = 0;

	IMAGE img;
	loadimage( &img, "./res/Images/boss1.jpg", 900, 1600 );

	/***** �� boss ���·� ******/

	DWORD* pem = GetImageBuffer( &img );
	for ( int n = 0; n <= 1400; n += 200 )
	{
		for ( int i = n * 800; i < (n + 100) * 900; i++ )
		{
			COLORREF c = BGR( pem[i] );
			BYTE r, g, b;

			r = GetRValue(c);
			g = GetGValue(c);
			b = GetBValue(c);

			if ( r >= 200 && r <= 255 && g >= 0 && g <= 182 && b >= 60 && b <= 200 )
				pem[i] = BGR( RGB(240, 16, 251) );
		}
	}
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )
	{
		for ( int x = 0; x < 9; x++ )
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 100, i * 100, 100, 100 );		// 1
				getimage( &walk[0][x][i], x * 100, 200 + i * 100, 100, 100 );
			}
			getimage( &attack[1][x][i], x * 100, 400 + i * 100, 100, 100 );		// 2
			getimage( &attack[0][x][i], x * 100, 600 + i * 100, 100, 100 );

			if ( x < 5 )
			{
				getimage( &kick_away[1][x][i], x * 100, 800 + i * 100, 100, 100 );	// 3
				getimage( &kick_away[0][x][i], x * 100, 1000 + i * 100, 100, 100 );

				getimage( &throw_away[1][x][i], x * 100, 1200 + i * 100, 100, 100 );	// 4
				getimage( &throw_away[0][x][i], x * 100, 1400 + i * 100, 100, 100 );
			}
		}
	}
	SetWorkingImage();
}

//

void B_FOUR::Walk( int id, const int& player )
{
	int i;			// ����ѭ������

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y + 20 > Player[i].Rp->y )
			OTHER::boss = true;
		else
			OTHER::boss = false;
	}

	BASE_BOSS::Walk( id, player );
	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
		int c_x = Player[i].Rp->x;
		int c_y = Player[i].Rp->y;
		int bx = x + 10;	// ȡboss���ĵ���������ĵ�Ƚ�
		int by = y + 20;

		// ֱ���ǹ�����Ч����ֹ boss �����������
		// ���� boss �Ѷȣ������ Check_position()��ǰ�󶼿ɹ���
		// Check_n_x() ���һ�� boss ��ͬ

		if ( abs(bx - c_x) < 50 && abs(by - c_y) < 15 && /*Check_position(Dir, bx, c_x) &&*/
			!Player[i].die && !Player[i].lying && Player[i].Rp->Check_n_x(0XFBB) )
		{
			a_kind = 3;
			n_w = w = 0;
			n_a = a = 0;		// ��ֹ�������ж��в���
			Player[i].Set_bol( 0x00 );
			Player[i].lying = true;
			f[i] = &Role::Kick_down;
			Player[i].be_kick_counter++;
			bf[id] = &BASE_BOSS::Attack;

			if ( bx > c_x )
				Player[i].Rp->Dir = RIGHT;
			else
				Player[i].Rp->Dir = LEFT;
		}
	}
}

//

void B_FOUR::Attack( int id, const int& player )
{
	int n;			// ����ѭ������

	UNREFERENCED_PARAMETER(player);
	putimage( x, y, &attack[Dir][a][1], SRCAND );
	putimage( x, y, &attack[Dir][a][0], SRCINVERT );

	switch ( a_kind )
	{
		case 0:		// ��ǰ���й���
			{
				if ( n_a == 0 )		a = 1;
				if ( n_a == 11 )	a = 2;

				int _x[2] = { -9, 9 };
				if ( x + _x[Dir] >= rec->left && x + _x[Dir] <= rec->right && n_a < 33 )
					x += _x[Dir];

				if ( ++n_a > 50 )
				{
					n_a = a = 0;
					bf[id] = &BASE_BOSS::Walk;
				}
			}
			break;

		case 1:		// ��б���й���
			{
				if ( n_a == 0 )		a = 1;
				if ( n_a == 11 )	a = 2;
				if ( n_a == 0 )
					_a = atan2( (double)(y+20- Player[Boss[id].role].Rp->y), Player[Boss[id].role].Rp->x - x-10 );
				int _x = x + (int)( 11 * cos(_a) );
				int _y = y - (int)( 11 * sin(_a) );
				if ( _x > rec->left && _x < rec->right && _y > rec->top && _y < rec->bottom && n_a < 33 )
				{
					x = _x;
					y = _y;
					temp_y = y;
				}

				if ( ++n_a > 50 )
				{
					n_a = a = 0;
					bf[id] = &BASE_BOSS::Walk;
				}
			}
			break;

		case 3:		// ����
			if ( ++n_a > 9 )
			{
				int turn_dir[4] = { (Dir + 1) % 2, (Dir + 1) % 2, DOWN, UP };	// ������� boss ����
				move_dir = turn_dir[move_dir];
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
				return;
			}
			break;

		case 2:		// �����Ĺ���
			{
				Boss[id].lying = false;		// ����Ѫ����һ�벻����� Walk()������lying���࣬����������
				if ( n_a <= 10 )
					a = 3;

				if ( n_a == 11 )
				{
					/****** ��ֹ˫��ģʽ boss ֻ�ṥ��һ���� ******/

					int num = rand() % player;
					if ( Player[num].life >= 0 )
						Boss[id].role = (byte)num;

					Dir = SetDir( x, Player[Boss[id].role].Rp->x );
					_a = atan2( (double)(y+20- Player[Boss[id].role].Rp->y), Player[Boss[id].role].Rp->x - x-10 );
				}

				if ( n_a > 10 && n_a <= 33 )
				{
					if ( n_a % 5 == 4 )
						a = (++a > 7) ? 4 : a;
					int _x = x + (int)( 20 * cos(_a) );
					int _y = y - (int)( 20 * sin(_a) );
					if ( _x > rec->left && _x < rec->right && _y > rec->top && _y < rec->bottom && n_a < 33 )
					{
						/****** ���� boss ��˸����Ч�� *******/

						int dir_x[2] = { 1, -1 };
						int sx[3];
						for ( n = 0; n < 3; n++ )
							sx[n] = x + rand() % 40 * dir_x[Dir];
						if ( rand() % 2 == 0 )
						{
							for ( n = 0; n < 3; n++ )
							{
								putimage( sx[n], y, &attack[Dir][a][1], SRCAND );
								putimage( sx[n], y, &attack[Dir][a][0], SRCINVERT );
							}
						}

						x = _x;
						y = _y;
						temp_y = y;
					}
					else
						n_a = 34;
				}
				if ( n_a >= 34 && n_a <= 70 )
					a = 8;
				if ( ++n_a > 70 )
				{
					n_a = 11;
					a = 4;
				}
			}
			break;
		default:	printf("B_FOUR::Attack����\n");	break;
	}

	if ( a_kind <= 2 )
	{
		/***** x ��ⷶΧ����������߼�ⷶΧ ******/

		for ( int i = 0 ;i < PLAYER::player_num; i++ )
		{
			int bx = x + 10;
			int by = y + 20;
			int c_x = Player[i].Rp->x;
			int c_y = Player[i].Rp->y;

			if ( Player[i].Rp->Check_n_x(0x239) && !Player[i].lying &&
				!Player[i].die && Check_position(Dir, x, c_x) && n_a < 28 &&
				abs( x - c_x ) < 60 && abs(by - c_y) < 15 )
			{
				Player[i].lying = true;
				f[i] = &Role::Kick_down;
				Player[i].Set_bol( 0x00 );
				Player[i].be_kick_counter = 3;

				if ( bx > c_x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;
			}
		}
	}
}

//

void B_FOUR::Kick_away( int id, const int& player )
{
	int _r = ( Dir + 1 ) % 2;
	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	BASE_BOSS::Kick_away( id, player );
}

//

void B_FOUR::Throw_away( int id, const int& player )
{
	BASE_BOSS::Throw_away( id, player );
	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );
}

//

void B_FOUR::Throw_back( int id, const int& player )
{
	BASE_BOSS::Throw_back( id, player );	// �ȵ��ö�λ temp_y�������ͼƬ
	putimage( x, temp_y, &throw_away[Dir][tb][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][tb][0], SRCINVERT );
}

//

void B_FOUR::Die( int id, const int& player )
{
	if ( n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}
	BASE_BOSS::Die( id, player );
}

/************ B_FIVE **************************************/

B_FIVE::B_FIVE( int* p, RECT* rec ):
BASE_BOSS( p, rec )
{
	IMAGE img;
	loadimage( &img, "./res/Images/boss4.jpg", 1200, 1600 );
	SetWorkingImage( &img );

	for ( int i = 0; i < 2; i++ )
	{
		for ( int x = 0; x < 7; x++ )
		{
			if ( x < 4 )
			{
				getimage( &walk[1][x][i], x * 100, i * 100, 100, 100 );		// 1
				getimage( &walk[0][x][i], x * 100, 200 + i * 100, 100, 100 );

				getimage( &attack[1][x+10][i], 300 + x * 80, 400 + i * 100, 80, 80 );	// 4
				getimage( &attack[0][x+10][i], 300 + x * 80, 600 + i * 100, 80, 80 );
			}
			if ( x < 7 )	// attack ͼ��������
			{
				getimage( &attack[1][x][i], x * 100, i * 100, 100, 100 );		// 1
				getimage( &attack[0][x][i], x * 100, 200 + i * 100, 100, 100 );
			}
			if ( x < 3 )
			{
				getimage( &attack[1][x+7][i], x * 100, 400 + i * 100, 100, 100 );	// 2
				getimage( &attack[0][x+7][i], x * 100, 600 + i * 100, 100, 100 );
				
				getimage( &bullet_away[x][i], 700 + x * 100, 400 + i * 100, 100, 100 );	// 2
			}

			if ( x < 5 )
			{
				getimage( &kick_away[1][x][i], x * 100, 800 + i * 100, 100, 100 );	// 3
				getimage( &kick_away[0][x][i], x * 100, 1000 + i * 100, 100, 100 );

				getimage( &throw_away[1][x][i], x * 100, 1200 + i * 100, 100, 100 );	// 4
				getimage( &throw_away[0][x][i], x * 100, 1400 + i * 100, 100, 100 );
			}
		}
	}
	SetWorkingImage();
}

//

void B_FIVE::Walk( int id, const int& player )
{
	int i;			// ����ѭ������

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
		if ( y + 30 > Player[i].Rp->y )
			OTHER::boss = true;
		else
			OTHER::boss = false;
	}

	temp_y = y;
	Boss[id].lying = false;		// Kick_away() �� Throw_away() û�����ã�������룡
	putimage( x, y, &walk[Dir][w][1], SRCAND );
	putimage( x, y, &walk[Dir][w][0], SRCINVERT );

	/***** ����˫��ģʽ ******/

	if ( rand() % 40 == 0 )		start = true;
	if ( start )
	{
		start = false;
		int num = rand() % player;

		if ( Player[num].life >= 0 )
			Boss[id].role = (byte)num;
	}
	Dir = SetDir( x, Player[Boss[id].role].Rp->x );	// boss ��·ʼ�ճ������

	//*************** ���ȭͷ����

	for ( i = 0; i < PLAYER::player_num; i++ )
	{
		int c_x = Player[i].Rp->x;
		int c_y = Player[i].Rp->y;
		int bx = x + 10;
		int by = y + 20;

		if ( abs(bx - c_x) < 50 && abs(by - c_y) < 15 &&
			!Player[i].die && !Player[i].lying && Player[i].Rp->Check_n_x(0XAB9) )
		{
			a_kind = 0;
			n_w = w = 0;
			n_a = 0;
			a = 7;
			Player[i].Set_bol( 0x00 );
			Player[i].lying = true;
			f[i] = &Role::Kick_down;
			Player[i].be_kick_counter = 3;
			bf[id] = &BASE_BOSS::Attack;

			if ( bx > c_x )
				Player[i].Rp->Dir = RIGHT;
			else
				Player[i].Rp->Dir = LEFT;
			return;
		}
	}

	/********* ����Ƿ������ƶ����� **********/

	if ( x >= -20 && x < -10 || x > 410 && x <= 420 )
	{
		move_dir = UP;
		a_kind = 1;			// �����ƶ�����ȭͷ
		n_w = w = 0;
		n_a = a = 0;
		bf[id] = &BASE_BOSS::Attack;
		return;
	}

	int _x = x - Drtx[Dir] * 2;			// boss ��������˶�
	if ( _x >= -20 && _x <= 440 )
		x = _x;

	if ( n_w++ % 4 == 0 )
		w = ( ++w > 3 ) ? 0 : w;
}

//

void B_FIVE::Attack( int id, const int& player )
{
	UNREFERENCED_PARAMETER(player);
	switch (a_kind)
	{
	case 0:			// ��ȭ����
		{
			a = rand() % 3 + 7;
			if ( ++n_a > 20 )
			{
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
			}
		}break;

	case 1:			// // �����ƶ�����ȭͷ
		{
			if ( n_a++ < 90 )
			{
				int dy[4] = { 0, 0, -4, 4 };	// ֻ�ú�����������
				if ( y + dy[move_dir] >= rec->top && y + dy[move_dir] <= rec->bottom )
				{
					y += dy[move_dir];
					temp_y = y;
				}
				else
					move_dir = 5 - move_dir;

				if ( n_a % 3 == 0 )
					a = (++a > 3) ? 0 : a;
				if ( n_a % 12 == 0 )
				{
					a = 4;

					/**** �������ڲ���ȭͷ ****/

					SetSound( NULL, "shoot", "boss" );

					L::pnem = L::pn;
					L::pn = new LINK(x, y+25, Dir);
					L::pn->pP = L::pnem;
				}
			}
			else
			{
				if ( n_a % 3 == 0 )		a = 5;
				if ( n_a % 6 == 0 )		a = 6;
			}
			
			if ( n_a > 122 )
			{
				n_a = a = 0;

				/***** �����ҿ��� boss һ����������ҳ�� ******/

				for ( int i = 0; i < PLAYER::player_num; i++ )
				{
					int p_x = Player[i].Rp->x;
					if ( abs(p_x - x) < 100 && rand() % 4 < 3 )
					{
						a_kind = 2;
						a = 7;
						Boss[id].role = (byte)i;
					}
				}
			}
			// �ù������Ž� link.cpp �ڵ� PutImg()
		}break;

	case 2:			// ���ҳ��
		{
			if ( n_a++ < 40 )
				a = rand() % 3 + 7;

			if ( n_a >= 40 && n_a < 54 )
			{
				int dx[2] = { -17, 17 };
				x += dx[Dir];		////////////////// �᲻��Խ�磿����������������
				if ( n_a <= 46 )	temp_y -= 1;
				if ( n_a >= 47 )	temp_y += 1;
			}

			for ( int i = 0; i < PLAYER::player_num; i++ )
			{
				int bx = x + 10;
				int by = y + 20;
				int p_x = Player[i].Rp->x;
				int p_y = Player[i].Rp->y;
				if ( !Player[i].lying && !Player[i].die && Player[i].Rp->Check_n_x(0x239) &&
					 abs(bx - p_x) < 50 && abs(by - p_y) < 15 )
				{
					Player[i].lying = true;
					f[i] = &Role::Kick_down;
					Player[i].Set_bol( 0x00 );
					Player[i].be_kick_counter = 3;

					if ( bx > p_x )
						Player[i].Rp->Dir = RIGHT;
					else
						Player[i].Rp->Dir = LEFT;
				}
			}

			if ( n_a >= 54 )
			{
				n_a = a = 0;
				bf[id] = &BASE_BOSS::Walk;
			}
		}break;

	default: MessageBox( NULL, "B_FIVE::Attack()����", "ds", MB_OK );	break;
	}

	putimage( x, temp_y, &attack[Dir][a][1], SRCAND );
	putimage( x, temp_y, &attack[Dir][a][0], SRCINVERT );
}

//

void B_FIVE::Kick_away( int id, const int& player )
{
	int _r = ( Dir + 1 ) % 2;
	putimage( x, temp_y, &kick_away[_r][ka][1], SRCAND );
	putimage( x, temp_y, &kick_away[_r][ka][0], SRCINVERT );

	BASE_BOSS::Kick_away( id, player );
}

//

void B_FIVE::Throw_away( int id, const int& player )
{
	BASE_BOSS::Throw_away( id, player );
	putimage( x, temp_y, &throw_away[Dir][ta][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][ta][0], SRCINVERT );
}

//

void B_FIVE::Throw_back( int id, const int& player )
{
	BASE_BOSS::Throw_back( id, player );	// �ȵ��ö�λ temp_y�������ͼƬ
	putimage( x, temp_y, &throw_away[Dir][tb][1], SRCAND );
	putimage( x, temp_y, &throw_away[Dir][tb][0], SRCINVERT );
}

//

void B_FIVE::Die( int id, const int& player )
{
	if ( n_d % 2 == 0 )
	{
		putimage( x, y, &throw_away[Dir][3][1], SRCAND );
		putimage( x, y, &throw_away[Dir][3][0], SRCINVERT );
	}
	BASE_BOSS::Die( id, player );
}



/********** ��ɢ������ʵ�� ***********/

// �趨 boss �ĳ���

int SetDir( const int& b_x, const int& p_x )
{
	if ( b_x >= p_x )
		return LEFT;
	else
		return RIGHT;
}


// �ﵽ�߽����»�ȡ�ƶ�����

int Change_move_dir( const int& e_x, const int& e_y )
{
	/***** ÿ�ֱ߽緽�� *****/

	int l_u[2] = { RIGHT, DOWN	};	// �������ϽǷ��� �� �� ��
	int l_d[2] = { RIGHT, UP	};
	int r_u[2] = { LEFT, DOWN	};
	int r_d[2] = { LEFT, UP		};

	if ( e_x < 250 )
	{
		if ( e_y < 230 )			// ����
			return l_u[rand() % 2];
		else						// ����
			return l_d[rand() % 2];
	}
	else
	{
		if ( e_y < 230 )			// ����
			return r_u[rand() % 2];
		else						// ����
			return r_d[rand() % 2];
	}
	return 0;
}

//

bool Check_kiss_x( const int& x, const int& lef, const int& rig )
{
	if ( x > lef && x < rig )
		return true;
	else
		return false;
}

// ���������ǰ���� boss

void Dispatch_five_boss( int part )
{
	for ( int i = 0; i < BOSS::Boss_num; i++ )
		if ( !Boss[i].die && !Boss[i].appear )
		{
			// ������ҵ�����
			for ( int id = 0; id < PLAYER::player_num; id++ )
			{
				WEAPON::out = false;
				Player[id].Rp->have_weapon = false;
			}

			Boss[i].lying = false;
			Boss[i].appear = true;
			Boss[i].hp = 440;
			Boss[i].max_hp = Boss[i].hp;
			int a[3] = { 420, 274, 20 };
			static RECT rec = { -20, 180, 420, 275 };

			COLORREF old = getfillcolor();
			setfillcolor( BLACK );
			bar( 0, 370, 500, 450 );
			setfillcolor(old);

			delete Boss[i].Bp;
			switch (part)
			{
				case 2:
					{
						Boss[i].Bp = new B_THREE( a, &rec );
						Boss[i].ph = &BOSS::head[2];
						putimage( 10, 380, &BOSS::head[2] );
						if ( i + 1 == BOSS::Boss_num )
						{
							char c[12][3] = { "��","��","��","ɱ","��","С","��","��","��","��","��","��" };
							mciSendString( "open ./res/Sounds/dada.mp3 alias da", 0, 0, 0 );
							mciSendString( "play da repeat", 0, 0, 0 );
							
							for ( int n = 0; n < 12; n++ )
							{
								Sleep(250);
								outtextxy( 10 + n * 20, 420, c[n] );
								FlushBatchDraw();
							}
							mciSendString( "close da", NULL, 0, NULL );
							Sleep( 1000 );
						}
					}
					break;

				case 5:
					{
						Boss[i].Bp = new B_FOUR( a, &rec );
						Boss[i].ph = &BOSS::head[3];
						putimage( 10, 380, &BOSS::head[3] );
						if ( i + 1 == BOSS::Boss_num )
						{
							outtextxy( 100, 420, "���������������㣡" );
							FlushBatchDraw();
							Sleep(3500);
						}
					}
					break;

				default:
					MessageBox( GetHWnd(), "boss_define.cpp��Dispatch_five_boss()����", "not", MB_OK );
					break;
			}
		}
}


////
////		����
////


void LINK::PutImg()
{
	if ( pP == NULL )
		return;

	int dx[2] = { -13, 13 };
	if ( x + dx[dir] >= -20 && x + dx[dir] <= 440 )
	{
		putimage( x, y, &B_FIVE::attack[dir][a][1], SRCAND );
		putimage( x, y, &B_FIVE::attack[dir][a][0], SRCINVERT );
		x += dx[dir];
		if ( n_a++ % 7 == 0 )
			a = ( ++a > 13 ) ? 11 : a;
	}
	else
	{
	//	 delete this;//	�����Ļ�ò���˶�����!
		delete pP;	// ɾ������ָ����һ�������ָ�룡
		pP = NULL;	// �ǵù� NULL����ֹɾ������Ȼ������ else ��ɾ��һ�Σ�ϵͳ����
		return;		// ɾ�������ڵ��� pP->Cout() ��
	}

	/******* ���ȭͷ�Ƿ������� *******/

	for ( int i = 0; i < PLAYER::player_num; i++ )
	{
		int p_x = Player[i].Rp->x;
		int p_y = Player[i].Rp->y;
		if ( !Player[i].lying && !Player[i].die &&
			 abs(x - p_x) < 30 && abs(y - p_y) < 15 )
		{
			if ( dir == Player[i].Rp->Dir || Player[i].Rp->Check_n_x(0x1fb9) )
			{
				Player[i].lying = true;
				Player[i].be_kick_counter = 3;
				Player[i].Set_bol( 0x00 );
				f[i] = &Role::Kick_down;

				if ( x+10 > p_x )
					Player[i].Rp->Dir = RIGHT;
				else
					Player[i].Rp->Dir = LEFT;
			}

			//ȭͷ������,,������
			if ( dir != Player[i].Rp->Dir &&
				 Player[i].Rp->Getn_a() != 0 || Player[i].Rp->n_bb != 0 )
			{
				SetSound( NULL, "����", "bos" );
				L::p_temp = L::p_b;
				L::p_b = new LINK( true, x, y, ++dir );
				L::p_b->p_B = L::p_temp;

				/**** ֱ�ӽ������ж���! ********/

				delete pP;
				pP = NULL;
				return;
			}
		}
	}
	pP->PutImg();
}


// ����ȭͷ������Ļ���

void LINK::Kick_down()
{
	if ( p_B == NULL )
		return;

	int _x[2] = {-20, 20};
	if ( n_a == 0 )
	{
		X = b_x + _x[b_r];
		Y = b_y - 40;
	}
	if ( n_a == 22 )
	{
		X = b_x + _x[b_r] / 2;
		Y = b_y - 30;
	}

	if ( n_a < 22 )
	{
		b_x += _x[b_r] / 10;
		b_y = (b_x - X) * (b_x - X) / 13 + Y;
	}
	else
	{
		b_x += _x[b_r] / 20;
		b_y = (b_x - X) * (b_x - X) / 9 + Y;
	}

	putimage( b_x, b_y, &B_FIVE::bullet_away[a][1], SRCAND );
	putimage( b_x, b_y, &B_FIVE::bullet_away[a][0], SRCINVERT );

	if ( ++n_a % 4 == 0 )
		a = (++a > 2) ? 0 : a;
	if ( n_a > 42 )
	{
		delete p_B;
		p_B = NULL;
		return;
	}
	p_B->Kick_down();
}




/********** ��ɢ����ʵ�� ***********/

int Revise_xy( int id, const int& x, const int& y, int& dx, int& dy )
{
	if ( Map.map_id != 2 || Map.part[Map.map_id] >= 2 )	// ����޿�
		return 0;

	Emeny[id].Mp->Setxy(x + dx, y + dy);
	if ( !Check_hole(PE_emeny, id) )
	{
		Emeny[id].Mp->Setxy(x - dx, y - dy);
		return 0;
	}
	Emeny[id].Mp->Setxy(x - dx, y - dy);

	if ( y >= 223 )
	{
		dx = 0;		// ������������ӣ����������ƶ�
		dy = -5;
	}
	else
	{
		dx *= 2;
		dy = 0;
	}

	return 0;
}

// �������ݵ��˳��ֵ� x λ��

void Revise_dispatch( int& x )
{
	if ( Map.map_id == 3 && Map.part[Map.map_id] == 5 )
	{
		int _x[2] = { 50, 350 };
		x = _x[rand() % 2];
	}
}

// �������ݵ����ƶ�����

bool Revise_lift( const int& id, const int& dx )
{
	if ( Map.map_id == 3 && Map.part[Map.map_id] == 5 )
	{
		int ex = Emeny[id].Mp->Getx();
		if ( ex + dx < 35 || ex + dx > 370 )
			return false;
		else
			return true;
	}
	else
		return true;
}





/******* PLAYER �ṹʵ�� ********************************************************/

void PLAYER::Set_bol( int val )
{
	for ( int i = 0; i < 6; i++ )
	{
		if ( val & 0x20 )
			bol[i] = true;
		else
			bol[i] = false;
		val <<= 1;
	}
}




/******* һЩ��ɢ�ĺ��� ***************/

// ������������

void SetSound( const char* close1, const char* name, const char* type_name )
{
	char c1[20], close[20], open[90], play[20];

	if ( close1 != NULL )
	{
		sprintf( c1, "close %s", close1 );
		mciSendString( c1, 0, 0, 0 );
	}

	sprintf( close, "close %s", type_name );
	sprintf( open, "open ./res/Sounds/%s.wav alias %s", name, type_name );
	sprintf( play, "play %s", type_name );

	mciSendString( close, 0, 0, 0 );
	mciSendString( open, 0, 0, 0 );
	mciSendString( play, 0, 0, 0 );
}


// �����˵�ǰ�Ƿ�������ҹ���

bool Check_position( const int& e_r, const int& e_x, const int& p_x )
{
	switch ( e_r )
	{
		case LEFT:
			if ( e_x >= p_x )
				return true;
			else
				return false;

		case RIGHT:
			if ( e_x <= p_x )
				return true;
			else
				return false;
		default:	printf( "Check_position(..)����\n" );	break;
	}

	return true;
}

// �������IN-��  UNIN-δ��

bool Check_hole( PE kind, int id )
{
	switch ( kind )
	{
		case PE_player:
			if ( Check_xy(Player[id].Rp->x, Player[id].Rp->y ) == _IN )
				return _IN;
			else
				return _UNIN;

		case PE_emeny:
			if ( Check_xy(Emeny[id].Mp->Getx(), Emeny[id].Mp->Gety() ) == _IN )
				return _IN;
			else
				return _UNIN;

		default: printf( "Check_hole() ����\n" );	break;
	}
	return false;
}
//
bool Check_xy( int xx, int yy )
{
	int x = xx - Map.x[2];
	int y = yy;

	if ( y > 223 /*&& y < 293*/ )
	{
		if ( x > 583 && x < 810 )	return _IN;
		if ( x > 1099 && x < 1190 )	return _IN;
		if ( x > 1289 && x < 1390 )	return _IN;
	}
	return _UNIN;
}

//// ��⵱ǰλ���Ƿ���µ���

bool Check_lift( PE kind, int id )
{
	if ( Map.map_id != 3 || Map.part[Map.map_id] != 5 )
		return _UNIN;

	int x = 0;
	switch ( kind )
	{
		case PE_player:	x = Player[id].Rp->x;		break;
		case PE_emeny:	x = Emeny[id].Mp->Getx();	break;
		default: printf( "Check_lift()����\n" );	break;
	}

	if ( x < 35 || x > 370 )
		return _IN;
	else
		return _UNIN;
}

//
bool Check_lift_xy( const int& x )
{
	if ( Map.map_id != 3 || Map.part[Map.map_id] != 5 )
		return _UNIN;

	if ( x < 35 || x > 370 )
		return _IN;
	else
		return _UNIN;
}

//	ֻ�� WEAPON::out == false ���ܳ��ֵڶ�������������̬���ݽ��ᱻ����
void Weapon_Out( const int& _x, const int& _y, const int& id, const int& player )
{
	int kind = rand() % player;
	WEAPON::weapon_kind = Player[kind].role_kind;	// ���ָ���ҵ�����

	int r = Emeny[id].Mp->GetDir();
	int dx[2] = { 10, 20 };

	WEAPON::out = true;
	WEAPON::time = 0;
	WEAPON::X = _x + dx[r] - Map.x[Map.map_id];
	WEAPON::y = _y + 40;
}

/*** ���־�̬���ݳ�ʼ�� ***********/
IMAGE B_FIVE::attack[2][14][2];
IMAGE B_FIVE::bullet_away[3][2];

IMAGE Small_Monkey::walk[2][4][2];
IMAGE Small_Monkey::attack[2][2][2];
IMAGE Small_Monkey::kick_away[2][6][2];
IMAGE Small_Monkey::throw_away[2][5][2];
IMAGE Small_Monkey::be_catch[2][5][2];
int Small_Monkey::Hp[6][7] = {	{50, 55, 60, 65, 70, 75, 80},
								{55, 60, 65, 70, 75, 80, 85},
								{60, 65, 70, 75, 80, 85, 90},
								{65, 70, 75, 80, 85, 90, 95},
								{70, 75, 80, 85, 90, 95, 100},
								{75, 80, 85, 90, 95, 100, 105}};

IMAGE Small_Dai::walk[2][5][2];
IMAGE Small_Dai::attack[2][2][2];
IMAGE Small_Dai::kick_away[2][5][2];
IMAGE Small_Dai::throw_away[2][5][2];
IMAGE Small_Dai::be_catch[2][5][2];
int Small_Dai::Hp[6][7] = { {55, 60, 65, 70, 75,   80,  85},
							{60, 65, 70, 75, 80,   85,  90},
							{65, 70, 75, 80, 85,   90,  95},
							{70, 75, 80, 85, 90,   95, 100},
							{75, 80, 85, 90, 95,  100, 105},
							{80, 85, 90, 95, 100, 105, 110}};

IMAGE Big_Dai::walk[2][5][2];
IMAGE Big_Dai::attack[2][2][2];
IMAGE Big_Dai::kick_away[2][6][2];
IMAGE Big_Dai::throw_away[2][5][2];
IMAGE Big_Dai::be_catch[2][5][2];
int Big_Dai::Hp[6][7] = {	{ 40,  50,  60,  70,  80,  90, 100},
							{ 60,  70,  80,  90, 100, 110, 120},
							{ 80,  90, 100, 110, 120, 130, 140},
							{100, 110, 120, 130, 140, 150, 160},
							{120, 130, 140, 150, 160, 170, 180},
							{140, 150, 160, 170, 180, 190, 200}};

IMAGE Girl::walk[2][4][2];
IMAGE Girl::attack[2][8][2];
IMAGE Girl::kick_away[2][5][2];
IMAGE Girl::throw_away[2][5][2];
IMAGE Girl::be_catch[2][5][2];
int Girl::Hp[6][7] = {	{ 40,  50,  60,  70,  80,  90, 100},
						{ 50,  60,  70,  80,  90, 100, 110},
						{ 60,  70,  80,  90, 100, 110, 120},
						{ 70,  80,  90, 100, 110, 120, 130},
						{ 80,  90, 100, 110, 120, 130, 140},
						{ 90, 100, 110, 120, 130, 140, 150}};

IMAGE Thin_Old_Man::walk[2][5][2];
IMAGE Thin_Old_Man::attack[2][10][2];
IMAGE Thin_Old_Man::kick_away[2][5][2];
IMAGE Thin_Old_Man::throw_away[2][5][2];
IMAGE Thin_Old_Man::be_catch[2][5][2];
int Thin_Old_Man::Hp[6][7] = {	{ 40,  50,  60,  70,  80,  90, 100},
								{ 50,  60,  70,  80,  90, 100, 110},
								{ 60,  70,  80,  90, 100, 110, 120},
								{ 70,  80,  90, 100, 110, 120, 130},
								{ 80,  90, 100, 110, 120, 130, 140},
								{ 90, 100, 110, 120, 130, 140, 150}};

IMAGE Big_Savage::walk[2][5][2];
IMAGE Big_Savage::attack[2][4][2];
IMAGE Big_Savage::kick_away[2][5][2];
IMAGE Big_Savage::throw_away[2][5][2];
IMAGE Big_Savage::be_catch[2][5][2];
int Big_Savage::Hp[6][7] = {{ 90, 100, 110, 120, 130, 140, 150},
							{100, 110, 120, 130, 140, 150, 160},
							{110, 120, 130, 140, 150, 160, 170},
							{120, 130, 140, 150, 160, 170, 180},
							{130, 140, 150, 160, 170, 180, 190},
							{140, 150, 160, 170, 180, 190, 200}};

LINK* L::pn = new LINK;
LINK* L::pnem;
LINK* L::p_b = new LINK;
LINK* L::p_temp;

int BOSS::Boss_num = 1;
IMAGE BOSS::head[5];

int EMENY::emeny_num = 0;
IMAGE EMENY::head[6];
IMAGE EMENY::papaw[2];

int PLAYER::exp_lev[7] = { 50, 70, 90, 110, 130, 140, 150 };
int PLAYER::player_num = 1;
IMAGE PLAYER::light[2];
IMAGE PLAYER::blood[2][6][2];
IMAGE PLAYER::head[3];

bool LEVEL::next_lev	= true;
int LEVEL::walk_dt		= 0;
int LEVEL::a_level		= 0;
double LEVEL::run_fly[8] = { 1, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4 };
double LEVEL::wave[8] = { 1, 1.5, 2, 2.2, 2.5, 3, 3.2, 3.5 };
int LEVEL::harm[8] = { 8,8,9,9,10,10,11,11 };
void LEVEL::Level_Up()
{
	Map.map_id++;		///////////////////// ���ܻ�Խ�磡����
	a_level++;
	walk_dt++;
}

bool OTHER::emeny = false;
bool OTHER::boss = false;
IMAGE OTHER::lift_up_down[2];
int OTHER::y[3];

int WEAPON::X = 0;
int WEAPON::x = 0;
int WEAPON::y = 0;
bool WEAPON::out = false;
IMAGE WEAPON::weapon[3][2];
int WEAPON::weapon_kind = 0;
int WEAPON::time = 0;

bool FOOD::out = false;
IMAGE FOOD::img[2];
int FOOD::X = 0;
int FOOD::x = 0;
int FOOD::y = 0;
int FOOD::time = 0;