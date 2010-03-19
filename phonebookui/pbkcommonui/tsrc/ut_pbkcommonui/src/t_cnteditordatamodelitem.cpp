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

#include "cnteditordatamodelitem.h"
#include "t_cnteditordatamodelitem.h"
#include <hbinputstandardfilters.h>

void TestCntEditorDataModelItem::initTestCase()
{
    mNote = new QContactNote();
	
    mLabelItem = new CntEditorDataModelItem(*mNote, QContactNote::FieldNote, 1500, 
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1));

    mEmail = new QContactEmailAddress;
    mModel = new QStandardItemModel();
	mComboItem = new CntEditorDataModelItem(*mEmail, QContactEmailAddress::FieldEmailAddress, mModel, 
                    HbDataFormModelItem::CustomItemBase);

	mButtonItem = new CntEditorDataModelItem(*mEmail, tr("Add email address"), 
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+2));
}

void TestCntEditorDataModelItem::contactDetail()
{
    QVERIFY(mLabelItem->contactDetail().definitionName() == QContactNote::DefinitionName);
    QVERIFY(mComboItem->contactDetail().definitionName() == QContactEmailAddress::DefinitionName);
}

void TestCntEditorDataModelItem::key()
{
    QVERIFY(mLabelItem->key() == QContactNote::FieldNote);
    QVERIFY(mComboItem->key()  == QContactEmailAddress::FieldEmailAddress);
}

void TestCntEditorDataModelItem::buttonLabel()
{
    QVERIFY(mButtonItem->buttonLabel() == tr("Add email address"));
}

void TestCntEditorDataModelItem::fieldModel()
{
    QVERIFY(mComboItem->fieldModel() == mModel);
}

void TestCntEditorDataModelItem::maxLength()
{
    QVERIFY(mLabelItem->maxLength() == 1500);
}

void TestCntEditorDataModelItem::inputFilter()
{
    mComboItem->setTextFilter(HbEmailAddressFilter::instance());
    QVERIFY(mLabelItem->inputFilter() == 0);
    QVERIFY(mComboItem->inputFilter() != 0);
}

void TestCntEditorDataModelItem::cleanupTestCase()
{
    delete mLabelItem;
    mLabelItem = 0;
	delete mButtonItem;
	mButtonItem = 0;
	delete mComboItem;
	mComboItem = 0;
	delete mModel;
	mModel = 0;
	delete mNote;
	mNote = 0;
	delete mEmail;
	mEmail = 0;
}


// EOF
