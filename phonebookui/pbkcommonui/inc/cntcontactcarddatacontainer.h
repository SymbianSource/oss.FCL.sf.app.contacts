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

class QStringList;
class CntCommands;

Q_DECLARE_METATYPE(QContactDetail)

class CntContactCardContainerData : public QSharedData
{
public:
    CntContactCardContainerData() { }
    ~CntContactCardContainerData() { }

public:
    QList<QVariantList> mDataList;
};

class CntContactCardDataContainer: public QObject
{
    Q_OBJECT    

public:
    enum CntCommLaucherDataId
    {
        action = 0,
        text = 1,
        valueText = 2,
        icon = 3,
        detail = 4
    };

public:
    CntContactCardDataContainer(QContact* contact, QObject *parent = 0);
    virtual ~CntContactCardDataContainer();

public:
    QVariant data(int index, int role = Qt::DisplayRole) const;
    int rowCount() const;

public slots:
    void preferredUpdated();

public:
    int separatorIndex() { return mSeparatorIndex; }

#ifdef PBK_UNIT_TEST
public:
#else
private:    
#endif  
    void initializeData();
    void initializeGroupData();
    void initializeDetailsData();
    QList<QContactDetail> actionDetails(const QString &actionName, const QContact &contact);
    bool supportsDetail(const QString &actionName, const QContactDetail &contactDetail);
    void addSeparator(int index);

        
#ifdef PBK_UNIT_TEST
public:
#else
private:    
#endif       
    QContact*                   mContact;
    QSharedDataPointer<CntContactCardContainerData>  mDataPointer;
    int                         mSeparatorIndex;
    CntCommands*                mCommands;
    CntStringMapper             mStringMapper;

};

#endif /* CNTCOMMLAUNCHERLISTMODEL_H_ */
