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

#include "cntcontactcardview_p.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsSceneResizeEvent>
#include <QStringList>
#include <qtcontacts.h>
#include <hbscrollarea.h>
#include <hblabel.h>
#include <hbmenu.h>
#include <hbview.h>
#include <hbtoolbar.h>
#include <hbgroupbox.h>
#include <hbaction.h>
#include <hbmessagebox.h>
#include <hbicon.h>
#include <shareui.h>
#include <thumbnailmanager_qt.h>
#include <cntmaptileservice.h>  //For maptile processing
#include <qversitcontactexporter.h>
#include <qversitwriter.h>
#include <xqservicerequest.h>
#include "cntcontactcarddatacontainer.h"
#include "cntcontactcarddetailitem.h"
#include "cntcontactcardheadingitem.h"
#include "cntcontactcarddataitem.h"
#include "cntmainwindow.h"
#include "cntstringmapper.h"
#include "cntdocumentloader.h"
#include "cntimagelabel.h"

const char *CNT_CONTACTCARDVIEW_XML = ":/xml/contacts_contactcard.docml";

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntContactCardViewPrivate::CntContactCardViewPrivate() :
    QObject(), 
    mScrollArea(NULL),
    mContainerWidget(NULL),
    mContainerLayout(NULL),
    mContact(NULL),
    mDetailsWidget(NULL),
    mDataContainer(NULL),
    mHeadingItem(NULL),
    mThumbnailManager(NULL),
    mGroupContact(NULL),
    mAvatar(NULL),
    mIsGroupMember(false),
    mIsHandlingMenu(false),
    mFavoriteGroupId(-1),
    mLoader(NULL),
    mContactAction(NULL),
    mBackKey(NULL),
    mImageLabel(NULL),
    mVCardIcon(NULL)
{
    bool ok;
    document()->load(CNT_CONTACTCARDVIEW_XML, &ok);
    if (!ok) 
    {
        qFatal("Unable to read :/xml/contacts_contactcard.docml");
    }
    
    mView = static_cast<HbView*>(document()->findWidget("view"));

    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
    
    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)),
        this, SLOT(thumbnailReady(QPixmap, void*, int, int)));
    
    //back button
    mBackKey = new HbAction(Hb::BackNaviAction, mView);
    mView->setNavigationAction(mBackKey);  
    connect(mBackKey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
}

/*!
Destructor
*/
CntContactCardViewPrivate::~CntContactCardViewPrivate()
{
    delete mContact;
    mContact = 0;
    
    delete mDataContainer;
    mDataContainer = NULL;
    
    delete mGroupContact;
    mGroupContact = NULL;
    
    delete mAvatar;
    mAvatar = NULL;
    
    delete mLoader;
    mLoader = NULL;
    
    delete mVCardIcon;
    mVCardIcon = NULL;
}

/*!
Activates a previous view
*/
void CntContactCardViewPrivate::showPreviousView()
{
    emit backPressed();
    
    CntViewParameters viewParameters;
    if (mIsGroupMember)
    {
        QVariant var;
        var.setValue(*mGroupContact);
        viewParameters.insert(ESelectedContact, var);
    }
    mViewManager->back(viewParameters);
}

/*
Activates a default view and setup name label texts
*/
void CntContactCardViewPrivate::activate(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{   
    mView->installEventFilter(this);
    
    mViewManager = aMgr;
    
    HbMainWindow* window = mView->mainWindow();
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    setOrientation(window->orientation());
    
    QContact contact = aArgs.value(ESelectedContact).value<QContact>();
    mContact = new QContact(contact);

    //my card
    if (mContact->localId() == contactManager()->selfContactId())
    {
        mView->menu()->clearActions();
        mView->toolBar()->removeAction(static_cast<HbAction*>(document()->findObject(QString("cnt:history"))));
    }
    else
    {
        mView->toolBar()->removeAction(static_cast<HbAction*>(document()->findObject(QString("cnt:sendMyCard"))));
    }
    
    if (aArgs.value(ESelectedAction).toString() == "FromGroupMemberView")
    {
        mIsGroupMember = true;
        QContact groupContact = aArgs.value(ESelectedGroupContact).value<QContact>();
        mGroupContact = new QContact(groupContact);
    }
    // add heading widget to the content
    QGraphicsWidget *c = document()->findWidget(QString("content"));
    QGraphicsLinearLayout* l = static_cast<QGraphicsLinearLayout*>(c->layout());

    mHeadingItem = static_cast<CntContactCardHeadingItem*>(document()->findWidget(QString("cnt_contactcard_heading")));
    mHeadingItem->setDetails(mContact, mContact->localId() == contactManager()->selfContactId());
    mHeadingItem->setSecondaryIcon(isFavoriteGroupContact());
    
    connect(mHeadingItem, SIGNAL(passLongPressed(const QPointF&)), this, SLOT(drawMenu(const QPointF&)));

    mImageLabel = static_cast<CntImageLabel*>(document()->findWidget("cnt_contactcard_image"));
    connect(mImageLabel, SIGNAL(iconClicked()), this, SLOT(doChangeImage()));
    
    // avatar
    QList<QContactAvatar> details = mContact->details<QContactAvatar>();
    for (int i = 0;i < details.count();i++)
    {
        if (details.at(i).imageUrl().isValid())
            {
            mAvatar = new QContactAvatar(details.at(i));
            mThumbnailManager->getThumbnail(mAvatar->imageUrl().toString());
            break;
            }
    }
    
    // data
    mDataContainer = new CntContactCardDataContainer(mContact);

    // scroll area + container widget
    mScrollArea = static_cast<HbScrollArea*>(document()->findWidget(QString("scrollArea")));
    mScrollArea->setScrollDirections(Qt::Vertical);
    mContainerWidget = new QGraphicsWidget(mScrollArea);
    mScrollArea->setContentWidget(mContainerWidget);
        
    mContainerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mContainerLayout->setContentsMargins(0, 0, 0, 0);
    mContainerLayout->setSpacing(0);
    mContainerLayout->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    
    mContainerWidget->setLayout(mContainerLayout);
 
    for (int index = 0; index < mDataContainer->itemCount(); index++)
    {
        // communication methods
        if (mDataContainer->separatorIndex() == -1 || index < mDataContainer->separatorIndex())
        { 
            CntContactCardDetailItem* item = new CntContactCardDetailItem(index, mContainerWidget);

            connect(item, SIGNAL(clicked()), this, SLOT(onItemActivated()));
            connect(item, SIGNAL(longPressed(const QPointF&)), this, SLOT(onLongPressed(const QPointF&)));

            CntContactCardDataItem* dataItem = mDataContainer->dataItem(index);
            
            if (mContact->isPreferredDetail(dataItem->action(), dataItem->detail()))
            {
                dataItem->setSecondaryIcon(HbIcon("qtg_mono_favourites"));
                mPreferredItems.insert(dataItem->action(), item);
            }
         
            item->setDetails(dataItem);
            mContainerLayout->addItem(item);
        }

        // separator
        else if (index == mDataContainer->separatorIndex())
        {      
            HbGroupBox* details = new HbGroupBox(mView);
            details->setHeading(mDataContainer->dataItem(index)->titleText());
            mContainerLayout->addItem(details);
        }

        // details
        else
        {
            CntContactCardDataItem* dataItem = mDataContainer->dataItem(index);
            if (!dataItem->icon().isNull())
            { 
                HbLabel* iconLabel = new HbLabel(mView);
                iconLabel->setIcon(dataItem->icon());
                iconLabel->setPreferredSize(dataItem->icon().width(), dataItem->icon().height());
                mContainerLayout->addItem(iconLabel);
            } 
            else
            {    
                CntContactCardDetailItem* item = new CntContactCardDetailItem(index, mContainerWidget, false);
                item->setDetails(dataItem);
                mContainerLayout->addItem(item);
            }
        }
    }
    
    bool setAsFavorite = false;
    if(isFavoriteGroupCreated())
    {
        QContact favoriteGroup = contactManager()->contact(mFavoriteGroupId);
        // Use relationship filter to get list of contacts in the relationship (if any)
        QContactRelationshipFilter filter;
        filter.setRelationshipType(QContactRelationship::HasMember);
        filter.setRelatedContactRole(QContactRelationship::First); 
        filter.setRelatedContactId(favoriteGroup.id());
        
        QList<QContactLocalId> mContactsList = contactManager()->contactIds(filter);
        int count = mContactsList.count();
        if (count)
        {
            for (int i = 0 ; i < count ; i++)
            {
                if (mContactsList.at(i) == mContact->localId()  )
                {
                setAsFavorite = true;
                }
            }
        }
    }
    
    if (setAsFavorite)
    {
        mView->menu()->removeAction(qobject_cast<HbAction *>(document()->findObject("cnt:setasfavorite")));
    }
    else
    {
        mView->menu()->removeAction(qobject_cast<HbAction *>(document()->findObject("cnt:removefromfavorite")));
    }
        
    // Menu items
    connect(qobject_cast<HbAction *>(document()->findObject("cnt:sendbusinesscard")), SIGNAL(triggered()),
                this, SLOT (sendBusinessCard()));
    connect(qobject_cast<HbAction *>(document()->findObject("cnt:deletecontact")), SIGNAL(triggered()),
                this, SLOT (deleteContact()));
    connect(qobject_cast<HbAction *>(document()->findObject("cnt:setasfavorite")), SIGNAL(triggered()),
                this, SLOT (setAsFavorite()));
    connect(qobject_cast<HbAction *>(document()->findObject("cnt:removefromfavorite")), SIGNAL(triggered()),
                this, SLOT (removeFromFavorite()));    
    connect(qobject_cast<HbAction *>(document()->findObject("cnt:placecontacttohs")), SIGNAL(triggered()),
                this, SLOT (sendToHs()));
    
    // Toolbar items
    connect(qobject_cast<HbAction *>(document()->findObject("cnt:edit")), SIGNAL(triggered()),
                this, SLOT(editContact()));
    connect(qobject_cast<HbAction *>(document()->findObject("cnt:history")), SIGNAL(triggered()),
                this, SLOT(viewHistory()));
    connect(qobject_cast<HbAction *>(document()->findObject("cnt:sendMyCard")), SIGNAL(triggered()),
                this, SLOT (sendBusinessCard()));
 
    emit viewActivated(*mContact, aArgs.value(ESelectedDetail).value<QContactDetail>());
}

void CntContactCardViewPrivate::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    if (!error)
    {
        QIcon qicon(pixmap);
        HbIcon icon(qicon);
        mHeadingItem->setIcon(icon);
        mVCardIcon = new HbIcon(qicon);
        mImageLabel->clear();
        mImageLabel->setIcon(icon);
    }
}

/*!
Place contact to homescreen as widget
*/  
void CntContactCardViewPrivate::sendToHs()
{
    QVariantHash preferences;
    preferences["contactId"] = mContact->id().localId();
    
    XQServiceRequest snd("com.nokia.services.hsapplication.IHomeScreenClient",
                         "addWidget(QString,QVariantHash)"
                         ,false);
    snd << QString("hscontactwidgetplugin");
    snd << preferences;
    snd.send();
}

/*!
Launch contact editor 
*/
void CntContactCardViewPrivate::editContact()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, editView);
    QVariant var;
    var.setValue(*mContact);
    viewParameters.insert(ESelectedContact, var);
    mViewManager->changeView(viewParameters);
}

void CntContactCardViewPrivate::addToGroup()
{
}

void CntContactCardViewPrivate::setAsFavorite()
{
    QContact favoriteGroup;
    if (!isFavoriteGroupCreated() )
    {
        //Create Fav grp
        favoriteGroup.setType(QContactType::TypeGroup);
        QContactName favoriteGroupName;
        favoriteGroupName.setCustomLabel("Favorites");
        favoriteGroup.saveDetail(&favoriteGroupName);
        contactManager()->saveContact(&favoriteGroup);
        mFavoriteGroupId = favoriteGroup.localId();
    }
    else
    {
        favoriteGroup = contactManager()->contact(mFavoriteGroupId);
    }
    
    // new contact added to the favorite group
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(favoriteGroup.id());
    relationship.setSecond(mContact->id());
    // save relationship
    contactManager()->saveRelationship(&relationship);
    
    mView->menu()->removeAction(qobject_cast<HbAction *>(document()->findObject("cnt:setasfavorite")));
    mView->menu()->addAction(qobject_cast<HbAction *>(document()->findObject("cnt:removefromfavorite")));
    mHeadingItem->setSecondaryIcon(true);  
}

void CntContactCardViewPrivate::removeFromFavorite()
    {
    QContact favoriteGroup = contactManager()->contact(mFavoriteGroupId);
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(favoriteGroup.id());
    relationship.setSecond(mContact->id());
    contactManager()->removeRelationship(relationship);
   
    mView->menu()->removeAction(qobject_cast<HbAction *>(document()->findObject("cnt:removefromfavorite")));
    mView->menu()->addAction(qobject_cast<HbAction *>(document()->findObject("cnt:setasfavorite")));
    mHeadingItem->setSecondaryIcon(false); 
    }

/*!
Delete contact
*/
void CntContactCardViewPrivate::deleteContact()
{    
    QString name = contactManager()->synthesizedDisplayLabel(*mContact);

    HbMessageBox *note = new HbMessageBox(hbTrId("txt_phob_info_delete_1").arg(name), HbMessageBox::MessageTypeQuestion);
    note->setPrimaryAction(new HbAction(hbTrId("txt_phob_button_delete"), note));
    note->setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), note));
    HbAction *selected = note->exec();
    if (selected == note->primaryAction())
    {
        contactManager()->removeContact(mContact->localId());
        CntViewParameters viewParameters;
        viewParameters.insert(EViewId, namesView);
        QVariant var;
        var.setValue(*mContact);
        viewParameters.insert(ESelectedContact, var);
        viewParameters.insert(ESelectedAction, "delete");
        mViewManager->changeView(viewParameters);
    }
    delete note;   
}

/*!
Launch history view 
*/
void CntContactCardViewPrivate::viewHistory()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, historyView);
    QVariant var;
    var.setValue(*mContact);
    viewParameters.insert(ESelectedContact, var);
    mViewManager->changeView(viewParameters);
}

/*!
Deactivate the view
*/
void CntContactCardViewPrivate::deactivate()
{
    mView->removeEventFilter(this);    
}

/*!
Send the business card / my card 
*/
void CntContactCardViewPrivate::sendBusinessCard()
{
    QString tempDir = QDir::tempPath().append("/tempcntvcard");
    
    QDir dir(tempDir);
    
    // Temporary directory to store the vCard file
    if (!dir.exists()) 
    {
        // Create a temp directory
        QDir::temp().mkdir("tempcntvcard");
    } 
    else 
    {    
        // Empty the temp directory since the other vCards are not required
        QStringList l = dir.entryList();
        foreach(QString s, l) 
        {
            if (dir.exists(s))
            {
                dir.remove(s);
            }
        }
    }
    
    QString vCardName = QString(mContact->displayLabel().append(".vcf"));
    QString vCardPath = dir.absolutePath().append(QDir::separator());
    vCardPath.append(vCardName);
    vCardPath = QDir::toNativeSeparators(vCardPath);
    
    QString service("com.nokia.services.hbserviceprovider.conversationview");
    QString type("send(QVariant)");
    
    // Create the vCard and send it to messaging service
    if (createVCard(vCardPath)) 
    {
        ShareUi s;
        QStringList l;
        l << vCardPath;
        s.send(l,false);
    }
}

/*!
Set orientation of the view
*/
void CntContactCardViewPrivate::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) 
    {
        // reading "portrait" section
        document()->load(CNT_CONTACTCARDVIEW_XML, "portrait");
    } 
    else 
    {
        // reading "landscape" section
        document()->load(CNT_CONTACTCARDVIEW_XML, "landscape");
    }
}

/*!
Called after user clicked on the listview.
*/
void CntContactCardViewPrivate::onItemActivated()
{
    CntContactCardDetailItem *item = qobject_cast<CntContactCardDetailItem*>(sender());
    int index = item->index();
    launchAction(*mContact, mDataContainer->dataItem(index)->detail(), mDataContainer->dataItem(index)->action());
}

/*!
Launch the call / message / email action
*/
void CntContactCardViewPrivate::launchAction(QContact contact, QContactDetail detail, QString action)
{
    // detail might be empty -> in that case engine uses the preferred detail for the selected action
    QList<QContactActionDescriptor> callActionDescriptors = QContactAction::actionDescriptors(action, "symbian");
    mContactAction = QContactAction::action(callActionDescriptors.at(0));
    connect(mContactAction, SIGNAL(stateChanged(QContactAction::State)),
                this, SLOT(progress(QContactAction::State)));
    mContactAction->invokeAction(contact, detail);
}

void CntContactCardViewPrivate::progress(QContactAction::State status)
{
    switch(status)
    {
    case QContactAction::FinishedState:
    case QContactAction::FinishedWithErrorState:
        mContactAction->deleteLater();
        mContactAction = 0;
        break;
    default:
        break;
    }
}

/*!
Set selected detail as preferred for selected action
*/
void CntContactCardViewPrivate::setPreferredAction(const QString &aAction, const QContactDetail &aDetail)
{
    mContact->setPreferredDetail(aAction, aDetail);
    contactManager()->saveContact(mContact);
    emit preferredUpdated();
}

/*!
Item specific menu
*/
void CntContactCardViewPrivate::onLongPressed(const QPointF &aCoords)
{
    CntContactCardDetailItem *item = qobject_cast<CntContactCardDetailItem*>(sender());
    int index = item->index();
    
    HbMenu *menu = new HbMenu();
    HbAction *communicationAction = 0;
    HbAction *preferredAction = 0;
    CntStringMapper stringMapper;
    
    QString action = mDataContainer->dataItem(index)->action();
    QContactDetail detail = mDataContainer->dataItem(index)->detail();
    
    if (action.compare("call", Qt::CaseInsensitive) == 0)
    {       
        QContactDetail detail = mDataContainer->dataItem(index)->detail();
        if (!detail.contexts().isEmpty())
        {
            communicationAction = menu->addAction(stringMapper.getContactCardMenuLocString(detail.definitionName(), detail.contexts().first()));
        }
        else
        {
            communicationAction = menu->addAction(stringMapper.getContactCardMenuLocString(detail.definitionName(), QString()));
        }          
    }
    else if (action.compare("message", Qt::CaseInsensitive) == 0)
    {
        communicationAction = menu->addAction(hbTrId("txt_phob_menu_send_message"));
    }
    else if (action.compare("email", Qt::CaseInsensitive) == 0)
    {
        if (!detail.contexts().isEmpty())
        {
            communicationAction = menu->addAction(stringMapper.getContactCardMenuLocString(detail.definitionName(), detail.contexts().first()));
        }
        else
        {
            communicationAction = menu->addAction(stringMapper.getContactCardMenuLocString(detail.definitionName(), QString()));
        }        
    }
    
    if (action.compare("call", Qt::CaseInsensitive) == 0)
    {
        preferredAction = menu->addAction(hbTrId("txt_phob_menu_set_as_default_number"));
    }
    else if (action.compare("email", Qt::CaseInsensitive) == 0)
    {
        preferredAction = menu->addAction(hbTrId("txt_phob_menu_set_as_default_email"));
    }
      
    if (mContact->isPreferredDetail(action, detail))
    {
        preferredAction->setEnabled(false);
    }
    
    if (preferredAction)
    {
        menu->insertSeparator(preferredAction);
    }

    HbAction *selectedAction = menu->exec(aCoords);

    if (selectedAction)
    {
        if (selectedAction == communicationAction)
        {
            launchAction(*mContact, detail, action);
        }
        else if (selectedAction == preferredAction)
        {
            setPreferredAction(action, detail);
            //item->setUnderLine(true);
      
            if (mPreferredItems.contains(action))
            {
                //mPreferredItems.value(action)->setUnderLine(false);
            }
            mPreferredItems.insert(action, item);     
        }
    }
    menu->deleteLater();
}

/*!
Event filter for green key
*/
bool CntContactCardViewPrivate::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && static_cast<QKeyEvent*>(event)->key() == Qt::Key_Yes)
    {
        QList<QContactActionDescriptor> actionDescriptors = mContact->availableActions();
        QStringList availableActions;
        for (int i = 0; i < actionDescriptors.count();i++)
        {
            availableActions << actionDescriptors.at(i).actionName();
        }
        if (availableActions.contains("call", Qt::CaseInsensitive))
        {
            launchAction(*mContact, QContactDetail(), "call");
        }
        return true;
    }
    else if (event->type() == QEvent::LayoutRequest && mScrollArea)
    {
        mContainerWidget->resize(mScrollArea->size().width(), mScrollArea->size().height());
        return true;
    }
    else
    {
        return QObject::eventFilter(obj,event);
    }
}

/*!
Called after the user clicked "Change Image" from popup menu after 
longpressing the image in this view.
*/
void CntContactCardViewPrivate::doChangeImage()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, imageEditorView);
    QVariant var;
    var.setValue(*mContact);
    viewParameters.insert(ESelectedContact, var);
    mViewManager->changeView(viewParameters);
}

/*!
Called after the user clicked "Remove Image" from popup menu after 
longpressing the image in this view.
*/
void CntContactCardViewPrivate::doRemoveImage()
{
    if (mAvatar) 
    {
        bool success = mContact->removeDetail(mAvatar);
        if (success) 
        { 
            mHeadingItem->setIcon(HbIcon("qtg_large_avatar"));
            contactManager()->saveContact(mContact);
        }
    }
}

/*!
Draw the image specific content menu
*/
void CntContactCardViewPrivate::drawMenu(const QPointF &aCoords)
{
    if (mIsHandlingMenu) return;
    
    // To avoid re-drawing the menu and causing a crash due to 
    // multiple emitted signals, set state that we are handling the signal
    mIsHandlingMenu = true;
    
    HbMenu *menu = new HbMenu();
    HbAction *changeImageAction = menu->addAction(hbTrId("txt_phob_menu_change_picture"));
    HbAction *removeAction = menu->addAction(hbTrId("txt_phob_menu_remove_image"));
    
    HbAction *selectedAction = menu->exec(aCoords);

    if (selectedAction) 
    {
        if (selectedAction == changeImageAction) 
        {
            doChangeImage();
        }
        else if (selectedAction == removeAction) 
        {
            doRemoveImage();
        }
    }

    mIsHandlingMenu = false;  
    menu->deleteLater();
}

/*!
Check if the favourite group created
*/
bool CntContactCardViewPrivate::isFavoriteGroupCreated()
{
    bool favoriteGroupCreated = false;
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = contactManager()->contactIds(groupFilter);
    
    if (!groupContactIds.isEmpty())
    {
        for(int i = 0;i < groupContactIds.count();i++)
        {
            QContact contact = contactManager()->contact(groupContactIds.at(i));
            QContactName contactName = contact.detail<QContactName>();
            QString groupName = contactName.customLabel();
            if(groupName.compare("Favorites") == 0)
            {
                favoriteGroupCreated = true;
                mFavoriteGroupId = groupContactIds.at(i);
                break;
            }
        }
    }
    return favoriteGroupCreated;
}

/*!
Check if the contact is the favourite group
*/
bool CntContactCardViewPrivate::isFavoriteGroupContact()
{
    bool favoriteGroupContact = false;
    
    if (isFavoriteGroupCreated())
    {
        QContact favoriteGroup = contactManager()->contact(mFavoriteGroupId);
        QContactRelationshipFilter rFilter;
        rFilter.setRelationshipType(QContactRelationship::HasMember);
        rFilter.setRelatedContactRole(QContactRelationship::First);
        rFilter.setRelatedContactId(favoriteGroup.id());
    
        QList<QContactLocalId> contactsLocalIdList = contactManager()->contactIds(rFilter);
        if (!contactsLocalIdList.isEmpty())
        {
            for(int i = 0;i < contactsLocalIdList.count();i++)
            {
                QContact favContact = contactManager()->contact(contactsLocalIdList.at(i));
                if (favContact.localId() == mContact->localId())
                {
                    favoriteGroupContact = true;
                }  
             }
        }
    }
    return favoriteGroupContact;
}

/*!
Creates the v-card
*/
bool CntContactCardViewPrivate::createVCard(QString& vCardPath)
{
    QList<QContact> list;
    QContact tempContact(*mContact);
    bool createVCard( false );
    
    // Check if the contact has an image.
    QList<QContactAvatar> avatars = tempContact.details<QContactAvatar>();
    
    foreach(QContactAvatar a, avatars)
    {
        if (!a.imageUrl().isEmpty())
        {
            // If true and query the user if they want to add it to
            // the business card
        
            // TODO: Missing translation
            HbMessageBox *note = new HbMessageBox(hbTrId("txt_phob_info_add_contact_card_image_to_business_c"),
                    HbMessageBox::MessageTypeQuestion);
            note->setIcon(*mVCardIcon);
            
            note->setPrimaryAction(new HbAction(hbTrId("txt_common_button_ok"), note));
            note->setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), note));
            HbAction *selected = note->exec();
            if (selected == note->secondaryAction())
            {
                // Remove the avatar detail from the temp contact
                createVCard = tempContact.removeDetail(&a);
            } 
            else if (selected == note->primaryAction()) 
            {
                createVCard = true;
            }
            delete note;
            break;
        }
        else
        {
            // Contact does not have an image
            createVCard = true;
        }
    } 
    
    // False = User clicked outside the popup, dismissing it
    //       = User prompted the removal of the avatar but it failed
    if (!createVCard)
    {
        return false;
    }
    
    list.append(tempContact);
    QVersitContactExporter exporter;
    // The vCard version needs to be 2.1 due to backward compatiblity when sending 
    if (!exporter.exportContacts(list, QVersitDocument::VCard21Type))
        return false;
    
    QList<QVersitDocument> docs = exporter.documents();
    
    QFile f(vCardPath);
    if (!f.open(QIODevice::WriteOnly)) 
    {
        return false;
    }
    
    // Start creating the vCard
    QVersitWriter writer;
    writer.setDevice(&f);
    
    bool ret = writer.startWriting(docs);
    ret = writer.waitForFinished();
    
    return ret;
}

/*!
Return the pointer to the document loader
*/
CntDocumentLoader* CntContactCardViewPrivate::document()
{
    if (!mLoader) 
    {
        mLoader = new CntDocumentLoader();
    }
    return mLoader;
}

/*!
Return pointer to the contact manager
*/
QContactManager* CntContactCardViewPrivate::contactManager()
{
    return mViewManager->contactManager(SYMBIAN_BACKEND);
}

// end of file
