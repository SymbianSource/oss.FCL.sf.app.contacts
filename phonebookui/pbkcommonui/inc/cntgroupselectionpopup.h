/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef CNTGROUPSELECTIONPOPUP_H
#define CNTGROUPSELECTIONPOPUP_H

#include <QObject>
#include <QGraphicsLinearLayout>
#include <hbdialog.h>
#include <hblistview.h>
#include "qmobilityglobal.h"

class HbListView;
class HbSearchPanel;
class HbTextItem;
class HbListView;
class MobCntModel;

QTM_BEGIN_NAMESPACE
class QContactManager;
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntGroupSelectionPopup : public HbDialog
{
    Q_OBJECT

public:
    CntGroupSelectionPopup(QContactManager *manager, QContact *contact, QGraphicsItem *parent = 0);
    ~CntGroupSelectionPopup();

    bool saveNewGroup();
    bool saveOldGroup();
    void populateListOfContact();
    
#ifdef PBK_UNIT_TEST
public slots:
#else
private slots:
#endif
    void closeFind();
    void setFilter(const QString &filterString);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    HbListView              *mListView;
    HbSearchPanel           *mSearchPanel;
    HbTextItem              *mEmptyListLabel;
    QContactManager         *mContactManager;
    MobCntModel             *mCntModel;
    QContact                *mContact;
    HbWidget                *mContainerWidget;
    QGraphicsLinearLayout   *mContainerLayout;  
};

#endif // CNTGROUPSELECTIONPOPUP_H
