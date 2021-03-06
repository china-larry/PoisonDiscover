﻿#include "CHidCmdThread.h"
#include <QApplication>
#include <QTime>
#include "HidOpertaionUtility.h"
#include "ProtocolUtility.h"
#include <QDebug>

CHidCmdThread* CHidCmdThread::sm_pHidCmdThreadInstance = NULL;

CHidCmdThread::CHidCmdThread()
{
    m_bStopped = true;
    connect(HIDOpertaionUtility::GetInstance(), SIGNAL(SignalOperationComplete(quint16,bool)),this,
            SLOT(_SlotHIDCmdComplete(quint16,bool)));

    m_curHIDCmdData.iCmdType = ProtocolUtility::sm_kiCmdDevClose;
}

CHidCmdThread::~CHidCmdThread()
{
    disconnect(HIDOpertaionUtility::GetInstance(), SIGNAL(SignalOperationComplete(quint16,bool)),this,
                                                       SLOT(_SlotHIDCmdComplete(quint16,bool)));
    if(NULL != sm_pHidCmdThreadInstance)
    {
        delete sm_pHidCmdThreadInstance;
    }
}

CHidCmdThread *CHidCmdThread::GetInstance()
{
    if(NULL == sm_pHidCmdThreadInstance)
    {
        sm_pHidCmdThreadInstance = new CHidCmdThread();
    }

    return  sm_pHidCmdThreadInstance;
}

/**
* @brief   ClearCmd
* @param   清空HID命令
* @return
*/
void CHidCmdThread::ClearCmd()
{
    m_qCmdMutex.lock();
    m_hidCmdDataQueue.clear();
    m_qCmdMutex.unlock();
}


/**
* @brief   AddCmd
* 添加HID命令
* @param   HIDCmdData
* @return
*/
void CHidCmdThread::AddCmd(HIDCmdData sHidCmdData)
{
    m_qCmdMutex.lock();
    //停止命令需要插队执行,其他命令清空
    if(ProtocolUtility::sm_kiCmdDevClose == sHidCmdData.iCmdType)
    {
        HIDOpertaionUtility::GetInstance()->HIDClose();
        SetStopped(true);
        m_hidCmdDataQueue.clear();
        m_curHIDCmdData = sHidCmdData;
    }
    else
    {
        m_hidCmdDataQueue.enqueue(sHidCmdData);
    }

    m_qCmdMutex.unlock();
}

/**
* @brief   AddResetMotorCmd
* 添加马达复位命令
* @param   iResetSpeed 速度
* @return
*/
void CHidCmdThread::AddResetMotorCmd(quint16 iResetSpeed)
{
    HIDCmdData sHidCmdData;
    sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdResetMotor;
    sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetMotorResetCmd(iResetSpeed));
    AddCmd(sHidCmdData);
}

/**
* @brief   AddRotateMotorCmd
* 添加马达转动命令
* @param   iSpeed 速度
* @param   iStep 电机步数
* @param   iDirection 方向
* @return
*/
void CHidCmdThread::AddRotateMotorCmd(quint16 iSpeed, quint16 iStep, quint16 iDirection)
{
    HIDCmdData sHidCmdData;
    sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdRotateMotor;
    sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetMotorRotateCmd(iDirection, iStep, iSpeed));
    AddCmd(sHidCmdData);
}

/**
* @brief   AddCloseHIDCmd
* 添加关闭HID通信命令
* @return
*/
void CHidCmdThread::AddCloseHIDCmd()
{
    //添加关闭HID通信命令，杀死HID读线程
    HIDCmdData sHidCmdData;
    sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdDevClose;
    AddCmd(sHidCmdData);
}

/**
* @brief   AddOpenLedCmd
* 添加开灯命令
* @param   iLedIndex 灯序号
* @param   iBrightness 灯光值
* @return
*/
void CHidCmdThread::AddOpenLedCmd(int iLedIndex, quint16 iBrightness)
{
    HIDCmdData sHidCmdData;
    sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetLEDCmd(iLedIndex,iBrightness));
    sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdOpenOrCloseLed;
    CHidCmdThread::GetInstance()->AddCmd(sHidCmdData);
}

/**
* @brief   AddWriteDevParamsCmd
* 添加写取设备参数命令
* @param   DevConfigParams 仪器参数
* @return
*/
void CHidCmdThread::AddWriteDevParamsCmd(DevConfigParams sDevConfigParams)
{
    m_qCmdMutex.lock();
    m_devConfigParams = sDevConfigParams;
    m_qCmdMutex.unlock();
    AddCmdWithoutCmdData(ProtocolUtility::sm_kiCmdWriteParamToDev);
}

/**
* @brief   AddReadDevParamsCmd
* 添加读取设备参数命令
* @return
*/
void CHidCmdThread::AddReadDevParamsCmd()
{
    //添加读取设备参数命令
    AddCmdWithoutCmdData(ProtocolUtility::sm_kiCmdReadParamFromDev);
}

/**
* @brief  AddCmdWithoutCmdData
* 添加不带命令数据的命令
* @param iCmdType 命令类型
* @return
*/
void CHidCmdThread::AddCmdWithoutCmdData(quint16 iCmdType)
{
    HIDCmdData sHidCmdData;
    switch(iCmdType)
    {
        case ProtocolUtility::sm_kiCmdCloseAllLed://添加关灯命令
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdCloseAllLed;
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetCloseAllLEDCmd());
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdCloseAllLedAndStopMotor://添加关灯关马达命令
        {
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetCloseAllLEDAndStopMotorCmd());
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdCloseAllLedAndStopMotor;
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdReadDevVersion://添加读下位机版本命令
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdReadDevVersion;
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetReadVersionCmd());
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdReadParamFromDev://读取仪器参数
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdReadParamFromDev;
            sHidCmdData.byteArrayVect.clear();
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdWriteParamToDev://写仪器参数到设备
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdWriteParamToDev;
            sHidCmdData.byteArrayVect.clear();
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdAddTestCount://仪器测量次数加1
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdAddTestCount;
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetAddTestCountCmd());
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdReadTestCount://读取仪器测量次数
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdReadTestCount;
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetReadTestCountCmd());
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdClearTestCount://仪器测量次数清零
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdClearTestCount;
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetClearTestCountCmd());
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdUpgradeAppStart://仪器下位机程序升级开始
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdUpgradeAppStart;
            sHidCmdData.byteArrayVect.clear();
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdUpgradeAppFlag: //仪器下位机程序升级标志
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdUpgradeAppFlag;
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetUpgradeAppFlagCmd());
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdWriteSerialNumberToDev:
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdWriteSerialNumberToDev;
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetWriteSerialNumber(m_strDevSerialNumber));
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdReadSerialNumberFromDev:
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdReadSerialNumberFromDev;
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetReadSerialNumber());
            AddCmd(sHidCmdData);
            break;
        }
        case ProtocolUtility::sm_kiCmdWriteRotateCompenSeptToDev:
        {
            sHidCmdData.iCmdType = ProtocolUtility::sm_kiCmdWriteRotateCompenSeptToDev;
            sHidCmdData.byteArrayVect.push_back(ProtocolUtility::GetWriteRotateCompenSept(m_strRotateCompenSept));
            AddCmd(sHidCmdData);
            break;
        }

        default:
            break;
    }
}

/**
* @brief  AddUpgradeSubControlCmd
* 添加升级命令
* @param strFilePath 升级文件路径
* @return
*/
void CHidCmdThread::AddUpgradeSubControlCmd(QString strFilePath)
{
    m_qCmdMutex.lock();
    m_strFilePath = strFilePath;
    m_qCmdMutex.unlock();
    AddCmdWithoutCmdData(ProtocolUtility::sm_kiCmdUpgradeAppStart);
}

void CHidCmdThread::SetStopped(bool bStopped)
{
    m_qStoppedMutex.lock();
    m_bStopped = bStopped;
    m_qStoppedMutex.unlock();
}

bool CHidCmdThread::GetStopped()
{
    return m_bStopped;
}


/**
* @brief  AddWriteDevSerialNumber
* 添加写仪器序列号指令
* @param strDevSerialNumber 仪器序列号
* @return
*/
void CHidCmdThread::AddWriteDevSerialNumber(QString strDevSerialNumber)
{
    m_qCmdMutex.lock();
    this->m_strDevSerialNumber = strDevSerialNumber;
    m_qCmdMutex.unlock();
    AddCmdWithoutCmdData(ProtocolUtility::sm_kiCmdWriteSerialNumberToDev);
}

/**
* @brief  AddWriteDevSerialNumber
* 添加写电机补偿步数指令
* @param strRotateCompenSept 电机补偿步数
* @return
*/
void CHidCmdThread::AddWriteRotateCompenSept(QString strRotateCompenSept)
{
    m_qCmdMutex.lock();
    this->m_strRotateCompenSept = strRotateCompenSept;
    m_qCmdMutex.unlock();
    AddCmdWithoutCmdData(ProtocolUtility::sm_kiCmdWriteRotateCompenSeptToDev);
}

/**
* @brief  AddReadDevSerialNumber
* 添加读仪器序列号指令
* @return
*/
void CHidCmdThread::AddReadDevSerialNumber()
{
    //添加读仪器序列号指令
    AddCmdWithoutCmdData(ProtocolUtility::sm_kiCmdReadSerialNumberFromDev);
}

/**
* @brief  run
* 线程开始运行
* @return
*/
void CHidCmdThread::run()
{

    HIDOpertaionUtility::GetInstance()->HIDOpen();//默认打开设备
    m_bStopped = false;
    while(!m_bStopped)
    {
        if(m_hidCmdDataQueue.size()>0)
        {
            m_qCmdMutex.lock();
            m_curHIDCmdData = m_hidCmdDataQueue.dequeue();
            m_qCmdMutex.unlock();
            _SetCmdCompleted(false);
            //qDebug() << "m_curHIDCmdData.iCmdType = " << m_curHIDCmdData.iCmdType;
            if(ProtocolUtility::sm_kiCmdReadParamFromDev == m_curHIDCmdData.iCmdType)
            {
                //qDebug() << "HIDOpertaionUtility::GetInstance()->HIDReadDevParams();";
                HIDOpertaionUtility::GetInstance()->HIDReadDevParams();
            }
            else if(ProtocolUtility::sm_kiCmdWriteParamToDev == m_curHIDCmdData.iCmdType)
            {
                HIDOpertaionUtility::GetInstance()->HIDWriteDevParams(m_devConfigParams);
            }
            else if(ProtocolUtility::sm_kiCmdUpgradeAppStart == m_curHIDCmdData.iCmdType)
            {
                HIDOpertaionUtility::GetInstance()->HIDUpgradeSubControl(m_strFilePath);
            }
            else if(m_curHIDCmdData.byteArrayVect.size() == 1)
            {
//                qDebug() << __LINE__;
                HIDOpertaionUtility::GetInstance()->HIDWrite(m_curHIDCmdData.byteArrayVect.at(0));
            }
            QTime oldTime = QTime::currentTime();
            while(!m_bCmdCompleted)//等待命令完成
            {
                int iMsecs = oldTime.msecsTo(QTime::currentTime());
                if(iMsecs > s_kiCmdWaitTime)
                {
                    break;
                }
            }
        }
        qApp->processEvents();
        msleep(5);
    }

}

void CHidCmdThread::_SetCmdCompleted(bool bCmdCompleted)
{
    m_qCmdCompleteMutex.lock();
    m_bCmdCompleted = bCmdCompleted;
    m_qCmdCompleteMutex.unlock();
}

//HID命令结果返回信号处理槽
void CHidCmdThread::_SlotHIDCmdComplete(quint16 iCmdType, bool /*bResult*/)
{
    if(iCmdType == m_curHIDCmdData.iCmdType)
    {
        _SetCmdCompleted(true);
    }
}

