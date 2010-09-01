/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 select property phase factory.
*
*/


#ifndef PBK2SELECTPROPERTYPHASEFACTORY_H
#define PBK2SELECTPROPERTYPHASEFACTORY_H

// INCLUDES
#include <e32std.h>
#include <TVPbkContactStoreUriPtr.h>

// FORWARD DECLARATIONS
class MPbk2ServicePhase;
class MPbk2ServicePhaseObserver;
class MPbk2SelectFieldProperty;
class MVPbkContactLinkArray;

// CLASS DECLARATION

/**
 * Phonebook 2 select property phase factory.
 * Responsible for selecting correct select property phase.
 */
class Pbk2SelectPropertyPhaseFactory
    {
    public: // Factory interface

        /**
         * Creates and returns a select create new property phase.
         *
         * @param aObserver             Observer.
         * @param aFilterBuffer         Field type selector buffer.
         * @param aDataBuffer           Data buffer.
         * @param aSavingStoreUri       Saving store uri.
         * @param aSelectFieldProperty  Select field property.
         * @param aResult               Selection result.
         * @param aNoteFlags            Information note flags.
         * @return  A new instance of this class.
         */
        static MPbk2ServicePhase* CreateSelectCreateNewPropertyPhaseL(
                MPbk2ServicePhaseObserver& aObserver,
                HBufC8* aFilterBuffer,
                HBufC* aDataBuffer,
                TVPbkContactStoreUriPtr aSavingStoreUri,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult,
                TUint& aNoteFlags );

        /**
         * Creates and returns a select single property phase.
         *
         * @param aObserver             Observer.
         * @param aFilterBuffer         Field type selector buffer.
         * @param aDataBuffer           Data buffer.
         * @param aContactLinks         Contact links.
         * @param aSelectFieldProperty  Select field property.
         * @param aResult               Selection result.
         * @param aNoteFlags            Information note flags.
         * @return  A new instance of this class.
         */
        static MPbk2ServicePhase* CreateSelectSinglePropertyPhaseL(
                MPbk2ServicePhaseObserver& aObserver,
                HBufC8* aFilterBuffer,
                HBufC* aDataBuffer,
                MVPbkContactLinkArray* aContactLinks,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult,
                TUint& aNoteFlags );

        /**
         * Creates and returns a select multi property phase.
         *
         * @param aObserver             Observer.
         * @param aFilterBuffer         Field type selector buffer.
         * @param aDataBuffer           Data buffer.
         * @param aContactLinks         Contact links.
         * @param aSelectFieldProperty  Select field property.
         * @param aResult               Selection result.
         * @param aNoteFlags            Information note flags.
         * @return  A new instance of this class.
         */
        static MPbk2ServicePhase* CreateSelectMultiPropertyPhaseL(
                MPbk2ServicePhaseObserver& aObserver,
                HBufC8* aFilterBuffer,
                HBufC* aDataBuffer,
                MVPbkContactLinkArray* aContactLinks,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult,
                TUint& aNoteFlags );
    };

#endif // PBK2SELECTPROPERTYPHASEFACTORY_H

// End of File
