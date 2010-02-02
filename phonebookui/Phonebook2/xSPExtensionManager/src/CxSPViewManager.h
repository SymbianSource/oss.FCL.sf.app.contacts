/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CXSPVIEWMANAGER_H__
#define __CXSPVIEWMANAGER_H__

// INCLUDES
#include <e32base.h>
#include "MxSPCommandMapper.h"

// forward declarations
class MxSPView;
class CxSPCommandInfo;
class CEikMenuPane;
class MPbk2ContactUiControl;

/**
 * Composite of view extensions.
 */
class CxSPViewManager : public CBase,
                               public MxSPCommandMapper
    {
    public:
        /**
         * Creates new composite view extension object with empty
         * set of contained views.
         */
        static CxSPViewManager* NewL();

        /**
         * Appends an extension view to this composite view.
         * Ownership is transferred to this object.
         *
         * @param aView view to append.
         * @param aId xSP id
         */
        void AppendL(MxSPView* aView, TUint32 aId);

        /**
         * Destructor.
         */
        ~CxSPViewManager();

    public:
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
        TBool HandleCommandL(TInt aCommandId, MPbk2ContactUiControl* aUiControl);

	public: // from MxSPCommandMapper

		TInt GetOldCommand( TUint32 aId, TInt aNewCommand,
        							TInt& aOldCommand ) const;
        TInt GetNewCommand( TUint32 aId, TInt aOldCommand,
        							TInt& aNewCommand ) const;

    private: // Implementation
        CxSPViewManager();

    private: // Data
        /// Array of view extensions.
        CArrayPtrFlat<MxSPView> iViews;
        /// New commands pool
        TInt iNewCommands;
        /// Array of command id mappings
        RPointerArray<CxSPCommandInfo> iCommandIdMap;
    };


#endif // __CXSPVIEWMANAGER_H__

// End of File
