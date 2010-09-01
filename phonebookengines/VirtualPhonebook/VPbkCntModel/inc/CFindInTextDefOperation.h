/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A find operation that uses 
*                CContactDataBase::FindInTextDefAsyncL
*
*/


#ifndef C_CFINDINTEXTDEFOPERATION_H
#define C_CFINDINTEXTDEFOPERATION_H

#include <e32base.h>
#include <cntdb.h>
#include <MVPbkContactOperation.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;
class MVPbkContactFindFromStoresObserver;
class CVPbkContactLinkArray;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;

/**
 *  A find operation that uses CContactDataBase::FindInTextDefAsyncL
 *
 */
NONSHARABLE_CLASS(CFindInTextDefOperation) 
    :   public CBase,
        public MVPbkContactOperation,
        private MIdleFindObserver
    {
public:
    static CFindInTextDefOperation* NewL( 
        CContactStore& aContactStore,
        const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack );
    virtual ~CFindInTextDefOperation();

// from base class MVPbkContactOperation
    void StartL();
    void Cancel();

private:
// from base class MIdleFindObserver
    void IdleFindCallback();
    
private:

    CFindInTextDefOperation( CContactStore& aContactStore,
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack );
    void ConstructL( const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes );
    static TInt ConvertResults( TAny* aThis );
    TInt ConvertResultsL();
    void HandleError( TInt aError );

private: // data
    /// Ref: the store implementation
    CContactStore& iStore;
    /// Ref: the observer of the operation
    MVPbkContactFindFromStoresObserver& iObserver;
    /// Ref: the word parser from the client
    TCallBack iWordParserCallBack;
    /// Own: the search strings
    CDesCArrayFlat* iSearchStrings;
    /// Own: defines the field types that used for searching
    CContactTextDef* iTextDefs;
    /// Own: a find operation from contact model
    CIdleFinder* iAsyncFinder;
    /// Own: matched contact ids
    CContactIdArray* iContactIds;
    /// Own: an idle for asynchronous find completion
    CIdle* iIdle;
    /// Own: an arrau of contact find results
    CVPbkContactLinkArray* iResults;
    };
} // VPbkCntModel

#endif // C_CFINDINTEXTDEFOPERATION_H
