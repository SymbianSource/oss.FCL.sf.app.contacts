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
* Description:  Offers different sim status information
*
*/


#ifndef CVPBKSIMSTATEINFORMATION_H
#define CVPBKSIMSTATEINFORMATION_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkSimPhone;

//  CLASS DECLARATION

/**
 * Offers different sim status information
 */
class CVPbkSimStateInformation : public CBase
    {
    public: // Construction
        
        /**
        * Two-phased constructor.
        *
        * @return a new instance of this class
        */
        IMPORT_C static CVPbkSimStateInformation* NewL();
        
        /**
         * Destructor.
         */
        ~CVPbkSimStateInformation();
    
    public: // Functions from MVPbkSimStateInformation
        
        /**
        * Check is SIM inserted (offline mode).
        *
        * @return ETrue if SIM is inserted, EFalse otherwise
        */
        IMPORT_C TBool SimInsertedL();

        /**
        * Check is SIM in BT Sap mode ( SIM access profile )
		*
        * @return ETrue if SIM is in BT Sap mode, EFalse otherwise
        */
        IMPORT_C TBool BTSapEnabledL();

        /**
        * Checks if FDN is active and ADN must be blocked.
        *
        * @param aSimPhone an open SIM phone
        * @return ETrue if it's not possible to open ADN store
        */
        IMPORT_C TBool ActiveFdnBlocksAdnStoreL(MVPbkSimPhone& aSimPhone);

        /**
        * Checks if the current SIM card is a new one.
        *
        * @return ETrue if the card is new.
        */
        IMPORT_C TBool NewSimCardL();
    
    private:
        CVPbkSimStateInformation();
        void ConstructL();
        
    };

#endif  // CVPBKSIMSTATEINFORMATION_H
            
// End of File
