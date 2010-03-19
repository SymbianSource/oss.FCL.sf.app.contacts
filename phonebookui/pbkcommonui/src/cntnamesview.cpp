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

#include "cntnamesview.h"
#include "qtpbkglobal.h"

#include <hbnotificationdialog.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbsearchpanel.h>
#include <hbtextitem.h>

#include <QGraphicsLinearLayout>

/*!
\class CntNamesView
\brief

This is the namesview class that shows list of contacts for the user. View contains banner for OVI and a listview that shows actual contacts.
There is also toolbar and menu for navigating between different views. Instance of this class is created by our viewmanager but view itself is
owned by the mainwindow which will also delete it in the end.

*/

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)

*/
CntNamesView::CntNamesView(CntViewManager *viewManager, QGraphicsItem *parent)
    : CntBaseListView(viewManager, parent),
    mSearchPanel(0),
    mEmptyListLabel(0)
{
    connect(commands(), SIGNAL(commandExecuted(QString, QContact)),
            this, SLOT(handleExecutedCommand(QString, QContact)));
}

/*!
Destructor
*/
CntNamesView::~CntNamesView()
{
    if (mSearchPanel)
    {
        delete mSearchPanel;
        mSearchPanel = 0;
    }
    delete mEmptyListLabel;
}

void CntNamesView::aboutToCloseView()
{
    if (mSearchPanel)
    {
        closeFind();
    }
    else
    {
        qApp->quit();
    }
}

/*!
Add actions also to toolbar
*/
void CntNamesView::addActionsToToolBar()
{
    //Add Action to the toolbar
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:nameslist") << actions()->baseAction("cnt:collections")
        << actions()->baseAction("cnt:find") << actions()->baseAction("cnt:activitystream");
    actions()->addActionsToToolBar(toolBar());

    actions()->baseAction("cnt:nameslist")->setEnabled(false);

    connect(actions()->baseAction("cnt:collections"), SIGNAL(triggered()),
           this, SLOT(openCollections()));
    
    connect(actions()->baseAction("cnt:find"), SIGNAL(triggered()),
            this, SLOT(showFind()));
}

void CntNamesView::openCollections()
{
    CntViewParameters viewParameters(CntViewParameters::collectionView);
    viewManager()->onActivateView(viewParameters);
}

void CntNamesView::showFind()
{
    if (mSearchPanel == 0)
    {
        toolBar()->hide();
        mSearchPanel = new HbSearchPanel();
        setBannerName(hbTrId("txt_phob_subtitle_find_all_contacts"));
        banner()->setVisible(true);
        listLayout()->addItem(mSearchPanel);
        contactModel()->showMyCard(false);
        setFilter(QString());

        connect(mSearchPanel, SIGNAL(exitClicked()), this, SLOT(closeFind()));
        connect(mSearchPanel, SIGNAL(criteriaChanged(QString)), this, SLOT(setFilter(QString)));
    }
}

void CntNamesView::setFilter(const QString &filterString)
{
    QStringList searchList = filterString.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    /*QContactDetailFilter findFilter;
    findFilter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);
    findFilter.setMatchFlags(QContactFilter::MatchStartsWith);
    findFilter.setValue(searchList);   
    QContactDetailFilter typeFilter;
    typeFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    QString typeContact = QContactType::TypeContact;
    typeFilter.setValue(typeContact);
    
    QContactIntersectionFilter filter;
    filter.append(findFilter);
    filter.append(typeFilter);*/
    
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);
    filter.setMatchFlags(QContactFilter::MatchStartsWith);
    filter.setValue(searchList);
    
    contactModel()->setFilterAndSortOrder(filter);

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

void CntNamesView::closeFind()
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

        QContactDetailFilter filter;
        filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        QString typeContact = QContactType::TypeContact;
        filter.setValue(typeContact);

        contactModel()->setFilterAndSortOrder(filter);
        contactModel()->showMyCard(true);

        mSearchPanel->deleteLater();
        mSearchPanel = 0;
        toolBar()->show();
    }
}

/*!
Add actions to menu
*/
void CntNamesView::addMenuItems()
{
    actions()->clearActionList();

    actions()->actionList() << actions()->baseAction("cnt:newcontact") << actions()->baseAction("cnt:import");
    actions()->addActionsToMenu(menu());

    connect(actions()->baseAction("cnt:newcontact"), SIGNAL(triggered()),
            commands(), SLOT (newContact()));
    connect(actions()->baseAction("cnt:import"), SIGNAL(triggered()),
            commands(), SLOT (importFromSim()));
}

void CntNamesView::handleExecutedCommand(QString command, QContact contact)
{
    if (command == "delete")
    {
        QString name = contactManager()->synthesizeDisplayLabel(contact);
        HbNotificationDialog::launchDialog(hbTrId("txt_phob_dpophead_1_deleted").arg(name));
    }
}

void CntNamesView::activateView(const CntViewParameters &viewParameters)
{
    addSoftkeyAction();

    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    QString typeContact = QContactType::TypeContact;
    filter.setValue(typeContact);

    contactModel()->setFilterAndSortOrder(filter);
    contactModel()->showMyCard(true);
    
    if (listView()->model() == 0)
    {
        setDataModel();
    }

    if (viewParameters.selectedAction() == "save")
    {
        QString name = contactManager()->synthesizeDisplayLabel(viewParameters.selectedContact());
        HbNotificationDialog::launchDialog(hbTrId("txt_phob_dpophead_contact_1_saved").arg(name));

        listView()->scrollTo(contactModel()->indexOfContact(viewParameters.selectedContact()));
    }
    else if (viewParameters.selectedAction() == "delete")
    {
        QString name = contactManager()->synthesizeDisplayLabel(viewParameters.selectedContact());
        HbNotificationDialog::launchDialog(hbTrId("txt_phob_dpophead_1_deleted").arg(name));
    }
    else if (viewParameters.selectedAction() == "failed")
    {
        HbNotificationDialog::launchDialog(hbTrId("SAVING FAILED!"));
    }
}

void CntNamesView::deActivateView()
{
    closeFind();
    listView()->setModel(0);
}

// end of file
