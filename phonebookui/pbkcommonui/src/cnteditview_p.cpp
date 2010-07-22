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
#include "cntglobal.h"

#include <qtcontacts.h>
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
#include <hbdevicenotificationdialog.h>
#include <xqaiwrequest.h>
#include <xqaiwdecl.h>
#include <QDir>

const char *CNT_EDIT_XML = ":/xml/contacts_ev.docml";

CntEditViewPrivate::CntEditViewPrivate() :
mModel( NULL ),
mImageLabel( NULL ),
mThumbnailManager( NULL ),
mContact( NULL ),
mReq(0),
mMenu(NULL)
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
    mDiscard->setParent(mView);

    mSave = static_cast<HbAction*>( mDocument->findObject("cnt:savecontact") );
    mSave->setParent(mView);

    mDelete = static_cast<HbAction*>( mDocument->findObject("cnt:deletecontact") );
    mDelete->setParent(mView);
    
    HbAction* add = static_cast<HbAction*>( mDocument->findObject("cnt:adddetail_options") );
    add->setParent(mView);

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
    delete mModel;
    delete mContact;
    if (mMenu) 
    {
        delete mMenu;
    }
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
        connect(window, SIGNAL(viewReady()), this, SLOT(setScrollPosition()) );
        connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
        setOrientation(window->orientation());
    }
        
    QString myCard = mArgs.value( EMyCard ).toString();
    QString selectedAction = mArgs.value( ESelectedAction ).toString();
    QVariant contact = aArgs.value( ESelectedContact );
    
    setSelectedContact( contact.value<QContact>() );
    mModel = new CntEditViewListModel( *mContact );
    mListView->setModel( mModel );
    
    QContactLocalId localId = mContact->localId();
    QContactManager* cm = mMgr->contactManager(SYMBIAN_BACKEND);
    QContactLocalId selfContactId = cm->selfContactId();
    mIsMyCard = ( localId == selfContactId && localId != 0 ) || myCard == "myCard";
    
    if ( mHeading )
        mHeading->setDetails( mContact, mIsMyCard );

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
    if ( *mContact == cm->contact( mContact->localId()) )
    {
        mDiscard->setEnabled( false );
        mSave->setEnabled( false );
    }
    
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

void CntEditViewPrivate::openView(CntViewParameters& viewParams)
{
    QList<int> keys = viewParams.keys();
    for(int i = 0;i < keys.count();i++)
    {
        mArgs.insert(keys.at(i), viewParams.value(keys.at(i)));
    }
    mMgr->changeView( mArgs );
}

void CntEditViewPrivate::requestRefresh()
{
    mModel->refreshExtensionItems();
}
    
void CntEditViewPrivate::activated( const QModelIndex& aIndex )
{
    CntEditViewItem* item = mModel->itemAt( aIndex );
    QVariant type = item->data( ERoleItemType );
    if ( type == QVariant(ETypeUiExtension) )
    {
        item->activated(this);
    }
    else if( item->data(ERoleEditorViewId) == ringToneFetcherView )
    {
       fetchTone();
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
        item->longPressed( aCoords, this );
    }
    // only detail items are able to show context specific menu
    else
    {
        if (mMenu) 
        {
            delete mMenu;
        }
        mMenu = createPopup( aItem->modelIndex(), item );
        mMenu->setPreferredPos( aCoords );
        mMenu->setModal( true );
        mMenu->open( this, SLOT(handleMenuAction(HbAction*)) );
    }
}

void CntEditViewPrivate::handleMenuAction( HbAction* aAction )
{
    HbMenu *menu = static_cast<HbMenu*>(sender());
    int row = aAction->data().toInt();
    mIndex = mModel->index(row, 0);
    CntEditViewItem* item = mModel->itemAt( mIndex );
    if ( aAction )
    {
        switch ( aAction->property("menu").toInt() )  
        {
        case HbAction::EditRole:
        {
            if( item->data(ERoleEditorViewId) == ringToneFetcherView )
                {
                fetchTone();
                menu->close();
                }
            else
                {
                editDetail( item );
                }
        }
        break;
        
        case HbAction::NewRole:
        {
            addDetail( item );
        }
        break;
        
        case HbAction::DeleteRole:
        {
          removeDetail( item, mIndex );
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
                mArgs.insert( ESelectedAction, CNT_ADD_ACTION );
                changeEditorView();
                break;
            case addressEditorView:
            case dateEditorView:
            case companyEditorView:
            case familyDetailEditorView:
                mArgs.insert( ESelectedAction, CNT_FOCUS_ACTION );
                changeEditorView();
                break;            
            case ringToneFetcherView:
                fetchTone();
                break;                
            default:
                break;                
            }            
        }
    }
}

void CntEditViewPrivate::changeEditorView()
{
        QVariant var;
        var.setValue(*mContact);
        mArgs.insert(ESelectedContact, var);
        mMgr->changeView( mArgs );   
}

// Call IToneFetch Service
void CntEditViewPrivate::fetchTone()
{
        if(mReq)
         {
             delete mReq;
             mReq = 0;
         }
             //launch media fetcher
         mReq = mAppMgr.create(XQI_TONE_FETCH, XQOP_TONE_FETCH, true); // read strings from header file constants
         if (!mReq)
         {
           return;
         }
         else
         {
             connect(mReq, SIGNAL( requestOk( const QVariant&)), SLOT( ringToneFetchHandleOk(const QVariant&)) );
             connect(mReq, SIGNAL( requestError( int,const QString&)), SLOT(ringToneFetchHandleError(int,const QString&)) );
         }
         
         // Make the request
         mReq->send();
}

void CntEditViewPrivate::ringToneFetchHandleOk(const QVariant &result)
{
          
    QContactRingtone ringtone = mContact->detail<QContactRingtone>();
    QUrl ringtoneUrl = ringtone.audioRingtoneUrl();
    QFileInfo ringtoneFileInfo(ringtoneUrl.toString());
    QString ringtoneFileName = ringtoneFileInfo.fileName();
     // Contact can have only one ringtone at a time
     // remove existing ringtone detail if any
     
     if(!(ringtone.isEmpty()))
         {
         mContact->removeDetail(&ringtone);
         }
     
     ringtone.setAudioRingtoneUrl(result.value<QString>());
     mContact->saveDetail( &ringtone );
     mModel->updateRingtone();
     mSave->setEnabled( true );
     mDiscard->setEnabled( true );
     
}
 
void CntEditViewPrivate::ringToneFetchHandleError(int errorCode, const QString& errorMessage)
{
    Q_UNUSED(errorCode);
    Q_UNUSED(errorMessage);
    
}


void CntEditViewPrivate::deleteContact()
{
    if ( mIsMyCard )
    {
        HbMessageBox::question(hbTrId("txt_phob_dialog_remove_all_personal_data_from_my_c"), this, 
                SLOT(handleDeleteContact(HbAction*)), 
                hbTrId("txt_common_button_yes"), 
                hbTrId("txt_common_button_no"));
    }
    else
    {
        QContactManager* cm = mMgr->contactManager( SYMBIAN_BACKEND );
        QString name = cm->synthesizedDisplayLabel( *mContact );
        HbMessageBox::question(HbParameterLengthLimiter(hbTrId("txt_phob_info_delete_1")).arg(name), this, SLOT(handleDeleteContact(HbAction*)), 
                hbTrId("txt_common_button_delete"), hbTrId("txt_common_button_cancel"));
    }
}

void CntEditViewPrivate::handleDeleteContact(HbAction *action)
{
    Q_Q(CntEditView);
    
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(dlg && action == dlg->actions().first())
    {
        QContactManager* cm = mMgr->contactManager( SYMBIAN_BACKEND );

        emit q->contactRemoved(cm->removeContact( mContact->localId() ));
        
        mMgr->back( mArgs );
    }
}

void CntEditViewPrivate::discardChanges()
{
    Q_Q(CntEditView);
    
    emit q->changesDiscarded();
    
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
    Q_Q(CntEditView);

    QContactManager* mgr = mMgr->contactManager( SYMBIAN_BACKEND );
    bool isSavedContact = mContact->localId() > 0;
    
    // if the contact is really changed or a new one
    if ( (*mContact) != mgr->contact(mContact->localId()) || !isSavedContact )
    {
        int detailCount = mContact->details().count();
         
        setPreferredDetails( mContact );
        
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
                
                QString name = mgr->synthesizedDisplayLabel( *mContact );
                if (name.isEmpty())
                {
                    name = hbTrId("txt_phob_dblist_unnamed");
                }
                
                if ( success )
                {
                    emit q->contactUpdated(1);
                    HbDeviceNotificationDialog::notification(QString(),HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_contact_1_saved")).arg(name));
                }
                else
                {
                    emit q->contactUpdated(0);
                    //TODO: localization is missing
                    HbDeviceNotificationDialog::notification(QString(),qtTrId("SAVING FAILED!"));
                }
                
                QVariant var;
                var.setValue(*mContact);
                mArgs.insert(ESelectedContact, var);
                mArgs.insert(ESelectedAction, CNT_CREATE_ACTION);
            }
            else
            {
                // nothing happened to the contact. Flags should be used
                emit q->contactUpdated(-2);
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
                emit q->contactRemoved(success);
            }
            else
            {
                bool success = mgr->saveContact(mContact);
                
                QString name = mgr->synthesizedDisplayLabel( *mContact );
                if (name.isEmpty())
                {
                    name = hbTrId("txt_phob_dblist_unnamed");
                }
                
                if ( success )
                {   
                    emit q->contactUpdated(1);
                    HbDeviceNotificationDialog::notification(QString(),HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_contacts_1_updated")).arg(name));
                    mArgs.insert(ESelectedAction, CNT_EDIT_ACTION);           
                }
                else
                {
                    emit q->contactUpdated(0);
                    //TODO: localization is missing
                    HbDeviceNotificationDialog::notification(QString(),qtTrId("SAVING FAILED!"));
                }
                
                QVariant var;
                var.setValue(*mContact);
                mArgs.insert(ESelectedContact, var);           
            }
        }
    }
    else
    {
        emit q->changesDiscarded();
    }
    
    mMgr->back( mArgs );
}

void CntEditViewPrivate::setPreferredDetails( QContact* aContact )
{
    QList<QContactPhoneNumber> numberList( aContact->details<QContactPhoneNumber>() );
    //set preferred number for call if there is only one phone number
    if ( aContact->preferredDetail("call").isEmpty() && numberList.count() == 1 )
    {
        aContact->setPreferredDetail( "call", numberList.first() );
    }
    //set preferred number for message if there is only one mobile phone number
    if ( aContact->preferredDetail("message").isEmpty() && numberList.count() >= 1 )
    {
        int mobileNumbers = 0;
        int mobileNumberIndex = -1;
        for (int i = 0; i < numberList.count(); i++)
        {
            if (numberList.at(i).subTypes().count() && numberList.at(i).subTypes().first() == QContactPhoneNumber::SubTypeMobile)
            {
                mobileNumbers++;
                mobileNumberIndex = i;
            }      
        }
        if ( mobileNumbers == 1 )
        {
            aContact->setPreferredDetail( "message", numberList.at(mobileNumberIndex) );
        }
    }
    QList<QContactEmailAddress> emailList( aContact->details<QContactEmailAddress>() );
    //set preferred number for email if there is only one email address
    if ( aContact->preferredDetail("email").isEmpty() && emailList.count() == 1 )
    {
        aContact->setPreferredDetail( "email", emailList.first() );
    }   
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

void CntEditViewPrivate::setScrollPosition()
{
    if ( mArgs.contains(ESelectedDetail) )
    {
        QContactDetail d = mArgs.value( ESelectedDetail ).value<QContactDetail>();
        
        QModelIndex index = mModel->itemIndex( d );
        if ( index.isValid() )
        {
            mListView->scrollTo( index, HbListView::EnsureVisible );
        }
    }
}

void CntEditViewPrivate::thumbnailReady( const QPixmap& pixmap, void *data, int id, int error )
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    if (!error)
    {
        HbIcon icon(pixmap);
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
            HbAction* add = menu->addAction(map->getContactEditorAddLocString(detail.definitionName(), ""));
            add->setProperty( "menu", HbAction::NewRole );
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
        
        HbAction* edit = menu->addAction(hbTrId("txt_common_menu_edit"));
        HbAction* del = menu->addAction(map->getContactEditorDelLocString(subtype, context));
        edit->setProperty( "menu", HbAction::EditRole );
        del->setProperty( "menu" , HbAction::DeleteRole );
        edit->setData( data );
        del->setData( data );
    }
    else
    {
        HbAction* add = menu->addAction(map->getContactEditorAddLocString(detail.definitionName(), ""));
        add->setProperty( "menu", HbAction::NewRole );
        add->setData( data );
    }
    delete map;
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
    viewParameters.insert(ESelectedAction, CNT_ADD_ACTION );
                                            
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

void CntEditViewPrivate::setSelectedContact( QContact aContact )
{
    if ( mContact )
    {
        delete mContact;
        mContact = NULL;
    }
    mContact = new QContact( aContact );
    /*
    mListView->setModel( NULL );
    
    if ( mModel )
    {
        delete mModel;
        mModel = NULL;
    }
    mModel = new CntEditViewListModel( mContact );
    
    if ( mListView )
        mListView->setModel( mModel );
        */
}
// End of File

