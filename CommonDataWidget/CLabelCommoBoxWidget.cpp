﻿/*****************************************************
  * Copyright: 万孚生物
  * Author: 刘青
  * Date: 2017-7-9
  * Description: 复合组合控件Label和CommoBox
  * -------------------------------------------------------------------------
  * History:
  *
  *
  *
  * -------------------------------------------------------------------------
  ****************************************************/
#include "CLabelCommoBoxWidget.h"
#include <QBoxLayout>
CLabelCommoBoxWidget::CLabelCommoBoxWidget(QWidget *parent)
    : QWidget(parent)
{
    m_pLabel = new QLabel(this);
    m_pComboBox = new QComboBox(this);
    _InitLayout();
}

CLabelCommoBoxWidget::CLabelCommoBoxWidget(QString strLabel, QStringList strComboList, QWidget *parent)
    : QWidget(parent)
{
    m_pLabel = new QLabel(strLabel, this);
    m_pComboBox = new QComboBox(this);
    m_pComboBox->setMaximumWidth(180);
    connect(m_pComboBox, SIGNAL(currentIndexChanged(int)),  this, SIGNAL(SignalCurrentSelectChange(int)));
    if(strComboList.count())
    {
        m_pComboBox->addItems(strComboList);
        m_pComboBox->setCurrentIndex(0);
    }
    _InitLayout();
}

QString CLabelCommoBoxWidget::GetCurrentSelectText()
{
    m_strCurrentSelectText = m_pComboBox->currentText();
    return m_strCurrentSelectText;
}

int CLabelCommoBoxWidget::GetCurrentSelectIndex()
{
    return m_pComboBox->currentIndex();
}

void CLabelCommoBoxWidget::SetCupType(QStringList strCupList)
{
    if(m_pComboBox != NULL)
    {
        m_pComboBox->clear();
        if(strCupList.count() > 0)
        {
            m_pComboBox->addItems(strCupList);
            m_pComboBox->setCurrentIndex(0);
        }
    }
}

void CLabelCommoBoxWidget::SetCurrentIndex(int /*iIndex*/)
{
    m_pComboBox->setCurrentIndex(0);
}

void CLabelCommoBoxWidget::SetCurrentText(QString strCupType)
{
    m_pComboBox->setCurrentText(strCupType);
}

void CLabelCommoBoxWidget::AddItemText(QString strText)
{
    m_pComboBox->addItem(strText);
}

void CLabelCommoBoxWidget::_InitLayout()
{
    QHBoxLayout *pLayout = new QHBoxLayout;
    pLayout->setMargin(0);
    pLayout->addStretch(1);
    pLayout->addWidget(m_pLabel);
    pLayout->addWidget(m_pComboBox);
    this->setLayout(pLayout);
}
