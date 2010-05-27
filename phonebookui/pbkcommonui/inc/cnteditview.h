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

#ifndef CNTEDITVIEW__H
#define CNTEDITVIEW__H

#include <QObject>
#include <cntabstractview.h>
#include "qtpbkglobal.h"

class CntEditViewPrivate;
class CntAbstractViewManager;
class HbView;

class QTPBK_EXPORT CntEditView : public QObject, public CntAbstractView
{ 
    Q_OBJECT

public:
    CntEditView();
    ~CntEditView();
    
signals:
    void contactUpdated(bool aSuccess);
    void contactRemoved(bool aSuccess);
    void changesDiscarded();
    
    
public:
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const;
    HbView* view() const;
    int viewId() const;
    
private:
    CntEditViewPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, CntEditView)
    Q_DISABLE_COPY(CntEditView)   
};
#endif //CNTEDITVIEW__H
// EOF