#include "fiveinline.h"
#include "ui_fiveinline.h"

#include <QPainter>
#include<QPoint>
#include<QEvent>
#include<QMessageBox>

FiveInLine::FiveInLine(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FiveInLine)
    , m_board( FIL_COLS , std::vector<int>(FIL_ROWS,0) ) //vector 有参构造 : 长度 ,  初值是什么
    ,m_movePoint(0,0),m_moveFlag(false),m_blackorwhite(1)
    ,m_isOver(false),m_status(Black)
{
    ui->setupUi(this);

    InitAiVector();

    m_pieceColor[None] = QBrush(QColor(0,0,0,0)); // 最后一个参数表示透明度
    m_pieceColor[Black] = QBrush(QColor(0,0,0));
    m_pieceColor[White] = QBrush(QColor(255,255,255));


//    m_board[0][0] = Black;
//    m_board[2][2] = White;

    connect( &m_timer , SIGNAL(timeout()) , this , SLOT(repaint()));
    //repaint()  会触发重绘
    m_timer.start(1000/25);// 1秒25次

    //落子和落子处理 连接  通过kernel转发了 不能再用了
    connect(this,SIGNAL(SIG_pieceDown(int,int,int)),this,SLOT(slot_pieceDown(int,int,int)));

    //slot_startGame();
    clear();
}

FiveInLine::~FiveInLine()
{
    m_timer.stop();
    delete ui;
}

//重绘事件:绘图背景 棋盘 棋子等
void FiveInLine::paintEvent(QPaintEvent *event)
{
    //绘制棋盘
    QPainter painer(this);//QPainter用于绘图 , 使用有参构造 , 传入当前对象 , 得到当前控件的画布
    painer.setBrush( QBrush ( QColor(255,160,0) ) );
    painer.drawRect( FIL_MARGIN_WIDTH - FIL_DISTANCE
                     ,FIL_MARGIN_HEIGHT - FIL_DISTANCE
                     ,( FIL_COLS + 1 ) * FIL_SPACE + 2 * FIL_DISTANCE
                     ,( FIL_ROWS + 1 ) * FIL_SPACE + 2 * FIL_DISTANCE);

    //绘制网格线
    // |
    for(int i = 1; i <= FIL_COLS ; ++i)
    {
        painer.setBrush(QBrush(QColor(0,0,0)));
        QPoint p1( FIL_MARGIN_WIDTH + FIL_SPACE * i, FIL_MARGIN_HEIGHT + FIL_SPACE );
        QPoint p2( FIL_MARGIN_WIDTH + FIL_SPACE * i, FIL_MARGIN_HEIGHT + (FIL_ROWS) * FIL_SPACE );
        painer.drawLine( p1 , p2 );
    }

    // --
    for( int i = 1;i <= FIL_ROWS ; ++i )
    {
        painer.setBrush(QBrush(QColor(0,0,0)));
        QPoint p1( FIL_MARGIN_WIDTH + FIL_SPACE , FIL_MARGIN_HEIGHT + FIL_SPACE * i );
        QPoint p2( FIL_MARGIN_WIDTH + (FIL_COLS) * FIL_SPACE , FIL_MARGIN_HEIGHT + FIL_SPACE * i );
        painer.drawLine( p1 , p2 );
    }

    //画中心点
    painer.setBrush(QBrush(QColor(0,0,0)));//通过画刷设置颜色
    painer.drawEllipse(
                QPoint(290,290),6/2,6/2
                );// 画圆  中心点坐标  半径x , 半径y


    //画棋子
    //x,y 点对应的颜色 -->  m_pieceColor[m_board[x][y]]
    for( int x = 0; x < FIL_COLS; ++x)
    {
        for(int y = 0; y < FIL_ROWS; ++y)
        {
            if(m_board[x][y] != None ){
                painer.setBrush(m_pieceColor[m_board[x][y]]);
                painer.drawEllipse(
                    QPoint(FIL_MARGIN_WIDTH + FIL_SPACE + FIL_SPACE * x, FIL_MARGIN_HEIGHT + FIL_SPACE + FIL_SPACE *y),
                           FIL_PIECE_SIZE / 2 , FIL_PIECE_SIZE / 2
                            );// 画圆  中心点坐标 , 半径x , 半径y
            }
        }
    }

    if(m_moveFlag){
        //m_movePoint
        painer.setBrush(m_pieceColor[getBlackOrWhite()]); // 通过画刷设置颜色
        painer.drawEllipse(
            QPoint(m_movePoint.x(),
                   m_movePoint.y()),
                   FIL_PIECE_SIZE / 2 , FIL_PIECE_SIZE / 2
                    );// 画圆  中心点坐标 , 半径x , 半径y
    }

    event->accept();
}

//鼠标点击事件，出现棋子
void FiveInLine::mousePressEvent(QMouseEvent *event)
{
    //捕捉点 -> 相对坐标
    m_movePoint = event->pos();
    ///位置坐标
    /// 相对坐标(相对窗口的)
    /// QMouseEvent ::pos();鼠标位置
    /// 绝对坐标
    /// QCursor ::pos();鼠标位置
    /// QMouseEvent ::globalPos();
    if(m_isOver)goto quit;
    if(m_status!=getBlackOrWhite())goto quit;
    m_moveFlag = true;
quit:
    event->accept();
}

//鼠标移动事件，棋子跟着移动
void FiveInLine::mouseMoveEvent(QMouseEvent *event)
{
    //捕捉点 -> 相对坐标
    if( m_moveFlag )
    m_movePoint = event->pos();

    event->accept();
}

//鼠标释放事件,棋子落子
void FiveInLine::mouseReleaseEvent(QMouseEvent *event)
{
    m_moveFlag = false;

    int x = 0;
    int y = 0;
    float fx = (float)event->pos().x();
    float fy = (float)event->pos().y();
    if(m_isOver)goto quit;
    if(m_status!=getBlackOrWhite())goto quit;
    fx = (fx - FIL_MARGIN_WIDTH - FIL_SPACE)/FIL_SPACE;
    fy = (fy - FIL_MARGIN_HEIGHT - FIL_SPACE)/FIL_SPACE;

    x = (fx - (int)fx)>0.5?((int)fx+1):(int)fx;
    y = (fy - (int)fy)>0.5?((int)fy+1):(int)fy;

    if(isCrossLine(x,y))return;
    Q_EMIT SIG_pieceDown(getBlackOrWhite(),x,y);
quit:
    event->accept();
}

//判断是否出界
bool FiveInLine::isCrossLine(int x, int y)
{
    if(x < 0|| x >= 15 || y < 0|| y >= 15)return true;
    return false;
}

bool FiveInLine::isWin(int x, int y)
{
    //看四条直线 八个方向的同色棋子个数 只要有一个个数数到5 就结束了
    int count = 1;
    //循环 看四条线
    for(int dr = d_z;dr<d_count;dr+=2){
        //先看一条线
        //<-  ->
        //<-
        for(int i = 1;i <=4 ;++i)
        {
            //获取偏移后的棋子坐标
            int ix = dx[dr]*i+x;
            int iy = dy[dr]*i+y;
            //判断是否出界
            if(isCrossLine(ix,iy))break;
            //看是否同色
            if(m_board[ix][iy]==m_board[x][y])
            {
                count++;
            }else break;
        }
        //->
        for(int i = 1;i <=4 ;++i)
        {
            //获取偏移后的棋子坐标
            int ix = dx[dr+1]*i+x;
            int iy = dy[dr+1]*i+y;
            //判断是否出界
            if(isCrossLine(ix,iy))break;
            //看是否同色
            if(m_board[ix][iy]==m_board[x][y])
            {
                count++;
            }else break;
        }
        if(count>=5)break;
        else count=1;
    }
    if(count >= 5)
    {
        //结束
        m_isOver = true;
        return true;
    }
    return false;
}

void FiveInLine::clear()
{
    //默认不开ai
    setCpuColor(None);
    //清空赢法棋子个数统计
    for(int i = 0;i < m_vecWin.size();++i)
    {
        m_vecWin[i].clear();
    }
    //状态位
    m_isOver = true;
    m_blackorwhite = Black;
    m_moveFlag = false;

    //界面
    for(int x = 0;x < FIL_COLS;++x)
    {
        for(int y = 0;y < FIL_ROWS;++y)
        {
            m_board[x][y] = None;
        }
    }
    ui->lb_winner->setText("获胜者: ???");
    ui->lb_color->setText("先手落子");
}

void FiveInLine::setSelfStatus(int _status)
{
    m_status = _status;
}

int FiveInLine::getBlackOrWhite() const
{
    return m_blackorwhite;
}

void FiveInLine::changeBlackAndWhite()
{
    m_blackorwhite = m_blackorwhite+1;
    if(m_blackorwhite==3)
        m_blackorwhite = Black;

    if(m_blackorwhite==Black)
        ui->lb_color->setText("先手落子");
    else
        ui->lb_color->setText("后手落子");
}

void FiveInLine::slot_pieceDown( int blackorwhite , int x,int y)
{
    if(blackorwhite != getBlackOrWhite())return;

    if(m_board[x][y]==None)
    {
        m_board[x][y] = blackorwhite;

        //更新该点棋子颜色后 , 判断输赢
        if( isWin(x , y) )
        {
            QString str = getBlackOrWhite() == Black?"获胜:先手胜":"获胜:后手胜";
            ui->lb_winner->setText(str);

            Q_EMIT SIG_playerWin(getBlackOrWhite());
            //QMessageBox::about(this , "胜负已分" , str);
        }else{
            if(m_cpuColor!=getBlackOrWhite()){
            //更新玩家每种赢法 玩家的棋子个数
                for(int i = 0;i < m_vecWin.size();i++)
                {
                    if(m_vecWin[i].board[x][y]==1){
                        m_vecWin[i].playerCount+=1;
                        m_vecWin[i].cpuCount+=100;
                    }
                }
            }
            else{
                for(int k = 0; k < m_vecWin.size();++k)
                {
                    if(m_vecWin[k].board[x][y] ==1 )
                    {
                        m_vecWin[k].cpuCount += 1;
                        m_vecWin[k].playerCount = 100;
                    }
                }
            }
            //更换回合
            changeBlackAndWhite();
            //判断是电脑的回合 电脑下棋
            if(m_cpuColor==getBlackOrWhite())
                pieceDownByCpu();
        }
    }

}

void FiveInLine::slot_startGame()
{
    clear();
    m_isOver = false;
}

/*电脑AI*/
//初始化所有赢法棋子布局
void FiveInLine::InitAiVector()
{
    //   -- 赢法
    for(int y=0; y< FIL_ROWS;y++)
    {
        for(int x = 0; x < FIL_COLS-4;x++)
        {
            stru_win w;
            for(int k = 0;k < 5;k++){
                w.board[x+k][y] = 1;
            }
            m_vecWin.push_back(w);
        }

    }
    //   |  赢法
    for(int x=0; x< FIL_COLS;x++)
    {
        for(int y = 0; y < FIL_ROWS-4;y++)
        {
            stru_win w;
            for(int k = 0;k < 5;k++){
              w.board[x][y+k] = 1;
            }
            m_vecWin.push_back(w);
        }

    }
    //   /  赢法
    for(int x = FIL_COLS -1;x >=4;x--)
    {
        for(int y = 0;y < FIL_ROWS -4;y++)
        {
            stru_win w;
            for(int k = 0; k <5;k++)
            {
                w.board[x-k][y+k] = 1;
            }
            m_vecWin.push_back(w);
        }
    }
    //   \  赢法
    for(int x = 0; x < FIL_COLS-4;x++){
        for(int y = 0;y < FIL_ROWS-4;y++)
        {
            stru_win w;
            for(int k = 0; k < 5;k++)
            {
                w.board[x+k][y+k] = 1;
            }
            m_vecWin.push_back(w);
        }
    }
}

//电脑落子
void FiveInLine::pieceDownByCpu()
{
    if(m_isOver)return;

    if(m_cpuColor!=getBlackOrWhite())return;
    int x=0,y=0,k=0;
    int u=0,v=0;    //最优目标坐标
    int MyScore[FIL_COLS][FIL_ROWS] = {};//记录玩家所有位置的分数
    int CpuScore[FIL_COLS][FIL_ROWS] = {};//记录电脑所有位置的分数
    int max = 0;//最高分数

    //估分 找到没有子的点 , 看每种赢法(有该点) , 对应的棋子个数 , 进行这个点的分数计算
    for(x = 0;  x < FIL_COLS ; ++x)
    {
        for(y = 0; y <FIL_ROWS ; ++y)
        {
            if(m_board[x][y] == None)
            {
                //遍历所有赢法
                for(k = 0;k < m_vecWin.size();++k)
                {
                    //评价玩家在 x , y 分数
                    if(m_vecWin[k].board[x][y]==1)
                    {
                        switch (m_vecWin[k].playerCount) {
                        case 1: MyScore[x][y] += 220;
                            break;
                        case 2: MyScore[x][y] += 420;
                            break;
                        case 3: MyScore[x][y] += 2000;
                            break;
                        case 4: MyScore[x][y] += 10000;
                            break;
                        }
                    }
                    //评价电脑在 x , y 分数
                    if(m_vecWin[k].board[x][y]==1)
                    {
                        switch (m_vecWin[k].cpuCount) {
                        case 1: CpuScore[x][y] += 200;
                            break;
                        case 2: CpuScore[x][y] += 400;
                            break;
                        case 3: CpuScore[x][y] += 4100;
                            break;
                        case 4: CpuScore[x][y] += 20000;
                            break;
                        }

                    }
                }
            }
        }
    }
    //估分之后找最优点
    for(x = 0 ; x < FIL_COLS; ++x)
    {
        for ( y = 0 ; y < FIL_ROWS ; ++y ) {
            if( m_board[x][y] == None ) {
                //先看玩家 MyScore看做是防守 CpuScore看做是进攻
                if( MyScore[x][y] > max ) {
                    max = MyScore[x][y];
                    u = x;
                    v = y;
                }else if( MyScore[x][y] == max ) {
                    if( CpuScore[x][y] > CpuScore[u][v] ) {
                        u = x;
                        v = y;
                    }
                }
                //再看电脑
                if( CpuScore[x][y] > max ) {
                    max = CpuScore[x][y];
                    u = x;
                    v = y;
                }else if(CpuScore[x][y]==max ) {
                    if( MyScore[x][y]>MyScore[u][v] ) {
                        u = x;
                        v = y;
                    }
                }
            }
        }
    }
    //统计个数要更新
//    for( k = 0; k < m_vecWin.size();++k)
//    {
//        if(m_vecWin[k].board[u][v] ==1 )
//        {
//            m_vecWin[k].cpuCount += 1;
//            m_vecWin[k].playerCount = 100;
//        }
//    }
    //得到最优目标坐标
    _sleep(1000);
    Q_EMIT SIG_pieceDown(getBlackOrWhite(),u,v);
}

void FiveInLine::setCpuColor(int newCpuColor)
{
    m_cpuColor = newCpuColor;
}
