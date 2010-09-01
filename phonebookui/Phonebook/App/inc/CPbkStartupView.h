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
*       
*
*/


#ifndef __CPbkStartupView_H__
#define __CPbkStartupView_H__

// INCLUDES
#include "CPbkAppView.h"
#include <MPbkAppUiExtension.h>

// FORWARD DECLARATIONS
class MPbkAppUiExtension;
template<class ControlType> class CPbkControlContainer;

//  CLASS DECLARATION

/**
 * Phonebook "Startup view" application view class. 
 */
class CPbkStartupView : public CPbkAppView,
                        public MPbkExtensionStartupObserver
	{
	public: // Construction and destruction
		static CPbkStartupView* NewL(MPbkAppUiExtension& aAppUiExtension);
        static CPbkStartupView* NewLC(MPbkAppUiExtension& aAppUiExtension);

        ~CPbkStartupView();
    private: // From CAknView
        void DoActivateL(const TVwsViewId& aPrevViewId,
                         TUid aCustomMessageId,
                         const TDesC8& aCustomMessage);
	    void DoDeactivate();  
        TUid Id() const;
        
    private: // From MPbkExtensionStartupObserver
        void HandleStartupComplete();
        void HandleStartupFailedL(TInt aError);

	private: // Implementation
		CPbkStartupView(MPbkAppUiExtension& aAppUiExtension);
		void ConstructL();	

        static TInt ViewActivationTimeout(TAny* aThis);
        TInt ViewActivationTimeout();
	private: // Data
        /// Ref: Application UI extension that will be started
        MPbkAppUiExtension& iAppUiExtension;
        typedef CPbkControlContainer<CCoeControl> CContainer;
        /// Own: this view's control container
        CContainer* iContainer;
        /// Own: timer for view activation
        CPeriodic* iTimer;
	};

#endif // __CPbkStartupView_H__

// End of File
