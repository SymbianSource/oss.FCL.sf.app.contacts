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

#ifndef CNTSERVICEVIEWMANAGER_H
#define CNTSERVICEVIEWMANAGER_H

#include "cntviewparameters.h"
#include "cntdefaultviewmanager.h"

#include <QObject>

class CntBaseView;
class CntServiceHandler;

class CntServiceViewManager : public CntDefaultViewManager
{
    Q_OBJECT

public:
    CntServiceViewManager(CntMainWindow *mainWindow, CntViewParameters::ViewId defaultView, 
            CntServiceHandler *aHandler);
    ~CntServiceViewManager();
    
public slots:
    void launchFetch(const QString &title, const QString &action, const QString &filter);
    void launchEditor(QContact contact);
    void launchContactSelection(QContactDetail detail);
    void launchContactCard(QContact contact);
    void launchAssignContactCard(QContact contact, QContactDetail detail);

private:
    CntBaseView *getView(const CntViewParameters &aArgs);
    
private:
    CntServiceHandler *mServiceHandler;
};

#endif // CNTSERVICEVIEWMANAGER_H
