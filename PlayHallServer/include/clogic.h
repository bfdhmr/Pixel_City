#ifndef CLOGIC_H
#define CLOGIC_H

#include"TCPKernel.h"
#include"leaderboard.h"

class CLogic
{
public:
    CLogic( TcpKernel* pkernel );
public:
    //设置协议映射
    void setNetPackMap();
    /************** 发送数据*********************/
    void SendData( sock_fd clientfd, char*szbuf, int nlen )
    {
        m_pKernel->SendData( clientfd ,szbuf , nlen );
    }
    /************** 网络处理 *********************/
    //注册
    void RegisterRq(sock_fd clientfd, char*szbuf, int nlen);
    //登录
    void LoginRq(sock_fd clientfd, char*szbuf, int nlen);
    //加入分区
    void JoinZoneRq(sock_fd clientfd, char*szbuf, int nlen);
    //离开分区
    void LeaveZoneRq(sock_fd clientfd, char*szbuf, int nlen);
    //加入房间
    void JoinRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    //离开房间
    void LeaveRoomRq(sock_fd clientfd, char*szbuf, int nlen);
    //五子棋游戏命令转发
    void FIL_MsgSendRq(sock_fd clientfd, char*szbuf, int nlen);
    //五子棋落子转发
    void FIL_PieceDownRq(sock_fd clientfd, char*szbuf, int nlen);

    void FIL_WinMsgSendRq(sock_fd clientfd, char*szbuf, int nlen);
    
    //打印排行榜
    void ZONE_LD_BOADR(sock_fd clientfd, char*szbuf, int nlen);
    /*******************************************/

private:
    TcpKernel* m_pKernel;
    CMysql * m_sql;
    Block_Epoll_Net * m_tcp;

    bool ld_flag;
    STRU_LD_BOARD ld_board;
    
    leaderboard m_LeaderBoard;

    MyMap<int,UserInfo*> m_mapIdToUserInfo;

    vector<list<int>> m_roomUserList;

    pthread_mutex_t m_roomListMutex;

    std::vector<CPlayer> resultVector;
};

#endif // CLOGIC_H
