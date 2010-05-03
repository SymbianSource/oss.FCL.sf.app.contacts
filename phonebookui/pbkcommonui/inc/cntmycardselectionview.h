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

#ifndef CNTMYCARDSELECTIONVIEW_H
#define CNTMYCARDSELECTIONVIEW_H

#include "cntbaseselectionview.h"

class QModelIndex;

class CntMyCardSelectionView : public CntBaseSelectionView
{
    Q_OBJECT
public:
    CntMyCardSelectionView();
    ~CntMyCardSelectionView();

public:
    int viewId() const { return myCardSelectionView; }
    
private slots:
    void onListViewActivated(const QModelIndex& index);
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void removeFromGroup(const QContact* contact);
};

#endif /* CNTMYCARDSELECTIONVIEW_H */
