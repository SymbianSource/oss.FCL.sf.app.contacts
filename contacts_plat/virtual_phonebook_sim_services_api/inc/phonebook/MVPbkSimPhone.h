/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An interface for SIM phone
*
*/



#ifndef MVPBKSIMPHONE_H
#define MVPBKSIMPHONE_H

//  INCLUDES
#include <e32cmn.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkSimPhoneObserver;

// CLASS DECLARATION

/**
 * Virtual Phonebook SIM phone interface.
 * An interface for SIM phone.
 */
class MVPbkSimPhone
    {
    public: // Types
        /**
         * Flags for SIM supported phonebooks
         */
        enum TVPbkSimPhoneSTFlags
            {
            KVPbkCapsADNSupported       = 0x00000001,
            KVPbkCapsSDNSupported       = 0x00000002,
            KVPbkCapsFDNSupported       = 0x00000004,
            KVPbkCapsONSupported        = 0x00000008,
            KVPbkCapsLocalPbkSupported  = 0x00000010
            };
    
        /**
         * FDN status. Practically redefined from MM ETel to make clients
         * independent from ETel headers
         */
        enum TFDNStatus
            {
            EFdnIsNotActive,
		    EFdnIsActive,
		    EFdnIsPermanentlyActive,
		    EFdnIsNotSupported,
		    EFdnStatusUnknown
            };
            
    public:  // Constructors and destructor
        /**
         * Destructor.
         */
        virtual ~MVPbkSimPhone() {}

    public: // New functions
        /**
         * Opens the phone. The phone must be open before using it
         * @param aObserver the observer to notify after phone is open
         */
        virtual void OpenL( MVPbkSimPhoneObserver& aObserver ) = 0;

        /**
         * Closes the phone. After this the observer doesn't get
         * any calls
         */
        virtual void Close( MVPbkSimPhoneObserver& aObserver ) = 0;

        /**
         * Offers information about SIM access
         * @return ETrue if USIM access supported, EFalse if SIM
         */
        virtual TBool USimAccessSupported() const = 0;

        /**
         * Returns the service table (TVPbkSimPhoneSTFlags)
         * @return The service table.
         */
        virtual TUint32 ServiceTable() const = 0;
        
        /**
         * Returns the status of the fixed dialing store
         * @return The status of the fixed dialing store
         */
        virtual TFDNStatus FixedDialingStatus() const = 0;

        /**
         * Returns an extension point for this interface or NULL.
         */
        virtual TAny* SimPhoneExtension(TUid /*aExtensionUid*/) { return NULL; }

    };

#endif      // MVPBKSIMPHONE_H
            
// End of File
