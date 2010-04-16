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
#include "cntdetaileditor.h"
#include <hbmenu.h>
CntDetailEditor::CntDetailEditor(CntViewManager *viewManager, QGraphicsItem *parent) :
    CntBaseView(viewManager, parent), mDataForm(0), mDataFormModel(0)
{
    if (loadDocument(":/xml/contacts_editor.docml")) {
        QGraphicsWidget *content = findWidget(QString("content"));
        setWidget(content);
        mDataForm = static_cast<HbDataForm*> (findWidget(QString("dataForm")));
    }
}

void CntDetailEditor::setViewId( CntViewParameters::ViewId aId )
{
    iId = aId;
}

void CntDetailEditor::setInsertAction( const QString aInsert )
{
    HbAction* insert = new HbAction( aInsert );
    menu()->addAction( insert );
    connect( insert, SIGNAL(triggered()), this, SLOT(insertField()) );
}

void CntDetailEditor::insertField()
{
    mDataFormModel->insertDetailField();
}

CntDetailEditor::~CntDetailEditor()
{
    delete mDataForm;
    delete mDataFormModel;
    delete mPrototype;
    delete mHeader;
}

void CntDetailEditor::discardChanges()
{
    QContact selected( *mDataFormModel->contact() );
    CntViewParameters args;
    args.setSelectedContact( selected );
    viewManager()->back( args );
}

void CntDetailEditor::aboutToCloseView()
{
    mDataFormModel->saveContactDetails();
    
    QContact selected( *mDataFormModel->contact() );
    CntViewParameters args;
    args.setSelectedContact( selected );
    viewManager()->back( args );
}

void CntDetailEditor::setHeader(QString aHeader)
{
    if (!mHeader) {
        mHeader = static_cast<HbGroupBox*> (findWidget(QString("headerBox")));
    }
    mHeader->setHeading(aHeader);
}

void CntDetailEditor::setDetails(CntDetailEditorModel* aModel, HbAbstractViewItem* aPrototype)
{
    mDataFormModel = aModel;
    mPrototype = aPrototype;
}

void CntDetailEditor::activateView(const CntViewParameters &viewParameters)
{
    Q_UNUSED( viewParameters );
    HbAction* cancel = static_cast<HbAction*>(findObject("cnt:discardchanges"));
    menu()->addAction( cancel );
    connect( cancel, SIGNAL(triggered()), this, SLOT(discardChanges()) );
    
    // add new field if required
    if ( viewParameters.selectedAction() == "add" ) {
        mDataFormModel->insertDetailField();
    }
    
    mDataForm->setItemRecycling(true);
    mDataForm->setModel(mDataFormModel, mPrototype);
}

CntViewParameters::ViewId CntDetailEditor::viewId() const
{
    return iId;
}

// End of File
