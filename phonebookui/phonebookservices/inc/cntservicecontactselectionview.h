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

#ifndef CNTSERVICECONTACTSELECTIONVIEW_H
#define CNTSERVICECONTACTSELECTIONVIEW_H

#include <QObject>

#include "cntbaseselectionview.h"
#include "cntviewparameters.h"

class CntServiceHandler;

class CntServiceContactSelectionView : public CntBaseSelectionView
{
    Q_OBJECT

public:
    CntServiceContactSelectionView(CntServiceHandler *aServiceHandler, CntViewManager *aViewManager, 
            QGraphicsItem *aParent = 0, HbAbstractItemView::SelectionMode newMode = HbAbstractItemView::NoSelection);
    ~CntServiceContactSelectionView();

public slots:
    void onListViewActivated(const QModelIndex &aIndex);
    void aboutToCloseView();

public:
    void activateView(const CntViewParameters &viewParameters);
    CntViewParameters::ViewId viewId() const { return CntViewParameters::serviceContactSelectionView; }

private:
    CntServiceHandler *mServiceHandler;
    QContactDetail     mDetail;
};

#endif /* CNTSERVICECONTACTSELECTIONVIEW_H */
