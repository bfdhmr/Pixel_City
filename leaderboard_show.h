#ifndef LEADERBOARD_SHOW_H
#define LEADERBOARD_SHOW_H

#include <QWidget>
#include"QGridLayout"
#include"ld_userinfo.h"
#include"packdef.h"

namespace Ui {
class leaderboard_show;
}

class leaderboard_show : public QWidget
{
    Q_OBJECT

public:
    explicit leaderboard_show(QWidget *parent = nullptr);
    ~leaderboard_show();

    void leaderboard_put(int size,CPlayer player[]);

private:

    QVBoxLayout* m_layout;

    Ui::leaderboard_show *ui;
};

#endif // LEADERBOARD_SHOW_H
