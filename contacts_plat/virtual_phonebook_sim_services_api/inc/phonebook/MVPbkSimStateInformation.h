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
* Description:  Offers different sim state information
*
*/


#ifndef MVPBKSIMSTATEINFORMATION_H
#define MVPBKSIMSTATEINFORMATION_H

//  INCLUDES
#include <e32cmn.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkSimPhone;

//  CLASS DECLARATION

/**
 * Virtual Phonebook SIM state information interface.
 * Offers different sim status information
 */
class MVPbkSimStateInformation
    {
    public: // Destruction
        virtual ~MVPbkSimStateInformation() {}

    public: // New functions
        /**
         * Check is SIM inserted (offline mode).
         *
         * @return ETrue if SIM is inserted, EFalse otherwise
         */
        virtual TBool SimInsertedL() = 0;
        
        /**
         * Check is SIM in BT Sap mode ( SIM access profile )
		 *
         * @return ETrue if SIM is in BT Sap mode, EFalse otherwise
         */
        virtual TBool BTSapEnabledL() = 0;
        
        /**
         * Checks if FDN is active and ADN must be blocked.
         *
         * @param aSimPhone an open SIM phone
         * @return ETrue if it's not possible to open ADN store
         */
        virtual TBool ActiveFdnBlocksAdnStoreL(MVPbkSimPhone& aSimPhone) = 0;
        
        /**
         * Checks if the current SIM card is a new one.
         *
         * @return ETrue if the card is new.
         */
        virtual TBool NewSimCardL() = 0;

        /**
         * Returns an extension point for this interface or NULL.
         */
        virtual TAny* SimStateInformationExtension(TUid /*aExtensionUid*/) { return NULL; }

    };

#endif  // MVPBKSIMSTATEINFORMATION_H
            
// End of File
