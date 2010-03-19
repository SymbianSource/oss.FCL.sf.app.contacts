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

#include <QtTest/QtTest>
#include <QObject>
#include <QStringListModel>
#include <qtcontacts.h>
#include <hblistview.h>

#include "cntdetailpopup.h"
#include "t_cntdetailpopup.h"

QTM_USE_NAMESPACE

void TestCntDetailPopup::initTestCase()
{
    QStringList list;
    mPopup = new CntDetailPopup(list);

    QVERIFY(mPopup->mListView);
    QVERIFY(mPopup->mListModel);
    QVERIFY(mPopup->mListModel->rowCount(QModelIndex()) == 6);

    delete mPopup;
    mPopup = 0;

    list << QContactNote::DefinitionName << QContactAvatar::SubTypeAudioRingtone << 
    QContactBirthday::DefinitionName << QContactOrganization::DefinitionName << QContactFamily::FieldSpouse;
    mPopup = new CntDetailPopup(list);

    QVERIFY(mPopup->mListView);
    QVERIFY(mPopup->mListModel);
    QVERIFY(mPopup->mListModel->rowCount(QModelIndex()) == 1);
    
    delete mPopup;
    mPopup = 0;
    list.clear();

    list << QContactNote::DefinitionName << QContactAvatar::SubTypeAudioRingtone << 
    QContactAnniversary::DefinitionName << QContactOrganization::FieldAssistantName << QContactFamily::FieldChildren;
    mPopup = new CntDetailPopup(list);

    QVERIFY(mPopup->mListView);
    QVERIFY(mPopup->mListModel);
    QVERIFY(mPopup->mListModel->rowCount(QModelIndex()) == 1);
}

void TestCntDetailPopup::listItemSelected()
{
    // take the first item
    QModelIndex index = mPopup->mListModel->index(0);

    mPopup->listItemSelected(index);
    QVERIFY(!mPopup->selectedDetail().isEmpty());
}

void TestCntDetailPopup::selectDetail()
{
    // only this case can be tested (popup closed without selecting anything)
    QString empty = CntDetailPopup::selectDetail(QStringList());
    QVERIFY(empty.isEmpty());
}

void TestCntDetailPopup::cleanupTestCase()
{
    delete mPopup;
}

// EOF
