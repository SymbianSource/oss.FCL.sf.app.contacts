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
#include "cntcontactcardheadingitem.h"
#include "cntgroupselectionpopup.h"
#include "qtpbkglobal.h"
#include "cntimagelabel.h"
#include <hbnotificationdialog.h>
#include <hbmenu.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbframebackground.h>
#include <hbview.h>
#include <hbaction.h>
#include <hblabel.h>
#include <thumbnailmanager_qt.h>
#include <mobcntmodel.h>
#include <hbmainwindow.h>


#include <QGraphicsLinearLayout>

#include <hbdocumentloader.h>
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
    mListView(NULL),
    mImageLabel(NULL)    
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
    
    mListView = static_cast<HbListView*>( mDocument->findWidget("listView") );
    connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
        SLOT(showContextMenu(HbAbstractViewItem*,QPointF)));
    connect(mListView, SIGNAL(activated (const QModelIndex&)), this,
        SLOT(showContactView(const QModelIndex&)));
    
    mHeadingItem = static_cast<CntContactCardHeadingItem*>( mDocument->findWidget("editViewHeading") );

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
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
   
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
    
    if (mView->navigationAction() != mSoftkey)
        {
        mView->setNavigationAction(mSoftkey);   
        }
        
    QVariant contact = aArgs.value( ESelectedContact );
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
            mThumbnailManager->getThumbnail(details.at(i).imageUrl().toString());
            break;
            }
    }
    
    // create list & model
    mListView->setFrictionEnabled(true);
    mListView->setScrollingStyle(HbScrollArea::PanOrFlick);
    mListView->setUniformItemSizes(true);
    mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::Thumbnail);

    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    
    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRelatedContactRole(QContactRelationship::First);
    rFilter.setRelatedContactId(mGroupContact->id());
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirst);
    sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLast);
    sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QContactSortOrder> sortOrders;
    sortOrders.append(sortOrderFirstName);
    sortOrders.append(sortOrderLastName);

    mModel = new MobCntModel(mViewManager->contactManager(SYMBIAN_BACKEND), rFilter, sortOrders, false);
    mListView->setModel(mModel);
    
    if (aArgs.value(ESelectedAction).toString() == "save")
    {
        QString name = mViewManager->contactManager(SYMBIAN_BACKEND)->synthesizedDisplayLabel(*mGroupContact);
        HbNotificationDialog::launchDialog(hbTrId("Group \"%1\" saved").arg(name));
    }
}

void CntGroupMemberView::deactivate()
{
    
}

void CntGroupMemberView::showPreviousView()
{
    CntViewParameters viewParameters;
    mViewManager->back(viewParameters);
}

void CntGroupMemberView::openGroupActions()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, groupActionsView);
    QVariant var;
    var.setValue(*mGroupContact);
    viewParameters.insert(ESelectedContact, var);
    mViewManager->changeView(viewParameters);
}

void CntGroupMemberView::manageMembers()
{
    // save the group here
    CntGroupSelectionPopup *groupSelectionPopup = 
        new CntGroupSelectionPopup(mViewManager->contactManager(SYMBIAN_BACKEND), mGroupContact);
    mListView->setModel(0);
    groupSelectionPopup->populateListOfContact();
    
    HbAction* action = groupSelectionPopup->exec();
    if (action == groupSelectionPopup->primaryAction())
    {
        groupSelectionPopup->saveOldGroup();
    }
    delete groupSelectionPopup;
    mListView->setModel(mModel);
}

void CntGroupMemberView::editGroup()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, groupEditorView);
    QVariant var;
    var.setValue(*mGroupContact);
    viewParameters.insert(ESelectedContact, var);
    mViewManager->changeView(viewParameters);
}

void CntGroupMemberView::deleteGroup()
{
    // the delete command
     HbDialog popup;

     // disable dismissing & timout
     popup.setDismissPolicy(HbDialog::NoDismiss);
     popup.setTimeout(HbDialog::NoTimeout);
     
     QString groupName = mGroupContact->displayLabel();
     HbLabel *headingLabel = new HbLabel(hbTrId("Delete %1 group?").arg(groupName), &popup);
     headingLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
     popup.setHeadingWidget(headingLabel);

     // Set a label widget as content widget in the dialog
     HbLabel *label = new HbLabel(&popup);
     label->setPlainText("Only group will be removed, contacts can be found from All contacts list");
     label->setTextWrapping(Hb::TextWordWrap);
     label->setElideMode(Qt::ElideNone);
     label->setFontSpec(HbFontSpec(HbFontSpec::Secondary));
     popup.setContentWidget(label);
     
     // Sets the primary action and secondary action
     popup.setPrimaryAction(new HbAction(hbTrId("txt_phob_button_delete"),&popup));
     popup.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"),&popup));

     HbAction* action = popup.exec();
     if (action == popup.primaryAction())
     {
         mViewManager->contactManager(SYMBIAN_BACKEND)->removeContact(mGroupContact->localId());
         showPreviousView();
     }
}

/*!
Called when a list item is longpressed
*/
void CntGroupMemberView::showContextMenu(HbAbstractViewItem *aItem, const QPointF &aCoords)
{
    QModelIndex index = aItem->modelIndex();

    HbMenu *menu = new HbMenu();
    HbAction *removeFromGroupAction = 0;
    HbAction *openContactAction = 0;
    HbAction *editContactAction = 0;

    openContactAction = menu->addAction(hbTrId("txt_common_menu_open"));
    editContactAction = menu->addAction(hbTrId("txt_common_menu_edit"));
    removeFromGroupAction = menu->addAction(hbTrId("txt_phob_menu_remove_from_group"));
    

    HbAction *selectedAction = menu->exec(aCoords);

    if (selectedAction)
    {
        if (selectedAction == removeFromGroupAction)
        {
            removeFromGroup(index);
        }
        else if (selectedAction == editContactAction)
        {
            editContact(index);
        }
        else if (selectedAction == openContactAction)
        {
            showContactView(index);
        }
    }
    menu->deleteLater();
}

/*!
Called after user clicked on the listview.
*/
void CntGroupMemberView::showContactView(const QModelIndex &index)
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, commLauncherView);
    QVariant var;
    var.setValue(mModel->contact(index));
    viewParameters.insert(ESelectedContact, var);
    QVariant varGroup;
    varGroup.setValue(*mGroupContact);
    viewParameters.insert(ESelectedGroupContact, varGroup);
    viewParameters.insert(ESelectedAction, "FromGroupMemberView");
    mViewManager->changeView(viewParameters);
        
}

void CntGroupMemberView::removeFromGroup(const QModelIndex &index)
{
    // get contact id using index
    QContact selectedContact = mModel->contact(index);
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(mGroupContact->id());
    relationship.setSecond(selectedContact.id());
    mViewManager->contactManager(SYMBIAN_BACKEND)->removeRelationship(relationship);
}

void CntGroupMemberView::editContact(const QModelIndex &index)
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, editView);
    QVariant var;
    var.setValue(mModel->contact(index));
    viewParameters.insert(ESelectedContact, var);
    mViewManager->changeView(viewParameters);
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
                
    CntViewParameters viewParameters;
    viewParameters.insert(ESelectedContact, var);
    viewParameters.insert(EViewId, imageEditorView );
        
    mViewManager->changeView( viewParameters );
}



// end of file
