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
#include "cnteditorfactory.h"
#include "cntviewmanager.h"
#include "cntnameeditormodel.h"
#include "cntphonenumbermodel.h"
#include "cntphonenumberviewitem.h"
#include "cntemaileditormodel.h"
#include "cntemaileditorviewitem.h"
#include "cntaddressmodel.h"
#include "cntaddressviewitem.h"
#include "cnturleditormodel.h"
#include "cnturleditorviewitem.h"
#include "cntnoteeditormodel.h"
#include "cntnoteeditorviewitem.h"
#include "cntdateeditormodel.h"
#include "cntdateeditorviewitem.h"
#include "cntcompanyeditormodel.h"
#include "cntfamilyeditormodel.h"
#include "cntdetaileditor.h"

CntEditorFactory::CntEditorFactory(CntViewManager* aMgr, QObject* aParent) :
    QObject(aParent), iViewMgr(aMgr)
{
}

CntEditorFactory::~CntEditorFactory()
{
}

CntBaseView* CntEditorFactory::createNameEditorView(QContact aContact)
{
    return createEditor(CntViewParameters::namesEditorView,
        new CntNameEditorModel(new QContact(aContact)), 
        NULL, 
        qtTrId("Edit contact name"));
}

CntBaseView* CntEditorFactory::createNumberEditorView(QContact aContact)
{
    return createEditor( CntViewParameters::phoneNumberEditorView,
        new CntPhoneNumberModel(new QContact(aContact)),
        new CntPhoneNumberViewItem(), 
        qtTrId("Edit phone number"),
        qtTrId("Add number"));
}

CntBaseView* CntEditorFactory::createEmailEditorView(QContact aContact)
{
    return createEditor(CntViewParameters::emailEditorView,
        new CntEmailEditorModel(new QContact(aContact)),
        new CntEmailEditorViewItem(), 
        qtTrId("Edit email details"),
        qtTrId("Add email"));
}

CntBaseView* CntEditorFactory::createAddressEditorView(QContact aContact)
{
    return createEditor(CntViewParameters::addressEditorView,
        new CntAddressModel(new QContact(aContact)), 
        new CntAddressViewItem(),
        qtTrId("Edit address details"));
}

CntBaseView* CntEditorFactory::createUrlEditorView(QContact aContact)
{
    return createEditor(CntViewParameters::urlEditorView,
        new CntUrlEditorModel(new QContact(aContact)), 
        new CntUrlEditorViewItem(),
        qtTrId("Edit URL details"),
        qtTrId("Add URL"));
}

CntBaseView* CntEditorFactory::createNoteEditorView(QContact aContact)
{
    return createEditor(CntViewParameters::noteEditorView,
        new CntNoteEditorModel(new QContact(aContact)),
        new CntNoteEditorViewItem(), 
        qtTrId("Edit note details"),
        qtTrId("Add note"));
}

CntBaseView* CntEditorFactory::createDateEditorView(QContact aContact)
{
    return createEditor(CntViewParameters::dateEditorView,
        new CntDateEditorModel(new QContact(aContact)),
        new CntDateEditorViewItem(), 
        qtTrId("Edit date details"));
}

CntBaseView* CntEditorFactory::createCompanyEditorView(QContact aContact)
{
    return createEditor(CntViewParameters::companyEditorView,
        new CntCompanyEditorModel(new QContact(aContact)), 
        NULL, 
        qtTrId("Edit company details"));
}

CntBaseView* CntEditorFactory::createFamilyEditorView(QContact aContact)
{
    return createEditor(CntViewParameters::familyDetailEditorView,
        new CntFamilyEditorModel(new QContact(aContact)), 
        NULL, 
        qtTrId("Edit family details"));
}

CntBaseView* CntEditorFactory::createEditor(
    CntViewParameters::ViewId aId, CntDetailEditorModel* aModel,
    HbDataFormViewItem* aProto, QString aTitle, QString aInsert )
{
    CntDetailEditor* editor = new CntDetailEditor(iViewMgr);
    editor->setViewId( aId );
    editor->setHeader(aTitle);
    editor->setDetails(aModel, aProto);
    if ( aInsert.length() > 0 )
    {
        editor->setInsertAction( aInsert );
    }
    return editor;
}
