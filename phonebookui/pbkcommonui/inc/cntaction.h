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

#ifndef CNTACTION_H_
#define CNTACTION_H_

#include <QObject>
#include <qtcontactsglobal.h>
#include <qtcontacts.h>

QTM_USE_NAMESPACE

class CntAction : public QObject
    {
    Q_OBJECT
    
public:
    CntAction( QString aAction );
    ~CntAction();
    
public:
    void execute( QContact aContact, QContactDetail aDetail );
    
signals:
    void actionExecuted( CntAction* aAction );
    
private slots:
    void progress( QContactAction::State status, const QVariantMap& result );
    
private:
    QString mAction;
    QContactAction* mContactAction;
    };
#endif /* CNTACTION_H_ */