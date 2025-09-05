#include<leaderboard.h>
#include<string>


leaderboard::leaderboard()
{
}

leaderboard::~leaderboard()
{
    // 只释放哈希表中存储的CPlayerInfo对象
    for (auto& pair : m_ldboard) {
        delete pair.second;
    }
    //清空哈希表
    m_ldboard.clear();
}

void leaderboard::UpdataHash(CPlayerInfo *player)
{
    m_ldboard[player->id] = player;
}

CPlayer* leaderboard::UpdataBoard(int id, int score, char name[])
{
    //创建结点
    CPlayer* tmp_p = new CPlayer;
    tmp_p->id = id;
    strcpy(tmp_p->name,name);
    tmp_p->score = score;

    //加入跳表
    m_ldlst.insert(*tmp_p);

    return tmp_p;
}


void leaderboard::UpdataBoard(int idA, int scoreA, int idB, int scoreB)
{
    pthread_mutex_lock( &m_Hashmutex );
    CPlayerInfo* p1 = m_ldboard[idA];
    CPlayerInfo* p2 = m_ldboard[idB];
    m_ldlst.deleteNode(*p1->add_skip);
    m_ldlst.deleteNode(*p2->add_skip);
    p1->win++;
    p1->score+=scoreA;
    p1->all++;
    p2->all++;
    p2->score+=scoreB;
    p1->add_skip->score = p1->score;
    p2->add_skip->score = p2->score;
    m_ldlst.insert(*p1->add_skip);
    m_ldlst.insert(*p2->add_skip);
    pthread_mutex_unlock( &m_Hashmutex );
}
