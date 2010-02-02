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
* Description:  Encapsulates the usage of s60 security UI component
*
*/



#ifndef CPSU2SECUI_H
#define CPSU2SECUI_H

//  INCLUDES
#include <MVPbkSimPhoneObserver.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CSecuritySettings;
class MVPbkSimPhone;

// CLASS DECLARATION

/**
*  Encapsulates the usage of s60 security UI component
*  Offers an functions for activating and deactivating fixed dialling numbers.
*  Keeps status of accepted PIN2
*
*/
class CPsu2SecUi : public CBase,
                   private MVPbkSimPhoneObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aPhone the sim phone for getting fdn activity info
        * @return a new instance of this class
        */
        static CPsu2SecUi* NewL(MVPbkSimPhone& aPhone);

        /**
        * Destructor.
        */
        virtual ~CPsu2SecUi();

    public: // New functions

        /**
        * Activates FDN if not activated
        */
        void ActivateFDNL();

        /**
        * Deactivates FDN if activated
        */
        void DeactivateFDNL();

        /**
        * Shows PIN2 query if PIN2 hasn't been confirmed
        * @return ETrue if user has given a correct PIN2
        */
        TBool ConfirmPin2L();

        /**
        * Checks the FDN activity status
        * @return ETrue if FDN is active
        */
        TBool IsFDNActive();

        /**
        * Shows PIN2 query if PIN2 hasn't been confirmed
        * @return ETrue if user has given a correct PIN2
        */
        TBool AskPin2L();

        /**
        * Reset PIN2 query information
        */
       void Reset();

    private: // Construction

        /**
        * C++ default constructor.
        */
        CPsu2SecUi(MVPbkSimPhone& aPhone);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Functions from base classes

        /**
        * From MVPbkSimPhoneObserver
        */
        void PhoneOpened(MVPbkSimPhone& aPhone);

        /**
        * From MVPbkSimPhoneObserver
        */
        void PhoneError(MVPbkSimPhone& aPhone,
            TErrorIdentifier aIdentifier, TInt aError);

        /**
        * From MVPbkSimPhoneObserver
        */
        void ServiceTableUpdated(TUint32 aServiceTable);

        /**
        * From MVPbkSimPhoneObserver
        */
        void FixedDiallingStatusChanged(
            TInt aFDNStatus );

    private:    // Data
        /// Ref: The sim phone for getting actuvity information
        MVPbkSimPhone& iPhone;
        // Own: ETrue if PIN2 has already been confirmed
        TBool iPin2Confirmed;
        // Own: s60 security settings UI
        CSecuritySettings* iSecuritySettings;
    };

#endif      // CPSU2SECUI_H

// End of File
