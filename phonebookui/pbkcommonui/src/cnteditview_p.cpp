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
#include <hbmainwindow.h>
#include <hbview.h>
#include <hblistview.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbframebackground.h>

const char *CNT_EDIT_XML = ":/xml/contacts_ev.docml";

CntEditViewPrivate::CntEditViewPrivate() :
mModel( NULL ),
mImageLabel( NULL ),
mThumbnailManager( NULL )
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
    HbAction* removeContact = static_cast<HbAction*>( mDocument->findObject("cnt:deletecontact") );
    
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
        
    QVariant contact = aArgs.value( ESelectedContact );
    mContact = new QContact( contact.value<QContact>() );
    QContactLocalId localId = mContact->localId();
    
    QContactManager* cm = mMgr->contactManager(SYMBIAN_BACKEND);
    QContactLocalId selfContactId = cm->selfContactId();
    
    HbMenu* menu = mView->menu();
    // don't delete contact which is "MyCard" or not saved yet or both.
    if ( localId == selfContactId && selfContactId != 0 && mContact->details().count() <= 4 || localId == 0 )
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
        CntViewParameters viewParameters;
        viewParameters.insert(EViewId, id.toInt());
        QVariant var;
        var.setValue(*mContact);
        viewParameters.insert(ESelectedContact, var);
            
        mMgr->changeView( viewParameters );
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
        QVariant cd = item->data( ERoleContactDetail );
        
        HbMenu* menu = new HbMenu();
        menu->addActions( createPopup(item) );
        HbAction* selected = menu->exec( aCoords );
        
        if ( selected )
        {
            switch ( selected->commandRole() )  
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
                    removeDetail( item, aItem->modelIndex() );
                }
                break;
                
                default:
                    break;
            }
        }
        
        delete menu;
    }
}

void CntEditViewPrivate::addDetailItem()
{
    int detailEditorId = CntDetailPopup::selectDetail();
    if (detailEditorId != noView )
    {
        CntViewParameters viewParameters;
        viewParameters.insert(EViewId, detailEditorId );
        switch ( detailEditorId )
        {
        case phoneNumberEditorView:
        case emailEditorView:
        case urlEditorView:
        case noteEditorView:
            viewParameters.insert( ESelectedAction, "add" );
            break;
        case addressEditorView:
        case dateEditorView:
        case companyEditorView:
        case familyDetailEditorView:
            viewParameters.insert( ESelectedAction, "focus" );
            break;
        default:
            break;
        }
        QVariant var;
        var.setValue(*mContact);
        viewParameters.insert(ESelectedContact, var);
        mMgr->changeView(viewParameters);
    }
}

void CntEditViewPrivate::deleteContact()
{
    QContactManager* cm = mMgr->contactManager( SYMBIAN_BACKEND );
    QString name = cm->synthesizedDisplayLabel( *mContact );

    HbMessageBox *note = new HbMessageBox(hbTrId("txt_phob_info_delete_1").arg(name), HbMessageBox::MessageTypeQuestion);
    note->setPrimaryAction(new HbAction(hbTrId("txt_phob_button_delete"), note));
    note->setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), note));
    HbAction *selected = note->exec();
    if (selected == note->primaryAction())
    {
        cm->removeContact( mContact->localId() );
        emit contactRemoved();
        
        CntViewParameters viewParameters;
        mMgr->back( viewParameters );
    }
    delete note;
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
    mgr->saveContact( mContact );
    emit contactUpdated();
    
    QVariant var;
    var.setValue(*mContact);
        
    CntViewParameters viewParameters;
    viewParameters.insert(ESelectedContact, var);
    viewParameters.insert( ESelectedAction, "save" );    
    
    mMgr->back( viewParameters );
}

void CntEditViewPrivate::openNameEditor()
{
    QVariant var;
    var.setValue(*mContact);
            
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, namesEditorView);
    viewParameters.insert(ESelectedContact, var);
    
    mMgr->changeView( viewParameters );
}

void CntEditViewPrivate::openImageEditor()
{
    QVariant var;
    var.setValue(*mContact);
                
    CntViewParameters viewParameters;
    viewParameters.insert(ESelectedContact, var);
    viewParameters.insert(EViewId, imageEditorView );
        
    mMgr->changeView( viewParameters );
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

QList<QAction*> CntEditViewPrivate::createPopup( CntEditViewItem* aDetail )
{
    QList<QAction*> actions;
    CntStringMapper* map = new CntStringMapper();
    QVariant cd = aDetail->data( ERoleContactDetail );
    QContactDetail detail = cd.value<QContactDetail>();
    
    HbAction* edit = static_cast<HbAction*>(mDocument->findObject("cnt:editdetail"));
            
    HbAction* add = static_cast<HbAction*>(mDocument->findObject("cnt:adddetail_popup"));
    QString text = add->text();
    add->setText( text.arg( "%1", map->getMappedDetail(detail.definitionName())) );
            
    HbAction* del = static_cast<HbAction*>(mDocument->findObject("cnt:deletedetail"));
    text = edit->text();
    del->setText( text.arg("%1", map->getMappedDetail(detail.definitionName())) );
        
    add->setCommandRole( HbAction::NewRole );
    edit->setCommandRole( HbAction::EditRole );
    del->setCommandRole( HbAction::DeleteRole );
            
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
            actions.append( add );
        }
        actions.append( edit );
        actions.append( del );
    }
    else
    {
        actions.append( edit );
    }
    delete map;
    return actions;
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
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, id.toInt());
    QVariant var;
    var.setValue(*mContact);
    
    viewParameters.insert(ESelectedContact, var);
                                              
    mMgr->changeView( viewParameters );
}

void CntEditViewPrivate::removeDetail( CntEditViewItem* aDetail, const QModelIndex& aIndex )
{
    mModel->removeItem( aDetail, aIndex );
    mSave->setEnabled( true );
    mDiscard->setEnabled( true );
}
// End of File

