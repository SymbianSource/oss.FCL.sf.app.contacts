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


#ifndef CSIMSTATEINFORMATION_H
#define CSIMSTATEINFORMATION_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimStateInformation.h>

// FORWARD DECLARATIONS
class CVPbkSimStateInformation;

namespace VPbkSimStoreService {
    
//  CLASS DECLARATION

/**
 * Offers different sim status information
 */
NONSHARABLE_CLASS(CSimStateInformation) 
        :   public CBase, 
            public MVPbkSimStateInformation
    {
    public: // Construction
        
        /**
        * Two-phased constructor.
        *
        * @return a new instance of this class
        */
        static CSimStateInformation* NewL();
        
        /**
         * Destructor.
         */
        ~CSimStateInformation();
    
    public: // Functions from MVPbkSimStateInformation
        
        TBool SimInsertedL();
        TBool BTSapEnabledL();
        TBool ActiveFdnBlocksAdnStoreL(MVPbkSimPhone& aSimPhone);
        TBool NewSimCardL();
    
    private:    // Construction
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private: // Data
        CVPbkSimStateInformation* iSimStateInfo;
    };
} // namespace VPbkSimStoreService

#endif  // CSIMSTATEINFORMATION_H
            
// End of File
