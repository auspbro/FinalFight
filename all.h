#pragma once

#ifndef STR_CLA_H
#define STR_CLA_H

#include "graphics.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define MAX_MAP	5				// ��ͼ��Ŀ
#define LEFT	0
#define RIGHT	1
#define UP		2
#define DOWN	3
#define R_U		4				// ����
#define R_D		5				// ����
#define L_U		6				// ����
#define L_D		7				// ����

#define MINX	-20				// ��һ���ҷ�Χ
#define MAXX	440

enum THROW_DIR { FORWARD = 0x101, 
				 BACK = 0x201 };		// ���˦�˷���

enum SKILL { LIGHT_WAVE,				// �Ⲩ
			 THUMP,						// ����
			 THROW_BACK,				// ��ˤ
			 NONE };					// ��

enum PE { PE_player, PE_emeny };		// ���� Check_hole() ����
#define _IN		true					// ����
#define _UNIN	false					// û��


/////// ��ͼ�� /////////////////////////////////////////////////////
class MAP
{
	int line_1[MAX_MAP][9];							// ÿ�ֵ�ͼ�˲��ֵ� 9 ���ֽ���

	int line_2[MAX_MAP][8];							// ÿ��������ķֽ���

	IMAGE img[MAX_MAP];								// ����ÿһ�ֵ�ͼ

	int width[MAX_MAP];								// ��ͼ�Ŀ�һ��

	int xx, yy;
	int num;										// �����ⲿ����


public:
	MAP();

	bool Set_Map();								// ���õ�ͼ�ı仯
	bool Move_map( bool check = false );		// �ƶ���ͼ���������ƶ�����
	bool Check( int );							// ��������ܷ��ƶ����ø߶�

	bool move;									// ��ͼ�ƶ�(�������ж�)
	bool die_out[MAX_MAP][8];					// ÿ�ֵ�ͼ�ĸ����ֵ����Ƿ����⣬�������һ����
	int map_id;									// ��ǰ��ͼ id�����غ�����
	int part[MAX_MAP];							// ÿ�ֵ�ͼ��ʾ�ڴ��ڵĵ�ǰ����(0-7)
	int max_y[MAX_MAP][8], min_y[MAX_MAP][8];	// ÿ�ֵ�ͼ�� 8 ����ͬ���ֿ����ƶ������¿��
	int x[MAX_MAP];								// ÿ�ֵ�ͼ�����Ͻ� x ����
	int y;										// Ϊ�����������ͼ��Ч��

}Map;


//////////////////////////////////////////////////////////////////////////////////
class Role
{
public:
	Role();
	virtual ~Role()	{}
	virtual bool Check_n_x	( int );											// �������Ƿ�����Ծ�����еȲ����˺���״̬
	virtual int  Getn_a()		{ return n_a + n_ja; }							// Kady�����Ǹú���
	virtual int  Get_wavex()	{ printf("Role����\n����");return 0; }
	virtual int  Get_wavey()	{ printf("Role����2\n����");return 0; }
	virtual IMAGE* GetImg( int, int, int ) { printf("Role���ഢ����3\n����");return 0; }
	virtual int Get_weapx()	{ MessageBox(NULL, "virtual int Get_weapx()����", "ds", MB_OK); return 0; }
	virtual int Get_weapy()	{ MessageBox(NULL, "virtual int Get_weapy()����", "ds", MB_OK); return 0; }

	virtual void Stand		( int dir, int player );					// ͨ��
	virtual void Walk		( int dir, int player ) = 0;				// ֻ��ָ������
	virtual void Walk_all	( int dir, int player, int* x, int* y );	// ����ͨ��
	virtual void Jump		( int dir, int player );					// ��ǿͨ��
	virtual void Attack		( int dir, int player );					// Jack ���಻��ʹ��
	virtual void Weapon_attack( int dir, int player ) = 0;				// ��������
	virtual void Big_Blow	( int dir, int player );					// Jack ���಻��ʹ��

	virtual void Light_wave	( int dir, int player ) = 0;				// ���߲���
	virtual void Dragon_fist( int dir, int player ) = 0;				// ���߲���
	virtual void Throw_role	( int dir, int player );					// Jack ����
	virtual void Jump_forward(int dir, int player );					// ��ǿͨ��
	virtual void Jump_attack( int dir, int player );					// ͨ��
	virtual void Jump_knee	( int dir, int player );					// Jack ����
	virtual void Knee_attack( int dir, int player );					// ͨ��
	virtual void Kick_down	( int dir, int player );					// ͨ��
	virtual void Revive		( int dir, int player );					// ͨ��
	virtual void Be_kiss	( int dir, int player );					// ͨ�ã������ر� boss kiss

	int x, y;
	int temp_y;
	byte Dir : 1;

	bool have_weapon;			// ��ǰ�Ƿ�������
	byte weapon_counter : 4;	// ���� 15 �κ�������ʧ

	int w, a, j, ja, bb, jk, jf, lw, kd, df, tr, cm, ka, r, bk;
	int n_w, n_a, n_j, n_ja, n_bb, n_jk, n_jf, n_lw, n_kd, n_df, n_tr, n_cm, n_ka, n_r, n_bk;

protected:
	int X, Y;
	int Drtx;
	bool big;
	int temp_x;
};

//����������������������������������������������������������������������������
class Kady : public Role
{
	int wave_x, wave_y;			// ��������Ͻ����꣬�踲�ǻ���ĺ�������ȡ

	IMAGE walk[2][4][2], attack[2][2][2], jump[2][3][2], jump_attack[2][4][2];
	IMAGE b_blow[2][4][2], jump_knee[2][3][2], jump_forward[2][7][2], light_wave[2][10][2];
	IMAGE kick_down[2][6][2], dragon_fist[2][8][2], throw_role[2][5][2], catch_attack[2][6][2];
	IMAGE knife[2][3][2];		// С��

public:
	Kady();
	bool shoot_knife;			// С�����������ù⣬�ͷ���С��

	int Get_wavex()	{ return wave_x; }
	int Get_wavey()	{ return wave_y; }

	IMAGE* GetImg( int dir, int n, int kind )	{ return &kick_down[dir][n][kind]; }

	void Stand		( int dir, int player );
	void Walk		( int dir, int player );
	void Jump		( int dir, int player );
	void Attack		( int dir, int player );
	void Weapon_attack( int dir, int player );
	void Big_Blow	( int dir, int player );

	void Light_wave	( int dir, int player );
	void Dragon_fist( int dir, int player );
	void Throw_role	( int dir, int player );
	void Jump_forward(int dir, int player );
	void Jump_attack( int dir, int player );
	void Jump_knee	( int dir, int player );
	void Knee_attack( int dir, int player );
	void Kick_down	( int dir, int player );
	void Revive		( int dir, int player );
	void Be_kiss	( int dir, int player );
};

//������������������������������������������������������������������������������������
class Heye : public Role
{
	IMAGE walk[2][4][2], attack[2][7][2], jump[2][3][2], jump_attack[2][3][2];
	IMAGE b_blow[2][4][2], jump_knee[2][3][2], jump_forward[2][7][2], light_wave[2][7][2];
	IMAGE kick_down[2][6][2], dragon_fist[2][7][2], throw_role[2][7][2], catch_attack[2][6][2];
	IMAGE weapon[2][3][2];		// ���ڹ���

public:
	Heye();

	int weapx, weapy;			// ��������

	int Getn_a()		{ return (n_a + n_lw + n_ja); }
	int Get_weapx()		{ return weapx; }
	int Get_weapy()		{ return weapy; }
	IMAGE* GetImg( int dir, int n, int kind )	{ return &kick_down[dir][n][kind]; }

	void Stand		( int dir, int player );
	void Walk		( int dir, int player );
	void Jump		( int dir, int player );
	void Attack		( int dir, int player );
	void Weapon_attack( int dir, int player );
	void Big_Blow	( int dir, int player );

	void Light_wave	( int dir, int player );	// ������
	void Dragon_fist( int dir, int player );	// ������
	void Throw_role	( int dir, int player );
	void Jump_forward(int dir, int player );
	void Jump_attack( int dir, int player );
	void Jump_knee	( int dir, int player );
	void Knee_attack( int dir, int player );
	void Kick_down	( int dir, int player );
	void Revive		( int dir, int player );
	void Be_kiss	( int dir, int player );
};

//������������������������������������������������������������������������������������
class Jack : public Role
{
	bool thump;				// ����--�ػ�

	IMAGE walk[2][4][2], attack[2][3][2], jump[2][3][2], jump_attack[2][3][2];
	IMAGE b_blow[2][4][2], jump_knee[2][3][2], jump_forward[2][3][2], light_wave[2][2][2];
	IMAGE kick_down[2][6][2], dragon_fist[2][5][2], throw_role[2][4][2], catch_attack[2][6][2];
	IMAGE hammer[2][3][2];	// ���ӹ���

public:
	Jack();

	int Getn_a()		{ return (n_a + n_lw + n_ja); }
	IMAGE* GetImg( int dir, int n, int kind )	{ return &kick_down[dir][n][kind]; }

	void Stand		( int dir, int player );
	void Walk		( int dir, int player );
	void Jump		( int dir, int player );
	void Attack		( int dir, int player );
	void Weapon_attack( int dir, int player );
	void Big_Blow	( int dir, int player );

	void Light_wave	( int dir, int player );	// ����
	void Dragon_fist( int dir, int player );	// ˫�Ź���
	void Throw_role	( int dir, int player );	// ��ˤ
	void Jump_forward(int dir, int player );
	void Jump_attack( int dir, int player );
	void Jump_knee	( int dir, int player );	// ̩ɽѹ��
	void Knee_attack( int dir, int player );	// ��ͷ����
	void Kick_down	( int dir, int player );
	void Revive		( int dir, int player );
	void Be_kiss	( int dir, int player );
};


/******************** ������ *******************/
#define EMENY_NUM	6		// ��ͼÿ������ 6 ��

////////////// ������� /////////////////////////////////////////
class BASE_CLASS
{
public:
	virtual ~BASE_CLASS()	{}

	virtual int  Getx	()		= 0;
	virtual int  Gety	()		= 0;
	virtual void Setxy( int _x, int _y )	= 0;
	virtual int  GetDir	()		= 0;
	virtual void SetDir	( int )	= 0;
	virtual void SetHp	( int& )	= 0;
	virtual void Out_Put( int id )  = 0;

	virtual void Stand		( int id, const int& player ) = 0;		// ż����ֹ�������ϥ�ǻ���Ҳ��ֹ
	virtual void Walk		( int id, const int& player ) = 0;
	virtual void Attack		( int id, const int& player ) = 0;
	virtual void Kick_away	( int id, const int& player ) = 0;		// ������
	virtual void Throw_away	( int id, const int& player ) = 0;		// ��˦��
	virtual void Be_catch	( int id, const int& player ) = 0;		// �����ץס
	virtual void Die		( int id, const int& player ) = 0;		// ���˱�����
};

////////// С������� /////////////////////////////////////////////
class Small_Monkey : public BASE_CLASS
{
	int X, Y;				// �����������߶���
	int temp_x;				// ����Խ�����
	int Drtx[2];
	int attack_level[6];	// 6 ����ͼ����Ĺ������ʵ� ��ĸ, rand() % ��ĸ��

	int x, y, temp_y;		// img ���Ͻ�����
	int dx1, dy1;			// ��б�˶�����
	int dx2;				// ˮƽ�˶��������� y
	int step;				// ��ǰ�ƶ��Ĳ���
	int role;				// ��ú���ҿ���
	double _a;				// �������������ڵ�����
	byte Dir : 1;			// ���ҷ���

	bool start;				// ��ʼ�µ�һ���˹�
	bool leave;				// Զ�����

	int w, a, ka, ta, s, bc, d;
	int n_w, n_a, n_ka, n_ta, n_s, n_bc, n_d;

	DWORD w_t1, w_t2;		// �����ƶ��ٶ�
	DWORD w_dt[6];

	static IMAGE walk[2][4][2], attack[2][2][2], kick_away[2][6][2], throw_away[2][5][2], be_catch[2][5][2];
	static int Hp[6][7];

public:
	Small_Monkey();
	int  Getx	()			{ return x;		}
	int  Gety	()			{ return y;		}
	int	 GetDir	()			{ return Dir;	}
	void Setxy( int _x, int _y )	{ x = _x; y = _y; }
	void SetDir	( int dir )	{ Dir = dir;	};
	void SetHp	( int& hp );
	void Out_Put(int id){ UNREFERENCED_PARAMETER(id); }

	void Stand		( int id, const int& player );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Be_catch	( int id, const int& player );
	void Die		( int id, const int& player );

	static void Load();
};

/////// С�� ///////////////////////
class Small_Dai : public BASE_CLASS
{
	int X, Y;		// �����������߶���
	int temp_x;		// ����Խ�����
	int Drtx[2];
	int attack_level[6];

	int x, y, temp_y;
	int dx1, dy1;
	int dx2;
	int step;
	int role;
	double _a;
	byte Dir : 1;

	bool start;
	bool leave;

	int w, a, ka, ta, s, bc, d;
	int n_w, n_a, n_ka, n_ta, n_s, n_bc, n_d;
	DWORD w_t1, w_t2;
	DWORD w_dt[6];

	static IMAGE walk[2][5][2], attack[2][2][2], kick_away[2][5][2], throw_away[2][5][2], be_catch[2][5][2];
	static int Hp[6][7];

public:
	Small_Dai();
	int Getx()	{ return x; }
	int Gety()	{ return y; }
	int GetDir()	{ return Dir;	}
	void Setxy( int _x, int _y )	{ x = _x; y = _y; }
	void SetDir( int dir )	{ Dir = dir; };
	void SetHp	( int& hp );
	void Out_Put(int id){ UNREFERENCED_PARAMETER(id); }

	void Stand		( int id, const int& player );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Be_catch	( int id, const int& player );
	void Die		( int id, const int& player );

	static void Load();
};

////// ��� ///////////////////////
class Big_Dai : public BASE_CLASS
{
	int X, Y;		// �����������߶���
	int temp_x;		// ����Խ�����
	int Drtx[2];
	int attack_level[6];

	int x, y, temp_y;
	int dx1, dy1;
	int dx2;
	int step;
	int role;
	double _a;
	byte Dir : 1;

	bool start;
	bool leave;
	bool resist;		// �ֵ�

	int w, a, ka, ta, s, bc, d;
	int n_w, n_a, n_ka, n_ta, n_s, n_bc, n_d;
	DWORD w_t1, w_t2;
	DWORD w_dt[6];

	static IMAGE walk[2][5][2], attack[2][2][2], kick_away[2][6][2], throw_away[2][5][2], be_catch[2][5][2];
	static int Hp[6][7];

public:
	Big_Dai();
	int Getx()		{ return x;		}
	int Gety()		{ return y;		}
	int GetDir()	{ return Dir;	}
	void Setxy( int _x, int _y )	{ x = _x; y = _y; }
	void SetDir( int dir )	{ Dir = dir; };
	void SetHp	( int& hp );
	void Out_Put(int id){ UNREFERENCED_PARAMETER(id); }

	void Stand		( int id, const int& player );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Be_catch	( int id, const int& player );
	void Die		( int id, const int& player );

	static void Load();
};

//// ���� //////////////////////////////
class Girl : public BASE_CLASS
{
	int X, Y, Drtx[2], temp_x;
	int x, y, temp_y;
	int dx1, dy1;
	int dx2;
	int attack_level[6];

	int step;
	int role;
	double _a;
	byte Dir : 1;
	byte attack_kind : 1;	// ������ʽ

	bool start;
	bool leave;

	int w, a, ka, ta, s, bc, d;
	int n_w, n_a, n_ka, n_ta, n_s, n_bc, n_d;
	DWORD w_t1, w_t2;
	DWORD w_dt[6];

	static IMAGE walk[2][4][2], attack[2][8][2], kick_away[2][5][2], throw_away[2][5][2], be_catch[2][5][2];
	static int Hp[6][7];

public:
	Girl();
	int Getx()		{ return x;		}
	int Gety()		{ return y;		}
	int GetDir()	{ return Dir;	}
	void Setxy( int _x, int _y )	{ x = _x; y = _y; }
	void SetDir( int dir )	{ Dir = dir; };
	void SetHp	( int& hp );
	void Out_Put(int id){ UNREFERENCED_PARAMETER(id); }

	void Stand		( int id, const int& player );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Be_catch	( int id, const int& player );
	void Die		( int id, const int& player );

	static void Load();
};

//// �C��ͷ ///////////////////////
class Thin_Old_Man : public BASE_CLASS
{
	int x, y;
	int temp_y;					// ���� y ֵ��������Ծ���ʵ��˶�
	int knife_x, knife_y;		// �ɵ�����
	int k_dir;					// �ɵ�����

	int X, Y;
	int temp_x;					// ����Խ�����
	int dx1, dy1;
	int dx2;
	int Drtx[2];
	int attack_level[6];
	int step;
	int role;
	double _a;
	byte Dir : 1;
	int attack_kind;	

	bool start;
	bool leave;
	bool turn_dir;				// �ɵ�����һ��з��򣬿ɹ�������

	int w, a, ka, ta, s, bc, d;
	int n_w, n_a, n_ka, n_ta, n_s, n_bc, n_d;
	DWORD w_t1, w_t2;
	DWORD w_dt[6];

	static IMAGE walk[2][5][2], attack[2][10][2], kick_away[2][5][2], throw_away[2][5][2], be_catch[2][5][2];
	static int Hp[6][7];

public:
	Thin_Old_Man();
	int Getx()		{ return x;		}
	int Gety()		{ return y;		}
	int GetDir()	{ return Dir;	}
	void Setxy( int _x, int _y )	{ x = _x; y = _y; }
	void SetDir( int dir )	{ Dir = dir; };
	void SetHp	( int& hp );
	void Out_Put(int id);

	void Stand		( int id, const int& player );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Be_catch	( int id, const int& player );
	void Die		( int id, const int& player );

	static void Load();
};

///// �����Ұ�� //////////////////////////
class Big_Savage : public BASE_CLASS
{
	int X, Y, temp_x;
	int x, y, temp_y;
	int dx1, dy1;
	int dx2;

	int Drtx[2];
	int attack_level[6];
	int step;
	int role;
	double _a;
	byte Dir : 1;
	byte attack_kind : 1;

	bool start;
	bool leave;

	int w, a, ka, ta, s, bc, d;
	int n_w, n_a, n_ka, n_ta, n_s, n_bc, n_d;
	DWORD w_t1, w_t2;
	DWORD w_dt[6];

	static IMAGE walk[2][5][2], attack[2][4][2], kick_away[2][5][2], throw_away[2][5][2], be_catch[2][5][2];
	static int Hp[6][7];

public:
	Big_Savage();
	int Getx()		{ return x;		}
	int Gety()		{ return y;		}
	int GetDir()	{ return Dir;	}
	void Setxy( int _x, int _y )	{ x = _x; y = _y; }
	void SetDir( int dir )	{ Dir = dir; }
	void SetHp	( int& hp );
	void Out_Put(int id){ UNREFERENCED_PARAMETER(id); }

	void Stand		( int id, const int& player );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Be_catch	( int id, const int& player );
	void Die		( int id, const int& player );

	static void Load();
};


/************* boss �� **********************/
#define BOSS_NUM	2			// boss ����

class BASE_BOSS
{
public:
	BASE_BOSS( int* p, RECT* rec );								// ʹ��һ�����������ʼ������ boss
	virtual ~BASE_BOSS		(							)	{}
	virtual void Walk		( int id, const int& player );		// ����ɹ���
	virtual void Attack		( int id, const int& player ) = 0;	// ���ɹ���
	virtual void Kick_away	( int id, const int& player );		// ͨ��
	virtual void Throw_away	( int id, const int& player );		// ͨ��
	virtual void Throw_back	( int id, const int& player );		// ͨ�ã����ӱ�ˤ
	virtual void Die		( int id, const int& player );		// ͨ��

	int x, y;
	int temp_y;
	byte Dir : 1;			// boss ���泯����·ʱ��Զ�������

protected:					// �����Ϳɱ������������Ա����ֱ�ӷ���
	int X, Y;
	int Drtx[2];
	int temp_x;

	int a_kind;				// boss ��������

	int w, a, ka, ta, tb, d;
	int n_w, n_a, n_ka, n_ta, n_tb, n_d;

	bool start;				// ��ʼ��һ��׷��
	RECT* rec;				// boss �˶���Χ
	byte move_dir : 2;		// boss �˶�����
	DWORD w_t1, w_t2, w_dt;
};

//
class B_ONE : public BASE_BOSS
{
	IMAGE walk[2][4][2], attack[2][6][2], kick_away[2][5][2], throw_away[2][5][2];//, throw_back[2][5][2];

public:
	B_ONE( int* p, RECT* rec );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Throw_back	( int id, const int& player );
	void Die		( int id, const int& player );
};

//
class B_TWO : public BASE_BOSS
{
	IMAGE walk[2][4][2], attack[2][3][2], kick_away[2][5][2], throw_away[2][5][2];

public:
	B_TWO( int* p, RECT* rec );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Throw_back	( int id, const int& player );
	void Die		( int id, const int& player );
};

//
class B_THREE : public BASE_BOSS
{
	bool kiss;		// ��ǰ�Ƿ� kiss �����
	bool leave;
	double _a;
	int dx1, dx2, dy1;
	int step;
	IMAGE walk[2][4][2], attack[2][12][2], kick_away[2][5][2], throw_away[2][5][2];

public:
	B_THREE( int* p, RECT* rec );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Throw_back	( int id, const int& player );
	void Die		( int id, const int& player );
};

//
class B_FOUR : public BASE_BOSS
{
	double _a;
	// �� boss1 ����ͼƬ
	IMAGE walk[2][4][2], attack[2][9][2], kick_away[2][5][2], throw_away[2][5][2];

public:
	B_FOUR( int* p, RECT* rec );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Throw_back	( int id, const int& player );
	void Die		( int id, const int& player );
};

//
class B_FIVE : public BASE_BOSS
{
	IMAGE walk[2][4][2], kick_away[2][5][2], throw_away[2][5][2];
public:
	static IMAGE attack[2][14][2];		// �� LINK ��� PutImg() ʹ��
	static IMAGE bullet_away[3][2];		// �ӵ�������

public:
	B_FIVE( int* p, RECT* rec );
	void Walk		( int id, const int& player );
	void Attack		( int id, const int& player );
	void Kick_away	( int id, const int& player );
	void Throw_away	( int id, const int& player );
	void Throw_back	( int id, const int& player );
	void Die		( int id, const int& player );
};



////
////	����
////
//// ���һ�� boss ����ȭͷ��������

class LINK
{
public:
	LINK( int _x = 0, int _y = 0, byte r = LEFT ):
	  x(_x),y(_y),n_a(0),a(10),dir(r)
	  {}
	  LINK( bool flag, int xx=0, int yy=0, byte r=LEFT ):
	  b_x(xx), b_y(yy), b_r(r), n_a(0), a(0)
	  { UNREFERENCED_PARAMETER(flag); }

	void PutImg();
	void Kick_down();

private:
	int b_x, b_y;			// ȭͷ������
	int x, y;				// ��ȭͼƬ���Ͻ�����
	int n_a, a;				// a Ϊ IMAGE �±�
public:
	LINK* pP;				// ָ����һ������
	LINK* p_B;
	byte dir : 1;
	byte b_r : 1;			// ȭͷ�����ɷ���

	int X, Y;
};

struct L
{
	static LINK* pn;
	static LINK* pnem;		// ������������

	static LINK* p_b;
	static LINK* p_temp;
};


/************ BOSS �ṹ ***************/

struct BOSS
{
	byte role;
	BASE_BOSS* Bp;
	IMAGE* ph;			//

	int x, y;			//
	int hp;
	int max_hp;			//
	int be_kick_counter;

	bool die;			//
	bool appear;		//
	bool start_fly;
	bool lying;

	static IMAGE head[5];	//	ò���� 7 �� boss
	static int Boss_num;

}Boss[BOSS_NUM];

// ���˽ṹ�� ///////////////////////////////////////
struct EMENY
{
	BASE_CLASS* Mp;			// ��������
	byte role : 1;			// ��ǰ���˹��������
	IMAGE* phead;			// ָ����˵�ͷ��

	int x, y;				// ͷ�����Ͻ�����
	static IMAGE head[6];	// ����ͷ��
	static IMAGE papaw[2];		// ������
	static int emeny_num;	// �������;

	bool stand;				// ���ڱ�Ǳ������ϥ������ֹ״̬
	bool attack;
	bool lying;				// ��ǰ�����Ƿ����ţ������˺�
	bool be_catch;			// �Ƿ�ץס
	bool start_fly;			// ���˿�ʼ���ű����ɻ���
	bool appear[8];			// �õ����ڵ�ͼÿ�����Ƿ����

	int live[MAX_MAP][7];	// �õ����ڵ�ͼÿ���е����� ���ֵ 2 ��������
	int be_kick_counter;	// ��¼�����д������� 4 �α�����

	int hp;					// ���˵�ǰѪ��
	int max_hp;				// ���ڼ�¼��ǰ��Ѫ��

}Emeny[EMENY_NUM];

////////////////////////////////////////
struct PLAYER
{
	Role* Rp;						// �����ѡ�Ľ�ɫ
	THROW_DIR throw_dir;			// ���˦�˷���
	SKILL skill;					// ���ڼ�⵱ǰ����ͷź��ּ���
	IMAGE* ph;						// ָ�����ͷ��
	byte level : 3;					// ��Ҽ������ 7 ��

	bool be_kiss;					// ���� boss �������ͷ����� kiss �����
	bool lying;						// ���� boss �жϹ�����Ч���
	bool appear;					// ��������ù�󲻳���
	bool die;						// ����Ƿ�������
	bool catch_emeny;				// �Ƿ�ץ�ŵ���
	bool bol[6];					// ���� 6 ��������ǰ�ܷ��� 0~5 ���ζ�Ӧ����/��/��/�£���/��
	void Set_bol( int val );		// ���� 6 ������

	int x, y;						// ͷ������
	int dir;						// ������ҵ�ǰ����
	int be_kick_counter;			// ��¼������������3 ��Ϊһѭ��(�����)
	int emeny;						// ��Ұ󶨵ĵ���
	int boss;						// ���� Be_kiss() ����
	int role_kind;					// ��¼���ѡ��Ľ�ɫ

	int experience;					// ��ҵ�ǰ����
	int hp;							// Ѫ��
	int max_hp;						// ���ڻ�Ѫ��
	int life;						// �������

	static int exp_lev[7];			// ÿ���������辭��
	static int player_num;			// ��ǰ������� 1 - 2
	static IMAGE light[2];			// ��������Ч��
	static IMAGE blood[2][6][2];	// ������ѪЧ��
	static IMAGE head[3];			// ���ͷ��

}Player[2];

// ��Ϸ������Ϣ�ṹ
struct LEVEL
{
	static bool next_lev;			// ���Ž�����һ�صĻ���
	static double run_fly[8];		// jack �˸����˼���ӳ�
	static double wave[8];			// kady �˸����������ӳ�
	static int a_level;				// ���˹�������
	static int walk_dt;				// �����ƶ����ʱ��
	static int harm[8];				// ��ҹ���������ֵ�����˹�����������,

	static void Level_Up();
};

// ����Ч��
struct OTHER
{
	static bool emeny;					// true ���˵� y > ��ҵ� y�� ��ס��ң�false �����ס����
	static bool boss;
	static IMAGE lift_up_down[2];		// �������������ƶ�Ч��
	static int y[3];					// ���ݷ������Ͻ� y ����
};

// ���ӣ�С��������
struct WEAPON
{
	static bool out;			// ��ǰ��ͼ�Ƿ���ִ��ӣ�������������Ƿ��ô���
	static int X;				// ���ӳ���ʱ����� Map.x[Map.map_id] �����꣬���ڱ������������ͼ�˶� X = Emeny[i].x - Map.x[...]
	static int x, y;			// �������꣬x = X + Map.x[..]
	static IMAGE weapon[3][2];	// һ��Ϊ��С�������ڡ�����,��Ӧ PLAYER �ṹ�� role_kind
	static int weapon_kind;		// �������࣬��һ�ȡʱ role_kind == weapon ���ܻ�ȡ
	static int time;			// �����Ѿ����ֵ�ʱ�䣬һ��ʱ�����ʧ
};

// ����
struct FOOD
{
	static bool out;
	static IMAGE img[2];
	static int X;
	static int x, y;
	static int time;
};



/********* һЩ��ɢ���� ********/

void SetSound( const char*, const char*, const char* );						// ��������
bool Check_position( const int& e_dir, const int& e_x, const int& p_x );	// ����������˵ķ�λ
bool Check_hole( PE kind, int id );											// �����ҡ������Ƿ���������صĶ���
bool Check_xy( int xx, int yy );											// ������һ������
bool Check_lift( PE kind, int id );											// ��⵱ǰλ���Ƿ���µ���
bool Check_lift_xy( const int& x );											// ��һ�ּ�⣬���ڏC��ͷ���ع���������
void Weapon_Out( const int&, const int&, const int& id, const int& player );// �����������������Ҷ�Ӧ������
int Revise_xy( int id, const int& x, const int& y, int& dx, int& dy );		// ���ڵ����ص� �ӣ��޸ĵ����߽������λ����
void Revise_dispatch( int& );												// �������ݵ��˳��ֵ� x λ��
bool Revise_lift(const int&, const int& );									// ���������е��˶�����
int SetDir( const int& b_x, const int& p_x );								// �ж� boss �� player �� x ���귽λ������ boss ����( LEFT/RIGHT )
int Change_move_dir( const int& e_x, const int& e_y );						// boss �����߽�����»�ȡ�ƶ�����
bool Check_kiss_x( const int& x, const int& lef, const int& rig );			// ��� boss ������Χ�Ƿ�� kiss ���Խ��
void Dispatch_five_boss( int part );										// ��������ص�ǰ���� boss


#endif