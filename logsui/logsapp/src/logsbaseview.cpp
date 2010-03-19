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
#include <hbinstance.h>
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

Q_DECLARE_METATYPE(LogsCall*)
Q_DECLARE_METATYPE(LogsMessage*)
Q_DECLARE_METATYPE(LogsContact*)
Q_DECLARE_METATYPE(LogsDetailsModel*)

const int contextMenuTimeout = 5000000; //5 secs

// -----------------------------------------------------------------------------
// LogsBaseView::LogsBaseView
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
      mForceDialpadOpened(false),
      mCall(0),
      mMessage(0),
      mContact(0),
      mDetailsModel(0),
      mCallTypeMapper(0)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::LogsBaseView()" );
    
    mSoftKeyBackAction = new HbAction(Hb::BackAction, this);
    connect( mSoftKeyBackAction, SIGNAL( triggered() ), this, 
            SLOT( handleBackSoftkey() ) );

    mDialpad =  mRepository.dialpad();
    
    //mDialpad->setDismissPolicy(Dialpad::NoDismiss);

    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::LogsBaseView()" );
}
    
// -----------------------------------------------------------------------------
// LogsBaseView::~LogsBaseView
// -----------------------------------------------------------------------------
//
LogsBaseView::~LogsBaseView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::~LogsBaseView()" );

    clearSoftKey();    
    delete mSoftKeyBackAction;
    
    delete mCall;
    delete mMessage;
    delete mContact;
    delete mDetailsModel;
    
    delete mCallTypeMapper;

    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::~LogsBaseView()" );
}

// -----------------------------------------------------------------------------
// LogsBaseView::viewId
// -----------------------------------------------------------------------------
//
LogsAppViewId LogsBaseView::viewId() const
{
    return mViewId;
}

// -----------------------------------------------------------------------------
// LogsBaseView::isExitAllowed
// -----------------------------------------------------------------------------
//
bool LogsBaseView::isExitAllowed()
{
    return true;
}

// -----------------------------------------------------------------------------
// LogsBaseView::activated
// -----------------------------------------------------------------------------
//
void LogsBaseView::activated(bool showDialer, QVariant args)
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::activated()" );
    Q_UNUSED(args);
    //we have to set object tree of the repository to the current view
    mRepository.setObjectTreeToView( mViewId );

    if ( !mInitialized ) {
        initView();
    }
        
    connect( mDialpad, SIGNAL( aboutToClose() ), this, 
            SLOT( dialpadClosed() ), Qt::QueuedConnection );
    connect( &mDialpad->editor(), SIGNAL( contentsChanged() ), this,
            SLOT( dialpadEditorTextChanged() ) );
    
    connect( &mViewManager.mainWindow(), SIGNAL(aboutToChangeOrientation()),
            this, SLOT(handleAboutToChangeOrientation()) );
    
    connect( &mViewManager.mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(handleOrientationChanged()) );

    activateSoftKey();
    
    if ( showDialer && !mDialpad->isVisible() ) {
        openDialpad();
    }
    
    updateWidgetsSizeAndLayout();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::activated()" );
}

// -----------------------------------------------------------------------------
// LogsBaseView::deactivated
// -----------------------------------------------------------------------------
//
void LogsBaseView::deactivated()
{
    LOGS_QDEBUG( "logs [UI] <-> LogsBaseView::deactivated()" );
    disconnect( mDialpad, SIGNAL( aboutToClose() ), this, 
            SLOT( dialpadClosed() ) );
    disconnect( &mDialpad->editor(), SIGNAL( contentsChanged() ), this,
            SLOT( dialpadEditorTextChanged() ) );

    disconnect( &mViewManager.mainWindow(), SIGNAL(aboutToChangeOrientation()),
            this, SLOT(handleAboutToChangeOrientation()) );
    disconnect( &mViewManager.mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(handleOrientationChanged()) );
    clearSoftKey();
}

// -----------------------------------------------------------------------------
// LogsBaseView::notSupported
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
// LogsBaseView::handleBackSoftkey
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleBackSoftkey()
{
    //mViewManager.activatePreviousView();
    mViewManager.activateView( LogsRecentViewId, false, QVariant() );
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
// LogsBaseView::handleExit
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleExit()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::handleExit()" );   
    mViewManager.exitApplication();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::handleExit()" );
}

// -----------------------------------------------------------------------------
// LogsBaseView::showFilterMenu
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
                     
        if ( !hbInstance->allMainWindows().isEmpty() ){
            pos.setX(toolbarGeometry.bottomRight().x());
            pos.setY(toolbarGeometry.topRight().y());
        }
        mShowFilterMenu->setPreferredPos(pos,HbPopup::BottomRightCorner);
        LOGS_QDEBUG_2("logs [UI]    menupos:", pos)
        mShowFilterMenu->exec();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::showFilterMenu()" );
}

// -----------------------------------------------------------------------------
// LogsBaseView::openDialpad
// -----------------------------------------------------------------------------
//
void LogsBaseView::openDialpad()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::openDialpad()" );
    updateCallButton();
    setDialpadPosition();
    mDialpad->openDialpad();
    updateWidgetsSizeAndLayout();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::openDialpad()" );
}

// -----------------------------------------------------------------------------
// LogsBaseView::openContactsApp
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
// LogsBaseView::setDialpadPosition
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
// LogsBaseView::dialpadClosed
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
// LogsBaseView::dialpadEditorTextChanged
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
// LogsBaseView::changeFilter
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
// LogsBaseView::showListItemMenu
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
    
    if (itemContextMenu.actions().count() > 0) {
        itemContextMenu.exec(coords);
    }
}

// -----------------------------------------------------------------------------
// LogsBaseView::populateListItemMenu
// -----------------------------------------------------------------------------
//
void LogsBaseView::populateListItemMenu(HbMenu& menu)
{
    delete mCallTypeMapper;
    mCallTypeMapper = 0;

    if (mCall) {
        mCallTypeMapper = new QSignalMapper();
        foreach(LogsCall::CallType callType, mCall->allowedCallTypes()){
            if (callType != mCall->defaultCallType()) {
                HbAction* callAction = new HbAction;
                if (callType == LogsCall::TypeLogsVoiceCall){
                    callAction->setText(tr("Voice call"));
                }
                else if (callType == LogsCall::TypeLogsVideoCall){
                    callAction->setText(tr("Video call"));
                }
                else if (callType == LogsCall::TypeLogsVoIPCall){
                    callAction->setText(tr("VoIP call"));
                }
                
                connect(callAction, SIGNAL(triggered()),
                        mCallTypeMapper, SLOT( map()) );
                mCallTypeMapper->setMapping(callAction, callType);
                 
                menu.addAction(callAction);
            }
        }
        connect(mCallTypeMapper, SIGNAL(mapped(int)),
                this, SLOT( initiateCall(int)) );
    }
    if (mMessage) {
        HbAction* messageAction = new HbAction;
        messageAction->setText(tr("Create message"));
        menu.addAction(messageAction);
        QObject::connect( messageAction, SIGNAL(triggered()), 
                          this, SLOT( createMessage() ) );
    }
    if (mContact) {
        HbAction* contactAction = new HbAction;
        if (mContact->allowedRequestType() ==
                LogsContact::TypeLogsContactOpen) {
            contactAction->setText(tr("Open contact"));
            QObject::connect( contactAction, SIGNAL(triggered()), 
                              mContact, SLOT(open()) );
        }
        else {
            contactAction->setText(tr("Add to contacts"));
            QObject::connect( contactAction, SIGNAL(triggered()), 
                              mContact, SLOT(save()) );
        
        }
        menu.addAction(contactAction);
    }    
    if (mDetailsModel) {
        HbAction* callDetailsAction = new HbAction;
        callDetailsAction->setText(tr("Call details"));
        menu.addAction(callDetailsAction);
        QObject::connect(callDetailsAction, SIGNAL(triggered()), 
                         this, SLOT(showCallDetails()));
        
        HbAction* deleteAction = new HbAction;
        deleteAction->setText(tr("Delete"));
        menu.addAction(deleteAction);
        QObject::connect(deleteAction, SIGNAL(triggered()), 
                         this, SLOT(deleteEvent()));
    }
}

// -----------------------------------------------------------------------------
// LogsBaseView::updateListItemData
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
    //TODO:  replace following 'Not supported' -popup
    //       code with real message sending implementation
    notSupported();
}

// -----------------------------------------------------------------------------
// LogsBaseView::updateCall
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
// LogsBaseView::updateMessage
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
// LogsBaseView::updateContact
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
// LogsBaseView::updateDetailsModel
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
// LogsBaseView::clearSoftKey
// -----------------------------------------------------------------------------
//
void LogsBaseView::clearSoftKey()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::clearSoftKey()" );
    if ( !hbInstance->allMainWindows().isEmpty() ){
        hbInstance->allMainWindows().at(0)->removeSoftKeyAction( 
            Hb::SecondarySoftKey, mSoftKeyBackAction );  
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::clearSoftKey()" );
}

// -----------------------------------------------------------------------------
// LogsBaseView::activateSoftKey
// -----------------------------------------------------------------------------
//
void LogsBaseView::activateSoftKey()
{
    if ( !hbInstance->allMainWindows().isEmpty() ){
        hbInstance->allMainWindows().at(0)->addSoftKeyAction( 
            Hb::SecondarySoftKey, mSoftKeyBackAction );
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
    if ( orientation == Qt::Horizontal ) {
        if ( mDialpad->isVisible() && !ignoreDialpad ) {
            newLayout = QString( logsListLandscapeDialpadLayout );
        } else {
            newLayout = QString( logsListLandscapeLayout );
        }
    } else {
        newLayout = QString( logsListDefaultLayout );
    }
    
    if ( newLayout != listView.layoutName() ) {
        LOGS_QDEBUG_2( "logs [UI]  setting new list layout name: ", newLayout );
        listView.setLayoutName( newLayout );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::updateListLayoutName()" );
}

// -----------------------------------------------------------------------------
// Loads appropriate section from *.docml to resize list widget
// -----------------------------------------------------------------------------
//
void LogsBaseView::updateListSize( QString& currentSection )
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::updateListSize()" );
    QString newSection( logsViewDefaultSection );
    Qt::Orientation orientation = mViewManager.mainWindow().orientation();

    if ( mDialpad->isVisible() ) {
        if ( orientation == Qt::Horizontal ) {
            newSection = QString( logsViewLandscapeDialpadSection );
        } else {
            newSection = QString( logsViewPortraitDialpadSection );
        }
    } else {
        newSection = QString( logsViewDefaultSection );
    }
    
    if ( newSection != currentSection ) {
        currentSection = newSection;
        mRepository.loadSection( viewId(), newSection );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::updateListSize()" );
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleAboutToChangeOrientation()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::handleAboutToChangeOrientation()" );
    //we close dialpad until orientation switching is done,
    //otherwise dialpad widget is visible on top of orientation switch effect
    if ( mDialpad->isVisible() ) {
        mForceDialpadOpened = true;
        //temporary disconnect from the aboutToClose signal, since we don't want to 
        //do widgets size and layout update now, it will be done once orientation
        //switch is finnished
        disconnect( mDialpad, SIGNAL( aboutToClose() ), this, 
                SLOT( dialpadClosed() ) );
        mDialpad->closeDialpad();
        connect( mDialpad, SIGNAL( aboutToClose() ), this, 
                SLOT( dialpadClosed() ), Qt::QueuedConnection );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::handleAboutToChangeOrientation()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsBaseView::handleOrientationChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsBaseView::handleOrientationChanged()" );
    //view switching animation is over, now we have to reopen dialpad, if needed
    if ( mForceDialpadOpened ) {
        setDialpadPosition();
        mDialpad->openDialpad();
    }
    mForceDialpadOpened = false;
    updateWidgetsSizeAndLayout();
    LOGS_QDEBUG( "logs [UI] <- LogsBaseView::handleOrientationChanged()" );
}
