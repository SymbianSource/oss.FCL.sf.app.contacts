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

#include "cnteditview_p.h"
#include "cnteditviewlistmodel.h"
#include "cntdocumentloader.h"
#include "cntdetailpopup.h"
#include "cnteditviewheadingitem.h"
#include "cntimagelabel.h"

#include <hbdocumentloader.h>
#include <thumbnailmanager_qt.h>
#include <hbabstractviewitem.h>
#include <hbmessagebox.h>
#include <hbgroupbox.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hblistview.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbframebackground.h>
#include <hbparameterlengthlimiter.h>

const char *CNT_EDIT_XML = ":/xml/contacts_ev.docml";

CntEditViewPrivate::CntEditViewPrivate() :
mModel( NULL ),
mImageLabel( NULL ),
mThumbnailManager( NULL ),
mContact( NULL )
{
    mDocument = new CntDocumentLoader;
    
    bool ok;
    mDocument->load( CNT_EDIT_XML, &ok );
    if ( !ok ){
        qFatal( "Unable to load %S", CNT_EDIT_XML );
    }
    mView = static_cast<HbView*>( mDocument->findWidget("view") );
    mListView = static_cast<HbListView*>( mDocument->findWidget("listView") );
    mListView->setLayoutName("editviewlist");
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    
    mHeading = static_cast<CntEditViewHeadingItem*>( mDocument->findWidget("editViewHeading") );
    mImageLabel = static_cast<CntImageLabel*>(mDocument->findWidget("editViewImage"));
    
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    mDiscard = static_cast<HbAction*>( mDocument->findObject("cnt:discard") );
    mSave = static_cast<HbAction*>( mDocument->findObject("cnt:savecontact") );
    mDelete = static_cast<HbAction*>( mDocument->findObject("cnt:deletecontact") );
    
    HbAction* add = static_cast<HbAction*>( mDocument->findObject("cnt:adddetail_options") );

    connect( add, SIGNAL(triggered()), this, SLOT(addDetailItem()) );
    connect( mDelete, SIGNAL(triggered()), this, SLOT(deleteContact()) );
    connect( mDiscard, SIGNAL(triggered()), this, SLOT(discardChanges()) );
    connect( mSave, SIGNAL(triggered()), this, SLOT(saveChanges()) );
        
    connect( mSoftkey, SIGNAL(triggered()), this, SLOT(saveChanges()) );
    connect( mImageLabel, SIGNAL(iconClicked()), this, SLOT(openImageEditor()) );
    connect( mHeading, SIGNAL(textClicked()), this, SLOT(openNameEditor()) );
    connect( mHeading, SIGNAL(iconClicked()), this, SLOT(openImageEditor()) );
    connect( mListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(activated(const QModelIndex&)) );
    connect( mListView, SIGNAL(longPressed(HbAbstractViewItem*,const QPointF&)), this, SLOT(longPressed(HbAbstractViewItem*,const QPointF&)) );
}

CntEditViewPrivate::~CntEditViewPrivate()
{
    mView->deleteLater();
    delete mDocument;
    delete mListView;
    delete mModel;
    delete mContact;
    
    delete mThumbnailManager;
}

void CntEditViewPrivate::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) 
    {
        // reading "portrait" section
        mDocument->load( CNT_EDIT_XML, "portrait" );
    } 
    else 
    {
        // reading "landscape" section
        mDocument->load( CNT_EDIT_XML, "landscape" );
    }
}

void CntEditViewPrivate::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    mMgr = aMgr;
    mArgs = aArgs;
    
    if ( mView->navigationAction() != mSoftkey)
    {
        mView->setNavigationAction(mSoftkey);
    }
    HbMainWindow* window = mView->mainWindow();
    if ( window )
    {
        connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
        setOrientation(window->orientation());
    }
        
    QVariant selectedAction = mArgs.value( EMyCard );
    QString myCard = selectedAction.toString();
    
    QVariant contact = aArgs.value( ESelectedContact );
    mContact = new QContact( contact.value<QContact>() );
    QContactLocalId localId = mContact->localId();
    
    QContactManager* cm = mMgr->contactManager(SYMBIAN_BACKEND);
    QContactLocalId selfContactId = cm->selfContactId();
    mIsMyCard = ( localId == selfContactId && localId != 0 ) || myCard == "myCard"; 

    // if "MyCard", set slightly different heading and options menu
    if ( mIsMyCard )
    {
        mSave->setText(hbTrId("txt_phob_opt_save_my_card"));
        mDelete->setText(hbTrId("txt_phob_opt_clear_my_card"));
        HbGroupBox* groupBox = static_cast<HbGroupBox*>( mDocument->findWidget("groupBox") );
        if ( groupBox )
        {
            groupBox->setHeading(hbTrId("txt_phob_subtitle_edit_my_details"));
        }
    }

    if ( localId == 0 && !mIsMyCard )
    {
        HbMenu* menu = mView->menu();
        menu->removeAction( mDelete );
    }
    
    // don't delete contact which is "MyCard" or not saved yet or both.
    if ( mIsMyCard && (mContact->details().count() <= 4 || localId == 0) )
    {
        mDelete->setEnabled( false );
    }

    // save and discard disabled if no changes found
    if ( (*mContact) == cm->contact( mContact->localId()) )
    {
        mDiscard->setEnabled( false );
        mSave->setEnabled( false );
    }
    
    mHeading->setDetails( mContact );
    mModel = new CntEditViewListModel( mContact );
    mListView->setModel( mModel );

    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
    
    connect( mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)),
            this, SLOT(thumbnailReady(QPixmap, void*, int, int)) );

    loadAvatar();
}

void CntEditViewPrivate::deactivate()
{   
}
    
void CntEditViewPrivate::activated( const QModelIndex& aIndex )
{
    CntEditViewItem* item = mModel->itemAt( aIndex );
    QVariant type = item->data( ERoleItemType );
    if ( type == QVariant(ETypeUiExtension) )
    {
        item->activated();
        mModel->refreshExtensionItems( aIndex );
    }
    else
    {
        // open editor view
        QVariant id = item->data( ERoleEditorViewId );
        mArgs.insert(EViewId, id.toInt());
        
        QVariant var;
        var.setValue(*mContact);
        mArgs.insert(ESelectedContact, var);
        mArgs.insert(ESelectedAction, QString() );
        mMgr->changeView( mArgs );
    }
}

void CntEditViewPrivate::longPressed( HbAbstractViewItem* aItem, const QPointF& aCoords )
{
    CntEditViewItem* item = mModel->itemAt( aItem->modelIndex() );
    QVariant type = item->data( ERoleItemType );
    
    // Ui extensions handle the long press by themselves.
    if ( type == QVariant(ETypeUiExtension) )
    {
        item->longPressed( aCoords );
        mModel->refreshExtensionItems( aItem->modelIndex() );
    }
    // only detail items are able to show context specific menu
    else
    {
        HbMenu* menu = createPopup( aItem->modelIndex(), item );
        menu->setAttribute( Qt::WA_DeleteOnClose, true );
        menu->setPreferredPos( aCoords );
        menu->setModal( true );
        menu->open( this, SLOT(handleMenuAction(HbAction*)) );
    }
}

void CntEditViewPrivate::handleMenuAction( HbAction* aAction )
{
    int row = aAction->data().toInt();
    QModelIndex index = mModel->index(row, 0);
    CntEditViewItem* item = mModel->itemAt( index );
    if ( aAction )
    {
        switch ( aAction->commandRole() )  
        {
        case HbAction::EditRole:
        {
            editDetail( item );
        }
        break;
        
        case HbAction::NewRole:
        {
            addDetail( item );
        }
        break;
        
        case HbAction::DeleteRole:
        {
            removeDetail( item, index );
        }
        break;
        
        default:
            break;
        }
    }
}

void CntEditViewPrivate::addDetailItem()
{
    CntViewIdList list;
    mModel->allInUseFields( list );
    
    CntDetailPopup::selectDetail( list, this, SLOT(handleAddDetailItem(HbAction*)) );
}

void CntEditViewPrivate::handleAddDetailItem(HbAction *aAction)
{
    CntDetailPopup *popup = static_cast<CntDetailPopup*>(sender());
    
    if (popup && aAction != popup->actions().first())
    {
        int id = popup->selectedDetail();
        
        if (id != noView )
        {
            mArgs.insert(EViewId, id );

            switch ( id )
            {
            case phoneNumberEditorView:
            case emailEditorView:
            case urlEditorView:
            case noteEditorView:
                mArgs.insert( ESelectedAction, "add" );
                break;
            case addressEditorView:
            case dateEditorView:
            case companyEditorView:
            case familyDetailEditorView:
                mArgs.insert( ESelectedAction, "focus" );
                break;
            default:
                break;
            }
            QVariant var;
            var.setValue(*mContact);
            mArgs.insert(ESelectedContact, var);
            mMgr->changeView( mArgs );
        }
    }
}

void CntEditViewPrivate::deleteContact()
{
    if ( mIsMyCard )
    {
        HbMessageBox::question(hbTrId("txt_phob_info_clear_my_card"), this, 
                SLOT(handleDeleteContact(HbAction*)), 
                hbTrId("txt_phob_button_clear"), 
                hbTrId("txt_common_button_cancel"));
    }
    else
    {
        QContactManager* cm = mMgr->contactManager( SYMBIAN_BACKEND );
        QString name = cm->synthesizedDisplayLabel( *mContact );
        HbMessageBox::question(HbParameterLengthLimiter(hbTrId("txt_phob_info_delete_1")).arg(name), this, SLOT(handleDeleteContact(HbAction*)), 
                hbTrId("txt_phob_button_delete"), hbTrId("txt_common_button_cancel"));
    }
}

void CntEditViewPrivate::handleDeleteContact(HbAction *action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(dlg && action == dlg->actions().first())
    {
        QContactManager* cm = mMgr->contactManager( SYMBIAN_BACKEND );

        if ( mIsMyCard )
        {
            mContact->clearDetails();
            emit contactUpdated(cm->saveContact(mContact));
        }
        else
        {
            emit contactRemoved(cm->removeContact( mContact->localId() ));
        }
        
        mMgr->back( mArgs );
    }
}

void CntEditViewPrivate::discardChanges()
{
    emit changesDiscarded();
    
    // get a fresh one from backend.
    QContactManager* mgr = mMgr->contactManager(SYMBIAN_BACKEND);
    mContact = new QContact(mgr->contact( mContact->localId() ));
    
    QVariant var;
    var.setValue(*mContact);
    
    CntViewParameters viewParameters;
    viewParameters.insert(ESelectedContact, var);
            
    mMgr->back( viewParameters );
}

void CntEditViewPrivate::saveChanges()
{
    QContactManager* mgr = mMgr->contactManager( SYMBIAN_BACKEND );
    bool isSavedContact = mContact->localId() > 0;
    
    // if the contact is really changed or a new one
    if ( (*mContact) != mgr->contact(mContact->localId()) || !isSavedContact )
    {
        int detailCount = mContact->details().count();
            
        // If its a new contact
        if ( !isSavedContact )
        {
            if ( detailCount > 2 )
            {
                bool success = mgr->saveContact( mContact );
                if ( success && mIsMyCard )
                {
                    mgr->setSelfContactId( mContact->localId() );
                }
                QVariant var;
                var.setValue(*mContact);
                mArgs.insert(ESelectedContact, var);
                mArgs.insert( ESelectedAction, success ? "save" : "failed" );
                emit contactUpdated(success);
            }
        }
        else
        {
            // contact details has been cleared out.
            if ( detailCount <= 4 )
            {
                // get the contact from database, it should have the name still in it,
                // and show the delete notification to user
                QContact c = mgr->contact( mContact->localId() );
                
                bool success = mgr->removeContact( mContact->localId() );
                emit contactRemoved(success);
                if ( success )
                {
                    mArgs.insert( ESelectedAction, "delete" );
                    QVariant contact;
                    contact.setValue( c );
                    mArgs.insert( ESelectedContact, contact );
                }
            }
            else
            {
                bool success = mgr->saveContact(mContact);
                mArgs.insert( ESelectedAction, success ? "save" : "failed");
                
                QVariant var;
                var.setValue(*mContact);
                mArgs.insert(ESelectedContact, var);
                    
                emit contactUpdated( success );
            }
        }
    }
    else
    {
        emit changesDiscarded();
    }
    
    mMgr->back( mArgs );
}

void CntEditViewPrivate::openNameEditor()
{
    QVariant var;
    var.setValue(*mContact);
            
    mArgs.insert(EViewId, namesEditorView);
    mArgs.insert(ESelectedAction, QString());
    mArgs.insert(ESelectedContact, var);
    
    mMgr->changeView( mArgs );
}

void CntEditViewPrivate::openImageEditor()
{
    QVariant var;
    var.setValue(*mContact);
                
    mArgs.insert(ESelectedContact, var);
    mArgs.insert(ESelectedAction, QString());
    mArgs.insert(EViewId, imageEditorView );
        
    mMgr->changeView( mArgs );
}

void CntEditViewPrivate::loadAvatar()
{
    QList<QContactAvatar> details = mContact->details<QContactAvatar>();
    for (int i = 0;i < details.count();i++)
    {
        QUrl url = details.at(i).imageUrl();
        if ( url.isValid() )
        {
            mThumbnailManager->getThumbnail( url.toString() );
            break;
        }
    }
}

void CntEditViewPrivate::thumbnailReady( const QPixmap& pixmap, void *data, int id, int error )
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    if (!error)
    {
        QIcon qicon(pixmap);
        HbIcon icon(qicon);
        mHeading->setIcon(icon);
        
        mImageLabel->clear();
        mImageLabel->setIcon(icon);
    }
}

HbMenu* CntEditViewPrivate::createPopup( const QModelIndex aIndex, CntEditViewItem* aDetail )
{
    HbMenu* menu = new HbMenu();
    CntStringMapper* map = new CntStringMapper();
    QVariant cd = aDetail->data( ERoleContactDetail );
    QContactDetail detail = cd.value<QContactDetail>();
    
    QVariant data( aIndex.row() );
    
    if ( !mModel->isEmptyItem(aDetail) ) 
    {
        // add is supported only on these items:
        // QContactPhonenumber, QContactOnlineAccount, QContactEmail, QContactUrl and QContactNote
        QString def = detail.definitionName(); 
        if ( def == QContactPhoneNumber::DefinitionName   || 
             def == QContactOnlineAccount::DefinitionName ||
             def == QContactEmailAddress::DefinitionName  ||
             def == QContactUrl::DefinitionName || 
             def == QContactNote::DefinitionName )
        {
            HbAction* add = menu->addAction(HbParameterLengthLimiter(
                    map->getContactEditorAddLocString(detail.definitionName(), "")));
            add->setCommandRole( HbAction::NewRole );
            add->setData( data );
        }
        
        QString context;
        QString subtype;
        if ( def == QContactPhoneNumber::DefinitionName )
        {
            QContactPhoneNumber number(detail); 
            context = number.contexts().isEmpty() ? QString() : number.contexts().first();
            subtype = number.subTypes().isEmpty() ? number.definitionName() : number.subTypes().first();
        }
        else if ( def == QContactOnlineAccount::DefinitionName )
        {
            QContactOnlineAccount number(detail); 
            context = number.contexts().isEmpty() ? QString() : number.contexts().first();
            subtype = number.subTypes().isEmpty() ? number.definitionName() : number.subTypes().first();
        }
        else if ( def == QContactFamily::DefinitionName )
        {
            QStringList fields = aDetail->data(ERoleContactDetailFields).toStringList();
            subtype = fields.first();
        }
        else
        {
            context = QString();
            subtype = detail.definitionName();
        }
            HbAction* edit = menu->addAction(HbParameterLengthLimiter(map->getContactEditorEditLocString(subtype, context)));
            HbAction* del = menu->addAction(HbParameterLengthLimiter(map->getContactEditorDelLocString(subtype, context)));
            edit->setCommandRole( HbAction::EditRole );
            del->setCommandRole( HbAction::DeleteRole );
            edit->setData( data );
            del->setData( data );
    }
    else
    {
        HbAction* edit = menu->addAction(HbParameterLengthLimiter(map->getContactEditorEditLocString(detail.definitionName(), "")));
        edit->setCommandRole( HbAction::EditRole );
        edit->setData( data );
    }
    
    return menu;
}

void CntEditViewPrivate::addDetail( CntEditViewItem* aDetail )
{
    QVariant id = aDetail->data( ERoleEditorViewId );
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, id.toInt());
    QVariant var;
    var.setValue(*mContact);
    viewParameters.insert(ESelectedContact, var);
    viewParameters.insert(ESelectedAction, "add" );
                                            
    mMgr->changeView( viewParameters );
}

void CntEditViewPrivate::editDetail( CntEditViewItem* aDetail )
{
    QVariant id = aDetail->data( ERoleEditorViewId );
    
    mArgs.insert(EViewId, id.toInt());
    mArgs.insert(ESelectedAction, QString() );
    QVariant var;
    var.setValue(*mContact);
    
    mArgs.insert(ESelectedContact, var);
                                              
    mMgr->changeView( mArgs );
}

void CntEditViewPrivate::removeDetail( CntEditViewItem* aDetail, const QModelIndex& aIndex )
{
    mModel->removeItem( aDetail, aIndex );
    mSave->setEnabled( true );
    mDiscard->setEnabled( true );
}
// End of File

