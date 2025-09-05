TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt



SOURCES += \
        src/skiplist.cpp \
        src/leaderboard.cpp \
        src/Mysql.cpp \
        src/TCPKernel.cpp \
        src/Thread_pool.cpp \
        src/block_epoll_net.cpp \
        src/clogic.cpp \
        src/err_str.cpp \
        src/main.cpp

HEADERS += \
    include/Mysql.h \
    include/TCPKernel.h \
    include/Thread_pool.h \
    include/block_epoll_net.h \
    include/clogic.h \
    include/err_str.h \
    include/my_mutex.h \
    include/packdef.h \
    include/skiplist.h \
    include/leaderboard.h

INCLUDEPATH += ./include

LIBS += -lpthread -lmysqlclient
