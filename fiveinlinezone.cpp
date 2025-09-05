#include "fiveinlinezone.h"
#include "ui_fiveinlinezone.h"
#include<QMessageBox>



FiveInLineZone::FiveInLineZone(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FiveInLineZone)
{
    ui->setupUi(this);

    this->setWindowTitle("五子棋--决战巅峰");

    m_layout = new QGridLayout;
    ui->wdg_roomGrid->setLayout(m_layout);

    for(int i = 0; i < 120;i++)
    {
        RoomItem* item = new RoomItem;
        item->setInfo(i+1);
        connect(item,SIGNAL(SIG_JoinRoom(int)),
                this,SIGNAL(SIG_joinRoom(int)));
        m_layout->addWidget(item,i/2,i%2);
    }
}

FiveInLineZone::~FiveInLineZone()
{
    delete ui;
}

void FiveInLineZone::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this,"退出提示","真的不再玩一会吗??") == QMessageBox::Yes)
    {
        Q_EMIT SIG_close();
        event->accept();
    }
    else{
        event->ignore();
    }
}

std::vector<RoomItem *> &FiveInLineZone::getVecRoomItem()
{
    return m_vecRoomItem;
}

void FiveInLineZone::on_pb_ldboard_clicked()
{
    Q_EMIT SIG_getldboard();
}

