/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 USIM UI Extension service dialing info view.
*
*/


#ifndef CPSU2SERVICEDIALINGINFOVIEW_H
#define CPSU2SERVICEDIALINGINFOVIEW_H

// INCLUDES
#include "CPsu2InfoViewBase.h"

// CLASS DECLARATION

/**
 * Phonebook 2 USIM UI Extension service dialing info view.
 * Responsible for implementing service dialing info view.
 */
class CPsu2ServiceDialingInfoView : public CPsu2InfoViewBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aExtensionView    The view from the extension framework.
         * @param aViewManager      The view manager of USIM extension.
         * @return  A new instance of this class.
         */
        static CPsu2ServiceDialingInfoView* NewL(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );

        /**
         * Destructor.
         */
        ~CPsu2ServiceDialingInfoView();

    public: // From CPsu2InfoViewBase
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void DoDeactivate();
    public: // From MPbk2ControlObserver
        void HandleControlEventL(
            MPbk2ContactUiControl& aControl,
            const TPbk2ControlEvent& aEvent );
    private: // From MPbk2UIExtensionView
        void HandleCommandL(
                TInt aCommand );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );

    private: // From CPsu2InfoViewBase
        TInt NameListControlResourceId() const;
        HBufC* NaviPaneLabelL() const;
        void UpdateCbasL();

    private: // Implementation
        CPsu2ServiceDialingInfoView(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );
        void ConstructL();
        TBool ContactHasPhoneNumberL(
                const MVPbkStoreContact* aContact );
    };

#endif // CPSU2SERVICEDIALINGINFOVIEW_H

// End of File
