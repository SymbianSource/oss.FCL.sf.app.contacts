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

#ifndef CNTPRESENCELISTENER_H
#define CNTPRESENCELISTENER_H

#include <QObject>
#include <qtcontacts.h>
#include <QMap>

class PrcPresenceReader;
class PrcPresenceBuddyInfoQt;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntPresenceListener : public QObject
{
    friend class TestCntPresenceListener;
    
    Q_OBJECT
    
public:
    CntPresenceListener(const QContact& contact, QObject* parent = NULL);
    ~CntPresenceListener();
    
    QMap<QString, bool> initialPresences(bool &combinedOnlineStatus);
    
private slots:
    void handlePresenceUpdate(bool aSuccess, PrcPresenceBuddyInfoQt* aPresenceBuddyInfo);
    
private:
    bool parsePresence(QList<PrcPresenceBuddyInfoQt*> buddyList);
    
signals:
    void accountPresenceUpdated(QString accountUri, bool online);
    void fullPresenceUpdated(bool online);
    
private:
    PrcPresenceReader*  mReader; // own
    QStringList         mAccountList;
    
    const QContact&     mContact;
    
};

#endif // CNTPRESENCELISTENER_H
