#include "roomitem.h"
#include "roomitem.h"
#include "roomitem.h"
#include "ui_roomitem.h"

RoomItem::RoomItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoomItem),m_roomid(0)
{
    setUI();
    ui->setupUi(this);
}

RoomItem::~RoomItem()
{
    delete ui;
}

void RoomItem::setUI()
{
    this->setWindowTitle("登录&注册");
    //加载图片  资源路径 .qrc文件下  :/ 根目录
    QPixmap pixmap(":/images/background.jpg");
    //画板 添加图片
    QPalette pal(this->palette());
    pal.setBrush(QPalette::Background,pixmap);
    this->setPalette(pal);

}

void RoomItem::setInfo(int roomid)
{
    m_roomid = roomid;

    QString txt = QString("五子棋--决战%1").arg(roomid,3,10,QChar('0'));
    ui->lb_title->setText(txt);
}

void RoomItem::setRoomItem(int num)
{

}

void RoomItem::on_pb_play_clicked()
{
    Q_EMIT SIG_JoinRoom(m_roomid);
}

