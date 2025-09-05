#ifndef ROOMITEM_H
#define ROOMITEM_H

#include <QDialog>

namespace Ui {
class RoomItem;
}

class RoomItem : public QDialog
{
    Q_OBJECT

public:
    explicit RoomItem(QWidget *parent = nullptr);
    ~RoomItem();

    void setUI();
    void setInfo(int roomid);
    void setRoomItem(int num);
signals:
    void SIG_JoinRoom(int id);
private slots:
    void on_pb_play_clicked();

private:
    Ui::RoomItem *ui;
    int m_roomid;
};

#endif // ROOMITEM_H
