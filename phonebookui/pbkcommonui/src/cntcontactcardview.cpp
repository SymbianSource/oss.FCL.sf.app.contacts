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

#include "cntcontactcardview.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsSceneResizeEvent>
#include <qtcontacts.h>
#include <hbdocumentloader.h>
#include <hbscrollarea.h>

#include <hbmenu.h>
#include <hbgroupbox.h>
#include <thumbnailmanager_qt.h>

#include "cntcontactcarddatacontainer.h"
#include "cntcontactcarddetailitem.h"
#include "cntcontactcardheadingitem.h"
#include "cntmainwindow.h"
#include "cntcommands.h"

const char *CNT_COMMLAUNCERVIEW_XML = ":/xml/contacts_cc.docml";

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntContactCardView::CntContactCardView(CntViewManager *viewManager, QGraphicsItem *parent) :
    CntBaseView(viewManager, parent), 
    mScrollArea(0),
    mContainerWidget(0),
    mContainerLayout(0),
    mContact(0),
    mDetailsWidget(0),
    mDataContainer(0),
    mHeadingItem(0),
    mThumbnailManager(0),
    mGroupContact(0),
    mIsGroupMember(false)
{
    bool ok = false;
    ok = loadDocument(CNT_COMMLAUNCERVIEW_XML);

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
CntContactCardView::~CntContactCardView()
{
    delete mContact;
    delete mDataContainer;
    delete mGroupContact;
}

void CntContactCardView::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    Q_UNUSED(error);
    QIcon qicon(pixmap);
    HbIcon icon(qicon);
    mHeadingItem->setIcon(icon);
}

/*!
Add actions to menu
*/
void CntContactCardView::addMenuItems()
{
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:sendbusinesscard") << actions()->baseAction("cnt:editcontact") <<
                actions()->baseAction("cnt:addtogroup") << actions()->baseAction("cnt:deletecontact");
        actions()->addActionsToMenu(menu());
        
    connect(actions()->baseAction("cnt:sendbusinesscard"), SIGNAL(triggered()),
                this, SLOT (sendBusinessCard()));
        
    connect(actions()->baseAction("cnt:editcontact"), SIGNAL(triggered()),
            this, SLOT (editContact()));

    connect(actions()->baseAction("cnt:addtogroup"), SIGNAL(triggered()),
            this, SLOT (addToGroup()));

    connect(actions()->baseAction("cnt:deletecontact"), SIGNAL(triggered()),
            this, SLOT (deleteContact()));
    
    // to be enabled after implementation
    actions()->baseAction("cnt:sendbusinesscard")->setEnabled(false);
    actions()->baseAction("cnt:addtogroup")->setEnabled(false);

}

/*!
Add actions also to toolbar
*/
void CntContactCardView::addActionsToToolBar()
{
    //Add Action to the toolbar
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:edit") << actions()->baseAction("cnt:history") << actions()->baseAction("cnt:activitystream"); 
    actions()->addActionsToToolBar(toolBar());

    connect(actions()->baseAction("cnt:edit"), SIGNAL(triggered()),
            this, SLOT(editContact()));
    connect(actions()->baseAction("cnt:history"), SIGNAL(triggered()),
            this, SLOT(viewHistory()));
}

/*!
Launch contact editor 
*/
void CntContactCardView::editContact()
{
    CntViewParameters viewParameters(CntViewParameters::editView);
    viewParameters.setSelectedContact(*mContact);
    viewManager()->onActivateView(viewParameters);
}

void CntContactCardView::sendBusinessCard()
{
}

void CntContactCardView::addToGroup()
{
}

/*!
Delete contact
*/
void CntContactCardView::deleteContact()
{    
    // the delete command
    connect(commands(), SIGNAL(commandExecuted(QString, QContact)), this, 
            SLOT(handleExecutedCommand(QString, QContact)));
    commands()->deleteContact(*mContact);
}


/*!
Launch history view 
*/
void CntContactCardView::viewHistory()
{
    commands()->viewHistory(*mContact);
}

/*!
Activates a previous view
*/
void CntContactCardView::aboutToCloseView()
{   
    if(mIsGroupMember)
    {
        CntViewParameters viewParameters(CntViewParameters::groupMemberView);
        viewParameters.setSelectedContact(*mGroupContact);
        viewManager()->onActivateView(viewParameters);
    }
    else
    {
        viewManager()->onActivateView(CntViewParameters::namesView);
    }
    
}

void CntContactCardView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    if (mScrollArea)
    {
        mContainerWidget->resize(mScrollArea->size().width(), 0);
    }
    CntBaseView::resizeEvent(event);
}

void CntContactCardView::handleExecutedCommand(QString command, QContact /*contact*/)
{
    if (command == "delete")
    {
        CntViewParameters viewParameters(CntViewParameters::namesView);
        viewManager()->onActivateView(viewParameters);
    }
}

/*
Activates a default view and setup name label texts
*/
void CntContactCardView::activateView(const CntViewParameters &viewParameters)
{	
    QContact contact = viewParameters.selectedContact();
    mContact = new QContact(contact);

    if (viewParameters.selectedAction() == "FromGroupMemberView")
    {
        mIsGroupMember = true;
        QContact groupContact = viewParameters.selectedGroupContact();
        mGroupContact = new QContact(groupContact);
    }
    // add heading widget to the content
    QGraphicsWidget *c = findWidget(QString("content"));
    QGraphicsLinearLayout* l = static_cast<QGraphicsLinearLayout*>(c->layout());

    mHeadingItem = new CntContactCardHeadingItem(c);
    mHeadingItem->setDetails(mContact);

    l->insertItem(0, mHeadingItem);

    // avatar
    QList<QContactAvatar> details = mContact->details<QContactAvatar>();
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
    mDataContainer = new CntContactCardDataContainer(mContact);

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
            connect(item, SIGNAL(longPressed(const QPointF&)), this, SLOT(onLongPressed(const QPointF&)));

            HbIcon icon("");
            QString text;
            QString valueText;

            // DecorationRole
            QVariant decorationRole = mDataContainer->data(index, Qt::DecorationRole);
            QVariantList variantList;
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
   

            // DisplayRole
            QVariant displayRole = mDataContainer->data(index, Qt::DisplayRole);
            QStringList stringList;
            
            if (displayRole.canConvert<QString>())
            {
                stringList.append(displayRole.toString());
            }
            else if (displayRole.canConvert<QStringList>())
            {
                stringList = displayRole.toStringList();
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
            
            QVariant variant = mDataContainer->data(index, Qt::UserRole+1);
            const QMap<QString, QVariant> map = variant.toMap();
            QString action = map.value("action").toString();
            bool isUnderLine = mContact->isPreferredDetail(action, map.value("detail").value<QContactDetail>());
            if (isUnderLine)
            {
                mPreferredItems.insert(action, item);
            }
            if (action == "call" || action == "message")
            {
                item->setDetails(icon, text, valueText, Qt::ElideLeft, isUnderLine);
            }
            else
            {
                item->setDetails(icon, text, valueText, Qt::ElideRight, isUnderLine);
            }
            
            mContainerLayout->addItem(item);
        }

        // separator
        else if (index == mDataContainer->separatorIndex())
        {
            QVariant displayRole = mDataContainer->data(index, Qt::DisplayRole);
            if (displayRole.isValid())
            {
                if (displayRole.canConvert<QString>())
                {            
                    HbGroupBox* details = new HbGroupBox(this);
                    details->setHeading(displayRole.toString());
                    mContainerLayout->addItem(details);
                }
            }      
        }

        // details
        else
        {
            CntContactCardDetailItem* item = new CntContactCardDetailItem(index, mContainerWidget, false);

            HbIcon icon("");
            QString text;
            QString valueText;

            QVariant displayRole = mDataContainer->data(index, Qt::DisplayRole);
            QStringList stringList;
            if (displayRole.canConvert<QString>())
            {
                stringList.append(displayRole.toString());
            }
            else if (displayRole.canConvert<QStringList>())
            {
                stringList = displayRole.toStringList();
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

            item->setDetails(icon, text, valueText);
            mContainerLayout->addItem(item);
        }
    }
}

/*!
Called after user clicked on the listview.
*/
void CntContactCardView::onItemActivated()
{
    CntContactCardDetailItem *item = qobject_cast<CntContactCardDetailItem*>(sender());
    int index = item->index();
    QVariant variant = mDataContainer->data(index, Qt::UserRole+1);
    const QMap<QString, QVariant> map = variant.toMap();
    QString action = map.value("action").toString();
    QContactDetail detail = map.value("detail").value<QContactDetail>();
    
    commands()->launchAction(*mContact, detail, action);
}

/*!
Set selected detail as preferred for selected action
*/
void CntContactCardView::setPreferredAction(const QString &aAction, const QContactDetail &aDetail)
{
    mContact->setPreferredDetail(aAction, aDetail);
    contactManager()->saveContact(mContact);
    emit preferredUpdated();
}

/*!
Called after user longtaps the listview
*/
void CntContactCardView::onLongPressed(const QPointF &aCoords)
{
    CntContactCardDetailItem *item = qobject_cast<CntContactCardDetailItem*>(sender());
    int index = item->index();
    QVariant variant = mDataContainer->data(index, Qt::UserRole+1);
    const QMap<QString, QVariant> map = variant.toMap();
    
    HbMenu *menu = new HbMenu();
    HbAction *communicationAction = 0;
    HbAction *preferredAction = 0;
    
    QString action = map.value("action").toString();
    
    if (action.compare("call", Qt::CaseInsensitive) == 0)
    {
        communicationAction = menu->addAction(map.value("name").toString());
    }
    else if (action.compare("message", Qt::CaseInsensitive) == 0)
    {
        communicationAction = menu->addAction(hbTrId("txt_phob_dblist_send_message"));
    }
    else if (action.compare("email", Qt::CaseInsensitive) == 0)
    {
        communicationAction = menu->addAction(hbTrId("Send email"));
    }
    menu->addSeparator();

    preferredAction = menu->addAction(hbTrId("Set as preferred"));
    if (mContact->isPreferredDetail(action, map.value("detail").value<QContactDetail>()))
    {
        preferredAction->setEnabled(false);
    }

    //favoriteAction = menu->addAction(hbTrId("Make favorite"));
    //homeScreenAction = menu->addAction(hbTrId("Place to homescreen"));
    HbAction *selectedAction = menu->exec(aCoords);

    if (selectedAction)
    {
        if (selectedAction == communicationAction)
        {
            commands()->launchAction(*mContact, map.value("detail").value<QContactDetail>(), action);
        }
        else if (selectedAction == preferredAction)
        {
            setPreferredAction(action, map.value("detail").value<QContactDetail>());
            item->setUnderLine(true);
      
            if (mPreferredItems.contains(action))
            {
                mPreferredItems.value(action)->setUnderLine(false);
            }
            mPreferredItems.insert(action, item);     
        }
    }
    menu->deleteLater();
}

void CntContactCardView::keyPressEvent(QKeyEvent *event)
{
    if (this == viewManager()->mainWindow()->currentView() && event->key() == Qt::Key_Yes)
    {
        event->accept();
        QList<QContactActionDescriptor> actionDescriptors = mContact->availableActions();
        QStringList availableActions;
        for (int i = 0;i < actionDescriptors.count();i++)
        {
            availableActions << actionDescriptors.at(i).actionName();
        }
        if (availableActions.contains("call", Qt::CaseInsensitive))
        {
            commands()->launchAction(*mContact, QContactDetail(), "call");
        }
    }
    else
    {
        CntBaseView::keyPressEvent(event);
    }
}

// end of file
