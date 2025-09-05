#ifndef FIVEINLINEZONE_H
#define FIVEINLINEZONE_H

#include <QDialog>
#include"roomitem.h"
#include"QGridLayout"
#include"QCloseEvent"


namespace Ui {
class FiveInLineZone;
}

class FiveInLineZone : public QDialog
{
    Q_OBJECT
signals:
    void SIG_close();
    void SIG_joinRoom(int roomid);
    void SIG_getldboard();
public:
    explicit FiveInLineZone(QWidget *parent = nullptr);
    ~FiveInLineZone();
    void closeEvent(QCloseEvent * event);
    std::vector<RoomItem*>& getVecRoomItem();
private slots:
    void on_pb_ldboard_clicked();

private:
    Ui::FiveInLineZone *ui;

    QGridLayout* m_layout;

    std::vector<RoomItem*> m_vecRoomItem;
};

#endif // FIVEINLINEZONE_H
