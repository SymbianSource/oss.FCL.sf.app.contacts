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

#include "cntcontactcardview.h"
#include "cntcontactcardview_p.h"

#include <hbview.h>

CntContactCardView::CntContactCardView(bool isTemporary) : d_ptr(new CntContactCardViewPrivate(isTemporary))
{
    Q_D(CntContactCardView);
    connect(d_ptr, SIGNAL(backPressed()), this, SIGNAL(backPressed()));
    connect(d_ptr, SIGNAL(viewActivated(CntAbstractViewManager*, const CntViewParameters)), 
            this, SIGNAL(viewActivated(CntAbstractViewManager*, const CntViewParameters)));
    connect(d_ptr, SIGNAL(addToContacts()), this, SIGNAL(addToContacts()));
}

/*!
 Destructor
 */
CntContactCardView::~CntContactCardView()
{
    Q_D(CntContactCardView);
    delete d;
}

void CntContactCardView::activate(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{
    Q_D(CntContactCardView);
    d->activate( aMgr, aArgs );
}

void CntContactCardView::deactivate()
{
    Q_D(CntContactCardView);
    d->deactivate();
}

HbView* CntContactCardView::view() const
{
    Q_D(const CntContactCardView);
    return d->mView;
}

bool CntContactCardView::isDefault() const 
{ 
    return false; 
}

int CntContactCardView::viewId() const 
{ 
    return contactCardView; 
}

// end of file
