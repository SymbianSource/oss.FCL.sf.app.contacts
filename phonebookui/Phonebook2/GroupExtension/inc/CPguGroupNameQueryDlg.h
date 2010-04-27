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
* Description:  Phonebook 2 group name query dialog.
*
*/


#ifndef CPGUGROUPNAMEQUERYDLG_H
#define CPGUGROUPNAMEQUERYDLG_H

// INCLUDES
#include <AknQueryDialog.h>

// FORWARD DECLARATIONS
class MVPbkContactViewBase;
class MPbk2ContactNameFormatter;

// CLASS DECLARATION

/**
 * Phonebook 2 group name query dialog.
 * Responsible for quering group name.
 */
class CPguGroupNameQueryDlg : public CAknTextQueryDialog,
                                public MCoeForegroundObserver
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aDataText         Original name of the group.
         * @param aGroupsView       Groups list view.
         * @param aNameGeneration   Indicates whether to generate
         *                          group name.
         * @return  A new instance of this class.
         */
        static CPguGroupNameQueryDlg* NewL(
                TDes& aDataText,
                MVPbkContactViewBase& aGroupsView,
                MPbk2ContactNameFormatter& aNameFormatter,
                TBool aNameGeneration );

        /**
         * Destructor.
         */
        ~CPguGroupNameQueryDlg();

    public: // From CAknTextQueryDialog
        TBool OkToExitL(
                TInt aButtonId );
        
    private: // From MCoeForegroundObserver
        void HandleGainingForeground();
        void HandleLosingForeground();
        
    private: // Implementation
        CPguGroupNameQueryDlg(
                TDes& aDataText,
                MVPbkContactViewBase& aGroupsView,
                MPbk2ContactNameFormatter& aNameFormatter );
        void ConstructL(
                TDes& aDataText,
                TBool aNameGeneration );
        void UpdateGroupTitleL();
        TBool ContainsL(
                const TDesC& aText );

    private: // Data
        /// Ref: Groups view
        MVPbkContactViewBase& iGroupsListView;
        /// Ref: Contact name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Own: Copy of original group name
        HBufC* iOriginalName;
    };

#endif // CPGUGROUPNAMEQUERYDLG_H

// End of File
