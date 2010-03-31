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
#ifndef T_CCAPPCONTACTFETCHER_H
#define T_CCAPPCONTACTFETCHER_H

#include <e32std.h>
#include <e32base.h>

class CCmsContactFieldInfo;
class CCmsContactField;
class MCCAppContactFieldDataObserver;
class CCCAParameter;

enum TCmsVoIPSupport
    {
    ECmsVoIPSupportBasic    = 1,
    ECmsVoIPSupportXspId    = 2,
    ECmsVoIPSupportCallout  = 4,
    ECmsVoIPSupportSip      = 8
    };

enum TCmsContactStore
    {    
    ECmsContactStorePbk = 0,
    ECmsContactStoreSim,
    ECmsContactStoreSdn
    };
    
enum TCmsContactIdentifierType
    {    
    ECmsPackedContactLinkArray = 0
    //ECmsPackedContactLink,
    //ECmsContactId
    };

// -----------------------------------------------------------------------------
// CCCAppCmsContactFetcherWrapper
// dummy class
// -----------------------------------------------------------------------------
class CCCAppCmsContactFetcherWrapper : public CBase
    {
public:
    
    enum TOperationState
        {
        EInitial = 0,
        EOpeningContact = 1,
        ELoadingEnabledFields = 2,
        ELoadingFields = 4,
        EOperationsCompleted = 8
        };

    CCCAppCmsContactFetcherWrapper( );

    static CCCAppCmsContactFetcherWrapper* InstanceL( CCCAParameter* aParameter );
    static CCCAppCmsContactFetcherWrapper* InstanceL(  );
        
    void Release();
       
    //static CCCAppCmsContactFetcherWrapper* HandleL();

    const CCmsContactFieldInfo* ContactInfo();

    RPointerArray<CCmsContactField>& ContactFieldDataArray();

    static HBufC8* ContactIdentifierLC( const TCmsContactIdentifierType aIdType = ECmsPackedContactLinkArray );

    void AddObserverL( MCCAppContactFieldDataObserver& aObserver );

    void RemoveObserver( MCCAppContactFieldDataObserver& aObserver );  

    TBool IsServiceAvailable( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );
    
    TCmsContactStore ContactStore() const;
    
    void DeleteContactL() {};
    
    
    ~CCCAppCmsContactFetcherWrapper();
    void ConstructL();
    
    TInt GetContactActionFieldCount(
            VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction);
    
    virtual void RefetchContactL();
    TBool CCCAppCmsContactFetcherWrapper::IsTopContact();
        
    RPointerArray<MCCAppContactFieldDataObserver> iObservers;
    CCmsContactFieldInfo* iContactFieldInfo;
    RPointerArray<CCmsContactField> iFieldArray;
    TBool iContactStore_called;
    TBool iTopContact;
    };    
    
    
#endif // T_CCAPPCONTACTFETCHER_H
