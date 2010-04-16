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

#ifndef CNTACTIONMENUBUILDER_H_
#define CNTACTIONMENUBUILDER_H_

#include <QObject>
#include <hbmenu.h>
#include <qtcontactsglobal.h>

class CntStringMapper;
QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntActionMenuBuilder : public QObject
    {
    Q_OBJECT
public:
    CntActionMenuBuilder( QContactLocalId aMyCardId );
    ~CntActionMenuBuilder();
    
public:
    void execActionMenu( QContact& aContact, QPointF aPoint );
    
public:
    HbMenu* buildActionMenu( QContact& aContact );
    
signals:
    void openContact( QContact& aContact );
    void editContact( QContact& aContact );
    void deleteContact( QContact& aContact );
    void performContactAction( QContact& aContact, QString aAction );
    
private slots:
    void emitOpenContact();
    void emitEditContact();
    void emitDeleteContact();
    void emitCallContact();
    void emitSmsContact();
    void emitMailContact();
    
private:
    void createCallAction( HbMenu& aMenu, QContact& aContact );
    void createSmsAction( HbMenu& aMenu, QContact& aContact );
    void createEmailAction( HbMenu& aMenu, QContact& aContact );
private:
    QContactLocalId iMyCardId;
    QContact* mContact;
    CntStringMapper* mMap;
    };
#endif /* CNTACTIONMENUBUILDER_H_ */