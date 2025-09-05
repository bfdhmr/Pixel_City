#include "ckernel.h"
#include"qdebug.h"
#include"TcpClientMediator.h"
#include<QMessageBox>
#include"md5/md5.h"
#include<QSettings>
#include<QCoreApplication>
#include<QFileInfo>

//获得MD5 函数
#define MD5_KEY 1234
static std::string getMD5(QString val)
{
    QString tmp = QString("%1_%2").arg(val).arg(MD5_KEY);
    MD5 md(tmp.toStdString());
    return md.toString();
}

//宏定义setNetPackFunMap中映射偏移
#define NetPackMap(a) m_netPackFunMap[(a) - _DEF_PACK_BASE]

void CKernel::setNetPackFunMap()
{
  //m_netPackFunMap[_DEF_PACK_LOGIN_RS - _DEF_PACK_BASE]  = &CKernel::slot_dealLoginRS;
  //进行宏定义
    NetPackMap(_DEF_PACK_LOGIN_RS)      = &CKernel::slot_dealLoginRS;
    NetPackMap(_DEF_PACK_REGISTER_RS)   = &CKernel::slot_dealRegistRS;
    NetPackMap(DEF_JOIN_ROOM_RS)        = &CKernel::slot_dealJoinRoomRS;
    NetPackMap(DEF_ROOM_MEMBER)         = &CKernel::slot_dealRoomMemberRq;
    NetPackMap(DEF_LEAVE_ROOM_RQ)       = &CKernel::slot_dealLeaveRoomRq;

    NetPackMap( DEF_FIL_ROOM_READY  )   = &CKernel::slot_dealFilGameReadyRq;
    NetPackMap( DEF_FIL_GAME_START  )   = &CKernel::slot_dealFilGameStartRq;

    NetPackMap( DEF_FIL_AI_BEGIN    )   = &CKernel::slot_dealFilGameByCpuBegin;
    NetPackMap( DEF_FIL_AI_END      )   = &CKernel::slot_dealFilGameByCpuEnd;
//    NetPackMap( DEF_FIL_DISCARD     )   = &CKernel::slot_dealFil
//    NetPackMap( DEF_FIL_SURREND     )   = &CKernel::slot_dealFil

    NetPackMap( DEF_FIL_PIECEDOWN   )   = &CKernel::slot_dealFilPieceDownRq;
    NetPackMap( DEF_FIL_WIN         )   = &CKernel::slot_dealFilWinRq;

    NetPackMap( DEF_LD_BOARD        )   = &CKernel::slot_dealldboard;
}

void CKernel::ConfigSet()
{
    //获取配置文件的信息以及设置
    //.ini   配置文件
    //[net] 组名 GroupNmame
    //key = value

    //1.ip默认
   strcpy(m_serverIP,_DEF_SERVER_IP);

    //2.设置和获取配置文件 有还是没有 配置文件在哪里 ? 设置和exe同一级的目录
   //路径
   QString path = QCoreApplication::applicationDirPath() + "/config.ini";
   //查看文件是否存在
   QFileInfo info(path);
   if(info.exists())
   {
        //存在
       QSettings setting(path , QSettings::IniFormat,nullptr);
       //[net]组写入值
       setting.beginGroup("net");
       QVariant var = setting.value("ip");
       QString strip = var.toString();
       if(!strip.isEmpty())
       {    strcpy(m_serverIP,strip.toStdString().c_str()); }
       setting.endGroup();
   }
   else{
       //不存在
       QSettings setting(path , QSettings::IniFormat,nullptr);
       //[net]组写入值
       setting.beginGroup("net");
       setting.setValue("ip",QString::fromStdString(m_serverIP));
       setting.endGroup();
   }

   qDebug() <<  "ip : " << m_serverIP;
}

void CKernel::DestroyInstance()
{
    qDebug() << __func__;   //打印当前函数
    if(m_client)
    {
        m_client->CloseNet();
        delete m_client;
        m_client = nullptr;
    }

    delete m_mainDialog;
    delete m_loginDialog;
}

void CKernel::slot_loginCommit(QString tel, QString password)
{
    //加密
    //封包
    STRU_LOGIN_RQ rq;
    strcpy(rq.tel,tel.toStdString().c_str());

    qDebug() << password << " MD5 : " << getMD5(password).c_str();

    strcpy(rq.password,getMD5(password).c_str());
    //发送
    SendDate((char*)&rq,sizeof(rq));
}

void CKernel::slot_registerCommit(QString tel, QString password, QString name)
{
    //加密
    //封包
    STRU_REGISTER_RQ rq;
    strcpy(rq.tel,tel.toStdString().c_str());
    //通过md5进行加密
    strcpy(rq.password,getMD5(password).c_str());
    //兼容中文
    std::string strName = name.toStdString();
    strcpy(rq.name,strName.c_str());
    //发送
    SendDate((char*)&rq,sizeof(rq));
}

//提交加入分区
void CKernel::slot_joinZone(int zoneid)
{
    //成员改变属性
    m_zoneid = zoneid;
    //请求包
    STRU_JOIN_ZONE rq;
    rq.userid = m_id;
    rq.zoneid = zoneid;

    SendDate((char*)&rq,sizeof(rq));

    //ui跳转
    //显示专区
    switch(zoneid){
    case Five_In_Line:
        m_FiveInZone->show();
        break;
    }
}

void CKernel::slot_leaveZone()
{
    //成员属性修改
    m_zoneid = 0;
    //请求
    STRU_LEAVE_ZONE rq;
    rq.userid = m_id;
    SendDate((char*)&rq,sizeof(rq));
    //ui跳转
    m_mainDialog->show();
}

//提交加入房间
void CKernel::slot_joinRoom(int roomid)
{
    if(m_roomid != 0)
    {
        QMessageBox::about(nullptr,"提示","请玩家大大专注于一场游戏哦!!");
        return;
    }
    //加入成功后隐藏
    //m_FiveInZone->hide();
    //提交请求
    STRU_JOIN_ROOM_RQ rq;
    rq.roomid = roomid;
    rq.userid = m_id;

    SendDate((char*)&rq,sizeof(rq));
}

void CKernel::slot_ReadyData(unsigned int lSendIP, char *buf, int nlen)
{
    //协议映射表
    PackType type = *(int *)buf; //   *(int*)  按照整形取数据
    if(type >= _DEF_PACK_BASE && type <= _DEF_PACK_COUNT + _DEF_PACK_BASE)
    {
        //根据协议头跳转对应函数
        //协议映射处理
        PFUN pf = NetPackMap(type);
        (this->*pf)(lSendIP,buf,nlen);
    }



    //需要回收buf
    delete[] buf;
}

void CKernel::slot_dealLoginRS(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug() << __func__;
    //拆包
    STRU_LOGIN_RS *rs = (STRU_LOGIN_RS*)buf;
    //根据不同结果显示
    switch(rs->result)
    {
    case user_not_exist:
        QMessageBox::about(m_loginDialog,"提示","用户不存在");
        break;
    case password_error:
        QMessageBox::about(m_loginDialog,"提示","密码错误");
        break;
    case login_success:
        //ui切换
        m_loginDialog->hide();
        m_mainDialog->show();
        //存储
        m_id = rs->userid;
        m_userName = QString::fromStdString(rs->name);
        break;
    default:
        QMessageBox::about(m_loginDialog,"提示","登录异常提示");
        break;
    }
}

void CKernel::slot_dealRegistRS(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug() << __func__;
    //解释数据包
    STRU_REGISTER_RS * rs = (STRU_REGISTER_RS* )buf;
    //根据结束弹窗
    switch(rs->result)
    {
        case tel_is_exist:
        QMessageBox::about( this->m_loginDialog , "注册提示" , "注册失败,手机号已存在" );
        break;
        case name_is_exist:
        QMessageBox::about( this->m_loginDialog , "注册提示" , "注册失败,昵称已存在" );
        break;
        case register_success:
        QMessageBox::about( this->m_loginDialog , "注册提示" , "注册成功" );
        break;
        default:
        QMessageBox::about( this->m_loginDialog , "注册提示" , "注册异常" );
        break;
    }
}

//加入房间回复处理
void CKernel::slot_dealJoinRoomRS(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug()<< __func__;
    //拆包
    STRU_JOIN_ROOM_RS * rs = (STRU_JOIN_ROOM_RS*)buf;
    if(rs->result == 0){
        QMessageBox::about(m_FiveInZone,"提示","加入房间失败"); // todo  fix  具体专区 =========================
        return;
    }
    if(rs->status == _host)
    {
        m_isHost = true;
    }
    m_roomid = rs->roomid;
    //成功 跳转 成员赋值
    m_FiveInZone->hide();

    m_roomDialog->setInfo(m_roomid);
    m_roomDialog->show();
}

void CKernel::slot_dealRoomMemberRq(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包 别人给自己发 自己给自己发
    STRU_ROOM_MEMBER * rq = (STRU_ROOM_MEMBER *)buf;

    if(rq->status==_host)
    {
        m_roomDialog->setHostInfo(rq->userid,QString::fromStdString(rq->name));
    }
    if(rq->status==_player)
    {
        m_roomDialog->setPlayerInfo(rq->userid,QString::fromStdString(rq->name));
    }
    m_roomDialog->setUserStatus(m_isHost?_host:_player);
}


//离开房间
void CKernel::slot_leaveRoom()
{
    //这个人主动离开
    STRU_LEAVE_ROOM_RQ rq;
    rq.status = m_isHost?_host:_player;
    rq.userid = m_id;
    rq.roomid = m_roomid;
    SendDate((char*)&rq,sizeof(rq));

    //界面
    m_roomDialog->clearRoom();
    m_roomDialog->hide();
    m_FiveInZone->show();

    //后台数据
    m_roomid = 0;
    m_isHost= false;
}

//离开房间请求
void CKernel::slot_dealLeaveRoomRq(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包
    STRU_LEAVE_ROOM_RQ* rq =(STRU_LEAVE_ROOM_RQ*)buf;
    if(rq->status == _host)
    {
        //离开的对方是房主
        //界面
        m_roomDialog->clearRoom();
        m_roomDialog->hide();
        m_FiveInZone->show();
        //后台数据
        m_roomid = 0;
        m_isHost= false;
    }
    else{
        m_roomDialog->playLeave(rq->userid);
    }
}

void CKernel::slot_dealFilGameReadyRq(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包
    STRU_FIL_RQ* rq = (STRU_FIL_RQ*)buf;
    //什么专区 什么房间 谁 做了什么事
    if(rq->roomid==m_roomid)
    {
        m_roomDialog->setPlayerReady(rq->userid);
    }
}

void CKernel::slot_dealFilGameStartRq(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包
    STRU_FIL_RQ* rq = (STRU_FIL_RQ*)buf;
    //什么专区 什么房间 谁 做了什么事
    if(rq->roomid == m_roomid)
    {
        m_roomDialog->setGameStart();
    }
}
//处理落子
void CKernel::slot_dealFilPieceDownRq(unsigned int lSendIP, char *buf, int nlen)
{
    STRU_FIL_PIECEDOWN* rq = (STRU_FIL_PIECEDOWN*)buf;
    fil_count++;
    m_roomDialog->slot_pieceDown(rq->color,rq->x,rq->y);

}

void CKernel::slot_dealFilWinRq(unsigned int lSendIP, char *buf, int nlen)
{

}

//五子棋准备
void CKernel::slot_fil_gameReady(int zoneid, int roomid, int userid)
{
    STRU_FIL_RQ rq(DEF_FIL_ROOM_READY);
    rq.roomid = roomid;
    rq.zoneid = zoneid;
    rq.userid = userid;

    SendDate((char*)&rq,sizeof(rq));
}

//五子棋开局
void CKernel::slot_fil_gameStart(int zoneid, int roomid)
{
    STRU_FIL_RQ rq(DEF_FIL_GAME_START);
    rq.roomid = roomid;
    rq.zoneid = zoneid;
    rq.userid = m_id;

    SendDate((char*)&rq,sizeof(rq));
}

void CKernel::slot_fil_pieceDown(int blackorwhite, int x, int y)
{
    //封包
    STRU_FIL_PIECEDOWN rq;
    rq.color = blackorwhite;
    rq.x = x;
    rq.y = y;
    rq.userid = m_id;
    rq.roomid = m_roomid;
    rq.zoneid = m_zoneid;

    SendDate((char*)&rq,sizeof(rq));
}

#include"FiveInLine/fiveinline.h"
void CKernel::slot_fil_win(int blackorwhite)
{
   //发包
   struct STRU_Win_FIL_RQ rq;
   rq.roomid = m_roomid;
   rq.userid = m_id;
   rq.zoneid = m_zoneid;
    //弹窗
    QString res;
    if(m_isHost ){
        if(blackorwhite == FiveInLine::Black){
            res = QString("大获全胜");
            rq.winid = m_id;
        }
        else{
            res = QString("一败涂地");
        }
    }else{
        if(blackorwhite == FiveInLine::White){
            res = QString("大获全胜");
            rq.winid = m_id;
        }
        else{
            res = QString("一败涂地");
        }
    }
    if(fil_count>15)rq.score=1;
    else rq.score = 2;
    if(rq.winid==m_id)SendDate((char*) &rq , sizeof(rq));
    QMessageBox::about(m_roomDialog,"提示",res);
    //可以点击开局

    fil_count=0;
    m_roomDialog->resetAllPushButton();
}

void CKernel::slot_fil_playByCpuBegin(int zoneid, int roomid, int userid)
{
    //发包
   STRU_FIL_RQ rq(DEF_FIL_AI_BEGIN);
   rq.roomid = roomid;
   rq.userid = userid;
   rq.zoneid  =zoneid;

   SendDate((char*) &rq , sizeof(rq));
}

void CKernel::slot_fil_playByCpuEnd(int zoneid, int roomid, int userid)
{
    //发包
   STRU_FIL_RQ rq(DEF_FIL_AI_END);
   rq.roomid = roomid;
   rq.userid = userid;
   rq.zoneid  =zoneid;

   SendDate((char*) &rq , sizeof(rq));
}

void CKernel::slot_dealFilGameByCpuBegin(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包
    STRU_FIL_RQ * rq = (STRU_FIL_RQ*)buf;
    //查看身份
    rq->zoneid;
    rq->roomid;
    if(m_id == rq->userid)
    {
        if(m_isHost)
        {
            m_roomDialog->setHostPlayByCpu(true);
        }
        else
        {
            m_roomDialog->setPlayerPlayByCpu(true);
        }
    }
    else
    {
        if(m_isHost)
        {
            m_roomDialog->setPlayerPlayByCpu(true);
        }
        else
        {
            m_roomDialog->setHostPlayByCpu(true);
        }
    }
}

void CKernel::slot_dealFilGameByCpuEnd(unsigned int lSendIP, char *buf, int nlen)
{
    //拆包
    STRU_FIL_RQ * rq = (STRU_FIL_RQ*)buf;
    //查看身份
    rq->zoneid;
    rq->roomid;
    if(m_id == rq->userid)
    {
        if(m_isHost)
        {
            m_roomDialog->setHostPlayByCpu(false);
        }
        else
        {
            m_roomDialog->setPlayerPlayByCpu(false);
        }
    }
    else
    {
        if(m_isHost)
        {
            m_roomDialog->setPlayerPlayByCpu(false);
        }
        else
        {
            m_roomDialog->setHostPlayByCpu(false);
        }
    }
}

//发送排行榜请求
void CKernel::slot_getldboard()
{
    STRU_GET_LD_BOARD rq;
    rq.userid=m_id;
    SendDate((char*) &rq , sizeof(rq));
}

void CKernel::slot_dealldboard(unsigned int lSendIP, char *buf, int nlen)
{
    STRU_LD_BOARD* rq = (STRU_LD_BOARD*)buf;
    int size = rq->size;
    for(int i=0;i<size;i++){
        player[i] = rq->player[i];
    }

    ld_show->leaderboard_put(size,player);
    ld_show->show();
}


void CKernel::SendDate(char *buf, int nLen)
{
    m_client->SendData(0,buf,nLen);
}

CKernel::CKernel(QObject *parent) : QObject(parent),
    m_netPackFunMap(_DEF_PACK_COUNT , 0 ), m_id(0) , m_roomid(0), m_zoneid(0),m_isHost(false),fil_count(0)
{
     ConfigSet();
    setNetPackFunMap();


    m_mainDialog = new MainDialog;
    connect(m_mainDialog,SIGNAL(SIG_close()),
            this,SLOT(DestroyInstance()));
    connect(m_mainDialog,SIGNAL(SIG_joinZone(int)),
            this,SLOT(slot_joinZone(int)));
    //m_mainDialog->show();

    m_loginDialog = new LoginDialog;
    connect(m_loginDialog,SIGNAL(SIG_close()),
            this,SLOT(DestroyInstance()));
    connect(m_loginDialog,SIGNAL(SIG_loginCommit(QString,QString)),
            this,SLOT(slot_loginCommit(QString,QString)));
    connect(m_loginDialog,SIGNAL(SIG_registerCommit(QString,QString,QString)),
            this,SLOT(slot_registerCommit(QString,QString,QString)));
    m_loginDialog->show();

    m_FiveInZone = new FiveInLineZone;
    connect(m_FiveInZone,SIGNAL(SIG_joinRoom(int)),
            this,SLOT(slot_joinRoom(int)));
    connect(m_FiveInZone,SIGNAL(SIG_close()),
            this,SLOT(slot_leaveZone()));
    connect(m_FiveInZone,SIGNAL(SIG_getldboard()),
            this,SLOT(slot_getldboard()));

    m_roomDialog = new RoomDialog;
    connect(m_roomDialog,SIGNAL(SIG_close()),
            this,SLOT(slot_leaveRoom()));

    //m_roomDialog->show();

    m_client = new TcpClientMediator;
    m_client->OpenNet(m_serverIP,_DEF_TCP_PORT);
    connect(m_client,SIGNAL(SIG_ReadyData(uint,char*,int)),
            this,SLOT(slot_ReadyData(unsigned int , char *, int )));

    connect(m_roomDialog,SIGNAL(SIG_gameReady(int,int,int)),
            this , SLOT(slot_fil_gameReady(int,int,int)));
    connect(m_roomDialog,SIGNAL(SIG_gameStart(int,int)),
            this , SLOT(slot_fil_gameStart(int,int)));
    connect(m_roomDialog,SIGNAL(SIG_pieceDown(int,int,int)),
            this,SLOT(slot_fil_pieceDown(int,int,int)));
    connect(m_roomDialog , SIGNAL(SIG_playerWin(int)),
            this , SLOT(slot_fil_win(int)));
    connect(m_roomDialog , SIGNAL(SIG_playByCpuBegin(int,int,int)),
            this , SLOT(slot_fil_playByCpuBegin(int,int,int)));
    connect(m_roomDialog , SIGNAL(SIG_playByCpuEnd(int,int,int)),
            this , SLOT(slot_fil_playByCpuEnd(int,int,int)));

    ld_show = new leaderboard_show;

//    //模拟连接服务器，发送数据包
//    STRU_LOGIN_RQ rq;
//    m_client->SendData(0,(char*)&rq,sizeof(rq));

}
