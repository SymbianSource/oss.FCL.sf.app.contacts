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
 * Description:  Phonebook 2 merge conflict interface.
 *
 */

#ifndef MPBK2MERGECONFLICT_H_
#define MPBK2MERGECONFLICT_H_

// FORWARD DECLARATIONS
class MVPbkStoreContactField;

// ENUM DECLARATION
enum EPbk2ConflictType
    {
    EPbk2ConflictTypeNormal,
    EPbk2ConflictTypeAddress,
    EPbk2ConflictTypeImage
    };

enum EPbk2ConflictedNumber
    {
    EPbk2ConflictedFirst,
    EPbk2ConflictedSecond
    };

// CLASS DECLARATION
/**
 * Phonebook 2 merge conflict base class.
 */
class MPbk2MergeConflict
    {
public: // Implementation
    /**
     * Destructor.
     */
    virtual ~MPbk2MergeConflict(){};
    
    /**
     * Gets conflict type.
     *
     * @return  Type of conflict.
     */
    virtual EPbk2ConflictType GetConflictType() = 0;
    
    /**
     * Gets label.
     *
     * @return  Label.
     */
    virtual HBufC* GetLabelLC() = 0;
    
    /**
     * Gets text of conficted field or address.
     *
     * @param aNumber     First or Second.
     * @return  Conflicted text.
     */
    virtual HBufC* GetTextLC( EPbk2ConflictedNumber aNumber ) = 0;
    
    /**
     * Resolves conflict.
     *
     * @param aNumber     First or Second.
     */
    virtual void ResolveConflict( EPbk2ConflictedNumber aNumber ) = 0;
    
    /**
     * Gets chosen fields.
     *
     * @param aAddressFields     Array of chosen address fields, 
     *                           contact owns these fields, they can't be deleted.
     */
    virtual void GetChosenFieldsL( RPointerArray<MVPbkStoreContactField>& aAddressFields ) = 0;
    
    };
#endif /* MPBK2MERGECONFLICT_H_ */
