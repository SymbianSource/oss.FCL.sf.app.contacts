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
 * Description:  Phonebook 2 merge conflict.
 *
 */

#ifndef CPBK2MERGECONFLICT_H_
#define CPBK2MERGECONFLICT_H_

#include <e32base.h>
#include <MVPbkStoreContactField.h>
#include <Pbk2FieldProperty.hrh>
#include <MPbk2MergeConflict.h>

// FORWARD DECLARATIONS
class CPbk2PresentationContact;

// CLASS DECLARATION

/**
 * Phonebook 2 merge conflict.
 */
class CPbk2MergeConflict : public CBase,
                            public MPbk2MergeConflict
    {
public: // Construction and destruction
    /**
     * Creates a new instance of this class.
     *
     * @return  A new instance of this class.
     */
    static CPbk2MergeConflict* NewL();
    
    /**
     * Destructor.
     */
    ~CPbk2MergeConflict();
    
private: // Construction
    CPbk2MergeConflict();
    
public: // Implementation
    /**
     * Adds fields into conflict.
     *
     * @param aFieldFirst     First field.
     * @param aFieldSecond    Second field.
     */
    void AddFields(  
            const MVPbkStoreContactField& aFieldFirst, 
            const MVPbkStoreContactField& aFieldSecond );

    /**
     * Adds display fields into conflict. For special fields, normally not needed.
     * If not provided normal field values used for display. Takes ownership.
     *
     * @param aDisplayFieldFirst     First display field.
     * @param aDisplayFieldSecond    Second display field.
     */
    void AddDisplayFields(  
            HBufC* aDisplayFieldFirst, 
            HBufC* aDisplayFieldSecond );
    
    /**
     * Gets fields from conflict.
     *
     * @param aFieldFirst     First field.
     * @param aFieldSecond    Second field.
     */
    void GetFieldsL(  
            const MVPbkStoreContactField*& aFieldFirst, 
            const MVPbkStoreContactField*& aFieldSecond );
    
public: // CPbk2MergeConflictBase
    EPbk2ConflictType GetConflictType();
    HBufC* GetLabelLC();
    HBufC* GetTextLC( EPbk2ConflictedNumber aNumber );
    void ResolveConflict( EPbk2ConflictedNumber aNumber );
    void GetChosenFieldsL( RPointerArray<MVPbkStoreContactField>& aAddressFields );

private: // methods
    void CustomizeTextValueL( const MVPbkStoreContactField& aField, TDes& aBuf );
    HBufC* GetContentTextLC( 
            EPbk2ConflictedNumber aNumber, 
            const MVPbkStoreContactField* aField );
    
private: //Data
    // Not own: First field
    const MVPbkStoreContactField* iFieldFirst;
    // Not own: Second field
    const MVPbkStoreContactField* iFieldSecond;
    //own: optional display value 
    HBufC* iDisplayFieldFirst;
    //own: optional display value 
    HBufC* iDisplayFieldSecond;    
    // Not own: Chosen field
    const MVPbkStoreContactField* iChosenField;
    // Own: Conflict type
    EPbk2ConflictType iConflictType;
    };

/**
 * Phonebook 2 merge addresses conflict.
 */
class CPbk2MergeConflictAddress : public CBase,
                            public MPbk2MergeConflict
    {
public: // Construction and destruction
    /**
     * Creates a new instance of this class.
     *
     * @return  A new instance of this class.
     */
    static CPbk2MergeConflictAddress* NewL();
    
private: // Construction
    CPbk2MergeConflictAddress();
    
public: // Implementation
    /**
     * Adds addresses into conflict.
     *
     * @param aContactFirst     First contact.
     * @param aContactSecond    Second contact.
     * @param aAddressGroup     Address group.
     */
    void AddAddress( 
            CPbk2PresentationContact& aContactFirst, 
            CPbk2PresentationContact& aContactSecond, 
            TPbk2FieldGroupId aAddressGroup );
    
public: // CPbk2MergeConflictBase
    EPbk2ConflictType GetConflictType();
    HBufC* GetLabelLC();
    HBufC* GetTextLC( EPbk2ConflictedNumber aNumber );
    void ResolveConflict( EPbk2ConflictedNumber aNumber );
    void GetChosenFieldsL( RPointerArray<MVPbkStoreContactField>& aAddressFields );
    
private: //Data
    // Own: Address group
    TPbk2FieldGroupId iAddressGroup;
    // Not Own: First contact
    CPbk2PresentationContact* iContactFirst;
    // Not Own: Second contact
    CPbk2PresentationContact* iContactSecond;
    // Not own: Contact who owns chosen address
    const CPbk2PresentationContact* iContactChosenAddress;
    // Own: Conflict type
    EPbk2ConflictType iConflictType;
    };


#endif /* CPBK2MERGECONFLICT_H_ */
