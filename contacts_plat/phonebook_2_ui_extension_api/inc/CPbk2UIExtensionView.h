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
* Description:  Phonebook 2 UI extension view.
*
*/


#ifndef CPBK2UIEXTENSIONVIEW_H
#define CPBK2UIEXTENSIONVIEW_H

// INCLUDES
#include <CPbk2AppViewBase.h>

// FORWARD DECLARATIONS
class MPbk2UIExtensionView;
class CPbk2UIExtensionLoader;

// CLASS DECLARATION

/**
 * Phonebook 2 UI extension view.
 */
class CPbk2UIExtensionView : public CPbk2AppViewBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aViewId           Id of the view to create.
         * @param aExtensionLoader  Extension loader.
         * @return  A new instance of this class.
         */
        static CPbk2UIExtensionView* NewL(
                TUid aViewId,
                CPbk2UIExtensionLoader& aExtensionLoader );

        /**
         * Destructor.
         */
        ~CPbk2UIExtensionView();

    public: // From CAknView
        TUid Id() const;
        CEikButtonGroupContainer* Cba() const;

    private: // From CPbk2AppViewBase
        CPbk2ViewState* ViewStateLC() const;
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        TInt GetViewSpecificMenuFilteringFlagsL() const;
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
        void HandleLongTapEventL(
                const TPoint& aPenEventLocation, 
                const TPoint& aPenEventScreenLocation );

    private: // From CAknView
        void HandleCommandL(
                TInt aCommand );
        void HandleStatusPaneSizeChange();
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void DoDeactivate();
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        void HandleForegroundEventL(TBool aForeground);

    private: // Implementation
        CPbk2UIExtensionView(
                TUid aViewId,
                CPbk2UIExtensionLoader& aExtensionLoader );
        void ConstructL();
        void LoadViewL() const;
        TUid PluginForViewId(
                TUid aViewId ) const;

    private: // Data
        /// Own: Id of the view
        TUid iViewId;
        /// Own: View extension
        mutable MPbk2UIExtensionView* iView;
        /// Ref: Extension loader
        CPbk2UIExtensionLoader& iExtensionLoader;
    };

// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::Cba
// --------------------------------------------------------------------------
//
inline CEikButtonGroupContainer* CPbk2UIExtensionView::Cba() const
    {
    return CPbk2AppViewBase::Cba();
    }

#endif // CPBK2UIEXTENSIONVIEW_H

// End of File
