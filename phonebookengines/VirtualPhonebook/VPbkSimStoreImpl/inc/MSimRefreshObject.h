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
* Description:  An observer API for the SIM(SAT) refresh
*
*/


#ifndef VPBKSIMSTOREIMPL_MSIMREFRESHOBJECT_H
#define VPBKSIMSTOREIMPL_MSIMREFRESHOBJECT_H

//  INCLUDES
#include <e32std.h>

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class MSimRefreshCompletion;

// DATA TYPES
enum TSimRefreshFlag
    {
    /// Service table refreshed
    KSTRefresh      = 0x00000001,
    /// Abreaviated dialling numbers store refreshed
    KADNRefresh     = 0x00000002,
    /// Service dialling numbers store refreshed
    KSDNRefresh     = 0x00000004,
    /// Fixed dialling numbers store refreshed
    KFDNRefresh     = 0x00000008,
    /// Own numbers store (MSISDN) refreshed
    KONRefresh     = 0x00000010,
    /// Abreaviated dialling numbers extension refreshed
    KADNExtRefresh  = 0x00000020,
    /// Service dialling numbers extension refreshed
    KSDNExtRefresh  = 0x00000040,
    /// Fixed dialling numbers extension refreshed
    KFDNExtRefresh  = 0x00000080,
    /// Own numbers extension refreshed
    KONExtRefresh   = 0x00000100,
    /// SIM reset (see also msatrefreshobserver.h)
    KSimReset       = 0x00000200,
    /// Full file change notification
    KSimInitFullFileChangeNotification	= 0x00000400,
    /// USIM application reset
    KUsimApplicationReset				= 0x00000800,
    /// 3G session reset
    K3GSessionReset						= 0x00001000,
    /// SIM init
    KSimInit							= 0x00002000
    };

// CLASS DECLARATION
    
/**
*  An observer API for the SIM(SAT) refresh
*
*/
NONSHARABLE_CLASS( MSimRefreshObject )
    {
    public: // New functions
        
        /**
        * Called when there is a refresh in the sim and 
        * client should update it's content. Client must always call
        * aSimRefreshCompletion even it doesn't update anything.
        *
        * @param aSimRefreshFlags a combination of TSimRefreshFlag
        * @param 
        */
        virtual void SimRefreshed( TUint32 aSimRefreshFlags, 
            MSimRefreshCompletion& aSimRefreshCompletion ) = 0;

        /**
        * Called to check if refresh can't be allowed. If client has 
        * a sim operation on going that uses files given paramter
        * it should respond EFalse
        *
        * @param aSimRefreshFlags a combination of TSimRefreshFlag
        * @return EFalse if SIM shouldn't be refresh
        */
        virtual TBool AllowRefresh( TUint32 aSimRefreshFlags ) = 0;

        /**
        * @return the files (TSimRefreshFlags) that object is interested in.
        */
        virtual TUint32 ElementaryFiles() = 0;
        
    protected:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~MSimRefreshObject() {}
    };

NONSHARABLE_CLASS( MSimRefreshCompletion )
    {
    public: // New functions
    
        /**
        *
        * @param aReadAdditionalFiles EFalse if object used only
        *        files defined in TSimRefreshFlag. Otherwise ETrue. 
        */
        virtual void SatRefreshCompleted( MSimRefreshObject& aSimRefreshObject,
            TBool aReadAdditionalFiles ) = 0;
    
    protected:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~MSimRefreshCompletion() {}
    };

} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_MSIMREFRESHOBJECT_H
            
// End of File
