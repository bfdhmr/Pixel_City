#ifndef LD_USERINFO_H
#define LD_USERINFO_H
#include<string>
#include <QWidget>

namespace Ui {
class ld_userinfo;
}

class ld_userinfo : public QWidget
{
    Q_OBJECT

public:
    explicit ld_userinfo(QWidget *parent = nullptr);
    ~ld_userinfo();

    void setInfo(int num,std::string name,int score);

private:


    Ui::ld_userinfo *ui;
};

#endif // LD_USERINFO_H
