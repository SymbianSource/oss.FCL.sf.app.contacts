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
* Description:  A find operation that uses word parser function from client
*
*/


#ifndef C_VPBKSIMSTORE_CFINDWITHPARSEROPERATION_H
#define C_VPBKSIMSTORE_CFINDWITHPARSEROPERATION_H

#include <e32base.h>
#include <badesca.h>

// Virtual Phonebook
#include <MVPbkContactOperation.h>
#include <VPbkSimCntFieldTypes.hrh>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;
class MVPbkContactFindFromStoresObserver;
class CVPbkContactLinkArray;
class MVPbkContactFindPolicy;
class MVPbkSimCntStore;
class CVPbkSimContactBuf;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore; 

/**
 *  A find operation that uses word parser function from client
 *
 */
NONSHARABLE_CLASS(CFindWithParserOperation) 
    :   public CBase,
        public MVPbkContactOperation
    {
public:
    
    /**
     * @param aStore the sim store
     * @param aSearchStrings strings for find
     * @param aFieldTypes types of the fields to find from
     * @param aObserver the observer of the operation
     * @param aWordParserCallBack function pointer to clients parser.
     *
     * @return a new instance of this class
     */
    static CFindWithParserOperation* NewL( CContactStore& aStore,
        const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack );
        
    virtual ~CFindWithParserOperation();

// from base class MVPbkContactOperation
    void StartL();
    void Cancel();
    
private:

    CFindWithParserOperation( CContactStore& aStore,
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack );
    void ConstructL( const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes );
    static TInt LoopContacts( TAny* aThis );
    TInt LoopContactsL();
    TBool IsCurrentContactMatchL();
    void HandleError( TInt aError );

private: // data

    /// Ref: the sim store
    CContactStore& iStore;
    /// Ref: the native sim store
    MVPbkSimCntStore& iSimStore;
    /// Ref: the observer of the operation
    MVPbkContactFindFromStoresObserver& iObserver;
    /// Ref: a pointer to the clients word parser function
    TCallBack iWordParserCallBack;
    /// Own: search strings
    CDesCArrayFlat* iSearchStrings;
    /// Own: types of the fields to search from
    RArray<TVPbkSimCntFieldType> iSimFieldTypes;
    /// The total amount of SIM slots
    TInt iTotalSimSlots;
    /// The current SIM contact index
    TInt iCurSimIndex;
    /// Own: and idle for making operation asynchronous
    CIdle* iIdle;
    /// Own: results of the find
    CVPbkContactLinkArray* iResults;
    /// Own: reuse one contactbuf
    CVPbkSimContactBuf* iCurSimContact;
    /// Own: an array for client to parse words
    CDesCArrayFlat* iParsedWords;
    /// Own: the find policy
    MVPbkContactFindPolicy* iFindPolicy;
    };
} // namespace VPbkSimStore

#endif // C_VPBKSIMSTORE_CFINDWITHPARSEROPERATION_H
