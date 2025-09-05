#include<skiplist.h>
#include<unordered_map>
#include<Mysql.h>
#include<mutex>

struct CPlayerInfo{
    int id;
    int score;
    char name[_MAX_SIZE];
    int win;//胜场
    int all;//总场
    CPlayer* add_skip;
};

class leaderboard{
public:
    leaderboard();
    ~leaderboard();

    void UpdataHash(CPlayerInfo* player);

    //更新整个排行榜
    CPlayer* UpdataBoard(int id, int score,char name[]);
    void UpdataBoard(int idA,int scoreA,int idB,int scoreB);

private:
    SkipList m_ldlst;
    std::unordered_map<int,CPlayerInfo*> m_ldboard;
    pthread_mutex_t m_Hashmutex;
};
