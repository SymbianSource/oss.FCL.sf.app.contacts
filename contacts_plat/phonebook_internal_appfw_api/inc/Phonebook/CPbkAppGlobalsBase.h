/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*       Phonebook application globals base.
*
*/


#ifndef __CPbkAppGlobalsBase_H__
#define __CPbkAppGlobalsBase_H__


//  INCLUDES
#include <e32base.h>
#include <MPbkUiReleasable.h>


//  FORWARD DECLARATIONS
class MPbkCommandFactory;
class MPbkAiwInterestItemFactory;
class CPbkContactItem;
class TPbkContactItemField;
class CBCardEngine;
class CPbkContactEngine;
class CPbkAiwInterestArray;
class CSendUi;

//  CLASS DECLARATION 

/**
 * Phonebook application level globals base class. This is a singleton class.
 */
class CPbkAppGlobalsBase : public CBase,
                           public MPbkUiReleasable
    {
    public:  // Interface
        /**
         * Returns the singleton instance of this class. This function's
		 * implementation is expensive. Cache the returned reference if it is
		 * used frequently.
		 *
		 * @return	a reference to the singleton globals object.
         */
        IMPORT_C static CPbkAppGlobalsBase* InstanceL();

		/**
		 * Returns Phonebook command factory.
		 * @see MPbkCommandFactory
		 */
		virtual MPbkCommandFactory& CommandFactory() =0;


		/**
		 * Returns Phonebook AIW interest item factory.
		 * @see MPbkAiwInterestItemFactory
		 */
		virtual MPbkAiwInterestItemFactory& AiwInterestItemFactory() =0;


		/**
		 * Returns Phonebook AIW interest item array.
		 */
        virtual CPbkAiwInterestArray* AiwInterestArrayL
            (CPbkContactEngine& aContactEngine) = 0;
		
        /**
         * Returns the shared Send Ui object.
         */
        virtual CSendUi* SendUiL() =0;

        /**
         * Returns the shared vCard import/export engine.
         *
         * @param aContactEngine    Phonebook database engine.
         */
        virtual CBCardEngine& BCardEngL(CPbkContactEngine& aContactEngine) =0;

	protected:  // Constructors and destructor
		CPbkAppGlobalsBase() { }
		~CPbkAppGlobalsBase() { }
    };


#endif // __CPbkAppGlobalsBase_H__

// End of File
