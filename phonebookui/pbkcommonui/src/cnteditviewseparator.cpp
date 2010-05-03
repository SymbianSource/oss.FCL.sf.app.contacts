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
#include "cnteditviewlistmodel.h"
#include <hbnamespace.h>

CntEditViewSeparator::CntEditViewSeparator()
{   
}

CntEditViewSeparator::~CntEditViewSeparator()
{
}
    
QVariant CntEditViewSeparator::data(int role) const
{
    switch ( role )
    {
    case Hb::ItemTypeRole:
        return QVariant( Hb::SeparatorItem );
    case Qt::DisplayRole:
        return QVariant(qtTrId("Details"));
    default:
        return QVariant();
    }
}
void CntEditViewSeparator::activated()
{
}

void CntEditViewSeparator::longPressed(const QPointF &coords)
{
    Q_UNUSED( coords );
}
