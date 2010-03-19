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

#ifndef CNTSERVICECONTACTCARDVIEW_H
#define CNTSERVICECONTACTCARDVIEW_H

#include <QObject>
#include "cntcontactcardview.h"
#include "cntviewparameters.h"

class CntServiceHandler;

class CntServiceContactCardView : public CntContactCardView
{
    Q_OBJECT

public slots:

    void aboutToCloseView();
    void editContact();

public:
    CntServiceContactCardView(CntServiceHandler *aServiceHandler, CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntServiceContactCardView();

    CntViewParameters::ViewId viewId() const { return CntViewParameters::serviceContactCardView; }
    
private:
    CntServiceHandler *mServiceHandler;
   
};

#endif // CNTSERVICECONTACTCARDVIEW_H

// EOF
