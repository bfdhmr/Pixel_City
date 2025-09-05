#include "ld_userinfo.h"
#include "ui_ld_userinfo.h"
#include<string>

ld_userinfo::ld_userinfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ld_userinfo)
{
    ui->setupUi(this);
}

ld_userinfo::~ld_userinfo()
{
    delete ui;
}

void ld_userinfo::setInfo(int num, std::string name, int score)
{
    QString str = "第%1名";
    str.arg(num);
    ui->lb_num->setText(str);
    ui->lb_name->setText(name.c_str());
    ui->lb_score->setText(QString::number(score));
}
