#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <boost/bind.hpp>
#include "TcpClient.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include "Thread.h"
#include "Buffer.h"
#include "Logging.h"
#include "LogFile.h"
#include "codec.h"
#include "config.h"
#include "jaka_pick_and_place.h"
#include "agv.h"

using namespace jiazhi;
using namespace muduo;
using namespace muduo::net;

class RobotInterface : muduo::noncopyable
{
public:
    //机器人自身信息
    struct RobotInfo
    {
        u_char currentState; //当前状态
        u_char currentPower; //当前电量
    };
    enum CURRENT_STATE {
        IS_FREE = 0x00,
        IS_DEPOSITING = 0x01,
        IS_WITHDRAWING = 0x02,
        IS_CHARGING = 0x03,
        IS_CHECKING = 0x04,
        IS_WALKING = 0x05,
        IS_IN_WINDOW = 0x06,
        CHARGE_PREPARE = 0x07,
        CHARGE_FINISHED = 0x08,
        IS_IN_CAB = 0x09,
        IS_INITIALIZING = 0xF0,
        INITIALIZE_FINISH = 0xF1,
        NEED_INITIALIZING = 0xF2,
        DEPOSITE_WITHDRAW_ERROR = 0xE0,
        MECHANICAL_ERROR = 0xE1,
        EMERGENCY_STOP = 0xE2
    };
    //外部设备信息
    struct ExternalInfo
    {
        std::set<int64_t> readyCab; //准备就绪的档案柜
        bool singleArchiveFinishedReceived; //单次动作完成被成功接收
        bool withdrawCheckReceived; //取档校验外部接收
        bool withrdrawCheckResult; //取档校验外部结果
    };
    // 任务类型
    enum TASK_TYPE {
        HEAR_BEAT = 0x33,
        DEPOSIT_TASK = 0x01,
        WITHDRAW_TASK = 0x02,
        CHARGE_TASK = 0x03,
        DEPOSIT_CHECK = 0x04,
        INQUIRE = 0x05,
        WITHDRAW_CHECK = 0x06,
        DEPOSIT_PREPARE_TASK = 0x07,
        SINGLE_ARCHIVE_FINISH = 0x08,
        CANCEL_TASK = 0x09,
        CAB_STATE = 0x0A
    };
    RobotInterface(std::string path);
    void eventLoopThread(); //收发消息及定时器函数线程
    void execTaskThread(); //执行任务函数线程
    void write(const StringPiece& message); //往Buffer里写数据
    void onCompleteMessage(const muduo::net::TcpConnectionPtr&,
                         const muduo::string& message,
                         Timestamp); //收到完整指令回调，并执行对应任务
    void onConnection(const TcpConnectionPtr& conn); //连接回调

private:
    const Config config_;
    JAKAPickAndPlace pickAndPlace_;
    AGV agv_;

    string taskServerIP_;
    uint16_t taskServerPort_;

    LengthHeaderCodec codec_;
    MutexLock connectionMutex_;
    TcpConnectionPtr connection_ GUARDED_BY(connectionMutex_);

    MutexLock taskMessageMutex_;
    StringPiece taskMessage_ GUARDED_BY(taskMessageMutex_);
    Condition taskCondition_ GUARDED_BY(taskMessageMutex_);

    MutexLock robotInfoMutex_;
    RobotInfo robotInfo_ GUARDED_BY(robotInfoMutex_);

    MutexLock externalInfoMutex_;
    ExternalInfo externalInfo_ GUARDED_BY(externalInfoMutex_);
    Condition externalCondition_ GUARDED_BY(externalInfoMutex_);

    static const char powerLowerLimit = 0x1c; //电量下限28
    static const char powerHighLimit = 0x60; //电量上限96
    static const std::size_t temPositionTotalNum = 5; //背篓总位置数
};

#endif