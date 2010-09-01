/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor dialog UI field array.
*
*/

#ifndef CPBK2UIFIELDARRAY_H_
#define CPBK2UIFIELDARRAY_H_


// INCLUDE FILES
#include <e32def.h>
#include <e32base.h>
#include "MPbk2UIField.h"
#include "MPbk2UIFieldArray.h"
#include "MPbk2UIFieldFactory.h"

// FORWARD DECLARATIONS
class MPbk2UIFieldFactory;
class RFs;

/**
 * Phonebook 2 UI field array interface.
 *
 * @see MPbk2UIField
 */
class CPbk2UIFieldArray : public CBase,
					 	  public MPbk2UIFieldArray
    {
    public: 
    	~CPbk2UIFieldArray();
    	/**
	     * Creates a new instance of this class.
		 *
	     * @param aResFile                  File with resources
	     * @param aRFs                      File system session for reading
	     *                                  a resource file.
	     * @return  A new instance of this class.
	     */
	    IMPORT_C static CPbk2UIFieldArray* NewL(
	    		const TDesC& aResFile,
	            RFs& aRFs, 
	            MPbk2UIFieldFactory& aFactory);
    	        
        TInt Count() const;
        MPbk2UIField& At( TInt aIndex ) const;
		const MPbk2UIFieldFactory& GetUIFieldFactory() const;
		void AppendL(MPbk2UIFieldArray& aUIFieldArray);
		void RemoveAt( TInt aIndex );
		
    private:
    	CPbk2UIFieldArray( MPbk2UIFieldFactory& aFactory );
    	void ConstructL( const TDesC& aResFile, RFs& aRFs );
		
    private: // Data
        /// Own: Field properties
        RPointerArray<MPbk2UIField> iFieldsArr;
        MPbk2UIFieldFactory& iFactory;      
    };

#endif /*CPBK2UIFIELDARRAY_H_*/

// End of File
