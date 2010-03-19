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

#ifndef MOBCNTACTIONFACTORY_H
#define MOBCNTACTIONFACTORY_H

#include <qcontactactionfactory.h>
#include <qcontactaction.h>

class MobCntAction;

QTM_USE_NAMESPACE

class Q_DECL_EXPORT MobCntActionFactory : public QContactActionFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactActionFactory)

public:
    MobCntActionFactory();
    ~MobCntActionFactory();

    QString name() const;
    QList<QContactActionDescriptor> actionDescriptors() const;
    QContactAction* instance(const QContactActionDescriptor& descriptor) const;
    QVariantMap actionMetadata(const QContactActionDescriptor& descriptor) const;

private:
    QList<MobCntAction *> actionList;
};

#endif
