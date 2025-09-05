#include "leaderboard_show.h"
#include "ui_leaderboard_show.h"
#include<QVBoxLayout>

leaderboard_show::leaderboard_show(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::leaderboard_show)
{
    ui->setupUi(this);

    m_layout = new QVBoxLayout;
    m_layout->setSpacing(3);
    ui->ldboard_wdg->setLayout(m_layout);
}

void leaderboard_show::leaderboard_put(int size,CPlayer player[])
{



    for(int i = 0; i < size;i++)
    {
        ld_userinfo* item = new ld_userinfo;
        item->setInfo(i+1,player[i].name,player[i].score);
        m_layout->addWidget(item);
        item->show();
    }
}

leaderboard_show::~leaderboard_show()
{
    delete ui;
}
