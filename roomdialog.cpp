#include "roomdialog.h"
#include "ui_roomdialog.h"
#include"packdef.h"
#include<QMessageBox>

RoomDialog::RoomDialog(QWidget *parent) :
    QDialog(parent),m_roomid(0),m_status(_player),
    ui(new Ui::RoomDialog)
{
    ui->setupUi(this);

    ui->pb_start->setEnabled(false);

    //关于开局的按键  可以是kernel判断 , 然后给room发送 可以点击开局

    //点击开局 , 返回开局的信号

    //五子棋类 发送的信号 转发给Kernel
    connect(ui->widget,SIGNAL(SIG_pieceDown(int,int,int)),
            this , SIGNAL(SIG_pieceDown(int,int,int)));
    connect(ui->widget,SIGNAL(SIG_playerWin(int)),
            this , SIGNAL(SIG_playerWin(int)));
}

RoomDialog::~RoomDialog()
{
    delete ui;
}

void RoomDialog::setInfo(int roomid)
{
    m_roomid = roomid;

    QString txt = QString("五子棋--决战%1").arg(roomid,3,10,QChar('0'));

    this->setWindowTitle( txt );
}

#include"packdef.h"
void RoomDialog::setUserStatus(int status)
{
    m_status = status;

    ui->widget->setSelfStatus(m_status==_host?FiveInLine::Black:FiveInLine::White);
    //只有房主可以点击开局

}

//设置房主信息
void RoomDialog::setHostInfo(int id, QString name)
{
    ui->lb_player1_name->setText(name);
    ui->lb_icon_player1->setPixmap(QPixmap(":/icon/avatar_20.png"));
    m_roomUserList.push_back(id);
}

//设置玩家信息
void RoomDialog::setPlayerInfo(int id, QString name)
{
    ui->lb_player2_name->setText(name);
    ui->lb_icon_player2->setPixmap(QPixmap(":/icon/avatar_44.png").copy(0,0,230,280));
    m_roomUserList.push_back(id);
}

//清空房间
void RoomDialog::clearRoom()
{
    //ui清除
    ui->lb_player1_name->setText("等待加入中...");
    ui->lb_player2_name->setText("等待加入中...");
    ui->lb_icon_player1->setPixmap(QPixmap(":/icon/slotwait.png"));
    ui->lb_icon_player2->setPixmap(QPixmap(":/icon/slotwait.png"));

    //清空游戏界面
    ui->pb_start->setEnabled(false);
    ui->pb_player1_ready->setEnabled(true);
    ui->pb_player2_ready->setEnabled(true);

    //聊天窗口清空

    //后台数据清除
    m_roomid = 0;
    m_roomUserList.clear();
    m_status = _player;
}

//玩家离开
void RoomDialog::playLeave(int id)
{
    //ui
    ui->lb_player2_name->setText("等待加入中...");
    ui->lb_icon_player2->setPixmap(QPixmap(":/icon/slotwait.png"));

    //data
    for(auto ite = m_roomUserList.begin(); ite != m_roomUserList.end();++ite)
    {
        if(id == *ite)
        {
            ite = m_roomUserList.erase(ite);
            break;
        }
    }
}

void RoomDialog::setPlayerReady(int id)
{
    if(m_roomUserList.front()==id)
    {
        ui->pb_player1_ready->setChecked(true);
        ui->pb_player1_ready->setText("已准备");
    }
    if(m_roomUserList.back()==id && m_roomUserList.size()==2)
    {
        ui->pb_player2_ready->setChecked(true);
        ui->pb_player2_ready->setText("已准备");
    }
    if(ui->pb_player1_ready->isChecked()&&ui->pb_player2_ready->isChecked())
    {
        if(m_status==_host)
            ui->pb_start->setEnabled(true);
    }
}

//设置开局
void RoomDialog::setGameStart()
{
    //开始 准备 都不好用
    ui->pb_player1_ready->setEnabled(false);
    ui->pb_player2_ready->setEnabled(false);
    ui->pb_start->setEnabled(false);
    //五子棋开始操作
    ui->widget->clear();
    ui->widget->slot_startGame();
}

void RoomDialog::setHostPlayByCpu(bool yes)
{
    if(yes)
    {
        ui->pb_player1_cpu->setText("托管中");
    }
    else
    {
        ui->pb_player1_cpu->setText("托管");
    }
}

void RoomDialog::setPlayerPlayByCpu(bool yes)
{
    if(yes)
    {
        ui->pb_player2_cpu->setText("托管中");
    }
    else
    {
        ui->pb_player2_cpu->setText("托管");
    }
}

//离开房间
void RoomDialog::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this,"退出房间提示","真的不再多玩一会吗?")
                                == QMessageBox::Yes)
    {
        Q_EMIT SIG_close();
        event->accept();
    }else{
        event->ignore();
    }
}

void RoomDialog::resetAllPushButton()
{
    ui->pb_start->setEnabled(false);
    ui->pb_player1_ready->setEnabled(true);
    ui->pb_player2_ready->setEnabled(true);
    ui->pb_player1_ready->setChecked(false);
    ui->pb_player2_ready->setChecked(false);
    ui->pb_player1_ready->setText("待准备");
    ui->pb_player2_ready->setText("待准备");

    ui->pb_player1_cpu->setText("托管");
    ui->pb_player2_cpu->setText("托管");

    ui->pb_player1_cpu->setChecked(false);
    ui->pb_player2_cpu->setChecked(false);
}

void RoomDialog::on_pb_player1_ready_clicked(bool checked)
{
    //点击准备按钮
    //验证是不是自己点的
    if(m_status != _host) return;
    //是 切换状态
    if(ui->pb_player1_ready->isChecked())
    {
        ui->pb_player1_ready->setText("已准备");
        //发送信号
        Q_EMIT SIG_gameReady(0x10,m_roomid,m_roomUserList.front());
    }else
    {
        ui->pb_player1_ready->setText("待准备");
    }
}


void RoomDialog::on_pb_player2_ready_clicked(bool checked)
{
    //点击准备按钮
    //验证是不是自己点的
    if(m_status != _player) return;
    //是 切换状态
    if(ui->pb_player2_ready->isChecked())
    {
        ui->pb_player2_ready->setText("已准备");
        //发送信号
        Q_EMIT SIG_gameReady(0x10,m_roomid,m_roomUserList.back());
    }else
    {
        ui->pb_player2_ready->setText("待准备");
    }
}


void RoomDialog::on_pb_start_clicked()
{
    if(m_status != _host )return;
    Q_EMIT SIG_gameStart(0x10,m_roomid);
}

void RoomDialog::slot_pieceDown(int blackorwhite, int x, int y)
{
    ui->widget->slot_pieceDown(blackorwhite,x,y);
}


void RoomDialog::on_pb_player1_cpu_clicked(bool checked)
{
    //判断自己然后执行
    if(m_status == _host)
    {
        if(ui->pb_player1_cpu->isChecked()){
            //首先 托管通过网络发送 需要一个信号
            Q_EMIT SIG_playByCpuBegin(0x10,m_roomid,m_roomUserList.front());
            //托管信息发到服务器 再转回来 再改变文字 托管->托管中

            //电脑自动下棋
            ui->widget->setCpuColor(FiveInLine::Black);
            //调用一次下棋
            ui->widget->pieceDownByCpu();
        }else{
            //首先 托管通过网络发送 需要一个信号
            Q_EMIT SIG_playByCpuEnd(0x10,m_roomid,m_roomUserList.front());
            //托管信息发到服务器 再转回来 再改变文字 托管<-托管中

            //电脑停止下棋
            ui->widget->setCpuColor(FiveInLine::None);
        }
    }
}


void RoomDialog::on_pb_player2_cpu_clicked(bool checked)
{
    if(m_roomUserList.size()!=2)return;
    //判断自己然后执行
    if(m_status == _player)
    {
        if(ui->pb_player2_cpu->isChecked()){
            //首先 托管通过网络发送 需要一个信号
            Q_EMIT SIG_playByCpuBegin(0x10,m_roomid,m_roomUserList.back());
            //托管信息发到服务器 再转回来 再改变文字 托管->托管中

            //电脑自动下棋
            ui->widget->setCpuColor(FiveInLine::White);
            //调用一次下棋
            ui->widget->pieceDownByCpu();
        }else{
            //首先 托管通过网络发送 需要一个信号
            Q_EMIT SIG_playByCpuEnd(0x10,m_roomid,m_roomUserList.back());
            //托管信息发到服务器 再转回来 再改变文字 托管<-托管中

            //电脑停止下棋
            ui->widget->setCpuColor(FiveInLine::None);
        }
    }
}

