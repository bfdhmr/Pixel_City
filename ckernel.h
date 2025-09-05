#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>

#include"INetMediator.h"
#include"packdef.h"
#include<vector>
#include"maindialog.h"
#include"logindialog.h"
#include"fiveinlinezone.h"
#include"roomdialog.h"
#include"leaderboard_show.h"


//成员函数指针类型
class CKernel;
typedef void (CKernel::*PFUN)( unsigned int lSendIP , char* buf , int nlen );



//单例--静态单例
class CKernel : public QObject
{
    Q_OBJECT
public:
    static CKernel* GetInstance(){
         static CKernel kernel;
         return &kernel;
    }

public slots:
    //调用回收
    void DestroyInstance();

    void SendDate(char* buf,int nLen);

    //窗口处理
    void slot_loginCommit(QString tel,QString password);
    void slot_registerCommit(QString tel,QString password,QString name);
    void slot_joinZone(int zoneid);
    void slot_leaveZone();
    void slot_joinRoom(int roomid);

    //网络处理
    void slot_ReadyData( unsigned int lSendIP , char* buf , int nlen );
    void slot_dealLoginRS( unsigned int lSendIP , char* buf , int nlen );
    void slot_dealRegistRS(unsigned int lSendIP, char *buf, int nlen);
    void slot_dealJoinRoomRS(unsigned int lSendIP, char *buf, int nlen);
    void slot_dealRoomMemberRq(unsigned int lSendIP, char *buf, int nlen);
    void slot_leaveRoom();
    void slot_dealLeaveRoomRq(unsigned int lSendIP, char *buf, int nlen);

    void slot_dealFilGameReadyRq( unsigned int lSendIP , char* buf , int nlen );
    void slot_dealFilGameStartRq( unsigned int lSendIP , char* buf , int nlen );
    void slot_dealFilPieceDownRq( unsigned int lSendIP , char* buf , int nlen );
    void slot_dealFilWinRq( unsigned int lSendIP , char* buf , int nlen );

    //准备和开局
    void slot_fil_gameReady(int zoneid,int roomid,int userid);
    void slot_fil_gameStart(int zoneid,int roomid);
    void slot_fil_pieceDown(int blackorwhite,int x,int y);

    void slot_fil_win(int blackorwhite);
    //托管
    void slot_fil_playByCpuBegin(int zoneid,int roomid,int userid);
    void slot_fil_playByCpuEnd(int zoneid,int roomid,int userid);

    void slot_dealFilGameByCpuBegin(unsigned int lSendIP, char *buf, int nlen);
    void slot_dealFilGameByCpuEnd(unsigned int lSendIP, char *buf, int nlen);

    //排行榜
    void slot_getldboard();
    void slot_dealldboard(unsigned int lSendIP, char *buf, int nlen);
signals:

private:
    void setNetPackFunMap();

    void ConfigSet();

    explicit CKernel(QObject *parent = nullptr);
    ~CKernel(){/*DestroyInstance();*/}
    CKernel(const CKernel& kernel){}
    CKernel& operator = (const CKernel& kernel){
        return *this;
    }
    //成员属性 网络 ui类对象
    MainDialog* m_mainDialog;
    LoginDialog* m_loginDialog;
    FiveInLineZone* m_FiveInZone;
    RoomDialog*     m_roomDialog;
    leaderboard_show* ld_show;



    INetMediator* m_client;

    //协议映射表 协议头与处理函数的对应关系
    std::vector<PFUN> m_netPackFunMap;

    //个人信息
    int m_id;
    int m_roomid;
    int m_zoneid;
    bool m_isHost;
    QString m_userName;
    char m_serverIP[20];

    CPlayer player[10];

    int fil_count;

};

#endif // CKERNEL_H
