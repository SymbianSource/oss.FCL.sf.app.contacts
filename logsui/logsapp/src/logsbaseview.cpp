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
//USER
#include "logsbaseview.h"
#include "logscomponentrepository.h"
#include "logsabstractviewmanager.h"
#include "logsdefs.h"
#include "logslogger.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logscontact.h"
#include "logsabstractmodel.h"
#include "logsdetailsmodel.h"

//SYSTEM
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbmessagebox.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <dialpad.h>
#include <hblineedit.h>
#include <hbeffect.h>
#include <hbabstractviewitem.h>
#include <QSignalMapper>
#include <xqservicerequest.h>
#include <hblabel.h>
#include <hblistview.h>
#include <QGraphicsLinearLayout>
#include <hbpushbutton.h>


Q_DECLARE_METATYPE(LogsCall*)
Q_DECLARE_METATYPE(LogsMessage*)
Q_DECLARE_METATYPE(LogsContact*)
Q_DECLARE_METATYPE(LogsDetailsModel*)

const int contextMenuTimeout = 5000000; //5 secs

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsBaseView::LogsBaseView( 
    LogsAppViewId viewId, 
    LogsComponentRepository& repository, 
    LogsAbstractViewManager& viewManager )
    : HbView(0),
      mViewId( viewId ),
      mRepository( repository ),
      mViewManager( viewManager ),
      mShowFilterMenu(0),
      mEmptyListLabel(0),
      mInitialized(false),
      mCall(0),
      mMessage(0),
      mContact(0),
      mDetailsModel(0),
      mCallTypeMapper(0)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::LogsBaseView()" );
    
    mSoftKeyBackAction = new HbAction(Hb::BackNaviAction, this);
    connect(mSoftKeyBackAction, SIGNAL(triggered()), this, 
            SLOT(handleBackSoftkey()));

    mDialpad =  mRepository.dialpad();
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::LogsBaseView()" );
}
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsBaseView::~LogsBaseView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::~LogsBaseView()" );

    delete mCall;
    delete mMessage;
    delete mContact;
    delete mDetailsModel;    
    delete mCallTypeMapper;

    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::~LogsBaseView()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsAppViewId LogsBaseView::viewId() const
{
    return mViewId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsBaseView::isExitAllowed()
{
    return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::activated(bool showDialer, QVariant args)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::activated()" );
    Q_UNUSED(args);
    //we have to set object tree of the repository to the current view
    mRepository.setObjectTreeToView( mViewId );

    if (!mInitialized) {
        initView();
    }
        
    connect( mDialpad, SIGNAL( aboutToClose() ), this, 
            SLOT( dialpadClosed() ), Qt::QueuedConnection );
    connect( mDialpad, SIGNAL( aboutToOpen() ), this, 
            SLOT( dialpadOpened() ), Qt::QueuedConnection );
    connect( &mDialpad->editor(), SIGNAL( contentsChanged() ), this,
            SLOT( dialpadEditorTextChanged() ) );

    if ( navigationAction() != mSoftKeyBackAction ) {
        setNavigationAction(mSoftKeyBackAction);
    }
    
    if (showDialer && !mDialpad->isOpen()) {
        openDialpad();
    }
    
    updateWidgetsSizeAndLayout();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::activated()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::deactivated()
{
    LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::deactivated()" );
    disconnect( mDialpad, SIGNAL( aboutToClose() ), this, 
            SLOT( dialpadClosed() ) );
    disconnect( mDialpad, SIGNAL( aboutToOpen() ), this, 
            SLOT( dialpadOpened() ) );
    disconnect( &mDialpad->editor(), SIGNAL( contentsChanged() ), this,
            SLOT( dialpadEditorTextChanged() ) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::notSupported()
{
    HbMessageBox* messageBox = new HbMessageBox(QString("Not supported"));
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setTimeout( HbMessageBox::StandardTimeout );
    messageBox->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleBackSoftkey()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::handleBackSoftkey()" );
    //mViewManager.activatePreviousView();
    mViewManager.activateView( LogsRecentViewId, false, QVariant() );
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::handleBackSoftkey()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::initFilterMenu()
{
    mShowFilterMenu = 
        qobject_cast<HbMenu*>( mRepository.findWidget( logsShowFilterMenuId ) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::addActionNamesToMap()
{
    mActionMap.clear();
    mActionMap.insert(LogsServices::ViewReceived, logsShowFilterReceivedMenuActionId);
    mActionMap.insert(LogsServices::ViewCalled, logsShowFilterDialledMenuActionId);
    mActionMap.insert(LogsServices::ViewMissed, logsShowFilterMissedMenuActionId);
    mActionMap.insert(LogsServices::ViewAll, logsShowFilterRecentMenuActionId);  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::initView()
{   
    Q_ASSERT_X( !mInitialized, "logs [UI] ", "view is already initialized!!" );
    mInitialized = true;
    initFilterMenu();
    addActionNamesToMap();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QAbstractItemModel* LogsBaseView::model() const
{
    LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::model()" );
    return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleExit()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::handleExit()" );   
    mViewManager.exitApplication();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::handleExit()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::showFilterMenu()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::showFilterMenu()" );
    if ( mShowFilterMenu ) {
        QRectF toolbarGeometry = toolBar()->geometry();
        QSizeF menuSize = mShowFilterMenu->size();
        LOGS_QDEBUG_2("logs [UI]    menusize:", menuSize)
        
        QPointF pos( toolbarGeometry.bottomRight().x(),
                     toolbarGeometry.topRight().y() ); 
                     
        pos.setX(toolbarGeometry.bottomRight().x());
        pos.setY(toolbarGeometry.topRight().y());

        mShowFilterMenu->setPreferredPos(pos,HbPopup::BottomRightCorner);
        LOGS_QDEBUG_2("logs [UI]    menupos:", pos)
        mShowFilterMenu->exec();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::showFilterMenu()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::openDialpad()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::openDialpad()" );
    updateCallButton();
    setDialpadPosition();
    mDialpad->openDialpad();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::openDialpad()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::openContactsApp()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::openContactsApp()" );
    
    // Need to do request in async manner, otherwise new phonebook ui process
    // will be started due bug(?) in highway.
    XQServiceRequest snd("com.nokia.services.phonebookappservices.Launch","launch()", false);
    int retValue;
    snd.send(retValue);
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::openContactsApp()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::setDialpadPosition()
{
    HbMainWindow& window = mViewManager.mainWindow(); 
    QRectF screenRect = window.layoutRect();
    LOGS_QDEBUG_2( "logs [UI] -> LogsBaseView::setDialpadPosition(), screenRect:", 
            screenRect );
    if (window.orientation() == Qt::Horizontal) {
        // dialpad takes half of the screen width    
        mDialpad->setPos(QPointF(screenRect.width()/2,
                                 this->scenePos().y()));
        mDialpad->setPreferredSize(screenRect.width()/2,
                                   (screenRect.height()-scenePos().y()));                                  
    } else {
        // dialpad takes 65% of the screen height
        qreal screenHeight = screenRect.height();
        mDialpad->setPos(QPointF(0, screenHeight/2.25));
        mDialpad->setPreferredSize(screenRect.width(),
                                   screenHeight-screenHeight/2.25);        
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::setDialpadPosition()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::dialpadOpened()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::dialpadOpened()" );
    updateWidgetsSizeAndLayout();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::dialpadOpened()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::dialpadClosed()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::dialpadClosed()" );
    mDialpad->editor().setText(QString());
    updateWidgetsSizeAndLayout();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::dialpadClosed()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::dialpadEditorTextChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::dialpadEditorTextChanged()" );
    if ( mDialpad->editor().text().length() > 0 ) {
        mViewManager.activateView( LogsMatchesViewId, true, QVariant() );      
    } else {
        updateCallButton();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::dialpadEditorTextChanged()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::changeFilter(HbAction* action)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::changeFilter()" );
    LogsServices::LogsView view = mActionMap.key( action->objectName(),
            LogsServices::ViewAll );
    QVariant args(view);
    mViewManager.activateView( LogsRecentViewId, false, args );
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::changeFilter()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::showListItemMenu(
        HbAbstractViewItem* item, const QPointF& coords )
{
    HbMenu itemContextMenu(0);    
    itemContextMenu.setDismissPolicy(HbMenu::TapAnywhere);
    itemContextMenu.setTimeout(contextMenuTimeout);

    updateListItemData(item->modelIndex());    
    populateListItemMenu(itemContextMenu);

    if (mDialpad->isOpen()) {
        mDialpad->closeDialpad();
    }    
    if (itemContextMenu.actions().count() > 0) {
        itemContextMenu.exec(coords);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::populateListItemMenu(HbMenu& menu)
{
    delete mCallTypeMapper;
    mCallTypeMapper = 0;

    if (mCall) {
        mCallTypeMapper = new QSignalMapper();
                
        foreach(LogsCall::CallType callType, mCall->allowedCallTypes()){
            HbAction* callAction = new HbAction;
            if (callType == LogsCall::TypeLogsVoiceCall){
                callAction->setText(hbTrId("txt_common_menu_voice_call"));
            }
            else if (callType == LogsCall::TypeLogsVideoCall){
                callAction->setText(hbTrId("txt_common_menu_video_call"));
            }
            else if (callType == LogsCall::TypeLogsVoIPCall){
                callAction->setText(hbTrId("txt_common_menu_internet_call"));
            }
            
            connect(callAction, SIGNAL(triggered()),
                    mCallTypeMapper, SLOT( map()) );
            mCallTypeMapper->setMapping(callAction, callType);
            
            // Default call type must be the first item in context menu
            if (callType != mCall->defaultCallType() ||
                menu.actions().count() == 0){
                menu.addAction(callAction);
            } else {   
                menu.insertAction(menu.actions().at(0), callAction);
            }           
        }    
        connect(mCallTypeMapper, SIGNAL(mapped(int)),
                this, SLOT( initiateCall(int)) );
    }
    
    if (mMessage) {
        HbAction* messageAction = new HbAction;
        messageAction->setText(hbTrId("txt_common_menu_create_message"));
        menu.addAction(messageAction);
        QObject::connect( messageAction, SIGNAL(triggered()), 
                          this, SLOT( createMessage() ) );
    }
    if (mContact) {
        HbAction* contactAction = new HbAction;
        if (mContact->allowedRequestType() ==
                LogsContact::TypeLogsContactOpen) {
            contactAction->setText(hbTrId("txt_dialer_ui_menu_open_contact"));
            QObject::connect( contactAction, SIGNAL(triggered()), 
                              mContact, SLOT(open()) );
        }
        else {
            contactAction->setText(hbTrId("txt_common_menu_add_to_contacts"));
            QObject::connect( contactAction, SIGNAL(triggered()), 
                              this, SLOT(saveContact()) );
        
        }
        menu.addAction(contactAction);
    }    
    if (mDetailsModel) {
        HbAction* callDetailsAction = new HbAction;
        callDetailsAction->setText(hbTrId("txt_dialer_ui_menu_call_details"));
        menu.addAction(callDetailsAction);
        QObject::connect(callDetailsAction, SIGNAL(triggered()), 
                         this, SLOT(showCallDetails()));
        
        HbAction* deleteAction = new HbAction;
        deleteAction->setText(hbTrId("txt_common_menu_delete"));
        menu.addAction(deleteAction);
        QObject::connect(deleteAction, SIGNAL(triggered()), 
                         this, SLOT(deleteEvent()));
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateListItemData(const QModelIndex& listIndex)
{
    updateCall(listIndex);
    updateMessage(listIndex);
    updateContact(listIndex);
    updateDetailsModel(listIndex);
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateCallButton()
{
    mDialpad->setCallButtonEnabled( !mDialpad->editor().text().isEmpty() );
}
 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::initiateCallback(const QModelIndex& index)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::initiateCallback()" );
    updateCall(index);

    if (mCall) {
        mCall->initiateCallback();
    }
    delete mCall;
    mCall = 0;
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::initiateCallback()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsBaseView::initiateCall(int callType)
{       
    if (mCall) {
        mCall->call( static_cast<LogsCall::CallType>(callType) );
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::createMessage()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::createMessage()" );
    if (mMessage) {
        mMessage->sendMessage();
    }
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::createMessage()" );   
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::saveContact()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::saveContact()" ); 
    
    if (mContact){
        HbDialog* popup = new HbDialog();
        popup->setDismissPolicy(HbDialog::NoDismiss);
        popup->setHeadingWidget(
                new HbLabel(hbTrId("txt_dial_title_add_to_contacts"), popup));
        popup->setAttribute(Qt::WA_DeleteOnClose);
        popup->setTimeout( HbPopup::NoTimeout );
        popup->setSecondaryAction(
                new HbAction(hbTrId("txt_dial_button_cancel"), popup));

        HbWidget* buttonWidget = new HbWidget(popup);
        QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
        
        HbPushButton* addButton = new HbPushButton(buttonWidget);
        addButton->setOrientation(Qt::Horizontal);
        addButton->setText(hbTrId("txt_dial_list_save_as_a_new_contact"));
        HbIcon plusIcon("qtg_mono_plus");
        addButton->setIcon(plusIcon);
        connect(addButton, SIGNAL(clicked()), popup, SLOT(close()));
        connect(addButton, SIGNAL(clicked()), mContact, SLOT(addNew()));
        
        HbPushButton* updateButton = new HbPushButton(buttonWidget);
        updateButton->setOrientation(Qt::Horizontal);
        updateButton->setText(hbTrId("txt_dial_list_update_existing_contact"));
        updateButton->setIcon(plusIcon);
        connect(updateButton, SIGNAL(clicked()), popup, SLOT(close()));
        connect(updateButton, SIGNAL(clicked()),
                mContact, SLOT(updateExisting()));
        
        layout->addItem(addButton);
        layout->addItem(updateButton);
        
        buttonWidget->setLayout(layout);
        popup->setContentWidget(buttonWidget);

        popup->exec();
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::saveContact()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateCall(const QModelIndex &listIndex)
{
    delete mCall;
    mCall = 0;
    if ( model() ) {
        mCall = qVariantValue<LogsCall*>( model()->data( 
                listIndex, LogsAbstractModel::RoleCall ) );    
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateMessage(const QModelIndex &listIndex)
{
    delete mMessage;
    mMessage = 0;
    if ( model() ) {
        mMessage = qVariantValue<LogsMessage*>( model()->data( 
                listIndex, LogsAbstractModel::RoleMessage ) );
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateContact(const QModelIndex &listIndex)
{
    delete mContact;
    mContact = 0;
    if ( model() ) {
        mContact = qVariantValue<LogsContact*>( model()->data( 
                listIndex, LogsAbstractModel::RoleContact ) );    
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateDetailsModel(const QModelIndex &listIndex)
{
    delete mDetailsModel;
    mDetailsModel = 0;
    if ( model() ) {
        mDetailsModel = qVariantValue<LogsDetailsModel*>( model()->data( 
                listIndex, LogsAbstractModel::RoleDetailsModel ) );    
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::activateEmptyListIndicator(QAbstractItemModel* model)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::activateEmptyListIndicator()" );
    if ( !mEmptyListLabel ){
        mEmptyListLabel = qobject_cast<HbLabel*>(
                            mRepository.findWidget( logsLabelEmptyListId ) );
    }
    if ( model ){
         // Listen for changes in model and update empty list label accordingly
         connect( model, SIGNAL(rowsInserted(const QModelIndex&,int,int)), 
             this, SLOT(updateEmptyListWidgetsVisibility()));
         connect( model, SIGNAL(rowsRemoved(const QModelIndex&,int,int)), 
             this, SLOT(updateEmptyListWidgetsVisibility()));
         connect( model, SIGNAL(modelReset()), 
             this, SLOT(updateEmptyListWidgetsVisibility()));
         // Update to reflect current situation
         updateEmptyListWidgetsVisibility();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::activateEmptyListIndicator()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::deactivateEmptyListIndicator(QAbstractItemModel* model)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::deactivateEmptyListIndicator()" );
    if ( model ){
         disconnect( model, SIGNAL(rowsInserted(const QModelIndex&,int,int)), 
             this, SLOT(updateEmptyListWidgetsVisibility()));
         disconnect( model, SIGNAL(rowsRemoved(const QModelIndex&,int,int)), 
             this, SLOT(updateEmptyListWidgetsVisibility()));
         disconnect( model, SIGNAL(modelReset()), 
             this, SLOT(updateEmptyListWidgetsVisibility()));
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::deactivateEmptyListIndicator()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::addViewSwitchingEffects()
{
    // Add view switching effects, these are called when 
    // HbMainWindow::setCurrentView is called, may be removed once orbit
    // has some built-in effects for view switching.
    HbEffect::add(this, ":/view_show.fxml", "show");
    HbEffect::add(this, ":/view_hide.fxml", "hide");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateEmptyListWidgetsVisibility()
{
    updateEmptyListLabelVisibility();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateEmptyListLabelVisibility()
{
    if ( mEmptyListLabel && model() ){
        LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::updateEmptyListLabelVisibility()" );   
        bool visible(model()->rowCount() == 0);
        mEmptyListLabel->setVisible(visible);
    }  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::showCallDetails()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::showCallDetails()" );
    
    if ( !mDetailsModel ){
        return;
    }
    
    QVariant arg = qVariantFromValue( mDetailsModel );
    if ( mViewManager.activateView(LogsDetailsViewId, false, arg) ){
        mDetailsModel = 0;
    }
   
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::showCallDetails()" );
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::deleteEvent()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::deleteEvent()" );
    if ( mDetailsModel ) {
        mDetailsModel->clearEvent();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::deleteEvent()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateWidgetsSizeAndLayout()
{
}

// -----------------------------------------------------------------------------
// Sets appropriate list widget layout using listView.setLayoutName()
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateListLayoutName( HbListView& listView, bool ignoreDialpad )
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::updateListLayoutName()" );
    QString newLayout( logsListDefaultLayout  );
    Qt::Orientation orientation = mViewManager.mainWindow().orientation();    
    if (orientation == Qt::Horizontal) {
        if (mDialpad->isOpen()) {
            if (ignoreDialpad) {
                newLayout = QString( logsListDefaultLayout );
            } else {
                newLayout = QString( logsListLandscapeDialpadLayout );
            }
        } else {
            newLayout = QString( logsListLandscapeLayout );
        }
    } else {
        newLayout = QString( logsListDefaultLayout );
    }
    
    if (newLayout != listView.layoutName()) {
        LOGS_QDEBUG_2( "logs [UI]  setting new list layout name: ", newLayout );
        listView.setLayoutName( newLayout );
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::updateListLayoutName()" );
}

// -----------------------------------------------------------------------------
// Loads appropriate section from *.docml to resize list widget
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateListSize()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::updateListSize()" );
    QString newSection( logsViewDefaultSection );
    Qt::Orientation orientation = mViewManager.mainWindow().orientation();

    if (mDialpad->isOpen()) {
        if (orientation == Qt::Horizontal) {
            newSection = QString( logsViewLandscapeDialpadSection );
        } else {
            newSection = QString( logsViewPortraitDialpadSection );
        }
    } else {
        newSection = QString( logsViewDefaultSection );
    }
    
    if (newSection != mLayoutSectionName) {
        mLayoutSectionName = newSection;
        LOGS_QDEBUG_2( "logs [UI]  loading new section: ", newSection );
        mRepository.loadSection( viewId(), newSection );
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::updateListSize()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleOrientationChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::handleOrientationChanged()!" );
    setDialpadPosition();
    updateWidgetsSizeAndLayout();    
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::handleOrientationChanged()");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::toggleActionAvailability( HbAction* action, bool available )
{
    if ( action ){
        action->setVisible( available );
    }
}
