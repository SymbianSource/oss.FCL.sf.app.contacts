/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 field selector.
*
*/


#ifndef CPBK2FIELDSELECTOR_H
#define CPBK2FIELDSELECTOR_H

// INCLUDES
#include <MVPbkContactFieldSelector.h>
#include <CVPbkFieldTypeSelector.h>

// FORWARD DECLARATIONS
class MVPbkBaseContactField;
class MVPbkFieldTypeList;

// CLASS DECLARTIONS

/**
 * Class for managing multiple selectors. Selectors can be included or 
 * excluded.
 */
NONSHARABLE_CLASS( CPbk2FieldSelector )
:       public CBase, public MVPbkContactFieldSelector
    {
    public: // C'tor and d'tor
        /** 
         * Creates instance of this class.
         * New instance is left to the CleanupStack.
         */
        static CPbk2FieldSelector* NewLC();
                
        /** 
         * Destructor
         */
        ~CPbk2FieldSelector();
        
    public: // Interface
        /** 
         * Adds new field type selector to this field selector.
         * These fields are included to this selector.
         * @param aIncludedSelector A selector to be included to this selector.
         */    
        void AddIncludedSelector( 
                CVPbkFieldTypeSelector* aIncludedSelector );
        
        /** 
         * Adds new field type selector to this field selector.
         * These fields are excluded from this selector.
         * @param aIncludedSelector 
         *          A selector to be excluded from this selector.
         */    
        void AddExcludedSelector( 
                CVPbkFieldTypeSelector* aExcludedSelector );
        
    public: // From MVPbkContactFieldSelector
        TBool IsFieldIncluded(
                const MVPbkBaseContactField& aField) const;
                
    private: // Implementation
        CPbk2FieldSelector();
        TBool IsIncluded( 
                TArray<CVPbkFieldTypeSelector*> aArray, 
                const MVPbkBaseContactField& aField ) const;
    
    private: // Data
        // Own: array of selectors to be included to this
        RPointerArray<CVPbkFieldTypeSelector> iIncludedSelectorArray;
        // Own: array of selectors to be excluded from this
        RPointerArray<CVPbkFieldTypeSelector> iExcludedSelectorArray;
    };
        
#endif // CPBK2FIELDSELECTOR_H

// End of file
