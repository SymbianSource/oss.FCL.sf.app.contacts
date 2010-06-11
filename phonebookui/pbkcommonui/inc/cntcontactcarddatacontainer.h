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

#ifndef CNTCOMMLAUNCHERLISTMODEL_H_
#define CNTCOMMLAUNCHERLISTMODEL_H_

#include <QSharedData>
#include <qtcontacts.h>

#include "cntstringmapper.h"

class CntContactCardDataItem;

class CntContactCardDataContainer: public QObject
{
    Q_OBJECT    

public:
    CntContactCardDataContainer(QContact* contact, QObject *parent = 0, bool myCard = false);
    virtual ~CntContactCardDataContainer();

public:
    CntContactCardDataItem* dataItem(int index) const;
    int itemCount() const;
    
#ifdef PBK_UNIT_TEST
public:
#else
private:    
#endif  
    void initializeActionsData(bool myCard);
    void initializeGroupData();
    void initializeDetailsData();
    QList<QContactDetail> actionDetails(const QString &actionName, const QContact &contact);
    bool supportsDetail(const QString &actionName, const QContactDetail &contactDetail);
    void addSeparator(int index);
    void sortDataItems();
    int getPosition(const QString& aId, const QString& aContext, bool dynamicAction = false);
    
#ifdef PBK_UNIT_TEST
public:
#else
private:    
#endif       
    QContact*                       mContact;
    QList<CntContactCardDataItem*>  mDataItemList;
    int                             mSeparatorIndex;
    CntStringMapper                 mStringMapper;
    bool                            mLocationFeatureEnabled;

};

#endif /* CNTCOMMLAUNCHERLISTMODEL_H_ */
