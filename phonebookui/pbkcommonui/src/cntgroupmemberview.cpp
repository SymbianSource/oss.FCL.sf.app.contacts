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
#include "qtpbkglobal.h"
#include "cntcommands.h"
#include <hbnotificationdialog.h>
#include <hbmenu.h>

#include <QStringListModel>
#include <QMap>
#include <hbpushbutton.h>
#include <QGraphicsLinearLayout>
#include <hbdocumentloader.h>
#include <hblistview.h>
#include <hblabel.h>
#include <hblistviewitem.h>
#include <hbtoolbar.h>
#include <thumbnailmanager_qt.h>
#include "cntcontactcardheadingitem.h"
#include "cntgroupselectionpopup.h"

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
CntGroupMemberView::CntGroupMemberView(CntViewManager *viewManager, QGraphicsItem *parent)
    : CntBaseListView(viewManager, parent),
    mNoGroupContactsPresent(0),
    mGroupContact(0),
    mHeadingItem(0),
    mThumbnailManager(0)
{
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
    delete mGroupContact;
}

void CntGroupMemberView::aboutToCloseView()
{
    CntViewParameters viewParameters;//(CntViewParameters::groupActionsView);
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->back(viewParameters);
}

/*!
Add actions also to toolbar
*/
void CntGroupMemberView::addActionsToToolBar()
{
    actions()->clearActionList();
       
           
        
   
    
    actions()->clearActionList();
        actions()->actionList() << actions()->baseAction("cnt:managemembers") << actions()->baseAction("cnt:deletegroup")
            << actions()->baseAction("cnt:groupactions");
        actions()->addActionsToToolBar(toolBar());

        connect(actions()->baseAction("cnt:managemembers"), SIGNAL(triggered()),
                    this, SLOT(manageMembers()));
        connect(actions()->baseAction("cnt:deletegroup"), SIGNAL(triggered()),
           this, SLOT(deleteGroup()));    
        connect(actions()->baseAction("cnt:groupactions"), SIGNAL(triggered()),
                this, SLOT(groupActions()));
    
 }

void CntGroupMemberView::groupActions()
{
    CntViewParameters viewParameters(CntViewParameters::groupActionsView);
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->changeView(viewParameters);
}


void CntGroupMemberView::manageMembers()
{
    // save the group here
    CntGroupSelectionPopup *groupSelectionPopup = new CntGroupSelectionPopup(contactManager(), mGroupContact);
    listView()->setModel(0);
    groupSelectionPopup->populateListOfContact();

    
    HbAction* action = groupSelectionPopup->exec();
    if (action == groupSelectionPopup->primaryAction())
    {
        groupSelectionPopup->saveOldGroup();
    }
    delete groupSelectionPopup;
    listView()->setModel(contactModel());
}
/*!
Add actions to menu
*/
void CntGroupMemberView::addMenuItems()
{
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:editgroupdetails");
    actions()->addActionsToMenu(menu());

    connect(actions()->baseAction("cnt:editgroupdetails"), SIGNAL(triggered()),
            this, SLOT (editGroup()));
       

}

void CntGroupMemberView::editGroup()
{
    CntViewParameters viewParameters(CntViewParameters::groupEditorView);
    viewParameters.setSelectedAction("EditGroupDetails");
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->changeView(viewParameters);
}

void CntGroupMemberView::deleteGroup()
{
    // the delete command
     HbDialog popup;

     // Set dismiss policy that determines what tap events will cause the dialog
     // to be dismissed
     popup.setDismissPolicy(HbDialog::NoDismiss);
     
     QContactName groupContactName = mGroupContact->detail( QContactName::DefinitionName );
     QString groupName(groupContactName.value( QContactName::FieldCustomLabel ));
     // Set the label as heading widget
     popup.setHeadingWidget(new HbLabel(hbTrId("Delete %1 group?").arg(groupName))); 

     // Set a label widget as content widget in the dialog
     HbLabel *label = new HbLabel;
     label->setPlainText("Only group will be removed, contacts can be found from All contacts list");
     label->setTextWrapping(Hb::TextWordWrap);
     popup.setContentWidget(label);
     
     // Sets the primary action and secondary action
     popup.setPrimaryAction(new HbAction(hbTrId("txt_phob_button_delete"),&popup));
     popup.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"),&popup));

     popup.setTimeout(0) ;
     HbAction* action = popup.exec();
     if (action == popup.primaryAction())
     {
         contactManager()->removeContact(mGroupContact->localId());
         CntViewParameters viewParameters(CntViewParameters::collectionView);
         viewParameters.setSelectedAction("EditGroupDetails");
         viewParameters.setSelectedContact(*mGroupContact);
         viewManager()->changeView(viewParameters);
     }
}

/*!
Called when a list item is longpressed
*/
void CntGroupMemberView::onLongPressed (HbAbstractViewItem *aItem, const QPointF &aCoords)
{
    QModelIndex index = aItem->modelIndex();
    QVariant variant = index.data(Qt::UserRole+1);
    const QMap<QString, QVariant> map = variant.toMap();

    HbMenu *menu = new HbMenu();
    HbAction *removeFromGroupAction = 0;
    HbAction *openContactAction = 0;
    HbAction *editContactAction = 0;
    QString action = map.value("action").toString();

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
            onListViewActivated(index);
        }
       
    }
    menu->deleteLater();
}

void CntGroupMemberView::editContact(const QModelIndex &index)
{
    QContact selectedContact = contactModel()->contact(index);
    CntViewParameters viewParameters(CntViewParameters::editView);
    viewParameters.setSelectedContact(selectedContact);
    viewManager()->changeView(viewParameters);
}

void CntGroupMemberView::removeFromGroup(const QModelIndex &index)
{
    // get contact id using index
    QContact selectedContact = contactModel()->contact(index);
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(mGroupContact->id());
    relationship.setSecond(selectedContact.id());
    contactManager()->removeRelationship(relationship);
}

/*!
Called after user clicked on the listview.
*/
void CntGroupMemberView::onListViewActivated(const QModelIndex &index)
{
    CntViewParameters viewParameters(CntViewParameters::commLauncherView);
    viewParameters.setSelectedContact(contactModel()->contact(index));
    viewParameters.setSelectedGroupContact(*mGroupContact);
    viewParameters.setSelectedAction("FromGroupMemberView");
    viewManager()->changeView(viewParameters);
        
}

void CntGroupMemberView::handleExecutedCommand(QString command, QContact /*contact*/)
{
    if (command == "delete")
    {
        CntViewParameters viewParameters(CntViewParameters::collectionView);
        viewManager()->changeView(viewParameters);
    }
}

void CntGroupMemberView::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    Q_UNUSED(error);
    QIcon qicon(pixmap);
    HbIcon icon(qicon);
    mHeadingItem->setIcon(icon);
}

void CntGroupMemberView::activateView(const CntViewParameters &viewParameters)
{
    QContact contact = viewParameters.selectedContact();
    mGroupContact = new QContact(contact);
    
    //QContactName groupContactName = mGroupContact->detail( QContactName::DefinitionName );
  // QString groupName(groupContactName.value( QContactName::FieldCustomLabel ));
  
    //setBannerName(groupName);
    
    // add heading widget to the content
    QGraphicsWidget *c = findWidget(QString("container"));
    QGraphicsLinearLayout* l = static_cast<QGraphicsLinearLayout*>(c->layout());

    mHeadingItem = new CntContactCardHeadingItem(c);
    mHeadingItem->setGroupDetails(mGroupContact);

    l->insertItem(0, mHeadingItem);
    
    // avatar
    QList<QContactAvatar> details = mGroupContact->details<QContactAvatar>();
    if (details.count() > 0)
    {
        for (int i = 0;i < details.count();i++)
        {
            if (details.at(i).subType() == QContactAvatar::SubTypeImage)
            {
                mThumbnailManager->getThumbnail(details.at(i).avatar());
                break;
            }
        }
    }
    
    // display group members
    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRelatedContactRole(QContactRelationshipFilter::First);
    rFilter.setRelatedContactId(mGroupContact->id());
    
    mLocalIdList = contactManager()->contactIds(rFilter);
    
    contactModel()->setFilterAndSortOrder(rFilter);

    contactModel()->showMyCard(false);
    if (viewParameters.selectedAction() == "save")
    {
        QString name = contactManager()->synthesizedDisplayLabel(viewParameters.selectedContact());
        HbNotificationDialog::launchDialog(hbTrId("Group \"%1\" saved").arg(name));
    }
    
    CntBaseListView::activateView(viewParameters);
}

// end of file
