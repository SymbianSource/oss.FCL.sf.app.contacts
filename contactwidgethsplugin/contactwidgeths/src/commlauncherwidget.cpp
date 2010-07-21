/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Communication Launcher widget for Friend widget 
*
*/

#include <QDebug>
#include <HbStyleLoader>
#include <QGraphicsLinearLayout>
#include <HbEffect>
#include <HbPushButton>
#include <qtcontacts.h>
#include <hbdocumentloader.h>
#include <xqappmgr.h>

#include "commlauncherwidget.h"

// ContactManager backend
#ifdef Q_OS_SYMBIAN
    QString cmBackend = "symbian";
#else
    QString cmBackend = "memory"; 
#endif
const QString commLauncherDocml = ":/commlauncherwidget.docml";
// Note: these are case sensitive
const QString callButtonName     = "ButtonCall";
const QString emailButtonName    = "ButtonEmail";
const QString messageButtonName  = "ButtonMessage";
const QString mycardButtonName   = "ButtonMycard";

const QString appearTL = "appear_tl";
const QString appearTR = "appear_tr";
const QString appearBL = "appear_bl";
const QString appearBR = "appear_br"; 

const int commLauncherMargin = 120;  // heights of titlebar & comm.launcher 

/*!
  \class CommLauncherWidget
*/

/*!
    Constructor
*/
CommLauncherWidget::CommLauncherWidget(QGraphicsItem *parent) :
 HbPopup(parent),
 mContact(0),
 mButtonCount(0),
 mRequest(NULL),
 mCallButton(0),
 mSendMsgButton(0),
 mEmailButton(0),
 mPhonebookButton(0),
 mApplicationManager(0)
{    
    
    HbStyleLoader::registerFilePath(":/commlauncherbuttons.css");
    // effect triggers
    connect(this, SIGNAL(aboutToShow()),  SLOT(popupAboutToShow()));
    connect(this, SIGNAL(aboutToClose()), SLOT(popupAboutToClose()));
    
    // create layout
    mLayout = new QGraphicsLinearLayout(this);
    qreal verticalMargin = 0.0;
    style()->parameter("hb-param-margin-gene-middle-vertical", verticalMargin);
    qreal horizontalMargin = 0.0;
    style()->parameter("hb-param-margin-gene-middle-horizontal", horizontalMargin);
    
    mLayout->setContentsMargins(horizontalMargin, verticalMargin,
                                horizontalMargin, 2*verticalMargin);
    
    // create document loader
    HbDocumentLoader *documentLoader = new HbDocumentLoader();
    bool result = false;
    documentLoader->load(commLauncherDocml, &result);
    ASSERT(result);
    
    // create buttons
    const QString callIconName = "qtg_large_active_call";
    mCallButton = createButton(callIconName, callButtonName, documentLoader);
    connect(mCallButton, SIGNAL(clicked()), this, SLOT(makeCall()));
    
    const QString messagingIconName = "qtg_large_message";
    mSendMsgButton = createButton(messagingIconName, messageButtonName, documentLoader);
    connect(mSendMsgButton, SIGNAL(clicked()), this, SLOT(sendMessage()));
    
    const QString emailIconName = "qtg_large_email";
    mEmailButton = createButton(emailIconName, emailButtonName, documentLoader);
    // EMAIL FUNCTIONALITY COMMENTED OUT BECAUSE PLATFORM DOES NOT SUPPORT IT!
    //connect(mEmailButton, SIGNAL(clicked()), this, SLOT(sendEmail()));
    
    const QString phonebookIconName = "qtg_large_mycard";
    mPhonebookButton = createButton(phonebookIconName, mycardButtonName, documentLoader);
    connect(mPhonebookButton, SIGNAL(clicked()), this, SLOT(openPhonebook()));
        
    delete documentLoader;
}

/*!
    Destructor
*/
CommLauncherWidget::~CommLauncherWidget()
{
    // Deleting request cancels all pending requests 
    delete mRequest;
}

/*!
    Sets the contact for the widget
*/
void CommLauncherWidget::setContact(QContact &contact)
{
    mContact = &contact;
}

/*!
    Sets the application manager for the widget
*/
void CommLauncherWidget::setApplicationManager(XQApplicationManager &appManager)
{
    mApplicationManager = &appManager;
}

/*!
    Creates the UI for the widget
*/
void CommLauncherWidget::createUI()
{        
    mButtonCount = 0;
    // Create call button, if number is available
    QList<QContactActionDescriptor> callActionDescriptors = 
            QContactAction::actionDescriptors("call", cmBackend);
    if (callActionDescriptors.count() > 0) {
        QList<QContactPhoneNumber> numberList = mContact->details<QContactPhoneNumber>();
        if (numberList.count() > 0) {
            mLayout->addItem(mCallButton);
            mCallButton->show();
            mButtonCount++;

        } else {
            mCallButton->hide();
            mLayout->removeItem(mCallButton);
            mLayout->invalidate();
            mLayout->activate();
        }
    } else {
        qDebug() << "Call button is not created";
    }
    
    // Create sms button, if number is available
    QList<QContactActionDescriptor> messageActionDescriptors =
            QContactAction::actionDescriptors("message", cmBackend);
    if (messageActionDescriptors.count() > 0) {
        QList<QContactPhoneNumber> numberList = mContact->details<QContactPhoneNumber>();
        if (numberList.count() > 0) {
            mLayout->addItem(mSendMsgButton);
            mSendMsgButton->show();
            mButtonCount++;
        } else {
            mSendMsgButton->hide();
            mLayout->removeItem(mSendMsgButton);
            mLayout->invalidate();
            mLayout->activate();
        }
    } else {
        qDebug() << "Sms button is not created";
    }
        
    // Create email button, if email count exists
    // EMAIL FUNCTIONALITY COMMENTED OUT BECAUSE PLATFORM DOES NOT SUPPORT IT!
    /*QList<QContactActionDescriptor> emailActionDescriptors =
            QContactAction::actionDescriptors("email", cmBackend);
    if (emailActionDescriptors.count() > 0) {
        QList<QContactEmailAddress> emailList = mContact->details<QContactEmailAddress>();
        if (emailList.count() > 0) {
            mLayout->addItem(mEmailButton);
            mEmailButton->show();
            mButtonCount++;
        } else {
            mEmailButton->hide();
            mLayout->removeItem(mEmailButton);
            mLayout->invalidate();
            mLayout->activate();
        }
    } else {
        qDebug() << "Email button not created";
    }
    */

    mLayout->addItem(mPhonebookButton);
    mButtonCount++;

    // add Effects
    HbEffect::add(this, QString(":/friend_expand_tl.fxml"), appearTL);
    HbEffect::add(this, QString(":/friend_expand_tr.fxml"), appearTR);
    HbEffect::add(this, QString(":/friend_expand_bl.fxml"), appearBL);
    HbEffect::add(this, QString(":/friend_expand_br.fxml"), appearBR);       
    HbEffect::add(this, QString(":/friend_minimize.fxml"),  "disappear");
    
    setLayout(mLayout);    
}

/*!
    This widget is about to show, show effect
*/
void CommLauncherWidget::popupAboutToShow()
{ 
    HbEffect::start(this, mAppearEffect);  
}

/*
 * Select which appear effect to use depending on FriendWidget position
 */
void CommLauncherWidget::selectAppearEffect(QPointF FriendPos, QPointF LauncherPos)
{
    bool left=true;
    bool top=true;
    
    qDebug() << "FriendPos "   << FriendPos;
    qDebug() << "LauncherPos " << LauncherPos;
    
    if (LauncherPos.y() > FriendPos.y()) {
        top = false;
    }
    if (LauncherPos.x() > FriendPos.x()) {
        left = false;
    }	

    if (top && left) {
        mAppearEffect = appearTL;
    }
    else if (top && !left) {
        mAppearEffect = appearTR;
    }
    else if (!top && left) {
        mAppearEffect = appearBL;
    }
    else {
        mAppearEffect = appearBR;
    }
    qDebug() << "---------------top " << top << "--- left " << left << " " << mAppearEffect;
}

/*!
    This widget is about to close, show effect
*/
void CommLauncherWidget::popupAboutToClose()
{
    HbEffect::start(this, "disappear");  
    emit launcherClosed();
}

/*!
    Returns a button with a given name and icon to layout
*/
HbPushButton* CommLauncherWidget::createButton(const QString iconName, const QString buttonName,
                                               const HbDocumentLoader *documentLoader)
{
    HbPushButton *button = 0;
    button = qobject_cast<HbPushButton *>
                    (documentLoader->findWidget(buttonName));
    if (button) {
        button->setIcon(HbIcon(iconName));
    }

    return button;
}

  
/*!
    Returns screen position for communication launcher for a widget
*/
QPointF CommLauncherWidget::commLauncherPosition(QPointF widgetPosition, QRectF& widgetBoundingRect,
                                              QRectF& sceneRect, QRectF& launcherRect)
{    
    QPointF commLauncherPosition(widgetPosition.x(), commLauncherMargin);        
    qDebug() << "friend widgetPosition " << widgetPosition.x() << widgetPosition.y();
    
    qreal topMargin = 0.0; 
    style()->parameter("hb-param-margin-gene-top",topMargin);
    qDebug() << "topMargin------------ " << topMargin;
    qDebug() << "launcher size.y " << launcherRect.size().height();
    
    // Calculate vertical position
    if (widgetPosition.y() > commLauncherMargin) {
        commLauncherPosition.setY(widgetPosition.y() - launcherRect.size().height() + topMargin );
    } else {
        commLauncherPosition.setY(widgetPosition.y() + widgetBoundingRect.height() 
                                  - launcherRect.size().height()/2);
    }
        
    // Calculate horizontal position
    if ((widgetPosition.x() + widgetBoundingRect.width()/2) < sceneRect.width()/2 ) {
        // Communication launcher to the right
        qDebug() << "launcher to right";
        commLauncherPosition.setX(widgetPosition.x() + widgetBoundingRect.width()/2);
        
    } else {
        // Communication launcher to the left
        qDebug() << "launcher to left";
        // The width needs to be calculated based on visible buttons.
        launcherRect.setWidth(commLauncherWidth());
        commLauncherPosition.setX(widgetPosition.x() + widgetBoundingRect.width()/2 
        		                  - launcherRect.size().width());
    }
    
    // Check that the communication launcher is not positioned outside the scene in y-axis
    // HbPopup takes care that the launcher is not positioned outside the scene in x-axis
    if (commLauncherPosition.y() > sceneRect.height()) {
        commLauncherPosition.setY(sceneRect.height());
    } else if (commLauncherPosition.y() < sceneRect.y()){
        commLauncherPosition.setY(sceneRect.y());
    }
    
    qDebug() << "commLauncherPosition " << commLauncherPosition.x() << commLauncherPosition.y();
    return commLauncherPosition;
}


/*!
    Makes a phone call to contact.
*/
void CommLauncherWidget::makeCall()
{
    QList<QContactActionDescriptor> callActionDescriptors = 
                QContactAction::actionDescriptors("call", cmBackend); 
    if (callActionDescriptors.count() > 0) {
        //Get preferred number
        QContactDetail detail = mContact->preferredDetail("call");
        QContactPhoneNumber phoneNumber;
            
        //If preferred number is available, cast it to phonenumber
        if(!detail.isEmpty()) {
            phoneNumber = static_cast<QContactPhoneNumber>(detail);
        } else {
            //if preferred is not set select the first number
            phoneNumber = mContact->detail<QContactPhoneNumber>();
        }
        // invoke action
        QContactAction *callAction = QContactAction::action(callActionDescriptors.at(0));
        if (!phoneNumber.isEmpty()) {
            callAction->invokeAction(*mContact, phoneNumber);
            
            qDebug() << "call to number " << phoneNumber.number();
        }
    } else {
        qDebug() << "contact has no Actions, can't make a call";
    }
    
    close();
}

/*!
    Sends a message to contact.
*/
void CommLauncherWidget::sendMessage()
{
    QList<QContactActionDescriptor> messageActionDescriptors = 
                    QContactAction::actionDescriptors("message", cmBackend); 
    if (messageActionDescriptors.count() > 0) {
        //Get preferred message number
        QContactDetail detail = mContact->preferredDetail("message");
        QContactPhoneNumber messageNumber;
            
        //If preferred number is available, cast it to phonenumber
        if(!detail.isEmpty()) {
            messageNumber = static_cast<QContactPhoneNumber>(detail);
        } else {
            //if preferred is not set select the first number
            messageNumber = mContact->detail<QContactPhoneNumber>();
        }
        // invoke action
        QContactAction *messageAction = QContactAction::action(messageActionDescriptors.at(0));
        if (!messageNumber.isEmpty()) {
            messageAction->invokeAction(*mContact, messageNumber);
            
            qDebug() << "send to number " << messageNumber.number();
        }
    } else {
        qDebug() << "contact has no Actions, can't send a message";
    }
    
    close();
}

/*!
    Sends an email to contact.
*/
// EMAIL FUNCTIONALITY COMMENTED OUT BECAUSE WK20 PLATFORM DOES NOT SUPPORT IT!

/*
void CommLauncherWidget::sendEmail()
{

    QList<QContactActionDescriptor> emailActionDescriptors =
                QContactAction::actionDescriptors("email", cmBackend);
    if (emailActionDescriptors.count() > 0) {
        QContactAction *emailAction = QContactAction::action(emailActionDescriptors.at(0));
    //TODO: implement
        QList<QContactEmailAddress> emailList = mContact->details<QContactEmailAddress>();
        if (emailList.count() > 0) {
            emailAction->invokeAction(*mContact, emailList.at(0));
       
            QString emailAddress = emailList.at(0).emailAddress();
            qDebug() << "send to email " << emailAddress;
        }
    } else {
        qDebug() << "contact has no Actions, can't send a email";
    }

    // Deleting request cancels all pending requests
    if (mRequest) {
        delete mRequest;
        mRequest = NULL;
    }
    
    QMap<QString, QVariant> map;
    static const QString emailSendToKey = "to";
    QString emailAddress;
    QList<QContactEmailAddress> emailList = mContact->details<QContactEmailAddress>();    
    if (emailList.count() > 0) {
        emailAddress = emailList.at(0).emailAddress();
    }
    
    // Add to recipient:
    map.insert(emailSendToKey, QVariant::fromValue(emailAddress));
    QVariant mapAsVariant = QVariant::fromValue(map);
    
    mRequest = mApplicationManager->create("com.nokia.symbian.IMessage",
                                           "Send", "send(QVariant)", false);
    if (mRequest) {
        mRequest->setSynchronous(false);
        QList<QVariant> arguments;
        arguments.append(mapAsVariant);
        mRequest->setArguments(arguments);
        bool result = mRequest->send();
        if (!result) {
            qDebug() << "Sending request failed: " << mRequest->lastErrorMessage();
        }
    } else {
            qDebug() << "Creating service request failed";
    }
    
    close();
}
*/

/*!
    Opens contact card from phonebook to contact.
*/
void CommLauncherWidget::openPhonebook()
{
    // Deleting request cancels all pending requests
    if (mRequest) {
        delete mRequest;
        mRequest = NULL;
    }
    mRequest = mApplicationManager->create("com.nokia.services.phonebookservices",
                                           "Fetch", "open(int)", false);
    if (mRequest) {
        mRequest->setSynchronous(false);
        QList<QVariant> arguments;
        arguments.append(QVariant(mContact->localId()));
        mRequest->setArguments(arguments);
        bool result = mRequest->send();
        if (!result) {
            qDebug() << "Sending request failed: " << mRequest->lastErrorMessage();
        }
    } else {
        qDebug() << "Creating service request failed";
    }
    
    close();
}

/*!
    Opens phonebook to create a new contact.
*/
void CommLauncherWidget::openPhonebookCreateNew()
{
    // Deleting request cancels all pending requests
    if (mRequest) {
        delete mRequest;
        mRequest = NULL;
    }
    mRequest = mApplicationManager->create("com.nokia.services.phonebookservices",
                                           "Fetch", "editCreateNew(QString,QString)", false);
    if (mRequest) {
        QList<QVariant> arguments;
        QString type = QContactPhoneNumber::DefinitionName;
        arguments.append(QVariant( type ));
        arguments.append(QVariant( "" ));        
        mRequest->setArguments(arguments);
   
        bool result = mRequest->send();
        if (!result) {
            qDebug() << "Sending request failed: " << mRequest->lastErrorMessage();
        }
    
    } else {
        qDebug() << "Creating service request failed";
    }
}

/*!
    Handles call key events
*/
void CommLauncherWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Yes) {
        // Call key initializes a call
        makeCall();
        event->accept();
    } else {
        HbPopup::keyPressEvent(event);
    } 
}

/*!
    Counts the width for the launcher. 
    The width is not known before the launcher is drawn for the first time.
*/
int CommLauncherWidget::commLauncherWidth()
{
    int width = 0;    
    qreal verticalMargin = 0.0;
    style()->parameter("hb-param-margin-gene-middle-vertical", verticalMargin);
    qreal unit = HbDeviceProfile::current().unitValue(); 
    const int buttonSize = 9 * unit;
    
    width = mButtonCount * buttonSize + (mButtonCount + 1) * verticalMargin;
    
    return width;
}

