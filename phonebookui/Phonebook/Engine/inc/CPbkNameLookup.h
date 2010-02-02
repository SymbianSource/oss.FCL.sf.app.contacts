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
*		A class for asyncronous phonebook name lookup
*
*/


#ifndef __CPBKNAMELOOKUP_H__
#define __CPBKNAMELOOKUP_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include <cntdef.h>     // TContactItemId

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkSimEntryCopierBase;
class CPbkContactItem;

// CLASS DECLARATION

/**
 * The name lookup observer interface.
 */
class MPbkNameLookupObserver
    {
    public: // interface
        virtual void NameLookupCompleted(TContactItemId aContactId) = 0;
        virtual void NameLookupFailed(TInt aError) = 0;
    };

/**
 * A class for asyncronous phonebook name lookup.
 * Finds a match for a array, or return null.
 */
 NONSHARABLE_CLASS(CPbkNameLookup) : public CActive
    {
    public: // interface
        static CPbkNameLookup* NewL(CPbkContactEngine& aEngine,
                MPbkNameLookupObserver& aObserver);
        void IssueNameLookupL(CContactIdArray* aContactIds, 
                              CPbkContactItem& aEntry);
        ~CPbkNameLookup();

    private: // from CActive
	    void DoCancel();
	    void RunL();
	    TInt RunError(TInt aError);

    private: // implementation
        CPbkNameLookup(CPbkContactEngine& aEngine,
                MPbkNameLookupObserver& aObserver);        
        void IssueRequest();

    private: // member data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Ref: name lookup observer
        MPbkNameLookupObserver& iObserver;
        /// Own: array for lookup
        CContactIdArray* iContactIds;
        // Ref:
        CPbkContactItem* iEntry;
    };

#endif // __CPbkNameLookup_H__

// End of File
