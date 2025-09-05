#pragma once

#include<memory.h>

#define _DEF_BUFFER         (4096)
#define _DEF_CONTENT_SIZE	(1024)
#define _MAX_SIZE           (40)
#define _MAX_PATH           (260)

//自定义协议   先写协议头 再写协议结构
//登录 注册 获取好友信息 添加好友 聊天 发文件 下线请求
#define _DEF_PACK_BASE	(10000)
#define _DEF_PACK_COUNT (100)

//注册
#define _DEF_PACK_REGISTER_RQ	(_DEF_PACK_BASE + 0 )
#define _DEF_PACK_REGISTER_RS	(_DEF_PACK_BASE + 1 )
//登录
#define _DEF_PACK_LOGIN_RQ	(_DEF_PACK_BASE + 2 )
#define _DEF_PACK_LOGIN_RS	(_DEF_PACK_BASE + 3 )



//返回的结果
//注册请求的结果
#define tel_is_exist		(0)
#define register_success	(1)
#define name_is_exist       (2)
//登录请求的结果
#define user_not_exist		(0)
#define password_error		(1)
#define login_success		(2)



typedef int PackType;

//协议结构
//注册
typedef struct STRU_REGISTER_RQ
{
	STRU_REGISTER_RQ():type(_DEF_PACK_REGISTER_RQ)
	{
		memset( tel  , 0, sizeof(tel));
		memset( name  , 0, sizeof(name));
		memset( password , 0, sizeof(password) );
	}
	//需要手机号码 , 密码, 昵称
	PackType type;
	char tel[_MAX_SIZE];
	char name[_MAX_SIZE];
	char password[_MAX_SIZE];

}STRU_REGISTER_RQ;

typedef struct STRU_REGISTER_RS
{
	//回复结果
	STRU_REGISTER_RS(): type(_DEF_PACK_REGISTER_RS) , result(register_success)
	{
	}
	PackType type;
	int result;

}STRU_REGISTER_RS;

//登录
typedef struct STRU_LOGIN_RQ
{
	//登录需要: 手机号 密码 
	STRU_LOGIN_RQ():type(_DEF_PACK_LOGIN_RQ)
	{
		memset( tel , 0, sizeof(tel) );
		memset( password , 0, sizeof(password) );
	}
	PackType type;
	char tel[_MAX_SIZE];
	char password[_MAX_SIZE];

}STRU_LOGIN_RQ;

typedef struct STRU_LOGIN_RS
{
	// 需要 结果 , 用户的id
	STRU_LOGIN_RS(): type(_DEF_PACK_LOGIN_RS) , result(login_success),userid(0)
	{
        memset(name , 0 , sizeof(name));
	}
	PackType type;
	int result;
	int userid;
    char name[_MAX_SIZE];

}STRU_LOGIN_RS;



/////////////////////////   游戏专区    ////////////////////////////////////////////////////////////////////////////


#define DEF_PACK_JOIN_ZONE  (_DEF_PACK_BASE + 4 )
#define DEF_PACK_LEAVE_ZONE (_DEF_PACK_BASE + 5 )

enum ENUM_PLAY_ZONE{ Five_In_Line = 0x10, E_L_S, D_Z };
//加入专区
struct STRU_JOIN_ZONE//解决这是什么包,谁加入哪个专区
{
    STRU_JOIN_ZONE():type(DEF_PACK_JOIN_ZONE),userid(0),zoneid(0){

    }
    PackType type;
    int userid;
    int zoneid;
};
//退出专区
struct STRU_LEAVE_ZONE//解决这是什么包,谁退出专区
{
    STRU_LEAVE_ZONE():type(DEF_PACK_LEAVE_ZONE),userid(0){

    }
    PackType type;
    int userid;
};

//专区内每个房间人数
#define DEF_ZONE_ROOM_INFO  (_DEF_PACK_BASE + 10 )
#define DEF_ZONE_INFO_RQ    (_DEF_PACK_BASE + 11 )

#define DEF_ZONE_ROOM_COUNT 121
struct STRU_ZONE_INFO_RQ//解决这是是什么包,请求专区每个房间人数
{
    STRU_ZONE_INFO_RQ():type(DEF_ZONE_INFO_RQ),zoonid(0){
    }
    PackType type;
    int zoonid;
};

struct STRU_ZONE_ROOM_INFO//解决这是是什么包,谁加入哪个房间
{
    STRU_ZONE_ROOM_INFO():type(DEF_ZONE_ROOM_INFO),zoonid(0){
        memset( roomInfo , 0 , sizeof(roomInfo));
    }
    PackType type;
    int zoonid;
    int roomInfo[DEF_ZONE_ROOM_COUNT];
};


#define DEF_JOIN_ROOM_RQ  (_DEF_PACK_BASE + 6 )
//加入房间
struct STRU_JOIN_ROOM_RQ//解决这是是什么包,谁加入哪个房间
{
    STRU_JOIN_ROOM_RQ():type(DEF_JOIN_ROOM_RQ),userid(0),roomid(0){

    }
    PackType type;
    int userid;
    int roomid;
};
//发给服务器，服务器会同步房间成员信息
//避免0出现歧义,(房间号是0,还是未初始化)(把0让出来,1-120,一共121个元素)



#define DEF_JOIN_ROOM_RS (_DEF_PACK_BASE + 7 )
enum ENUM_ROOM_STATUS{ _host, _player, _watcher }; //房主 玩家 观察者
struct STRU_JOIN_ROOM_RS//解决这是什么包, 谁加入哪个房间 是否成功,自己什么身份
{
    STRU_JOIN_ROOM_RS():type(DEF_JOIN_ROOM_RS),userid(0),roomid(0),status(_host),result(1){

    }
    PackType type;
    int userid;
    int roomid;
    int status;
    int result; // 0 --fail  1--success
};




#define DEF_ROOM_MEMBER (_DEF_PACK_BASE + 8 )
//房间成员
struct STRU_ROOM_MEMBER//解决这是什么包, 谁,哪个房间, 叫什么名字
{
    STRU_ROOM_MEMBER():type(DEF_ROOM_MEMBER),userid(0),status(_player){
        memset(name,0,sizeof(name));
    }
    PackType type;
    int userid;
    int status;  // 房间内的身份
    char name[_MAX_PATH];
};

#define DEF_LEAVE_ROOM_RQ  (_DEF_PACK_BASE + 9 )
//退出房间
struct STRU_LEAVE_ROOM_RQ//解决这是什么包,谁, 退出了房间
{
    STRU_LEAVE_ROOM_RQ():type(DEF_LEAVE_ROOM_RQ),userid(0),
        roomid(0),status(_player){

    }
    PackType type;
    int userid;
    int roomid;
    int status;
};
//--会被转发, 如果自己不是房主,房主退出,自己也跟着退出

#define DEF_FIL_ROOM_READY      (_DEF_PACK_BASE+12)
#define DEF_FIL_GAME_START      (_DEF_PACK_BASE+13)
#define DEF_FIL_AI_BEGIN        (_DEF_PACK_BASE+14)
#define DEF_FIL_AI_END          (_DEF_PACK_BASE+15)
#define DEF_FIL_DISCARD         (_DEF_PACK_BASE+16)
#define DEF_FIL_SURREND         (_DEF_PACK_BASE+17)
#define DEF_FIL_PIECEDOWN       (_DEF_PACK_BASE+18)
#define DEF_FIL_WIN             (_DEF_PACK_BASE+19)
#define DEF_LD_BOARD            (_DEF_PACK_BASE+20)

struct STRU_FIL_RQ
{
    STRU_FIL_RQ(PackType _type):type(_type),userid(0),zoneid(0),roomid(0){

    }
    PackType type;//准备 开始 胜利 托管 弃权(一次) 投降 复用
    int userid;
    int zoneid;
    int roomid;
};//只有知道 什么专区 什么房间 才能找到响应的人

//采用另一种方式 利用客户端知道 准备的个数以及开放开始

struct STRU_FIL_RS
{
    PackType type; // allready 房主可以开始了
    int userid;
    int zoneid;
    int roomid;
};

//落子
struct STRU_FIL_PIECEDOWN//什么专区的什么房间 谁在x,y位置放了一个什么子
{
    STRU_FIL_PIECEDOWN():type(DEF_FIL_PIECEDOWN),userid(0),zoneid(0),roomid(0),color(0),x(-1),y(-1){

    }
    PackType type;//落子
    int userid;
    int zoneid;
    int roomid;
    int color;
    int x;
    int y;
};

//获胜
struct STRU_Win_FIL_RQ
{
    STRU_Win_FIL_RQ():type(DEF_FIL_WIN),userid(0),zoneid(0),roomid(0),winid(0),score(0){

    }
    PackType type;
    int winid;
    int userid;
    int zoneid;
    int roomid;
    int score;
};//只有知道 什么专区 什么房间 才能找到响应的人

// 玩家结构体，包含分数
typedef struct CPlayer {
    int id;
    int score; // 用于排序的分数
    char name[_MAX_SIZE];// 可以添加其他玩家信息
}CPlayer;

//排行榜请求
struct STRU_GET_LD_BOARD
{
    STRU_GET_LD_BOARD():type(DEF_LD_BOARD){

    }
    PackType type;
    int userid;
};

//排行榜回复
struct STRU_LD_BOARD
{
    STRU_LD_BOARD():type(DEF_LD_BOARD){

    }
    PackType type;
    int size;
    CPlayer player[10];
};

