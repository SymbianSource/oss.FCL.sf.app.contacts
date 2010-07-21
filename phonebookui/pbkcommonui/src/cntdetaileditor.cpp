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
#include "cntgroupeditormodel.h"
#include "cntsavemanager.h"
#include "cntglobal.h"

#include <cntviewparams.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbdocumentloader.h>
#include <hbaction.h>
#include <hblineedit.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>
#include <cntdebug.h>
#include <hbscrollarea.h>
#include <hbscrollbar.h>
#include <hbdevicenotificationdialog.h>
#include <hbparameterlengthlimiter.h>

#include <QApplication>

const char *CNT_DETAILEDITOR_XML = ":/xml/contacts_detail_editor.docml";

CntDetailEditor::CntDetailEditor( int aId ) :
    QObject(),
    mDataFormModel(NULL),
    mHeader(NULL),   
    mId(aId),
    mView(NULL),
    mLoader(NULL),   
    mViewManager(NULL),
    mEditorFactory(NULL),
    mCancel(NULL),
    mSaveManager(NULL)
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
    
    // closing the application from task swapper or end key will cause the contact to be saved
    connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(saveContact()));
}

CntDetailEditor::~CntDetailEditor()
{
    mView->deleteLater();
    
    delete mDataForm;
    mDataForm = NULL;
    delete mDataFormModel;
    mDataFormModel = NULL;
    delete mHeader;
    mHeader = NULL;
    delete mLoader;
    mLoader = NULL;
    delete mEditorFactory;
    mEditorFactory = NULL;
    delete mSaveManager;
    mSaveManager = NULL;
}

void CntDetailEditor::setViewId( int aId )
{
    mId = aId;
}

void CntDetailEditor::setInsertAction( const QString aInsert )
{
    HbAction* insert = new HbAction( aInsert, mView );
    mView->menu()->insertAction(mCancel, insert);
    connect( insert, SIGNAL(triggered()), this, SLOT(insertField()) );
}

void CntDetailEditor::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    mViewManager = aMgr;
    mArgs = aArgs; //don't loose the params while swiching between editview and editorviews.
    
    mCancel = static_cast<HbAction*>(document()->findObject("cnt:discardchanges"));
    mCancel->setParent(mView);
    mView->menu()->addAction(mCancel);
    connect( mCancel, SIGNAL(triggered()), this, SLOT(discardChanges()) );
    
    if ( mView->navigationAction() != mSoftkey) {
        mView->setNavigationAction(mSoftkey);
    }
    
    QContact selectedContact;
    if ( mId == groupEditorView )
    {
        selectedContact = aArgs.value(ESelectedGroupContact).value<QContact>();
        connect( mDataForm, SIGNAL(itemShown(const QModelIndex&)), this, SLOT(handleItemShown(const QModelIndex&)) );
    }
    else
    {
        selectedContact = aArgs.value(ESelectedContact).value<QContact>();
        connect( mDataForm, SIGNAL(itemShown(const QModelIndex&)), this, SLOT(handleItemShown(const QModelIndex&)) );
    }
    mEditorFactory->setupEditorView(*this, selectedContact);
    
    QString myCard = mArgs.value( EMyCard ).toString();
    QContactLocalId localId = selectedContact.localId();
    QContactLocalId selfContactId = mViewManager->contactManager(SYMBIAN_BACKEND)->selfContactId();
    bool isMyCard = ( localId == selfContactId && localId != 0 ) || !myCard.isEmpty();
    
    if (isMyCard)
    {
        mSaveManager = new CntSaveManager(CntSaveManager::EMyCard);
    }
    else if ( mId == groupEditorView )
    {
        mSaveManager = new CntSaveManager(CntSaveManager::EGroup);
    }
    else
    {
        mSaveManager = new CntSaveManager();
    }
    
    mDataForm->setItemRecycling(true);

    // add new field if required
    if ( aArgs.value(ESelectedAction).toString() == CNT_ADD_ACTION )
    {
        mDataFormModel->insertDetailField();
    }
    mDataForm->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded); 
    mDataForm->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    mDataForm->verticalScrollBar()->setInteractive(true);
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

void CntDetailEditor::handleItemShown(const QModelIndex& aIndex )
{
    if ( mId == groupEditorView )
    {
        CntGroupEditorModel* groupModel = static_cast<CntGroupEditorModel*>( mDataFormModel );    
        if ( groupModel->isConferenceNumber(aIndex) )
        {
            HbDataFormViewItem* viewItem = static_cast<HbDataFormViewItem*>(mDataForm->itemByIndex( aIndex ));
            HbLineEdit* edit = static_cast<HbLineEdit*>( viewItem->dataItemContentWidget() );
            edit->setInputMethodHints( Qt::ImhDialableCharactersOnly );
        }
        else
        {
            HbDataFormViewItem* viewItem = static_cast<HbDataFormViewItem*>(mDataForm->itemByIndex( aIndex ));
            HbLineEdit* edit = static_cast<HbLineEdit*>( viewItem->dataItemContentWidget() );
            edit->setInputMethodHints( Qt::ImhNoPredictiveText );
        }
    }
    else
    {
        HbDataFormViewItem* viewItem = static_cast<HbDataFormViewItem*>(mDataForm->itemByIndex( aIndex ));
        HbLineEdit* edit = static_cast<HbLineEdit*>( viewItem->dataItemContentWidget() );
        edit->setInputMethodHints( Qt::ImhNoPredictiveText );
        
        HbDataFormModelItem* modelItem = mDataFormModel->itemFromIndex( aIndex );
        if (modelItem->contentWidgetData( "preferDigits" ).toBool())
        {
            edit->setInputMethodHints( Qt::ImhPreferNumbers );
        }
    }
}

void CntDetailEditor::discardChanges()
{
    QContact selected( *mDataFormModel->contact() );
    QVariant var;
    var.setValue(selected);
    mArgs.insert(ESelectedContact, var);
    mViewManager->back( mArgs );
}

/*!
    Saves all changes made to details (type of details depends on
    in which editor we are) of a contact. Doesn't save the contact.
*/
void CntDetailEditor::saveChanges()
{
    mDataFormModel->saveContactDetails();
    
    QContact selected( *mDataFormModel->contact() );
    QVariant var;
    var.setValue(selected);
    if ( mId == groupEditorView )
    {
       mArgs.insert(ESelectedGroupContact, var);
    }
    else
    {
        mArgs.insert(ESelectedContact, var);
        
        QContactDetail selectedDetail( mDataFormModel->detail() );
        QVariant var2;
        var2.setValue( selectedDetail );
        mArgs.insert( ESelectedDetail, var2 );
    }
    
    mViewManager->back( mArgs );
}

/*!
    First saves all changes made to details (type of details depends on
    in which editor we are) of a contact and then saves the contact. This
    is only called when phonebook is being closed either from task swapper
    or by end key.
*/
void CntDetailEditor::saveContact()
{
    mDataFormModel->saveContactDetails();
    
    QString name = mViewManager->contactManager(SYMBIAN_BACKEND)->synthesizedDisplayLabel(*mDataFormModel->contact());
    
    if (name.isEmpty())
    {
        name = hbTrId("txt_phob_list_unnamed");
    }
    
    CntSaveManager::CntSaveResult result = mSaveManager->saveContact(mDataFormModel->contact(), mViewManager->contactManager(SYMBIAN_BACKEND));
    
    if (mId != groupEditorView)
    {
        switch (result)
        {
        case CntSaveManager::ESaved:
            HbDeviceNotificationDialog::notification(QString(),HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_contact_1_saved")).arg(name));
            break;
        case CntSaveManager::EUpdated:
            HbDeviceNotificationDialog::notification(QString(),HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_contacts_1_updated")).arg(name));
            break;
        case CntSaveManager::EFailed:
            HbDeviceNotificationDialog::notification(QString(),hbTrId("SAVING FAILED!"));
            break;
        case CntSaveManager::EDeleted:
        case CntSaveManager::ENothingDone:
        default:
            break;
        }
    }
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
    //Append custom view item prototype
    if ( aPrototype )
    {
        QList <HbAbstractViewItem*> protos = mDataForm->itemPrototypes();
        protos.append( aPrototype );
        mDataForm->setItemPrototypes( protos );
    }

    mDataFormModel = aModel;
    mDataForm->setModel( mDataFormModel );

}

int CntDetailEditor::viewId() const
{
    return mId;
}

// End of File
