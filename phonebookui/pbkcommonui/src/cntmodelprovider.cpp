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

#include "cntmodelprovider.h"

#include <qtcontacts.h>
#include <qthreadstorage.h>

// TLS Object definiton
QThreadStorage<CntModelProvider*> tls;


/*!
    \return pointer to the CntModelProvider object. If it is first call then it constructs itself, 
    on second instance call it just returns pointer stored in TLS.
*/
CntModelProvider* CntModelProvider::instance()
{
    CntModelProvider* self=0;


    //Check Thread local storage
    if (!tls.hasLocalData())
    {
        // create new provider
        self = new CntModelProvider;
        // and store it to TLS
        tls.setLocalData(self);
    }
    else
    {
        // otherwise get data from TLS
        self = tls.localData();
    }
    // increase reference count i.e. how many instances there are
    ++self->mRefCount;
    // return to pointer to "this" object
    return self;
}

/*!
    Releases "handle" to the class. Decreases reference count and if this is the last client for the class it will delete itself.
*/
void CntModelProvider::release()
{
    // decrese reference count and delete the object if we are last one
    --mRefCount;
    if (mRefCount == 0) 
    {
        tls.setLocalData(0); // this will invoke "delete this"
    }
}

/*!
    Destructor
*/
CntModelProvider::~CntModelProvider()
{
    mRefCount=0;
    delete mContactModel;
    mContactModel=0;
    delete mContactManager;
    mContactManager=0;
    delete mContactSimManager;
    mContactSimManager=0;
}

/*!
    Constructor. Initialize reference count and contactmodel pointer.
*/
CntModelProvider::CntModelProvider():
    mRefCount(0),
    mContactModel(0),
    mContactManager(0),
    mContactSimManager(0)
{

}

/*!
    \return pointer to the contact model.
*/
MobCntModel *CntModelProvider::contactModel()
{
    // if no model exists, create a new one with default first name last name sort order
    if (!mContactModel)
    {
        QList<QContactSortOrder> sortOrders;
        QContactSortOrder sortOrderFirstName;
        sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
        sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);
        sortOrders.append(sortOrderFirstName);

        QContactSortOrder sortOrderLastName;
        sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLast);
        sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);
        sortOrders.append(sortOrderLastName);

        mContactModel=new MobCntModel(contactManager(), QContactFilter(), sortOrders);
        //create also a manager for SIM contacts
        if (!mContactSimManager)
        {
            mContactSimManager=new QContactManager("symbiansim");
        }
    }
    return mContactModel;
}

/*!
    \return pointer to the contact manager.
*/
QContactManager *CntModelProvider::contactManager()
{
    // if no model exists, create a new one
    if (!mContactManager)
    {
        mContactManager=new QContactManager("symbian");
    }
    return mContactManager;
}

/*!
    \return pointer to the SIM contacts manager.
*/
QContactManager *CntModelProvider::contactSimManager()
{
    // if no model exists, create a new one
    if (!mContactSimManager)
    {
        mContactSimManager=new QContactManager("symbiansim");
    }
    return mContactSimManager;
}

/*!
    \return number of handles to this object
*/
int CntModelProvider::referenceCount()
{
    return mRefCount;
}


