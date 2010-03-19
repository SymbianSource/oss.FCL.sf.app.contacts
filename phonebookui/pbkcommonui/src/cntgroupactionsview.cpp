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
    mThumbnailManager(0)
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
CntGroupActionsView::~CntGroupActionsView()
{
    delete mGroupContact;
    delete mDataContainer;
}


void CntGroupActionsView::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    Q_UNUSED(error);
    QIcon qicon(pixmap);
    HbIcon icon(qicon);
    mHeadingItem->setIcon(icon);
}
/*!
Add actions also to toolbar
*/
void CntGroupActionsView::addActionsToToolBar()
{
    //Add Action to the toolbar
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:editgroup") << actions()->baseAction("cnt:deletegroup")
        << actions()->baseAction("cnt:groupmembers");
    actions()->addActionsToToolBar(toolBar());

    connect(actions()->baseAction("cnt:editgroup"), SIGNAL(triggered()),
            this, SLOT(editGroupDetails()));
    connect(actions()->baseAction("cnt:deletegroup"), SIGNAL(triggered()),
       this, SLOT(deleteGroup()));    
    connect(actions()->baseAction("cnt:groupmembers"), SIGNAL(triggered()),
            this, SLOT(groupMembers()));

}


void CntGroupActionsView::editGroup()
{
    CntViewParameters viewParameters(CntViewParameters::groupEditorView);
    viewParameters.setSelectedAction("EditGroupMembers");
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->onActivateView(viewParameters);
}

void CntGroupActionsView::groupMembers()
{
    CntViewParameters viewParameters(CntViewParameters::groupMemberView);
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->onActivateView(viewParameters);
}

void CntGroupActionsView::openNamesView()
{
    CntViewParameters viewParameters(CntViewParameters::namesView);
    viewManager()->onActivateView(viewParameters);
}

void CntGroupActionsView::openCollections()
{
    CntViewParameters viewParameters(CntViewParameters::collectionView);
    viewManager()->onActivateView(viewParameters);
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
   viewManager()->onActivateView(CntViewParameters::collectionView);
}

void CntGroupActionsView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    if (mScrollArea)
    {
        mContainerWidget->resize(mScrollArea->size().width(), 0);
    }
    CntBaseView::resizeEvent(event);
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


void CntGroupActionsView::doConferenceCall()
{

}

void CntGroupActionsView::sendGroupMessage()
{

}

void CntGroupActionsView::sendGroupEmail()
{

}

/*!
Add actions to menu
*/
void CntGroupActionsView::addMenuItems()
{
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:placegrouptohs");
    actions()->addActionsToMenu(menu());

    connect(actions()->baseAction("cnt:placegrouptohs"), SIGNAL(triggered()),
            this, SLOT (placeGroupToHs()));

}


void CntGroupActionsView::manageMembers()
{
 /*   CntViewParameters viewParameters(CntViewParameters::groupMemberSelectionView);
    viewParameters.setSelectedAction("EditGroupMembers");
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->onActivateView(viewParameters);
*/
}

void CntGroupActionsView::editGroupDetails()
{
    CntViewParameters viewParameters(CntViewParameters::groupEditorView);
    viewParameters.setSelectedAction("EditGroupDetails");
    viewParameters.setSelectedContact(*mGroupContact);
    viewManager()->onActivateView(viewParameters);
}

void CntGroupActionsView::placeGroupToHs()
{
// wait for specs
}

void CntGroupActionsView::deleteGroup()
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
     popup.setContentWidget(new HbLabel(tr("Only group will be removed, contacts can be found frim All contacts list")));

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
         viewManager()->onActivateView(viewParameters);
     }
}

void CntGroupActionsView::onItemActivated()
{
// to be implemented
}

// end of file
