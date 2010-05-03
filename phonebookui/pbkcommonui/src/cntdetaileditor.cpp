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
#include "cnteditorfactory.h"
#include <cntviewparams.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbdocumentloader.h>
#include <hbaction.h>

const char *CNT_DETAILEDITOR_XML = ":/xml/contacts_detail_editor.docml";

CntDetailEditor::CntDetailEditor( int aId ) :
    QObject(),
    mId(aId),
    mView(NULL),
    mLoader(NULL),   
    mViewManager(NULL),
    mEditorFactory(NULL),
    mCancel(NULL)
{
    bool ok;
    document()->load(CNT_DETAILEDITOR_XML, &ok);
    if (!ok) {
        qFatal("Unable to read %S", CNT_DETAILEDITOR_XML);
    }
    else {
        mView = static_cast<HbView*> (document()->findWidget(QString("view")));

        mDataForm = static_cast<HbDataForm*> (document()->findWidget(QString("dataForm")));
    }
 
    mEditorFactory = new CntEditorFactory();
    
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect( mSoftkey, SIGNAL(triggered()), this, SLOT(saveChanges()) );
}

CntDetailEditor::~CntDetailEditor()
{
    delete mDataForm;
    delete mDataFormModel;
    delete mHeader;
    delete mLoader;
    delete mEditorFactory;
}

void CntDetailEditor::setViewId( int aId )
{
    mId = aId;
}

void CntDetailEditor::setInsertAction( const QString aInsert )
{
    HbAction* insert = new HbAction( aInsert );
    mView->menu()->insertAction(mCancel, insert);
    connect( insert, SIGNAL(triggered()), this, SLOT(insertField()) );
}

void CntDetailEditor::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    mViewManager = aMgr;
    
    mCancel = static_cast<HbAction*>(document()->findObject("cnt:discardchanges"));
    mView->menu()->addAction( mCancel );
    connect( mCancel, SIGNAL(triggered()), this, SLOT(discardChanges()) );
    
    if ( mView->navigationAction() != mSoftkey) {
        mView->setNavigationAction(mSoftkey);
    }
    
    QContact selectedContact = aArgs.value(ESelectedContact).value<QContact>();
    mEditorFactory->setupEditorView(*this, selectedContact);
    
    mDataForm->setItemRecycling(true);

    // add new field if required
    if ( aArgs.value(ESelectedAction).toString() == "add" ) {
        mDataFormModel->insertDetailField();
    }

    HbMainWindow* window = mView->mainWindow();
    if ( window ) {
        connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(handleOrientation(Qt::Orientation)));
        handleOrientation(window->orientation());
    }
}

void CntDetailEditor::deactivate()
{
    if( mId == groupEditorView) {
        QContactManager* mgr = mViewManager->contactManager(SYMBIAN_BACKEND);
        mgr->saveContact( mDataFormModel->contact() );
    }
}
    
bool CntDetailEditor::isDefault() const
{
    return false;
}

HbView* CntDetailEditor::view() const
{
    return mView;   
}

HbDocumentLoader* CntDetailEditor::document()
{
    if (!mLoader) {
        mLoader = new HbDocumentLoader();
    }
    
    return mLoader;    
}

void CntDetailEditor::insertField()
{
    mDataFormModel->insertDetailField();
}

void CntDetailEditor::handleOrientation(Qt::Orientation aOrientation)
{
    Q_UNUSED(aOrientation);
}

void CntDetailEditor::discardChanges()
{
    QContact selected( *mDataFormModel->contact() );
    CntViewParameters args;
    QVariant var;
    var.setValue(selected);
    args.insert(ESelectedContact, var);
    mViewManager->back( args );
}

void CntDetailEditor::saveChanges()
{
    mDataFormModel->saveContactDetails();
    
    QContact selected( *mDataFormModel->contact() );
    CntViewParameters args;
    QVariant var;
    var.setValue(selected);
    args.insert(ESelectedContact, var);
    mViewManager->back( args );
}

void CntDetailEditor::setHeader(QString aHeader)
{
    if (!mHeader) {
        mHeader = static_cast<HbGroupBox*> (document()->findWidget(QString("headerBox")));
    }
    mHeader->setHeading(aHeader);
}

void CntDetailEditor::setDetails(CntDetailEditorModel* aModel, HbAbstractViewItem* aPrototype)
{
    mDataFormModel = aModel;
    mDataForm->setModel(mDataFormModel, aPrototype);
}

int CntDetailEditor::viewId() const
{
    return mId;
}

// End of File
