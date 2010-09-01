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
*       Composite view extension.
*
*/


#ifndef __CPbkMultiViewExtension_H__
#define __CPbkMultiViewExtension_H__

// INCLUDES
#include <e32base.h>
#include <MPbkViewExtension.h>

/**
 * Composite of view extensions.	
 */
class CPbkMultiViewExtension : public CBase,
                               public MPbkViewExtension
    {
    public:
        /**
         * Creates new composite view extension object with empty
         * set of contained views.
         */
        static CPbkMultiViewExtension* NewL();
        
        /**
         * Appends an extension view to this composite view.
         * Ownership is transferred to this object.
         *
         * @param aView view to append. 
         */
        void AppendL(MPbkViewExtension* aView);

        /**
         * Destructor.	
         */
        ~CPbkMultiViewExtension();
    public: // From MPbkViewExtension
        /**
         * Filters the menu pane of the Phonebook view connect to this 
         * extension.
         *
         * @param aResourceId menu resource id.
         * @param aMenuPane menu pane which will be filtered.
         */
        void DynInitMenuPaneL
            (TInt aResourceId, CEikMenuPane* aMenuPane);

        /**
         * Handles phone book extension commands.
         *
         * @param aCommandId command id.
         * @return ETrue if command was handled.
         */
        TBool HandleCommandL(TInt aCommandId);

        /**
         * Handles phone book extension commands.
         *
         * @param aCommandId command id.
         * @param aObserver command execution observer
         * @return ETrue if command was handled.
         */
        TBool HandleCommandL(TInt aCommandId, 
                             MPbkMenuCommandObserver& aObserver);

        /**
         * Sets ui contact control for this view extension. 
         *
         * @param aContactControl for requesting data from the 
         *                        contact control; if NULL, association 
         *                        is removed.
         * @see MPbkContactUiControl
         */
        void SetContactUiControl
            (MPbkContactUiControl* aContactControl);

    private: // From MPbkViewExtension
        void DoRelease();

    private: // Implementation
        CPbkMultiViewExtension();

    private: // Data
        /// Array of view extensions.
        CArrayPtrFlat<MPbkViewExtension> iViews;
    };



#endif // __CPbkMultiViewExtension_H__

// End of File
