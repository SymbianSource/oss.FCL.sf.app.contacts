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

#include "cntviewparameters.h"
#include "t_cntviewparameters.h"


void TestCntViewParameters::initTestCase()
{

}

void TestCntViewParameters::create()
{
    CntViewParameters params(CntViewParameters::namesView, CntViewParameters::collectionView);
    QVERIFY(params.nextViewId() == CntViewParameters::namesView);
    QVERIFY(params.previousViewId() == CntViewParameters::collectionView);

    CntViewParameters secondParams(params);
    QVERIFY(secondParams.nextViewId() == params.nextViewId());
    QVERIFY(secondParams.previousViewId() == params.previousViewId());
}

void TestCntViewParameters::setNextViewId()
{
    //CntViewParameters params(CntViewParameters::namesView, CntViewParameters::collectionView);
    //params.setNextViewId(CntViewParameters::editView);
    //QVERIFY(params.nextViewId() == CntViewParameters::editView);
}

void TestCntViewParameters::setPreviousViewId()
{
    CntViewParameters params(CntViewParameters::namesView, CntViewParameters::collectionView);
    params.setPreviousViewId(CntViewParameters::editView);
    QVERIFY(params.previousViewId() == CntViewParameters::editView);
}

void TestCntViewParameters::selectedContact()
{
    CntViewParameters params(CntViewParameters::namesView, CntViewParameters::collectionView);
    params.setSelectedContact(mContact);
    QVERIFY(params.selectedContact() == mContact);

    params.setSelectedContact(mSecondContact);
    QVERIFY(params.selectedContact() == mSecondContact);
}

void TestCntViewParameters::selectedDetail()
{
    QContactDetail detail;
    QContactDetail otherDetail;
    CntViewParameters params(CntViewParameters::namesView, CntViewParameters::collectionView);
    params.setSelectedDetail(detail);
    QVERIFY(params.selectedDetail() == detail);

    params.setSelectedDetail(otherDetail);
    QVERIFY(params.selectedDetail() == otherDetail);
}

void TestCntViewParameters::selectedGroupContact()
{
    CntViewParameters params(CntViewParameters::namesView, CntViewParameters::collectionView);
    params.setSelectedGroupContact(mContact);
    QVERIFY(params.selectedGroupContact() == mContact);

    params.setSelectedGroupContact(mSecondContact);
    QVERIFY(params.selectedGroupContact() == mSecondContact);
}

void TestCntViewParameters::selectedAction()
{
    CntViewParameters params(CntViewParameters::namesView, CntViewParameters::collectionView);
    params.setSelectedAction("test");
    QVERIFY(params.selectedAction() == "test");
}

void TestCntViewParameters::cleanupTestCase()
{

}

// EOF
