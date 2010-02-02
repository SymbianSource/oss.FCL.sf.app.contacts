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
 * Description:  Phonebook 2 merge contacts resolver.
 *
 */

#ifndef CPBK2MERGERESOLVER_H_
#define CPBK2MERGERESOLVER_H_

// INCLUDES
#include <e32base.h>
#include <MVPbkStoreContactField.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkFieldType.h>
#include <Pbk2FieldProperty.hrh>
#include <MPbk2MergeResolver.h>
#include "CPbk2MergeConflict.h"

// FORWARD DECLARATIONS
class CPbk2PresentationContact;


// CLASS DECLARATION
/**
 * Phonebook 2 merge contacts resolver.
 */
class CPbk2MergeResolver : public CBase, 
                           public MPbk2MergeResolver
    {
public: // Construction and destruction

    /**
     * Creates a new instance of this class.
     *
     * @param aFirstContact     First contact to merge.
     * @param aSecondContact    Second contact to merge.
     * 
     * @return  A new instance of this class.
     */
    static CPbk2MergeResolver* NewL( 
            CPbk2PresentationContact* aFirstContact, 
            CPbk2PresentationContact* aSecondContact );

    /**
     * Destructor.
     */
    ~CPbk2MergeResolver();
    
public: // Implementation
    /**
     * Starts merging.
     */
    void MergeL();
    
    /**
     * Counts conflicts.
     *
     * @return  Number of conflicts.
     */
    TInt CountConflicts();
    
    /**
     * Gets conflict.
     *
     * @param aConflict    Index of conflict in array.
     * @return  Base class of conflict.
     */
    MPbk2MergeConflict& GetConflictAt( TInt aConflict );
        
    /**
     * Counts merged.
     *
     * @return  Number of merged.
     */
    TInt CountMerged();
    
    /**
     * Gets merged.
     *
     * @param aMerged    Index of merged in array.
     * @return  Base class of conflict.
     */
    MVPbkStoreContactField& GetMergedAt( TInt aMerged );
    
private: // Construction
    CPbk2MergeResolver( 
            CPbk2PresentationContact* aFirstContact, 
            CPbk2PresentationContact* aSecondContact );
    
private: // Declaration
    enum ECompareAddress
        {
        ECAUndefined,
        ECASame,
        ECAFirstIncludesSecond,
        ECASecondIncludesFirst,
        ECADifferent
        };
  
private: // Implementation
    TBool EqualsValues( 
            const MVPbkStoreContactField& aFieldFirst,
            const MVPbkStoreContactField& aFieldSecond );
    ECompareAddress CompareAddresses( TPbk2FieldGroupId aGroupId );
    CPbk2MergeResolver::ECompareAddress DoCompareAddresses( 
            TInt aCountEquals, 
            TInt aCountAddressFieldsFirst,
            TInt aCountAddressFieldsSecond );
    TBool AreFieldsTypeSame( 
            const MVPbkStoreContactField& aFieldFirst,
            const MVPbkStoreContactField& aFieldSecond );
    TBool AreAddressesSubTypeSame( 
            const MVPbkStoreContactField& aFieldFirst,
            const MVPbkStoreContactField& aFieldSecond );
    TBool AddressField( const MVPbkStoreContactField& aField );
    void CopyAddressToMergedL( 
            const CPbk2PresentationContact& aContact, 
            TPbk2FieldGroupId aAddressGroup );
    TPbk2FieldMultiplicity FieldMultiplicity( const MVPbkStoreContactField& aField );
    void AddConflictL( 
            const MVPbkStoreContactField& aFieldFirst,
            const MVPbkStoreContactField& aFieldSecond );
    void AddAddressConflictL( TPbk2FieldGroupId aGroupId );
    TInt FindConflictByFieldL( const MVPbkStoreContactField& aField );
    void PrintFieldL(const MVPbkStoreContactField & aField);
    void MergeAddressesL();
    void CompleteMergeArrL(
            RArray<TInt>& aDuplicatesInFirst, 
            RArray<TInt>& aDuplicatesInSecond );
    TBool IsTopContactField( 
            const MVPbkStoreContactField& aField );
    TInt CompareTopConactFields( 
                const MVPbkStoreContactField& aFieldFirst,
                const MVPbkStoreContactField& aFieldSecond );
    
private: // Data
    // Own: Array merged contact fields
    RPointerArray<MVPbkStoreContactField> iMerged; 
    
    // Own: Array conflicted contact fields
    RPointerArray<MPbk2MergeConflict> iConflicts;
    
    // Own: First contact  
    CPbk2PresentationContact* iFirstContact;
    
    // Own: Second contact  
    CPbk2PresentationContact* iSecondContact;
    };

#endif /* CPBK2MERGERESOLVER_H_ */
