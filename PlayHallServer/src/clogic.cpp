#include "clogic.h"
#include<list>

CLogic::CLogic( TcpKernel* pkernel ):m_roomUserList(121)
{
        m_pKernel = pkernel;
        m_sql = pkernel->m_sql;
        m_tcp = pkernel->m_tcp;
        pthread_mutex_init(&m_roomListMutex,NULL);
        
        //排行榜
        char sqlstr[1000] = "";
        list<string> lstRes;
        sprintf( sqlstr , "select id , name , total_matches , score , win_matches from t_user");
        m_sql->SelectMysql( sqlstr , 5 , lstRes );
        while(lstRes.size() > 0)
        {
            //创建缓存节点
            CPlayerInfo* p_info = new CPlayerInfo;
            p_info->id = atoi(lstRes.front().c_str());
            lstRes.pop_front();
            string name = lstRes.front();
            strcpy(p_info->name,name.c_str());
            lstRes.pop_front();
            p_info->all = atoi(lstRes.front().c_str());
            lstRes.pop_front();
            p_info->score = atoi(lstRes.front().c_str());
            lstRes.pop_front();
            p_info->win = atoi(lstRes.front().c_str());
            lstRes.pop_front();
            //创建跳表结点并指向
            p_info->add_skip = m_LeaderBoard.UpdataBoard(p_info->id,p_info->score,p_info->name);

            //将结点加入到哈希表
            m_LeaderBoard.UpdataHash(p_info);
        }
}


void CLogic::setNetPackMap()
{
    NetPackMap(_DEF_PACK_REGISTER_RQ)     = &CLogic::RegisterRq;
    NetPackMap(_DEF_PACK_LOGIN_RQ)        = &CLogic::LoginRq;
    NetPackMap(DEF_PACK_JOIN_ZONE)        = &CLogic::JoinZoneRq;
    NetPackMap(DEF_PACK_LEAVE_ZONE)       = &CLogic::LeaveZoneRq;
    NetPackMap(DEF_JOIN_ROOM_RQ)          = &CLogic::JoinRoomRq;
    NetPackMap(DEF_LEAVE_ROOM_RQ)         = &CLogic::LeaveRoomRq;

    NetPackMap(DEF_FIL_ROOM_READY )       = &CLogic::FIL_MsgSendRq;
    NetPackMap(DEF_FIL_GAME_START )       = &CLogic::FIL_MsgSendRq;
    NetPackMap(DEF_FIL_AI_BEGIN   )       = &CLogic::FIL_MsgSendRq;
    NetPackMap(DEF_FIL_AI_END     )       = &CLogic::FIL_MsgSendRq;
    NetPackMap(DEF_FIL_DISCARD    )       = &CLogic::FIL_MsgSendRq;
    NetPackMap(DEF_FIL_SURREND    )       = &CLogic::FIL_MsgSendRq;
    NetPackMap(DEF_FIL_WIN        )       = &CLogic::FIL_WinMsgSendRq;

    NetPackMap(DEF_FIL_PIECEDOWN  )       = &CLogic::FIL_PieceDownRq;
    NetPackMap(DEF_LD_BOARD       )       = &CLogic::ZONE_LD_BOADR;

}

//#define _DEF_COUT_FUNC_    printf("clientfd:%d  "<< clientfd << __func__ << endl;

//注册
void CLogic::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
{
    //cout << "clientfd:"<< clientfd << __func__ << endl;
    printf("clientfd:%d  RegisterRq\n",clientfd);

    //解析数据包  获取 tel password  name
    STRU_REGISTER_RQ* rq = (STRU_REGISTER_RQ*)szbuf;
    STRU_REGISTER_RS rs;

    //根据 tel 查询数据库 看 有没有
    list<string> lstRes;
    char sqlstr[1000] = "";
    sprintf( sqlstr , "select tel from t_user where tel = '%s';" , rq->tel);
    m_sql->SelectMysql( sqlstr , 1 , lstRes);

    //有 返回结果
    if( lstRes.size() > 0 )
    {
        rs.result = tel_is_exist;
    }else{
        //没有 看昵称 有没有
        lstRes.clear();
        sprintf( sqlstr , "select name from t_user where name = %s" , rq->name );
        m_sql->SelectMysql(sqlstr , 1 , lstRes);
        if(lstRes.size() > 0 ){
            rs.result  = name_is_exist;//有 返回注册结果
        }else{
            //没有 注册成功 更新数据库 写表
            rs.result = register_success;
            sprintf( sqlstr ,
                "insert into t_user (tel , password , name ) value( '%s' , '%s' , '%s' )" ,
                     rq->tel , rq->password , rq->name );
            bool res = m_sql->UpdataMysql( sqlstr );
            if(!res){
                printf("update failed : %s\n",sqlstr);
            }
        }
    }
    //返回结果
    SendData( clientfd , (char*)&rs , sizeof(rs) );
}

//登录
void CLogic::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
{
    //cout << "clientfd:"<< clientfd << __func__ << endl;
    printf("clientfd:%d  LoginRq\n",clientfd);

    //    STRU_LOGIN_RS rs;
    //    //rs.result = login_success;
    //    SendData( clientfd , (char*)&rs , sizeof(rs)  );

    //拆包 获取tel  password
    STRU_LOGIN_RQ * rq = (STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS rs;
    //根据 tel 查  id password  name
    char sqlstr[1000] = "";
    list<string> lstRes;
    sprintf( sqlstr , "select id,password,name from t_user where tel = '%s'",rq->tel );
    m_sql->SelectMysql( sqlstr , 3 , lstRes );
    if( lstRes.size() == 0 )
    {
        //没有  返回结果
        rs.result = user_not_exist;
    }
    else{
        int id = atoi(lstRes.front().c_str());
        lstRes.pop_front();
        string strPassword = lstRes.front();
        lstRes.pop_front();
        string strName = lstRes.front();
        lstRes.pop_front();
        //有  看密码是否一致
        if(strcmp(rq->password,strPassword.c_str())!=0)
        {
            //不一致返回结果
            rs.result = password_error;
        }
        else{
            //一致
            rs.result = login_success;
            //如该之前有用户信息  前置下线  回收
            UserInfo *info = nullptr;
            if(m_mapIdToUserInfo.find(id,info))
            {
                //强制下线
                //回收
                m_mapIdToUserInfo.erase(id);
                delete info;
            }
            //把id 和 套接字 捆绑在一起
            //保存 用户信息
            info = new UserInfo;
            info->m_id = id;
            info->m_sockfd = clientfd;
            strcpy(info->m_userName,strName.c_str());

            strcpy(rs.name,strName.c_str());
            rs.userid = id;
            m_mapIdToUserInfo.insert(id,info);
            //返回结果  id  name  result
            SendData( clientfd , (char*)&rs , sizeof(rs) );

            return;
        }


    }
    SendData(clientfd,(char*)&rs,sizeof(rs));




}

//加入专区
void CLogic::JoinZoneRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d  JoinZoneRq\n",clientfd);

    //拆包  更新信息
    STRU_JOIN_ZONE * rq = (STRU_JOIN_ZONE*)szbuf;
    UserInfo* info = nullptr;
    if(!m_mapIdToUserInfo.find(rq->userid,info))
    {
        return;
    }
    info->m_zoneid = rq->zoneid;
}

//离开专区
void CLogic::LeaveZoneRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d  LeaveZoneRq\n",clientfd);

    //拆包  更新信息
    STRU_LEAVE_ZONE * rq = (STRU_LEAVE_ZONE*)szbuf;
    UserInfo* info = nullptr;
    if(!m_mapIdToUserInfo.find(rq->userid,info))
    {
        return;
    }
    info->m_zoneid = 0;
}

//加入房间请求 加入时可能多个线程同时有客户端请求
void CLogic::JoinRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d  JoinRoomRq\n",clientfd);

    //拆包
    STRU_JOIN_ROOM_RQ * rq = (STRU_JOIN_ROOM_RQ*) szbuf;
    STRU_JOIN_ROOM_RS rs;
    pthread_mutex_lock(&m_roomListMutex);
    list<int> tmplst;

    //首先  0-120  数组  看房间里面的人数
    //0 加入房间的就是 房主  加入到房间列表 返回
    //1 玩家 加入房间列表 返回加入成功 需要同步信息
    //2 加入失败
    list<int> & userlst = m_roomUserList[rq->roomid];
    switch(userlst.size())
    {
    case 0:
        rs.result = 1;
        rs.roomid = rq->roomid;
        rs.status = _host;
        rs.userid = rq->userid;
        userlst.push_back(rq->userid);
        break;
    case 1:
        rs.result = 1;
        rs.roomid = rq->roomid;
        rs.status = _player;
        rs.userid = rq->userid;
        userlst.push_back(rq->userid);
        break;
    case 2:
        rs.result = 0;
        break;
    default:
        rs.result = 0;
        break;
    }
    tmplst = userlst;
    pthread_mutex_unlock(&m_roomListMutex);

    SendData(clientfd,(char*)&rs,sizeof (rs));

    //玩家给房主发 房主给玩家发
    //size为1自己给自己发
    if(tmplst.size()>0)
    {
        int loginid = rq->userid;
        //根据id 拿到用户信息
        UserInfo* loginInfo = nullptr;
        if(!m_mapIdToUserInfo.find(loginid,loginInfo))return;
        //写成员信息的请求
        STRU_ROOM_MEMBER loginrq;
        loginrq.userid = loginid;
        loginrq.status = rs.status;
        strcpy(loginrq.name,loginInfo->m_userName);

        bool flag = false;
        for(auto ite = tmplst.begin();ite!=tmplst.end();ite++)
        {
            int status = _player;
            if(!flag)
            {
                status = _host;
                flag = true;
            }
            //根据id 拿到用户信息
            int roomMenid = *ite;
            if(roomMenid!=loginid)
            {
                UserInfo* MemInfo = nullptr;
                if(!m_mapIdToUserInfo.find(roomMenid,MemInfo))continue;
                //写成员信息的请求
                STRU_ROOM_MEMBER Memrq;
                Memrq.userid = roomMenid;
                Memrq.status = status;
                strcpy(Memrq.name,MemInfo->m_userName);
                //彼此发送
                SendData(loginInfo->m_sockfd,(char*)&Memrq,sizeof(Memrq));
                SendData(MemInfo->m_sockfd,(char*)&loginrq,sizeof(loginrq));
            }
            else{
                //自己给自己发一条
                SendData(loginInfo->m_sockfd,(char*)&loginrq,sizeof(loginrq));
            }
        }
    }
}

//离开房间
void CLogic::LeaveRoomRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d  LeaveRoomRq\n",clientfd);
    //拆包
    STRU_LEAVE_ROOM_RQ *rq = (STRU_LEAVE_ROOM_RQ*)szbuf;
    //谁 什么身份 离开哪个房间
    int leaveid = rq->userid;

    //当前的这个人的离开要发给房间里所有人
    list<int> & lst = m_roomUserList[rq->roomid];
    //给除了这个离开的人以外的所有人转发离开信息
    for(auto ite = lst.begin();ite!=lst.end();++ite)
    {
        int memid = *ite;
        if(leaveid!=memid){
            UserInfo* memInfo = nullptr;
            if(!m_mapIdToUserInfo.find(memid,memInfo))continue;
            SendData(memInfo->m_sockfd,szbuf,nlen);
        }
    }

    //根据身份不同 房主 player
    pthread_mutex_lock(&m_roomListMutex);
    if(rq->status == _host)
    {
        lst.clear();
    }else if(rq->status==_player){
        //找到离开的人 清掉
        for(auto ite = lst.begin();ite!=lst.end();++ite)
        {
            if(leaveid == *ite)
            {
                ite = lst.erase(ite);
                break;
            }
        }
    }
    pthread_mutex_unlock(&m_roomListMutex);
}

//服务器转发五子棋游戏命令给房间内成员
void CLogic::FIL_MsgSendRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d  FIL_MsgSendRq\n",clientfd);
    //拆包
    STRU_FIL_RQ* rq = (STRU_FIL_RQ*)szbuf;
    //什么专区 什么房间 谁 发了什么
    //根据专区 拿到房间列表  根据房间 拿到房间内成员  转发给房间里的所有人
    //rq->zoneid //根据专区 拿到房间列表
    list<int>& lstRes = m_roomUserList[rq->roomid];
    for(auto ite = lstRes.begin();ite != lstRes.end();++ite)
    {
        int userid = *ite;
        UserInfo * info = nullptr;
        if(!m_mapIdToUserInfo.find(userid,info))continue;
        SendData(info->m_sockfd,szbuf,nlen);
    }
}

void CLogic::FIL_PieceDownRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d  FIL_PieceDownRq\n",clientfd);
    //拆包
    STRU_FIL_PIECEDOWN* rq = (STRU_FIL_PIECEDOWN*)szbuf;
    //什么专区 什么房间 谁 发了什么
    //根据专区 拿到房间列表  根据房间 拿到房间内成员  转发给房间里的所有人
    //rq->zoneid //根据专区 拿到房间列表
    list<int>& lstRes = m_roomUserList[rq->roomid];
    for(auto ite = lstRes.begin();ite != lstRes.end();++ite)
    {
        int userid = *ite;
        UserInfo * info = nullptr;
        if(!m_mapIdToUserInfo.find(userid,info))continue;
        SendData(info->m_sockfd,szbuf,nlen);
    }
}

//服务器转发五子棋游戏结束命令给房间内成员
void CLogic::FIL_WinMsgSendRq(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d  FIL_WinMsgSendRq\n",clientfd);
    //拆包
    STRU_Win_FIL_RQ* rq = (STRU_Win_FIL_RQ*)szbuf;
    //什么专区 什么房间 谁 发了什么
    //根据专区 拿到房间列表  根据房间 拿到房间内成员  转发给房间里的所有人
    //rq->zoneid //根据专区 拿到房间列表
    int player_id[2];
    int score = rq->score;
    list<int>& lstRes = m_roomUserList[rq->roomid];
    for(auto ite = lstRes.begin();ite != lstRes.end();++ite)
    {
        int userid = *ite;
        UserInfo * info = nullptr;
        if(!m_mapIdToUserInfo.find(userid,info))continue;
        SendData(info->m_sockfd,szbuf,nlen);
        if(info->m_id==rq->winid){
            player_id[0] = info->m_id;
        }else
        {
            player_id[1] = info->m_id;
        }
    }
    char sqlstr1[1000] = "";
    sprintf( sqlstr1 , "update t_user set total_matches=total_matches+1,win_matches=win_matches+1,score = score+%d where id = %d",score,player_id[0]);
    m_sql->UpdataMysql(sqlstr1);
    char sqlstr2[1000] = "";
    sprintf( sqlstr2 , "update t_user set total_matches=total_matches+1,score = GREATEST(score - %d, 0) where id = %d",score,player_id[1]);
    m_sql->UpdataMysql(sqlstr2);
    m_LeaderBoard.UpdataBoard(player_id[0],score,player_id[1],score);
}

void CLogic::ZONE_LD_BOADR(sock_fd clientfd, char *szbuf, int nlen)
{
    printf("clientfd:%d  ZONE_LD_BOADR\n",clientfd);
    STRU_GET_LD_BOARD* rq = (STRU_GET_LD_BOARD*)szbuf;
    int userid = rq->userid;
    UserInfo* info = nullptr;
    m_mapIdToUserInfo.find(userid,info);
    int size = m_LeaderBoard.Print_Board(resultVector);

    STRU_LD_BOARD rs;
    rs.size = size;

    for(int i = 0;i < size;i++)
    {
        rs.player[i].id = resultVector[i].id;
        strcpy(rs.player[i].name , resultVector[i].name);
        rs.player[i].score = resultVector[i].score;
        printf("\n%d\n",rs.player[i].score);
    }
    SendData(clientfd,(char*)&rs,sizeof(rs));
//    if(ld_flag==true){
//        int userid = rq->userid;
//        UserInfo * info = nullptr;
//        m_mapIdToUserInfo.find(userid,info);
//        printf("%d\n",info->m_sockfd);
//        SendData(info->m_sockfd,szbuf,nlen);
//        //SendData(clientfd,(char*)&rs,sizeof(rs));
//    }
}
