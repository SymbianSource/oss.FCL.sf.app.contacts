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

#include "cntgroupactionsview.h"

#include <QGraphicsLinearLayout>
#include <qtcontacts.h>
#include <hbdocumentloader.h>
#include <QGraphicsSceneResizeEvent>
#include <hbscrollarea.h>
#include <hbgroupbox.h>
#include <thumbnailmanager_qt.h>
#include "cntcontactcarddatacontainer.h"
#include "cntcontactcarddetailitem.h"
#include "cntcontactcardheadingitem.h"
#include "cntcommands.h"
#include <hbdialog.h>
#include <hbaction.h>
#include <hblabel.h>


const char *CNT_GROUPACTIONVIEW_XML = ":/xml/contacts_cc.docml";

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntGroupActionsView::CntGroupActionsView(CntViewManager *viewManager, QGraphicsItem *parent) :
    CntBaseView(viewManager, parent),
    mGroupContact(0),
    mScrollArea(0),
    mContainerWidget(0),
    mContainerLayout(0),
    mDataContainer(0),
    mHeadingItem(0),
    mBanner(0)
{
    bool ok = false;
    ok = loadDocument(CNT_GROUPACTIONVIEW_XML);

    if (ok)
       {
           QGraphicsWidget *content = findWidget(QString("content"));
           setWidget(content);
       }
       else
       {
           qFatal("Unable to read :/xml/contacts_cc.docml");
       }

}

/*!
Destructor
*/
CntGroupActionsView::~CntGroupActionsView()
{
    delete mGroupContact;
    delete mDataContainer;
}


void CntGroupActionsView::editGroup()
{
    CntViewParameters viewParameters(CntViewParameters::groupEditorView);
    viewParameters.setSelectedAction("EditGroupMembers");
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->changeView(viewParameters);
}

/*!
Launch contact editor 
*/
void CntGroupActionsView::editContact()
{
    commands()->editContact(*mGroupContact);
}

/*!
Activates a previous view
*/
void CntGroupActionsView::aboutToCloseView()
{
    CntViewParameters args;
    args.setSelectedContact(*mGroupContact);
    viewManager()->back( args );
}

void CntGroupActionsView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    if (mScrollArea)
    {
        mContainerWidget->resize(mScrollArea->size().width(), 0);
    }
    CntBaseView::resizeEvent(event);
}

void CntGroupActionsView::addActionsToToolBar()
{
    
}
/*
Activates a default view and setup name label texts
*/
void CntGroupActionsView::activateView(const CntViewParameters &viewParameters)
{	
    QContact contact = viewParameters.selectedContact();
    mGroupContact = new QContact(contact);

    // add heading widget to the content
    QGraphicsWidget *c = findWidget(QString("content"));
    QGraphicsLinearLayout* l = static_cast<QGraphicsLinearLayout*>(c->layout());

    QContactName groupContactName = mGroupContact->detail( QContactName::DefinitionName );
    QString groupName(groupContactName.value( QContactName::FieldCustomLabel ));
       
    mBanner = new HbGroupBox(this);
    l->insertItem(0, mBanner);
    mBanner->setHeading(groupName);
    
    
       
    // data
    mDataContainer = new CntContactCardDataContainer(mGroupContact);

    mScrollArea = static_cast<HbScrollArea*>(findWidget(QString("scrollArea")));
    mScrollArea->setScrollDirections(Qt::Vertical);

    mContainerWidget = new QGraphicsWidget(mScrollArea);
    mContainerWidget->setPreferredWidth(mScrollArea->size().width());
    mScrollArea->setContentWidget(mContainerWidget);

    mContainerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mContainerLayout->setContentsMargins(0, 0, 0, 0);
    mContainerLayout->setSpacing(0);
    mContainerWidget->setLayout(mContainerLayout); 
    
    for (int index = 0; index < mDataContainer->rowCount(); index++)
    {
       // communication methods
       if (mDataContainer->separatorIndex() == -1 || index < mDataContainer->separatorIndex())
       { 
           CntContactCardDetailItem* item = new CntContactCardDetailItem(index, mContainerWidget);

           connect(item, SIGNAL(clicked()), this, SLOT(onItemActivated()));
           
           HbIcon icon("");
           QString text;
           QString valueText;

           // DecorationRole
           QVariant decorationRole = mDataContainer->data(index, Qt::DecorationRole);
           QVariantList variantList;
           if (decorationRole.isValid())
           {
               if (decorationRole.canConvert<HbIcon>())
               {
                   icon = decorationRole.value<HbIcon>();
               }
               else if (decorationRole.canConvert< QList<QVariant> >())
               {
                   variantList = decorationRole.toList();
                   for (int j = 0; j < variantList.count(); j++)
                   {
                       if (j==0 && variantList.at(0).canConvert<HbIcon>())
                       {
                           icon = variantList.at(0).value<HbIcon>();
                       }
                   }
               }
           }

           // DisplayRole
           QVariant displayRole = mDataContainer->data(index, Qt::DisplayRole);
           QStringList stringList;
           if (displayRole.isValid())
           {
               if (displayRole.canConvert<QString>())
               {
                   stringList.append(displayRole.toString());
               }
               else if (displayRole.canConvert<QStringList>())
               {
                   stringList = displayRole.toStringList();
               }
           }

           for (int j = 0; j < stringList.count(); j++)
           {
               if (j==0)
               {
                   text = stringList.at(0);
               }
               else if (j==1)
               {
                   valueText = stringList.at(1);
               }
           }
           QString confCall("Conference Call");
           if(text==confCall) // conference call
               {
               item->setDetails(icon, text, valueText);
               }
           else
               {
               item->setDetails(icon, text); // for group Email and Message, we dont need any Value text
               }
           mContainerLayout->addItem(item);
       }
   }


}

/*!
Add actions to menu
*/
void CntGroupActionsView::addMenuItems()
{
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:editgroupdetails");
    actions()->addActionsToMenu(menu());

    connect(actions()->baseAction("cnt:editgroupdetails"), SIGNAL(triggered()),
            this, SLOT (editGroup()));
}



