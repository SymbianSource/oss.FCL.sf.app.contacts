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
* Description:  A phone that supports SAT refresh
*
*/



#ifndef CSATPHONE_H
#define CSATPHONE_H

//  INCLUDES
#include <e32base.h>
#include <mvpbksimphone.h>

namespace VPbkSimStoreImpl {
// FORWARD DECLARATIONS
class CPhone;
class CSatRefreshNotifier;

// CLASS DECLARATION

/**
*  A phone that supports SAT refresh
*
*/
NONSHARABLE_CLASS(CSatPhone) : public CBase,
                  public MVPbkSimPhone
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return a new instance of this class
        */
        static CSatPhone* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CSatPhone();

    public: // Functions from base classes
        
        /**
        * From MVPbkSimPhone
        */
        void OpenL( MVPbkSimPhoneObserver& aObserver );

        /**
        * From MVPbkSimPhone
        */
        void Close( MVPbkSimPhoneObserver& aObserver );

        /**
        * From MVPbkSimPhone
        */
        TBool USimAccessSupported() const;

        /**
        * From MVPbkSimPhone
        */
        TUint32 ServiceTable() const;
        
        /**
        * From MVPbkSimPhone
        */
        TFDNStatus FixedDialingStatus() const;
        
    private:

        /**
        * C++ default constructor.
        */
        CSatPhone();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// Own: a shared phone used by the stores
        VPbkSimStoreImpl::CPhone* iPhone;
        /// Own: a notifies observers when SIM files are refreshed
        VPbkSimStoreImpl::CSatRefreshNotifier* iSatNotifier;
    };
} // namespace VPbkSimStoreImpl
#endif      // CSATPHONE_H
            
// End of File
