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
*     UI control for Phonebook's "Add to group".
*
*/


#ifndef __CPbkAddToGroup_H__
#define __CPbkAddToGroup_H__

//  INCLUDES
#include    <e32base.h>
#include    <cntdef.h>

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkGroupPopup;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebooks Add to Group popup list.
 */
class CPbkAddToGroup : public CBase
    {
    public:  // interface
        /**
         * Creates a new instance of this class.
         */
        IMPORT_C static CPbkAddToGroup* NewL();

        /**
         * Executes and deletes (also if a leave occurs) this dialog.
         *
         * @param aEngine   Phonebook engine
		 * @return the selected group id or KNullContactId
         */
        IMPORT_C TContactItemId ExecuteLD(CPbkContactEngine& aEngine);

        /**
         * Destructor. Also makes ExecuteLD return like the dialog was cancelled.
         */
        ~CPbkAddToGroup();

    private:  // Implementation
        CPbkAddToGroup();

    private:  // data
        /// Own: group selection popup
        CPbkGroupPopup* iGroupPopup;
        /// Ref: destructor sets referred TBool to ETrue
        TBool* iDestroyedPtr;
    };

#endif // __CPbkAddToGroup_H__

// End of File
