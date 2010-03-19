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

#include "cntcollectionview.h"

#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <hbnotificationdialog.h>
#include <hblabel.h>
#include "hbinputdialog.h"
#include "hbdialog.h"
#include "hbaction.h"
#include "cntgroupselectionpopup.h"

const char *CNT_COLLECTIONVIEW_ACTIONS_XML = ":/xml/contacts_actions.docml";

/*!

*/
CntCollectionView::CntCollectionView(CntViewManager *viewManager, QGraphicsItem *parent)
    : CntBaseListView(viewManager, parent),
    mModel(0),
    mReorderAction(0), 
    mDeleteGroupAction(0), 
    mDisconnectAllAction(0),
    mOptionsMenu(0)
{
    setBannerName(hbTrId("txt_phob_subtitle_groups"));
}

/*!

*/
CntCollectionView::~CntCollectionView()
{
delete mModel;
mModel = 0;
delete mOptionsMenu;
mOptionsMenu = 0;
}

/*!

*/
void CntCollectionView::aboutToCloseView()
{
    CntViewParameters viewParameters(CntViewParameters::namesView);
    viewManager()->onActivateView(viewParameters);
}

/*!
Called after user clicked on the listview.
*/
void CntCollectionView::onListViewActivated(const QModelIndex &index)
{
    int id  = index.data(Qt::UserRole).toInt();
    
    if (id == -1)
    {
        CntViewParameters viewParameters(CntViewParameters::collectionFavoritesView);
        viewManager()->onActivateView(viewParameters);
    }
    else
    {
        QContact groupContact = contactManager()->contact(id);
        CntViewParameters viewParameters(CntViewParameters::groupActionsView);
        viewParameters.setSelectedContact(groupContact);
        viewManager()->onActivateView(viewParameters);
    }
}

void CntCollectionView::onLongPressed(HbAbstractViewItem *item, const QPointF &coords)
{
    HbMenu *menu = new HbMenu();
    HbAction *openAction = 0;
    HbAction *deleteAction = 0;
    HbAction *homeScreenAction = 0;
    
    openAction = menu->addAction(hbTrId("txt_common_menu_open"));
    
    if (item->modelIndex().data(Qt::UserRole).toInt() > -1)
    {
        deleteAction = menu->addAction(hbTrId("txt_phob_menu_delete_group"));
    }
    
    homeScreenAction = menu->addAction(hbTrId("txt_phob_opt_send_to_homescreen_as_widget"));
    homeScreenAction->setEnabled(false);
    
    HbAction *selectedAction = menu->exec(coords);

    if (selectedAction)
    {
        if (selectedAction == openAction)
        {
            onListViewActivated(item->modelIndex());
        }
        else if (selectedAction == deleteAction)
        {
            connect(commands(), SIGNAL(commandExecuted(QString, QContact)), this, 
                    SLOT(handleExecutedCommand(QString, QContact)));
            QContact groupContact = contactManager()->contact(item->modelIndex().data(Qt::UserRole).toInt());
            commands()->deleteContact(groupContact);
        }
        else if (selectedAction == homeScreenAction)
        {

        }
    }
    menu->deleteLater();
}

void CntCollectionView::handleExecutedCommand(QString command, QContact contact)
{
    if (command == "delete")
    {
        static_cast<CntCollectionListModel*>(listView()->model())->removeGroup(contact.localId());
    }
}

/*!
Add actions also to toolbar
*/
void CntCollectionView::addActionsToToolBar()
{
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:nameslist") << actions()->baseAction("cnt:collections")
        << actions()->baseAction("cnt:refresh") << actions()->baseAction("cnt:newgroup");
    actions()->addActionsToToolBar(toolBar());

    connect(actions()->baseAction("cnt:nameslist"), SIGNAL(triggered()),
       this, SLOT (aboutToCloseView()));

    actions()->baseAction("cnt:collections")->setEnabled(false);

    connect(actions()->baseAction("cnt:newgroup"), SIGNAL(triggered()),
               this, SLOT (newGroup()));
}

void CntCollectionView::newGroup()
{
    QString mTextOfNewItem("");
    
    HbInputDialog popup;
    
    HbGroupBox *headingLabel = new HbGroupBox();
    HbLabel *label = new HbLabel(hbTrId("txt_phob_title_new_group_name"));    
    headingLabel->setContentWidget(label);
    popup.setHeadingWidget(headingLabel);
    popup.setPrimaryAction(new HbAction(hbTrId("txt_phob_button_create"),&popup));
    popup.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"),&popup));
    popup.setInputMode(HbInputDialog::TextInput);
    popup.setPromptText("");
    popup.setBackgroundFaded(true);
    HbAction* action = popup.exec();
    QString text = popup.value().toString();
    
    if (action == popup.primaryAction())
    {
        mTextOfNewItem = text;
        
        QContact groupContact;
        groupContact.setType(QContactType::TypeGroup);
        
        
        QContactName groupName;
        groupName.setCustomLabel(mTextOfNewItem);
        
        groupContact.saveDetail(&groupName);
        contactManager()->saveContact(&groupContact);
        
        // call a dialog to display the contacts
        
        CntGroupSelectionPopup *groupSelectionPopup = new CntGroupSelectionPopup(contactManager(),contactModel(),&groupContact);
        groupSelectionPopup->populateListOfContact();
        HbAction* action = groupSelectionPopup->exec();
        if (action == groupSelectionPopup->primaryAction())
        {
            groupSelectionPopup->saveNewGroup();
            CntViewParameters viewParameters(CntViewParameters::groupActionsView);
            viewParameters.setSelectedContact(groupContact);
            viewManager()->onActivateView(viewParameters);
            delete groupSelectionPopup;
        }
        else if (action == groupSelectionPopup->secondaryAction())
        {
            delete groupSelectionPopup;
            QString groupNameCreated(groupName.value( QContactName::FieldCustomLabel ));
            HbNotificationDialog::launchDialog(hbTrId("txt_phob_dpophead_new_group_1_created").arg(groupNameCreated));
            //refresh the page 
            refreshDataModel();
            //reconstruct the menu items 
            addMenuItems();
        }
        
    }
}

void CntCollectionView::setDataModel()
{
    HbListViewItem *prototype = listView()->listItemPrototype();
    prototype->setGraphicsSize(HbListViewItem::LargeIcon);
    mModel = new CntCollectionListModel(contactManager(), this);
    listView()->setModel(mModel);
}

void CntCollectionView::refreshDataModel()
{
    delete mModel;
    mModel = 0;
    mModel = new CntCollectionListModel(contactManager(), this);
    listView()->setModel(mModel);
}

void CntCollectionView::addMenuItems()
{
    bool ok = false;
    HbDocumentLoader documentLoader;
    documentLoader.load(CNT_COLLECTIONVIEW_ACTIONS_XML, &ok);
    if (!ok)
    {
        qFatal("Unable to read :/xml/contacts_actions.docml");
    }
    
    //Uncomment this once spec for this is ready
  //  mReorderAction = qobject_cast<HbAction *>(documentLoader.findObject("cnt:reordergroups"));
    mDeleteGroupAction = qobject_cast<HbAction* >(documentLoader.findObject("cnt:deletegroups"));
 
    //Uncomment this once spec for this is ready
  //  mDisconnectAllAction =qobject_cast<HbAction *>(documentLoader.findObject("cnt:disconnectall"));
    
    delete mOptionsMenu;
    mOptionsMenu = 0;
    mOptionsMenu = new HbMenu();
    //Uncomment this once spec for this is ready
    //mOptionsMenu->addAction(mReorderAction);
    
    //add deletegroup action to option menu if group presents 
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));
    QList<QContactLocalId> groupContactIds = contactManager()->contacts(groupFilter);
    if (!groupContactIds.isEmpty())
        {
        mOptionsMenu->addAction(mDeleteGroupAction);
        }
    
    //Uncomment this once spec for this is ready
 //   mOptionsMenu->addAction(mDisconnectAllAction);
    
 //   connect(mReorderAction, SIGNAL(triggered()), this, SLOT (reorderGroup()));
    connect(mDeleteGroupAction, SIGNAL(triggered()), this, SLOT (deleteGroups()));  
   // connect(mDisconnectAllAction, SIGNAL(triggered()), this, SLOT (disconnectAll()));

    setMenu(mOptionsMenu);
}

void CntCollectionView::reorderGroup()
{
    // wait for specs
}

void CntCollectionView::deleteGroups()
{
    // save the group here
    QContact groupContact;
    groupContact.setType(QContactType::TypeGroup);
    
    CntGroupSelectionPopup *groupSelectionPopup = new CntGroupSelectionPopup(contactManager(),contactModel(),&groupContact);
    
    groupSelectionPopup->populateListOfGroup();
    HbAction* action = groupSelectionPopup->exec();
    if (action == groupSelectionPopup->primaryAction())
    {   
        groupSelectionPopup->deleteGroup();
    }
    
    delete groupSelectionPopup;
    
    //refresh the page 
    refreshDataModel();
    //reconstruct the menu items 
    addMenuItems();
}

void CntCollectionView::disconnectAll()
{
    // wait for specs
}

// EOF
