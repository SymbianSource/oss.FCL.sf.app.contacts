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
* Description:
*
*/

#include "cntcontactcardview_p.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsSceneResizeEvent>
#include <QStringList>
#include <QStandardItemModel>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <QDebug>
#include <QKeyEvent>
#include <QDir>

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
#include <hbparameterlengthlimiter.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
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
#include "cntcontactcardcontextmenu.h"
#include "cntmainwindow.h"
#include "cntstringmapper.h"
#include "cntdocumentloader.h"
#include "cntimagelabel.h"
#include "cntimageutility.h"
#include "cntfavourite.h"
#include "cntactionlauncher.h"
#include <hbselectiondialog.h>


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
    mAvatar(NULL),
    mIsHandlingMenu(false),
    mFavoriteGroupId(-1),
    mLoader(NULL),
    mContactAction(NULL),
    mBackKey(NULL),
    mImageLabel(NULL),
    mVCardIcon(NULL),
    mShareUi(NULL),
    mAcceptSendKey(true),
    mSendKeyListModel(NULL)
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
    mView->deleteLater();
    
    delete mContact;
    mContact = 0;
    
    delete mDataContainer;
    mDataContainer = NULL;
    
    delete mAvatar;
    mAvatar = NULL;
    
    delete mLoader;
    mLoader = NULL;
    
    if (mVCardIcon) {
        delete mVCardIcon;
        mVCardIcon = NULL;
    }
    
    if (mShareUi) {
        delete mShareUi;
        mShareUi = NULL;
    }
    
    delete mSendKeyListModel;
    mSendKeyListModel = NULL;
}

/*!
Activates a previous view
*/
void CntContactCardViewPrivate::showPreviousView()
{
    emit backPressed();
    
    //save the contact if avatar has been changed.
    QContact contact = contactManager()->contact(mContact->localId());
    if ( contact != *mContact )
    {
        QList<QContactAvatar> details = mContact->details<QContactAvatar>();
        for (int i = 0; i < details.count(); i++)
        {
            if (!details.at(i).imageUrl().isEmpty())
            {
                contactManager()->saveContact(mContact);
                break;
            }
        }
    }
       
    mViewManager->back( mArgs );
}

/*
Activates a default view and setup name label texts
*/
void CntContactCardViewPrivate::activate(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{   
    mViewManager = aMgr;
    mArgs = aArgs;
    
    mView->installEventFilter(this);
    
    HbMainWindow* window = mView->mainWindow();
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    connect(window, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(keyPressed(QKeyEvent*)));
    
    setOrientation(window->orientation());
        
    QContact contact = aArgs.value(ESelectedContact).value<QContact>();
    mContact = new QContact( contact );
    //my card
    bool myCard = mContact->localId() == contactManager()->selfContactId();
    if (myCard)
    {
        mView->menu()->clearActions();
        mView->toolBar()->removeAction(static_cast<HbAction*>(document()->findObject(QString("cnt:history"))));
    }
    else
    {
        mView->toolBar()->removeAction(static_cast<HbAction*>(document()->findObject(QString("cnt:sendMyCard"))));
    }
    
    // add heading widget to the content
    QGraphicsWidget *c = document()->findWidget(QString("content"));
    QGraphicsLinearLayout* l = static_cast<QGraphicsLinearLayout*>(c->layout());

    mHeadingItem = static_cast<CntContactCardHeadingItem*>(document()->findWidget(QString("cnt_contactcard_heading")));
    mHeadingItem->setDetails(mContact);
    connect(mHeadingItem, SIGNAL(passLongPressed(const QPointF&)), this, SLOT(drawMenu(const QPointF&)));
    connect(mHeadingItem, SIGNAL(passShortPressed(const QPointF&)), this, SLOT(doChangeImage()));

    mImageLabel = static_cast<CntImageLabel*>(document()->findWidget("cnt_contactcard_image"));
    connect(mImageLabel, SIGNAL(iconClicked()), this, SLOT(doChangeImage()));
    connect(mImageLabel, SIGNAL(iconLongPressed(const QPointF&)), this, SLOT(drawMenu(const QPointF&)));
    
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
    mDataContainer = new CntContactCardDataContainer(mContact, NULL, myCard);

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
        CntContactCardDataItem* dataItem = mDataContainer->dataItem(index);
        int pos = dataItem->position();
        
        // communication methods
        if (pos < CntContactCardDataItem::ESeparator && dataItem->isFocusable())
        { 
            CntContactCardDetailItem* item = new CntContactCardDetailItem(index, mContainerWidget);

            connect(item, SIGNAL(clicked()), this, SLOT(onItemActivated()));
            connect(item, SIGNAL(longPressed(const QPointF&)), this, SLOT(onLongPressed(const QPointF&)));
   
            if (mContact->isPreferredDetail(dataItem->action(), dataItem->detail()))
            {
                dataItem->setSecondaryIcon(HbIcon("qtg_mono_favourites"));
                mPreferredItems.insert(dataItem->action(), item);
            }
         
            item->setDetails(dataItem);
            mContainerLayout->addItem(item);
        }

        // separator
        else if (pos == CntContactCardDataItem::ESeparator)
        {      
            HbFrameItem* frameItem = new HbFrameItem(QString("qtg_fr_list_separator"), HbFrameDrawer::NinePieces);
            HbLabel* label = static_cast<HbLabel*>(document()->findWidget(QString("separator")));
            label->setPlainText(dataItem->titleText());
            label->setBackgroundItem(frameItem);
            mContainerLayout->addItem(label);
        }

        // details
        else
        {
            //map support (image only)
            if (pos >= CntContactCardDataItem::EAddress && pos <= CntContactCardDataItem::EAddressWork && !dataItem->icon().isNull())
            {         
                HbLabel* iconLabel = new HbLabel(mView);
                iconLabel->setIcon(dataItem->icon());
                
                int width = dataItem->icon().width();
                int height = dataItem->icon().height();    
                
                //HbLabel setPreferredSize is not working properly,
                //so added minimum , maximum size to fix the issue
                iconLabel->setPreferredSize(QSizeF(width,height));
                iconLabel->setMinimumSize(QSizeF(width, height));
                iconLabel->setMaximumSize(QSizeF(width, height));
                iconLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                    QSizePolicy::Fixed));            
                mContainerLayout->addItem(iconLabel);
            } 
            //other details
            else
            {    
                CntContactCardDetailItem* item = new CntContactCardDetailItem(index, mContainerWidget, false);
                item->setDetails(dataItem);
                mContainerLayout->addItem(item);
            }
        }
    }
    
    if (!myCard)
    {   
        bool setAsFavorite( false );
        QContactLocalId favouriteGroupId = CntFavourite::favouriteGroupId( contactManager() );
        if( favouriteGroupId != 0 )
        {
            setAsFavorite = CntFavourite::isMemberOfFavouriteGroup( contactManager(), mContact );
            mHeadingItem->setSecondaryIcon( setAsFavorite ); // if contact is part of favourites group
        }
        qobject_cast<HbAction *>(document()->findObject("cnt:setasfavorite"))->setVisible( !setAsFavorite );
        qobject_cast<HbAction *>(document()->findObject("cnt:removefromfavorite"))->setVisible( setAsFavorite );
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
 
    emit viewActivated( mViewManager, aArgs );
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
    snd << QString("contactwidgethsplugin");
    snd << preferences;
    snd.send();
}

/*!
Launch contact editor 
*/
void CntContactCardViewPrivate::editContact()
{
    QVariant var;
    var.setValue(*mContact);
    
    mArgs.insert(ESelectedContact, var);
    mArgs.insert(EViewId, editView);
    
    mViewManager->changeView( mArgs );
}

void CntContactCardViewPrivate::addToGroup()
{
}

void CntContactCardViewPrivate::setAsFavorite()
{
    QContactId id = mContact->id();
    CntFavourite::addContactToFavouriteGroup( contactManager(), id );
    
    qobject_cast<HbAction *>(document()->findObject("cnt:removefromfavorite"))->setVisible(true);
    qobject_cast<HbAction *>(document()->findObject("cnt:setasfavorite"))->setVisible(false);
    mHeadingItem->setSecondaryIcon(true);  
}

void CntContactCardViewPrivate::removeFromFavorite()
{
    QContactId id = mContact->id();   
    CntFavourite::removeContactFromFavouriteGroup( contactManager(), id );

    qobject_cast<HbAction *>(document()->findObject("cnt:removefromfavorite"))->setVisible(false);
    qobject_cast<HbAction *>(document()->findObject("cnt:setasfavorite"))->setVisible(true);
    mHeadingItem->setSecondaryIcon(false); 
}

/*!
Delete contact
*/
void CntContactCardViewPrivate::deleteContact()
{    
    QString name = contactManager()->synthesizedDisplayLabel(*mContact);
    
    HbMessageBox::question(HbParameterLengthLimiter(hbTrId("txt_phob_info_delete_1")).arg(name), this, SLOT(handleDeleteContact(HbAction*)),
            hbTrId("txt_phob_button_delete"), hbTrId("txt_common_button_cancel"));
}

/*!
Handle action for deleting a contact
*/
void CntContactCardViewPrivate::handleDeleteContact(HbAction *action)
{
    HbMessageBox *note = static_cast<HbMessageBox*>(sender());
    
    if (note && action == note->actions().first())
    {
        contactManager()->removeContact(mContact->localId());
        mArgs.insert(EViewId, namesView);
        mViewManager->changeView( mArgs );
    }
}

/*!
Launch history view 
*/
void CntContactCardViewPrivate::viewHistory()
{
    QVariant var;
    var.setValue(*mContact);
    mArgs.insert(ESelectedContact, var);
    mArgs.insert(EViewId, historyView);
        
    mViewManager->changeView( mArgs );
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
    qDebug() << "CntContactCardViewPrivate::sendBusinessCard - IN";
    // Check if the contact has an image.
    QList<QContactAvatar> avatars = mContact->details<QContactAvatar>();
    bool imageExists( false );
    /*foreach(QContactAvatar a, mContact->details<QContactAvatar>())
    {
        if (!a.imageUrl().isEmpty())
        {
            imageExists = true;
            HbMessageBox *note = new HbMessageBox(
                    hbTrId("txt_phob_info_add_contact_card_image_to_business_c"),
                    HbMessageBox::MessageTypeQuestion);
            note->setIcon(*mVCardIcon);
            note->clearActions();
            
            HbAction* ok = new HbAction(hbTrId("txt_common_button_ok"), note);
            HbAction* cancel = new HbAction(hbTrId("txt_common_button_cancel"), note);
            
            ok->setObjectName( "ok" );
            cancel->setObjectName( "cancel" );
             
            note->addAction( ok );
            note->addAction( cancel );
            
            note->setModal( true );
            note->setAttribute(Qt::WA_DeleteOnClose, true );
            note->open( this, SLOT(handleSendBusinessCard(HbAction*)));
            break;
        }
    }*/
    
    if ( !imageExists )
    {
        qDebug() << "CntContactCardViewPrivate::sendBusinessCard without image";
        handleSendBusinessCard( NULL ); // no image
    }
    qDebug() << "CntContactCardViewPrivate::sendBusinessCard - OUT";
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
    QString action = mDataContainer->dataItem(index)->action();
    // Check if action is internal
    QList<QContactActionDescriptor> actionDescriptors = QContactAction::actionDescriptors(action, "symbian", 1);
    if(0 < actionDescriptors.count())
    {
        // These actions are considered internal(vendor=symbian and version=1)
        executeAction(*mContact, mDataContainer->dataItem(index)->detail(), action);
    }
    else
    {
        //Handle dynamic actions differently
        executeDynamicAction(*mContact, mDataContainer->dataItem(index)->detail(), mDataContainer->dataItem(index)->actionDescriptor());
    }
}

/*!
Execute the call / message / email action
*/
void CntContactCardViewPrivate::executeAction(QContact& aContact, QContactDetail aDetail, QString aAction)
{
    CntActionLauncher* other = new CntActionLauncher( aAction );
    connect(other, SIGNAL(actionExecuted(CntActionLauncher*)), this, SLOT(actionExecuted(CntActionLauncher*)));
    other->execute(aContact, aDetail);
}

/*!
Execute dynamic action
*/
void CntContactCardViewPrivate::executeDynamicAction(QContact& aContact, QContactDetail aDetail, QContactActionDescriptor aActionDescriptor)
{
    CntActionLauncher* other = new CntActionLauncher( );
    connect(other, SIGNAL(actionExecuted(CntAction*)), this, SLOT(actionExecuted(CntAction*)));
    other->execute(aContact, aDetail, aActionDescriptor);
}

void CntContactCardViewPrivate::actionExecuted(CntActionLauncher* aAction)
{
    aAction->deleteLater();
}

/*!
Set selected detail as preferred for selected action
*/
void CntContactCardViewPrivate::setPreferredAction(const QString &aAction, const QContactDetail &aDetail)
{
    mContact->setPreferredDetail(aAction, aDetail);
    contactManager()->saveContact(mContact);
}

/*!
Item specific menu
*/
void CntContactCardViewPrivate::onLongPressed(const QPointF &aCoords)
{
    CntContactCardDetailItem *item = qobject_cast<CntContactCardDetailItem*>(sender());
    int index = item->index();
    
    CntContactCardContextMenu *menu = new CntContactCardContextMenu( item );
    HbAction *communicationAction = 0;
    HbAction *dynamicAction = 0;
    HbAction *videoCommunicationAction = 0;
    HbAction *preferredAction = 0;
    CntStringMapper stringMapper;
    
    QString action = mDataContainer->dataItem(index)->action();
    QContactDetail detail = mDataContainer->dataItem(index)->detail();
    
    if (action.compare("call", Qt::CaseInsensitive) == 0)
    {       
        QContactDetail detail = mDataContainer->dataItem(index)->detail();
        if (detail.definitionName() == QContactPhoneNumber::DefinitionName)
        {
            QContactPhoneNumber number = static_cast<QContactPhoneNumber>(detail);
            QString context = number.contexts().isEmpty() ? QString() : number.contexts().first();
            QString subtype = number.subTypes().isEmpty() ? number.definitionName() : number.subTypes().first();

            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(subtype, context));
            videoCommunicationAction = menu->addAction(hbTrId("txt_phob_menu_call_video_number"));
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
            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(detail.definitionName(), detail.contexts().first()));
        }
        else
        {
            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(detail.definitionName(), QString()));
        }
    }
    else if (action.compare("url", Qt::CaseInsensitive) == 0)
    {
        if (!detail.contexts().isEmpty())
        {
            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(detail.definitionName(), detail.contexts().first()));
        }
        else
        {
            communicationAction = menu->addAction(stringMapper.getItemSpecificMenuLocString(detail.definitionName(), QString()));
        }
    }
    else if (1 > QContactAction::actionDescriptors(action, "symbian", 1).count())
    {
        // Do verification that action is not internal type(vendor=symbian and version=1)
        // If hard coded action is not found, fetch localization from dynamic action
        // and create dynamic action item.
        dynamicAction = menu->addAction(mDataContainer->dataItem(index)->longPressText());
    }
    
    if (action.compare("call", Qt::CaseInsensitive) == 0)
    {
        preferredAction = menu->addAction(hbTrId("txt_phob_menu_set_as_default_number"));
    }
    else if (action.compare("message", Qt::CaseInsensitive) == 0)
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
   
    if ( communicationAction )
    {
        communicationAction->setObjectName( "communicationAction" );
    }
    if ( videoCommunicationAction )
    {
        videoCommunicationAction->setObjectName( "videoCommunicationAction" );
    }
   
    if ( dynamicAction )
    {
        dynamicAction->setObjectName( "dynamicAction" );
    }
    
    if ( preferredAction )
    {
        preferredAction->setObjectName( "preferredAction" );
        menu->insertSeparator(preferredAction);
    }
   
    menu->setPreferredPos( aCoords );
    menu->setAttribute( Qt::WA_DeleteOnClose, true );
    menu->open( this, SLOT(handleMenuAction(HbAction*)) );
}

void CntContactCardViewPrivate::handleMenuAction(HbAction* aAction)
{
    CntContactCardContextMenu* menu = static_cast<CntContactCardContextMenu*>(sender());
    CntContactCardDetailItem *item = menu->item();
    int index = item->index();
       
    QString action = mDataContainer->dataItem(index)->action();
    QContactDetail detail = mDataContainer->dataItem(index)->detail();
        
    QString name = aAction->objectName();
    
    if ( name == "communicationAction" )
    {
        executeAction( *mContact, detail, action );
    }

    if ( name == "videoCommunicationAction" )
    {
        //service name latter on should come from service table.
        executeAction( *mContact, detail, "videocall" );
    }
    if ( name == "dynamicAction" )
    {             
        executeDynamicAction(*mContact, detail, mDataContainer->dataItem(index)->actionDescriptor());
    }
    
    if ( name == "preferredAction" )
    {
        setPreferredAction(action, detail);
                    
        if (mPreferredItems.contains(action))
        {
            CntContactCardDetailItem *oldItem = mPreferredItems.value(action);
            mDataContainer->dataItem(oldItem->index())->setSecondaryIcon(HbIcon());
            oldItem->setDetails(mDataContainer->dataItem(oldItem->index()));
        }
            
        mDataContainer->dataItem(item->index())->setSecondaryIcon(HbIcon("qtg_mono_favourites"));
        item->setDetails(mDataContainer->dataItem(item->index()));
        
        mPreferredItems.insert(action, item);
    }
}

void CntContactCardViewPrivate::handleSendBusinessCard( HbAction* aAction )
{
    qDebug() << "CntContactCardViewPrivate::handleSendBusinessCard - IN";
    QList<QContact> list;
    /* if ( aAction && aAction->objectName() == "cancel" )
    {
        QContact tmpContact( *mContact );
        foreach ( QContactAvatar a, tmpContact.details<QContactAvatar>() )
        {
            tmpContact.removeDetail( &a );
        }
        list.append( tmpContact );
    }
    else
    {
        list.append( *mContact );
    }*/
    
    QContact tmpContact( *mContact );
    foreach ( QContactAvatar a, tmpContact.details<QContactAvatar>() )
    {
        tmpContact.removeDetail( &a );
    }
    list.append( tmpContact );
      
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
        
    QVersitContactExporter exporter;
    // The vCard version needs to be 2.1 due to backward compatiblity when sending 
    if (exporter.exportContacts(list, QVersitDocument::VCard21Type))
    {
        qDebug() << "CntContactCardViewPrivate::handleSendBusinessCard, VCard21Type";
        QList<QVersitDocument> docs = exporter.documents();
        QFile f(vCardPath);
        if ( f.open(QIODevice::WriteOnly) ) 
        {
            qDebug() << "CntContactCardViewPrivate::handleSendBusinessCard write VCard";
            // Start creating the vCard
            QVersitWriter writer;
            writer.setDevice(&f);
        
            bool ret = writer.startWriting(docs);
            ret = writer.waitForFinished();
        
            // Create the vCard and send it to messaging service
            if (!mShareUi) {
                mShareUi = new ShareUi();
            }
            QStringList l;
            l << vCardPath;
            mShareUi->send(l,false);
        }
    }
    qDebug() << "CntContactCardViewPrivate::handleSendBusinessCard - OUT";
}


/*!
Called after the user clicked "Change Image" from popup menu after 
longpressing the image in this view.
*/
void CntContactCardViewPrivate::doChangeImage()
{
    QVariant var;
    var.setValue(*mContact);
    
    mArgs.insert(ESelectedContact, var);
    mArgs.insert(EViewId, imageEditorView);
        
    mViewManager->changeView( mArgs );
}

/*!
Called after the user clicked "Remove Image" from popup menu after 
longpressing the image in this view.
*/
void CntContactCardViewPrivate::doRemoveImage()
{
    if (mAvatar) 
    {
        CntImageUtility imageUtility;
        QString filePath=mAvatar->imageUrl().toString();
        
        bool success = mContact->removeDetail(mAvatar);
        if (success) 
        { 
            if (!filePath.isEmpty())
            {
                // Check if image removable.
                CntImageUtility imageUtility;
                if(imageUtility.isImageRemovable(filePath))
                {
                    imageUtility.removeImage(filePath);
                }
            }
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
    menu->addAction(hbTrId("txt_phob_menu_change_picture"), this, SLOT(doChangeImage()) );
    if (mAvatar)
    {
        menu->addAction(hbTrId("txt_phob_menu_remove_image"), this, SLOT(doRemoveImage()) );
    }
    menu->setAttribute( Qt::WA_DeleteOnClose );
    menu->setPreferredPos( aCoords );
    menu->open();
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

void CntContactCardViewPrivate::keyPressed(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Yes )
    {
        sendKeyPressed();
    }
}

void CntContactCardViewPrivate::sendKeyPressed()
{   
    QContactDetail preferredDetail = mContact->preferredDetail("call");
    if (!preferredDetail.isEmpty())
    {
        executeAction(*mContact, preferredDetail, "call"); 
    }
    else 
    {   
        if(!mSendKeyListModel)
        {
            mSendKeyListModel = new QStandardItemModel();
        }
        mSendKeyListModel->clear();
        for (int index = 0; index < mDataContainer->itemCount(); index++)
        {
            CntContactCardDataItem* dataItem = mDataContainer->dataItem(index);
            if (dataItem->action().compare("call", Qt::CaseInsensitive) == 0)
            {
                QStandardItem *labelItem = new QStandardItem();
                QStringList textList;
                textList << dataItem->titleText() << dataItem->valueText();
                labelItem->setData(textList, Qt::DisplayRole);
                labelItem->setData(dataItem->icon(), Qt::DecorationRole);
                labelItem->setData(index, Qt::UserRole);
                mSendKeyListModel->appendRow(labelItem);
            }
        }
  
        if (mSendKeyListModel->rowCount() == 0)
        {
            XQServiceRequest snd("com.nokia.services.logsservices.starter", "start(int,bool)", false);
            snd << 0; // all calls
            snd << true; // show dialpad
            snd.send();
        }
        else if (mSendKeyListModel->rowCount() ==1 )
        {
            QContactDetail detail = mDataContainer->dataItem(0)->detail();
            executeAction( *mContact, detail, "call" ); 
        }
        else if (mSendKeyListModel->rowCount() >= 2 && mAcceptSendKey)
        {
            mAcceptSendKey = false;
            
            // Instantiate a dialog        
            mSendKeyPopup = new HbSelectionDialog();
            mSendKeyPopup->clearActions(); 
            mSendKeyPopup->setAttribute(Qt::WA_DeleteOnClose, true);
            mSendKeyPopup->setDismissPolicy(HbPopup::NoDismiss);
            mSendKeyPopup->setTimeout(HbPopup::NoTimeout);
            mSendKeyPopup->setModal(true);
            
            HbListView* listView = new HbListView(mSendKeyPopup);
            listView->setModel(mSendKeyListModel);
            listView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
            listView->setFrictionEnabled(true);
            
            connect(listView, SIGNAL(activated (const QModelIndex&)), this,
                SLOT(launchSendKeyAction(const QModelIndex&)));
            
            QString contactName = mContact->displayLabel();
        
            HbLabel *headingText = new HbLabel(mSendKeyPopup);
            headingText->setPlainText(contactName);
            
            mSendKeyPopup->setHeadingWidget(headingText);
            mSendKeyPopup->setContentWidget(listView);
            
            HbAction *cancelAction = new HbAction(hbTrId("txt_phob_button_cancel"), mSendKeyPopup);
            mSendKeyPopup->addAction(cancelAction);
            
            // Launch dialog asyncronously
            mSendKeyPopup->open(this, SLOT(sendKeyDialogSlot(HbAction*)));
        }
    }
}

void CntContactCardViewPrivate::sendKeyDialogSlot(HbAction* action)
{
    Q_UNUSED(action);
    mAcceptSendKey = true;
}

void CntContactCardViewPrivate::launchSendKeyAction(const QModelIndex &index)
{
    mAcceptSendKey = true;
    mSendKeyPopup->close();
    if (index.isValid())
    {
        int row = index.row();
        int selectedDetail = mSendKeyListModel->item(index.row())->data(Qt::UserRole).toInt();
        executeAction(*mContact, mDataContainer->dataItem(selectedDetail)->detail(), "call");
    }
}



// end of file
