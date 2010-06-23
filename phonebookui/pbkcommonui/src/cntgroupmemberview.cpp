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

#include "cntgroupmemberview.h"
#include "cntfetchcontactsview.h"
#include "cntcontactcardheadingitem.h"
#include "cntglobal.h"
#include "cntimagelabel.h"
#include "cntimageutility.h"
#include <hbnotificationdialog.h>
#include <hbmessagebox.h>
#include <hbmenu.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbframebackground.h>
#include <hbindexfeedback.h>
#include <hbscrollbar.h>
#include <hbview.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbparameterlengthlimiter.h>
#include <thumbnailmanager_qt.h>
#include <cntlistmodel.h>
#include <hbmainwindow.h>

#include "cntdocumentloader.h"

const char *CNT_GROUPMEMBERVIEW_XML = ":/xml/contacts_groupmembers.docml";

/*!
\class CntGroupMemberView
\brief

This is the group members view class that shows list of contacts for a user group. View contains a listview that shows actual contacts that
have been added to a particular group. There is also toolbar and menu for navigating between different views. Instance of this class is
created by our viewmanager but view itself is owned by the mainwindow which will also delete it in the end.

*/

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)

*/
CntGroupMemberView::CntGroupMemberView() :
    mGroupContact(NULL),
    mViewManager(NULL),
    mHeadingItem(NULL),
    mModel(NULL),
    mImageLabel(NULL), 
    mListView(NULL),
    mFetchView(NULL),
    mAvatar(NULL)
{

    mDocument = new CntDocumentLoader;
    
    bool ok;
    mDocument->load( CNT_GROUPMEMBERVIEW_XML, &ok );
    if ( !ok ){
        qFatal( "Unable to load %S", CNT_GROUPMEMBERVIEW_XML );
    }

    mView = static_cast<HbView*>( mDocument->findWidget("view") );
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    mImageLabel = static_cast<CntImageLabel*>(mDocument->findWidget("editViewImage"));
    connect( mImageLabel, SIGNAL(iconClicked()), this, SLOT(openImageEditor()) );
    connect( mImageLabel, SIGNAL(iconLongPressed(const QPointF&)), this, SLOT(drawImageMenu(const QPointF&)) );
    
    mListView = static_cast<HbListView*>( mDocument->findWidget("listView") );
    connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
        SLOT(showContextMenu(HbAbstractViewItem*,QPointF)));
    connect(mListView, SIGNAL(activated (const QModelIndex&)), this,
        SLOT(showContactView(const QModelIndex&)));
    
    mHeadingItem = static_cast<CntContactCardHeadingItem*>( mDocument->findWidget("editViewHeading") );
    connect(mHeadingItem, SIGNAL(passShortPressed(const QPointF&)), this, SLOT(openImageEditor()) );
    connect(mHeadingItem, SIGNAL(passLongPressed(const QPointF&)), this, SLOT(drawImageMenu(const QPointF&)));

    // menu actions
    mEditGroupAction = static_cast<HbAction*>( mDocument->findObject("cnt:editgroupdetails"));
    connect(mEditGroupAction, SIGNAL(triggered()), this, SLOT(editGroup()));
    
    // toolbar actions
    mManageAction = static_cast<HbAction*>( mDocument->findObject("cnt:managemembers"));
    connect(mManageAction, SIGNAL(triggered()), this, SLOT(manageMembers()));
    mDeleteAction = static_cast<HbAction*>( mDocument->findObject("cnt:deletegroup"));
    connect(mDeleteAction, SIGNAL(triggered()), this, SLOT(deleteGroup()));
    mShowActionsAction = static_cast<HbAction*>( mDocument->findObject("cnt:groupactions"));
    connect(mShowActionsAction, SIGNAL(triggered()), this, SLOT(openGroupActions()));
    
    // thumbnail manager
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
   
    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)),
               this, SLOT(thumbnailReady(QPixmap, void*, int, int)));
}

/*!
Destructor
*/
CntGroupMemberView::~CntGroupMemberView()
{
    mView->deleteLater();
    
    delete mGroupContact;
    mGroupContact = 0;
    
    delete mModel;
    mModel = 0;
    
    delete mFetchView;
    mFetchView = 0;
    
    delete mAvatar;
    mAvatar = NULL;
}

void CntGroupMemberView::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) 
    {
        // reading "portrait" section
        mDocument->load( CNT_GROUPMEMBERVIEW_XML, "portrait" );
    } 
    else 
    {
        // reading "landscape" section
        mDocument->load( CNT_GROUPMEMBERVIEW_XML, "landscape" );
    }
}

void CntGroupMemberView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    mViewManager = aMgr;
    mArgs = aArgs;
    
    if (mView->navigationAction() != mSoftkey)
        {
        mView->setNavigationAction(mSoftkey);   
        }
        
    QVariant contact = mArgs.value( ESelectedGroupContact );
    mGroupContact = new QContact( contact.value<QContact>() );
    
    HbMainWindow* window = mView->mainWindow();
    if ( window )
    {
        connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
        setOrientation(window->orientation());
    }
    
    mHeadingItem->setGroupDetails(mGroupContact);

    // avatar
    QList<QContactAvatar> details = mGroupContact->details<QContactAvatar>();
    for (int i = 0;i < details.count();i++)
    {
        if (details.at(i).imageUrl().isValid())
            {
            mAvatar = new QContactAvatar(details.at(i));
            mThumbnailManager->getThumbnail(mAvatar->imageUrl().toString());
            break;
            }
    }
    
    // create list & model
    mListView->setFrictionEnabled(true);
    mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    mListView->verticalScrollBar()->setInteractive(true);
    mListView->setUniformItemSizes(true);
    mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::Thumbnail);
    HbIndexFeedback *indexFeedback = new HbIndexFeedback(mView);
    indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    indexFeedback->setItemView(mListView);

    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    
    createModel();
    
    if (mArgs.value(ESelectedAction).toString() == "save")
    {
        QString name = getContactManager()->synthesizedDisplayLabel(*mGroupContact);
        HbNotificationDialog::launchDialog(HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_new_group_1_created").arg(name)));
    }
    
    mFetchView = new CntFetchContacts(mViewManager->contactManager( SYMBIAN_BACKEND ));
    connect(mFetchView, SIGNAL(clicked()), this, SLOT(handleManageMembers()));
}

void CntGroupMemberView::deactivate()
{
    
}

void CntGroupMemberView::showPreviousView()
{
    //save the contact if avatar has been changed.
    QContact contact = mViewManager->contactManager( SYMBIAN_BACKEND )->contact(mGroupContact->localId());
    if ( contact != *mGroupContact )
    {
        QList<QContactAvatar> details = mGroupContact->details<QContactAvatar>();
        for (int i = 0; i < details.count(); i++)
        {
            if (!details.at(i).imageUrl().isEmpty())
            {
                getContactManager()->saveContact(mGroupContact);
                break;
            }
        }
    }
    mViewManager->back(mArgs);
}

void CntGroupMemberView::openGroupActions()
{
    QVariant var;
    var.setValue(*mGroupContact);
    
    mArgs.insert(EViewId, groupActionsView);
    mArgs.insert(ESelectedGroupContact, var);
    mViewManager->changeView(mArgs);
}

void CntGroupMemberView::manageMembers()
{
    QContactRelationshipFilter membersFilter;
    membersFilter.setRelationshipType(QContactRelationship::HasMember);
    membersFilter.setRelatedContactRole(QContactRelationship::First);
    membersFilter.setRelatedContactId(mGroupContact->id());   
    mOriginalGroupMembers = getContactManager()->contactIds(membersFilter);
    
    QSet<QContactLocalId> contactsSet = mOriginalGroupMembers.toSet();
    QContactName groupContactName = mGroupContact->detail( QContactName::DefinitionName );
    QString groupName(groupContactName.value( QContactName::FieldCustomLabel ));
    
    if (!mFetchView) {
        mFetchView = new CntFetchContacts(mViewManager->contactManager( SYMBIAN_BACKEND ));
        connect(mFetchView, SIGNAL(clicked()), this, SLOT(handleManageMembers()));
    }
    mFetchView->setDetails(HbParameterLengthLimiter(hbTrId("txt_phob_title_members_of_1_group")).arg(groupName),
                           hbTrId("txt_common_button_save"));
    mFetchView->displayContacts(CntFetchContacts::popup,
                                HbAbstractItemView::MultiSelection,
                                contactsSet);
}

void CntGroupMemberView::handleManageMembers()
{
    
    QSet<QContactLocalId> selectedContacts = mFetchView->getSelectedContacts();
    
    if (mFetchView->wasCanceled()) {
        delete mFetchView;
        mFetchView = 0;
        return;
    }

    QList<QContactRelationship> removedMemberships;
    QList<QContactRelationship> addedMemberships;

    QSet<QContactLocalId> removedMembers = mOriginalGroupMembers.toSet() - selectedContacts;
    setRelationship(removedMembers, removedMemberships);

    QSet<QContactLocalId> addedMembers = selectedContacts - mOriginalGroupMembers.toSet();
    setRelationship(addedMembers, addedMemberships);
    
    QMap<int, QContactManager::Error> errors;
    if (!addedMemberships.isEmpty()) {
        getContactManager()->saveRelationships(&addedMemberships, &errors);
    }
    
    if (!removedMemberships.isEmpty()) {
        getContactManager()->removeRelationships(removedMemberships, &errors);
    }
    
    // delete the model and recreate it with relationship changes
    delete mModel;
    mModel = 0;
    
    createModel();
}

void CntGroupMemberView::createModel()
{
    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRelatedContactRole(QContactRelationship::First);
    rFilter.setRelatedContactId(mGroupContact->id());


    mModel = new CntListModel(getContactManager(), rFilter, false);
    mListView->setModel(mModel);
}

void CntGroupMemberView::editGroup()
{
    mArgs.insert(EViewId, groupEditorView);
    QVariant var;
    var.setValue(*mGroupContact);
    mArgs.insert(ESelectedGroupContact, var);
    mViewManager->changeView(mArgs);
}

void CntGroupMemberView::deleteGroup()
{
    QString groupName = mGroupContact->displayLabel();
    HbLabel *headingLabel = new HbLabel();
    headingLabel->setPlainText(HbParameterLengthLimiter(hbTrId("txt_phob_dialog_delete_1_group")).arg(groupName));
    
    HbMessageBox::question(hbTrId("txt_phob_dialog_only_group_will_be_removed_contac"), this, SLOT(handleDeleteGroup(HbAction*)),
            hbTrId("txt_common_button_delete"), hbTrId("txt_common_button_cancel"), headingLabel);
}

void CntGroupMemberView::handleDeleteGroup(HbAction *action)
{
    HbDialog *popup = static_cast<HbDialog*>(sender());
    
    if (popup && action == popup->actions().first())
    {
        getContactManager()->removeContact(mGroupContact->localId());
        showPreviousView();
    }
}

/*!
Called when a list item is longpressed
*/
void CntGroupMemberView::showContextMenu(HbAbstractViewItem *aItem, const QPointF &aCoords)
{
    QVariant data( aItem->modelIndex().row() );
   
    QModelIndex index = aItem->modelIndex();

    HbMenu *menu = new HbMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setPreferredPos( aCoords );
    
    HbAction *removeFromGroupAction = 0;
    HbAction *openContactAction = 0;
    HbAction *editContactAction = 0;

    openContactAction = menu->addAction(hbTrId("txt_common_menu_open"));
    editContactAction = menu->addAction(hbTrId("txt_common_menu_edit"));
    removeFromGroupAction = menu->addAction(hbTrId("txt_phob_menu_remove_from_group"));
    
    openContactAction->setData( data );
    editContactAction->setData( data );
    removeFromGroupAction->setData( data );

    menu->open(this, SLOT(handleMenu(HbAction*)));
}

void CntGroupMemberView::handleMenu(HbAction* action)
{
    int row = action->data().toInt();
    HbMenu *menuItem = static_cast<HbMenu*>(sender());
    QModelIndex index = mModel->index(row);
    
    if ( action == menuItem->actions().first() )
        {
        showContactView(index);
        }
    else if (action == menuItem->actions().at(1))
        {
        editContact(index);
        }
    else if (action == menuItem->actions().at(2))
        {
        removeFromGroup(index);
        }
}


/*!
Called after user clicked on the listview.
*/
void CntGroupMemberView::showContactView(const QModelIndex &index)
{
    mArgs.insert(EViewId, commLauncherView);
    QVariant var;
    var.setValue(mModel->contact(index));
    mArgs.insert(ESelectedContact, var);
    QVariant varGroup;
    varGroup.setValue(*mGroupContact);
    mArgs.insert(ESelectedGroupContact, varGroup);
    mViewManager->changeView(mArgs);
        
}

void CntGroupMemberView::removeFromGroup(const QModelIndex &index)
{
    // get contact id using index
    QContact selectedContact = mModel->contact(index);
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(mGroupContact->id());
    relationship.setSecond(selectedContact.id());
    getContactManager()->removeRelationship(relationship);
    
    // delete the model and recreate it with relationship changes
    delete mModel;
    mModel = NULL;
    
    createModel();
}

void CntGroupMemberView::editContact(const QModelIndex &index)
{

    mArgs.insert(EViewId, editView);
    QVariant var;
    var.setValue(mModel->contact(index));
    mArgs.insert(ESelectedContact, var);
    mViewManager->changeView(mArgs);
}

void CntGroupMemberView::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    Q_UNUSED(error);
    if (!error)
    {
        QIcon qicon(pixmap);
        HbIcon icon(qicon);
        mHeadingItem->setIcon(icon);
        
        mImageLabel->clear();
        mImageLabel->setIcon(icon);
    }
}

void CntGroupMemberView::openImageEditor()
{
    QVariant var;
    var.setValue(*mGroupContact);
                
    // SelectedGroupContact is needed so we know which group to show
    // when we come back to this view, and selected contact is needed
    // for image editor to be able to assign the image to correct contact.
    mArgs.insert(ESelectedGroupContact, var);
    mArgs.insert(ESelectedContact, var);
    
    mArgs.insert(EViewId, imageEditorView );
        
    mViewManager->changeView( mArgs );
}

QContactManager* CntGroupMemberView::getContactManager()
{
    return mViewManager->contactManager(SYMBIAN_BACKEND);
}

void CntGroupMemberView::setRelationship(QSet<QContactLocalId>        &aLocalId,
                                         QList<QContactRelationship>  &aRelationshipList)
{
    foreach (QContactLocalId id, aLocalId) {
        QContact contact = getContactManager()->contact(id);

        QContactRelationship membership;
        membership.setRelationshipType(QContactRelationship::HasMember);
        membership.setFirst(mGroupContact->id());
        membership.setSecond(contact.id());
        aRelationshipList.append(membership);
    }
}

/*!
Draw the image specific content menu
*/
void CntGroupMemberView::drawImageMenu(const QPointF &aCoords)
{
    HbMenu *menu = new HbMenu();
    HbAction *changeImageAction = menu->addAction(hbTrId("txt_phob_menu_change_picture"), this, SLOT(openImageEditor()));
    if (mAvatar)
    {
        HbAction *removeAction = menu->addAction(hbTrId("txt_phob_menu_remove_image"), this, SLOT(removeImage()));
    }
    menu->setPreferredPos(aCoords);
    menu->open();
}


void CntGroupMemberView::removeImage()
{
    if (mAvatar) 
    {
        if (!mAvatar->imageUrl().isEmpty())
        {
            mGroupContact->removeDetail(mAvatar);
            // Check if image removable.
            CntImageUtility imageUtility;
            if(imageUtility.isImageRemovable(mAvatar->imageUrl().toString()))
            {
                imageUtility.removeImage(mAvatar->imageUrl().toString());
            }
            mViewManager->contactManager( SYMBIAN_BACKEND )->saveContact(mGroupContact);
            mHeadingItem->setIcon(HbIcon("qtg_large_avatar"));
        }
    }
}



// end of file
