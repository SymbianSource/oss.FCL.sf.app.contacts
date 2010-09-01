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
* Description:  A service dialing name list view
*
*/



#ifndef CPSU2SERVICEDIALINGVIEW_H
#define CPSU2SERVICEDIALINGVIEW_H

//  INCLUDES
#include "CPsu2NameListViewBase.h"

// FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CAknNavigationDecorator;

// CLASS DECLARATION

/**
*  A service dialing name list view
*/
class CPsu2ServiceDialingView : public CPsu2NameListViewBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aExtensionView the view from the extension framework
        * @param aViewManager the view manager of the USIM extension
        * @return a new instance of this class
        */
        static CPsu2ServiceDialingView* NewL(CPbk2UIExtensionView& aExtensionView,
            CPsu2ViewManager& aViewManager);
        
        /**
        * Destructor.
        */
        ~CPsu2ServiceDialingView();
        
    public: // Functions from base classes
        /**
        * From MPbk2UIExtensionView
        */
        TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent, 
                                TEventCode aType);

        /**
        * From CPsu2NameListViewBase
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId, const TDesC8& aCustomMessage);
    
        /**
        * From CPsu2NameListViewBase
        */
        void DoDeactivate();
    public: // From MPbk2ControlObserver
        void HandleControlEventL(
            MPbk2ContactUiControl& aControl,
            const TPbk2ControlEvent& aEvent );
    private: // Implementation
        CPsu2ServiceDialingView(
            CPbk2UIExtensionView& aExtensionView,
            CPsu2ViewManager& aViewManager );
    
    private:    // Functions from base classes
        
        /**
        * From CPsu2NameListViewBase
        */
        void HandleCommandL(TInt aCommand);
        
        /**
        * From CPsu2NameListViewBase
        */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        
        /**
        * From CPsu2NameListViewBase
        */
        TInt NameListControlResourceId() const;
        
        /**
         * From CPsu2NameListViewBase
         */
        void UpdateCbasL();
        
    private:
        
        /**
        * Open service dialing info view
        * *@param aUiControl
        */
        void OpenInfoViewCmdL(MPbk2ContactUiControl& aUiControl) const;
        
    private:  // Data
        
        /// Ref: Navipane for setting activity text to it
        CAknNavigationControlContainer* iNaviPane;
        /// Own: Decorator for navipane, used to set the text to the navipane
        CAknNavigationDecorator* iNaviDecorator;

    };

#endif      // CPSU2SERVICEDIALINGVIEW_H
            
// End of File
