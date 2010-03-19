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

#include "cntbaselistview.h"
#include "qtpbkglobal.h"

#include <QGraphicsLinearLayout>
#include <hbwidget.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <hbframebackground.h>

const char *CNT_LISTVIEW_UI_XML = ":/xml/contacts_list.docml";

/*!
\class CntBaseListView
\brief

This class provides common functionality to all views that use HbListView component. View contains banner and a listview that shows actual data.
There is also toolbar and menu for navigating between different views.

*/

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)

*/
CntBaseListView::CntBaseListView(CntViewManager *viewManager, QGraphicsItem *parent)
    : CntBaseView(viewManager, parent),
      mBanner(0),
      mHasBanner(true),
      mListView(0),
      mListLayout(0)
{
    bool ok = false;
    ok = loadDocument(CNT_LISTVIEW_UI_XML);
    if (!ok)
    {
       qFatal("Unable to read :/xml/contacts_list.docml");
    }

}

/*!
Destructor
*/
CntBaseListView::~CntBaseListView()
{

}


/*!
[Virtual] Creates UI elements in this view
*/
void CntBaseListView::setupView()
{
    addItemsToLayout();
    CntBaseView::setupView();
}

void CntBaseListView::activateView(const CntViewParameters &viewParameters)
{
    Q_UNUSED(viewParameters)
    setDataModel();
}

/*!
Adds created controls to the layout
*/
void CntBaseListView::addItemsToLayout()
{
    listView();
    setWidget(findWidget(QString("container")));
}

/*!
Set text for the banner control
*/
void CntBaseListView::setBannerName(QString aBannerName)
{
    mBannerName = aBannerName;
    // add banner
    if (isBannerVisible())
    {
        listLayout()->insertItem(0,banner());
        banner()->setHeading(aBannerName);
    }

}

/*!
return banner control text
*/
QString CntBaseListView::bannerName()
{
    return mBannerName;
}

/*!
\return pointer to HbLabel (banner object)
*/
HbGroupBox *CntBaseListView::banner()
{
    if ((!mBanner) && isBannerVisible())
    {
        mBanner = new HbGroupBox(this);
    }
    return mBanner;
}

/*!
\return pointer to listLayout component
*/
QGraphicsLinearLayout *CntBaseListView::listLayout()
{
    QGraphicsWidget *w = findWidget(QString("container"));
    return static_cast<QGraphicsLinearLayout*>(w->layout());
}

/*!
returns true if banner is set to be visible.
*/
bool CntBaseListView::isBannerVisible()
{
    return mHasBanner;
}

/*!
set banner visibility. Currently in use only in class construction. TODO: expand to hide control if needed.
*/
void CntBaseListView::setBannerVisibility(bool aVisible)
{
    mHasBanner = aVisible;
}


/*!
[Virtual] \return pointer to HbListView component
*/
HbListView *CntBaseListView::listView()
{
    if ( mListView==0 )
    {
        mListView = static_cast<HbListView*>(findWidget(QString("listView")));

        HbListViewItem *prototype = mListView->listItemPrototype();
        prototype->setGraphicsSize(HbListViewItem::Thumbnail);

        mListView->setFrictionEnabled(true);
        mListView->setScrollingStyle(HbScrollArea::PanOrFlick);

        HbFrameBackground frame;
        frame.setFrameGraphicsName("qtg_fr_list_normal");
        frame.setFrameType(HbFrameDrawer::NinePieces);
        mListView->itemPrototypes().first()->setDefaultFrame(frame); 

        connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)),
                                  this,  SLOT(onLongPressed(HbAbstractViewItem*,QPointF)));

        connect(mListView, SIGNAL(activated (const QModelIndex&)),
                                  this,  SLOT(onListViewActivated(const QModelIndex&)));

    }
    return mListView;
}

/*!
Called when a list item is longpressed
*/
void CntBaseListView::onLongPressed (HbAbstractViewItem *item, const QPointF &coords)
{
    QModelIndex index = item->modelIndex();

    // Normal contact
    if (contactModel()->contact(index).localId() != contactManager()->selfContactId())
    {
        HbMenu *menu = new HbMenu();
        HbAction *callAction = 0;
        HbAction *smsAction = 0;
        HbAction *emailAction = 0;
        HbAction *openAction = 0;
        HbAction *editAction = 0;
        HbAction *deleteAction = 0;

        QContact contact = contactModel()->contact(index);
        QList<QContactActionDescriptor> actionDescriptors = contact.availableActions();
        QStringList actions;
        for (int i = 0;i < actionDescriptors.count();i++)
        {
            actions << actionDescriptors.at(i).actionName();
        }

        // Call action
        if (actions.contains("call", Qt::CaseInsensitive))
        {
            // Create call action
            QContactDetail detail = contact.preferredDetail("call");
            QContactPhoneNumber number;

            //if preferred is empty pick first phonenumber
            if(detail.isEmpty())
            {
                number = contact.detail<QContactPhoneNumber>();
            }
            else
            {
                number = detail;
            }

            QString detailName = mStringMapper.getMappedDetail(number.subTypes().first());
            if (!number.contexts().isEmpty())
            {
                detailName += hbTrId(" %1").arg(mStringMapper.getMappedDetail(number.contexts().first()));
            }
            callAction = menu->addAction(hbTrId("Call %1").arg(detailName));
        }

        // SMS action
        if (actions.contains("message", Qt::CaseInsensitive))
        {
            // Create call action
            smsAction = menu->addAction(hbTrId("txt_phob_menu_send_message"));
        }

        // Email action
        if (actions.contains("email", Qt::CaseInsensitive))
        {
            // Create call action
            QContactDetail detail = contact.preferredDetail("email");
            QContactEmailAddress email;

            //if preferred is empty pick first email address
            if(detail.isEmpty())
            {
                email = contact.detail<QContactEmailAddress>();
            }
            else
            {
                email = detail;
            }

            QString detailName = hbTrId("txt_phob_menu_email");
            if (!email.contexts().isEmpty())
            {
                detailName += hbTrId(" %1").arg(mStringMapper.getMappedDetail(email.contexts().first()));
            }
            emailAction = menu->addAction(hbTrId("Mail %1").arg(detailName));
        }

        //add separator in case actions exist
        if (actions.count())
        {
            menu->addSeparator();
        }

        //add open menu option
        openAction = menu->addAction(hbTrId("txt_common_menu_open"));

        //add edit menu option
        editAction = menu->addAction(hbTrId("txt_common_menu_edit"));

        //delete menu option
        deleteAction = menu->addAction(hbTrId("Delete"));

        HbAction *selectedAction = menu->exec(coords);

        if (selectedAction)
        {
            if (selectedAction == callAction)
            {
                commands()->launchAction(contact, QContactDetail(), "call");
            }
            else if (selectedAction == smsAction)
            {
                commands()->launchAction(contact, QContactDetail(), "message");
            }
            else if (selectedAction == emailAction)
            {
                commands()->launchAction(contact, QContactDetail(), "email");
            }
            else if (selectedAction == openAction)
            {
                commands()->openContact(contact);
            }
            else if (selectedAction == editAction)
            {
                commands()->editContact(contact);
            }
            else if (selectedAction == deleteAction)
            {
                commands()->deleteContact(contact);
            }
        }
        menu->deleteLater();
    }
    // MyCard (NOT empty, empty one is ignored -> no longtap menu shown for it)
    else if (contactModel()->contact(index).localId() == contactManager()->selfContactId() && contactModel()->contact(index).details().count() > 4)
    {
        HbMenu *menu = new HbMenu();
        HbAction *openAction = 0;
        HbAction *editAction = 0;
        HbAction *deleteAction = 0;

        QContact contact = contactModel()->contact(index);

        //add open menu option
        openAction = menu->addAction(hbTrId("txt_common_menu_open"));

        //add edit menu option
        editAction = menu->addAction(hbTrId("txt_common_menu_edit"));

        //delete menu option
        deleteAction = menu->addAction(hbTrId("Delete"));

        HbAction *selectedAction = menu->exec(coords);
        if (selectedAction)
        {
            if (selectedAction == openAction)
            {
                commands()->openContact(contact);
            }
            else if (selectedAction == editAction)
            {
                commands()->editContact(contact);
            }
            else if (selectedAction == deleteAction)
            {
                commands()->deleteContact(contact);
            }
        }
        menu->deleteLater();
    }
}

/*!
Called after user clicked on the listview.
*/
void CntBaseListView::onListViewActivated(const QModelIndex &index)
{
    if (contactModel()->contact(index).localId() == contactManager()->selfContactId() && contactModel()->contact(index).details().count() <= 4)
    {
        CntViewParameters viewParameters(CntViewParameters::myCardView);
        viewParameters.setSelectedContact(contactModel()->contact(index));
        viewManager()->onActivateView(viewParameters);
    }
    else
    {
        CntViewParameters viewParameters(CntViewParameters::commLauncherView);
        viewParameters.setSelectedContact(contactModel()->contact(index));
        viewManager()->onActivateView(viewParameters);
    }
}

/*!
[Virtual] Set model for the listview. By default we use QContactModel. Can be overridden in derived class
*/
void CntBaseListView::setDataModel()
{
    // notice that listview is created if it doesn't exists before calling this
    listView()->setModel(contactModel());
}


void CntBaseListView::clearDataModelFilter()
{
    setDataModel();
}

// end of file
