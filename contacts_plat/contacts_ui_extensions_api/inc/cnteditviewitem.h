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

#ifndef CNTEDITVIEWITEM_H
#define CNTEDITVIEWITEM_H

#include <QObject>
#include <QVariant>
#include <QPointF>
#include <hbnamespace.h>

// Value enum for EditViewItemRole::ERoleItemType
enum EditViewItemType
{
    ETypeUiExtension = 0,
    ETypeDetailItem
};

enum EditViewItemRole
{
    ERoleItemType = Hb::UserRole,
    ERoleEditorViewId,
    ERoleContactDetailFields,
    ERoleContactDetail,
    ERoleContactDetailDef
};

/**
 * Handle to specific item's data.
 */
class CntEditViewItem
{
public:

    virtual ~CntEditViewItem() {}

    /**
     * Returns data for a given role.
     * ETypeUiExtension must be returned for ERoleItemType
     *
     * @param int, role
     * @return QVariant, data for the given role
     */   
    virtual QVariant data(int role) const = 0;

    /**
     * Called when this item is activated (tapped) in the list.
     */   
    virtual void activated() = 0;

    /**
     * Called when this item is longpressed in the list.
     *
     * @param QPointF&, coordinates from where the longpress was initiated
     */   
    virtual void longPressed(const QPointF& coords) = 0;
};

#endif // CNTEDITVIEWITEM_H

