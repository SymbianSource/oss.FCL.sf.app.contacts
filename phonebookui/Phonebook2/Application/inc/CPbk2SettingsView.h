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
* Description:  Phonebook 2 settings application view.
*
*/


#ifndef CPBK2SETTINGSVIEW_H
#define CPBK2SETTINGSVIEW_H

// INCLUDES
#include "CPbk2AppView.h"

// FORWARD DECLARATIONS
template<class ControlType> class CPbk2ControlContainer;
class CPbk2SettingsListControl;
class MPbk2SettingsViewExtension;
class MPbk2SettingsControlExtension;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;

// CLASS DECLARATION

/**
 * Phonebook 2 settings application view.
 */
class CPbk2SettingsView : public CPbk2AppView
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2SettingsView* NewL();

        /**
         * Destructor.
         */
        ~CPbk2SettingsView();

    private: // From CPbk2AppView
        void HandleCommandL(
                TInt aCommand );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        TUid Id() const;
        void HandleStatusPaneSizeChange();
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void DoDeactivate();
        TInt GetViewSpecificMenuFilteringFlagsL() const;

    private: // Implementation
        CPbk2SettingsView();
        void ConstructL();
        void CreateControlsL();

    private: // Data
        /// This view's control container type
        typedef CPbk2ControlContainer<CPbk2SettingsListControl> CContainer;
        /// Own: This view's control container
        CContainer* iContainer;
        /// Ref: This view's control
        CPbk2SettingsListControl* iControl;
        /// Own: Id of the view that activated this view
        TVwsViewId iPreviousViewId;
        /// Own: Settings view extension
        MPbk2SettingsViewExtension* iSettingsExtension;
        /// Own: Settings view control extension
        MPbk2SettingsControlExtension* iSettingsControlExtension;
        /// Ref: Navipane for setting activity text to
        CAknNavigationControlContainer* iNaviPane;
        /// Own: Decorator for navipane for setting text to the navipane
        CAknNavigationDecorator* iNaviDecorator;
    };

#endif // CPBK2SETTINGSVIEW_H

// End of File
