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

#include "cnteditview.h"
#include "cnteditview_p.h"
#include "cntabstractviewmanager.h"

#include <hbview.h>
#include "cntglobal.h"
#include "cntdetailpopup.h"
#include "cnteditviewdetailitem.h"
#include "cnteditviewheadingitem.h"

#include <QGraphicsItem>
#include <QGraphicsLinearLayout>
#include <hbscrollarea.h>
#include <hbgroupbox.h>
#include <hbpushbutton.h>
#include <hbmenu.h>
#include <thumbnailmanager_qt.h>

CntEditView::CntEditView() : d_ptr(new CntEditViewPrivate())
{
    Q_D( CntEditView );
    d->q_ptr = this;
}

CntEditView::~CntEditView()
{
    Q_D(CntEditView);
    delete d;
}
    
void CntEditView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    Q_D(CntEditView);
    d->activate(aMgr,aArgs);
}

void CntEditView::deactivate()
{
    Q_D(CntEditView);
    d->deactivate();
}

bool CntEditView::isDefault() const
{
    return false;
}

HbView* CntEditView::view() const
{
    Q_D( const CntEditView );
    return d->mView;
}

int CntEditView::viewId() const
{
    return editView;
} 
// EOF
