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
#include <hbsearchpanel.h>
#include <hbtextitem.h>


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
    mSearchPanel(0),
    mEmptyListLabel(0)    
{

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
    if (mSearchPanel)
    {
        closeFind();
    }
    else
    {
    CntViewParameters viewParameters(CntViewParameters::groupActionsView);
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->onActivateView(viewParameters);
    }
}

/*!
Add actions also to toolbar
*/
void CntGroupMemberView::addActionsToToolBar()
{
    actions()->clearActionList();
       
    actions()->actionList() << actions()->baseAction("cnt:managemembers") << actions()->baseAction("cnt:find");
        actions()->addActionsToToolBar(toolBar());
        
        //setItemVisible
    connect(actions()->baseAction("cnt:managemembers"), SIGNAL(triggered()),
            this, SLOT(manageMembers()));
    connect(actions()->baseAction("cnt:find"), SIGNAL(triggered()),
                this, SLOT(find()));
    actions()->baseAction("cnt:find")->setEnabled(false); // to be enabled after Intersection filter implementation
    
 }
void CntGroupMemberView::find()
{
    if (mSearchPanel == 0)
        {
            toolBar()->hide();
            mSearchPanel = new HbSearchPanel();
            setBannerName(hbTrId("Find: Group contacts"));
            banner()->setVisible(true);
            listLayout()->addItem(mSearchPanel);
            contactModel()->showMyCard(false);
            setFilter(QString());

            connect(mSearchPanel, SIGNAL(exitClicked()), this, SLOT(closeFind()));
            connect(mSearchPanel, SIGNAL(criteriaChanged(QString)), this, SLOT(setFilter(QString)));
        }
    
}

void CntGroupMemberView::setFilter(const QString &filterString)
{
    QStringList searchList = filterString.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);
    filter.setMatchFlags(QContactFilter::MatchStartsWith);
    filter.setValue(searchList);
    
    mFilteredLocalIdList = contactManager()->contacts(filter);
    
    contactModel()->setFilterAndSortOrder(filter);
    // use intersection filter here <support from engine side>

    if (!contactModel()->rowCount())
    {
        listLayout()->removeItem(listView());
        listView()->setVisible(false);
        if (mEmptyListLabel == 0)
        {
            mEmptyListLabel = new HbTextItem(hbTrId("(no matching contacts)"));
            mEmptyListLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
            mEmptyListLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
            mEmptyListLabel->setAlignment(Qt::AlignCenter);
            listLayout()->insertItem(1, mEmptyListLabel);
        }
    }
    else
    {
        listLayout()->removeItem(mEmptyListLabel);
        delete mEmptyListLabel;
        mEmptyListLabel = 0;
        listLayout()->insertItem(1, listView());
        listView()->setVisible(true);
    }
}

void CntGroupMemberView::closeFind()
{
    if( mSearchPanel)
    {
        listLayout()->removeItem(mEmptyListLabel);
        delete mEmptyListLabel;
        mEmptyListLabel = 0;

        listLayout()->removeItem(banner());
        banner()->setVisible(false);
        listLayout()->removeItem(mSearchPanel);
        listLayout()->addItem(listView());
        listView()->setVisible(true);

        // display User-groups
        QContactRelationshipFilter rFilter;
        rFilter.setRelationshipType(QContactRelationship::HasMember);
        rFilter.setRole(QContactRelationshipFilter::Second);
        rFilter.setOtherParticipantId(mGroupContact->id());
        
        contactModel()->setFilterAndSortOrder(rFilter);
        contactModel()->showMyCard(false);

        mSearchPanel->deleteLater();
        mSearchPanel = 0;
        toolBar()->show();
    }
}

void CntGroupMemberView::groupActions()
{
    CntViewParameters viewParameters(CntViewParameters::groupActionsView);
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->onActivateView(viewParameters);
}

void CntGroupMemberView::callNamesList()
{
   CntViewParameters viewParameters(CntViewParameters::namesView);
   viewManager()->onActivateView(viewParameters);
}

void CntGroupMemberView::manageMembers()
{
    // save the group here
    CntGroupSelectionPopup *groupSelectionPopup = new CntGroupSelectionPopup(contactManager(),contactModel(),mGroupContact);
    listView()->hide();
    groupSelectionPopup->populateListOfContact();

    
    HbAction* action = groupSelectionPopup->exec();
    if (action == groupSelectionPopup->primaryAction())
    {
        groupSelectionPopup->saveOldGroup();
        CntViewParameters viewParameters(CntViewParameters::groupActionsView);
        viewParameters.setSelectedContact(*mGroupContact);
        viewManager()->onActivateView(viewParameters);
    }
    delete groupSelectionPopup;
    listView()->show();
}
/*!
Add actions to menu
*/
void CntGroupMemberView::addMenuItems()
{
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:editgroupdetails") << actions()->baseAction("cnt:managemembersmenu") <<
            actions()->baseAction("cnt:placegrouptohs") << actions()->baseAction("cnt:deletegroup");
    actions()->addActionsToMenu(menu());

    connect(actions()->baseAction("cnt:editgroupdetails"), SIGNAL(triggered()),
            this, SLOT (editGroup()));
    
    connect(actions()->baseAction("cnt:managemembersmenu"), SIGNAL(triggered()),
                this, SLOT (manageMembers()));

    connect(actions()->baseAction("cnt:placegrouptohs"), SIGNAL(triggered()),
            this, SLOT (placeGroupToHs()));

    connect(actions()->baseAction("cnt:deletegroup"), SIGNAL(triggered()),
            this, SLOT (deleteGroup()));

}

void CntGroupMemberView::editGroup()
{
    CntViewParameters viewParameters(CntViewParameters::groupEditorView);
    viewParameters.setSelectedAction("EditGroupDetails");
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->onActivateView(viewParameters);
}

void CntGroupMemberView::placeGroupToHs()
{
// wait for specs
}

void CntGroupMemberView::deleteGroup()
{
    // the delete command
    connect(commands(), SIGNAL(commandExecuted(QString, QContact)), this, 
            SLOT(handleExecutedCommand(QString, QContact)));
    commands()->deleteContact(*mGroupContact);
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
    HbAction *viewDetailsAction = 0;

    QString action = map.value("action").toString();

    removeFromGroupAction = menu->addAction(hbTrId("txt_phob_menu_remove_from_group"));
    viewDetailsAction = menu->addAction(hbTrId("View details"));

    HbAction *selectedAction = menu->exec(aCoords);

    if (selectedAction)
    {
        if (selectedAction == removeFromGroupAction)
        {
            removeFromGroup(index);
        }
        else if (selectedAction == viewDetailsAction)
        {
            onListViewActivated(index);
        }
    }
    menu->deleteLater();
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

void CntGroupMemberView::viewDetailsOfGroupContact(const QModelIndex &index)
{
    QContact selectedContact = contactModel()->contact(index);
    CntViewParameters viewParameters(CntViewParameters::commLauncherView);
    viewParameters.setSelectedContact(selectedContact);
    viewManager()->onActivateView(viewParameters);
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
    viewManager()->onActivateView(viewParameters);
        
}

void CntGroupMemberView::handleExecutedCommand(QString command, QContact /*contact*/)
{
    if (command == "delete")
    {
        CntViewParameters viewParameters(CntViewParameters::collectionView);
        viewManager()->onActivateView(viewParameters);
    }
}

void CntGroupMemberView::activateView(const CntViewParameters &viewParameters)
{
    QContact contact = viewParameters.selectedContact();
    mGroupContact = new QContact(contact);
    
    QContactName groupContactName = mGroupContact->detail( QContactName::DefinitionName );
    QString groupName(groupContactName.value( QContactName::FieldCustomLabel ));
  
    setBannerName(groupName);
    
    // display User-groups
    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRole(QContactRelationshipFilter::Second);
    rFilter.setOtherParticipantId(mGroupContact->id());
    
    mLocalIdList = contactManager()->contacts(rFilter);
    
    contactModel()->setFilterAndSortOrder(rFilter);

    contactModel()->showMyCard(false);
    if (viewParameters.selectedAction() == "save")
    {
        QString name = contactManager()->synthesizeDisplayLabel(viewParameters.selectedContact());
        HbNotificationDialog::launchDialog(hbTrId("Group \"%1\" saved").arg(name));
    }
    
    CntBaseListView::activateView(viewParameters);
}

// end of file
