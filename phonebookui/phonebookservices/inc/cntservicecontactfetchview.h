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

#ifndef CNTSERVICECONTACTFETCHVIEW_H
#define CNTSERVICECONTACTFETCHVIEW_H

#include <QObject>

#include "cntbaseselectionview.h"
#include "cntviewparameters.h"

class CntServiceHandler;

class CntServiceContactFetchView : public CntBaseSelectionView
{
    Q_OBJECT

public:
    CntServiceContactFetchView(CntServiceHandler *aServiceHandler, CntViewManager *aViewManager, QGraphicsItem *aParent = 0);
    ~CntServiceContactFetchView();

public slots:
    void aboutToCloseView();
    void cancelFetch();

public:
    void setActionFilter(QString action, QString filter);
    void addMenuItems();
    CntViewParameters::ViewId viewId() const { return CntViewParameters::serviceContactFetchView; }
    void activateView(const CntViewParameters &viewParameters);

private:
    CntServiceHandler *mServiceHandler;
};

#endif /* CNTSERVICECONTACTFETCHVIEW_H */
