﻿/*****************************************************
  * Copyright: 万孚生物
  * Author: 刘青
  * Date: 2017-7-15
  * Description: 实现
  * -------------------------------------------------------------------------
  * History:
  *
  *
  *
  * -------------------------------------------------------------------------
  ****************************************************/
#ifndef CSETTINGSETUPWINDOWWIDGET_H
#define CSETTINGSETUPWINDOWWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QGroupBox>
#include "CommonDataWidget/CLabelLineEditWidget.h"

class CSettingSetupWindowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSettingSetupWindowWidget(QWidget *parent = 0);

signals:
    void SignalReadFirmwareVersion();// 发送读取硬件版本信号
    void SignalReadUserTimes();// 发送仪器使用次数信号
    //
    void SignalAutoConnetPis(bool bAuto);
    void SignalAutoConnetPoct(bool bAuto);
public slots:
    void SlotGetFirmwareVersion(QString);// 获得硬件版本信息
    void SlotGetUserTimes(int);// 获得仪器使用次数信号


protected slots:
    void showEvent(QShowEvent *);
private slots:
    //
    void _PoctIP01TextChange();
    void _PoctIP02TextChange();
    void _PoctIP03TextChange();
    void _PoctIP04TextChange();
    void _SlotPoctServerPortLineEditChage();
    void _SlotOKButton();
    void _SlotCancel();

public:
    QString GetPoctServer();// 返回服务器
    int GetPoctPort();//
private:
    QGroupBox *_CreateServerGroup();
    QGroupBox *_CreateReportTitleGroup();
    QGroupBox *_CreateLanguageGroup();
    QGroupBox *_CreateAboutGroup();
    //
    void _InitWidget();
    void _InitLayout();
    //
    void _ChangeToEnglish();// 切换至英文版本
    void _ChangeToSpain();// 切换至西班牙版本
    //
    void _InitConfigDate();
    void _SaveConfigDate();

private:
    //
    QLabel *m_pPoctLabel;
    QLabel *m_pPoctServerIPLabel;
    QLabel *m_pPoctServerPortLabel;
    QLineEdit *m_pPoctIP01LineEdit;
    QLineEdit *m_pPoctIP02LineEdit;
    QLineEdit *m_pPoctIP03LineEdit;
    QLineEdit *m_pPoctIP04LineEdit;
    QLineEdit *m_pPoctServerPortLineEdit;
    QCheckBox *m_pPoctAutoConnectCBox;
    // language
    QRadioButton *m_pEnglishRButton;
    QRadioButton *m_pSpainRButton;
    // about
    QWidget *pPointWidget01;
    QWidget *pPointWidget02;
    QWidget *pPointWidget03;
    QWidget *pPointWidget04;
    QLabel *m_pSoftwareReleaseLabel;
    QLabel *m_pSoftwareFullLabel;
    QLabel *m_pFirmewareLabel;
    QLabel *m_pUserTimesLabel;

    QLabel *m_pSoftwareReleaseLabelValue;
    QLabel *m_pSoftwareFullLabelValue;
    QLabel *m_pFirmewareLabelValue;
    QLabel *m_pUserTimesLabelValue;
    //
    QPushButton *m_pOKButton;
    QPushButton *m_pCancleButton;
    // 服务器
//    QString m_strPisServer;
    QString m_strPoctServer;

    //tips
    QString strConnectSuccess;
    QString strConnectFaile;
    QString strSendToPisSuccess;
    QString strSendToPisFaile;

    //tips
    QString strPoctConnectSuccess;
    QString strPoctConnectFaile;
    QString strSendToPoctSuccess;
    QString strSendToPoctFaile;

    //Report Title
    QRadioButton *m_pReportTitleDefaultButton;
    QRadioButton *m_pReportTitleDefinedButton;
    QLineEdit *m_pDefinedLineEdit;
};

#endif // CSETTINGSETUPWINDOWWIDGET_H
