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
*       NamesList view Extension
*
*/


#ifndef __CXSPSORTNAMESLISTVIEWEXTENSION_H__
#define __CXSPSORTNAMESLISTVIEWEXTENSION_H__

//  INCLUDES
#include <e32base.h>   // CBase
#include "MxSPView.h"  // MxSPView

//  FORWARD DECLARATIONS
class CEikMenuPane;
class MxSPCommandMapper;
class MPbk2ContactUiControl;

// CLASS DECLARATION
/**
 * Names list extension for ExtensionManager.
 */
class CxSPSortNamesListViewExtension : public CBase, public MxSPView
    {
    public: // Constructor
        /**
         * Creates a new CxSPSortNamesListViewExtension.
         *
         * @return new instance of this class
         */
        static CxSPSortNamesListViewExtension* NewL();

    private: // From MxSPView, @see MxSPView.h
        void DoRelease();
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        void DynInitNamesListMainMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        TBool HandleCommandL(TInt aCommandId, MPbk2ContactUiControl* aUiControl);
        void RegisterCommandMapper( MxSPCommandMapper& aMapper );
        TInt CommandInfoResource() const;

    private: // Implementation
        /**
         * Standard C++ constructor
         *
         */
        CxSPSortNamesListViewExtension();

        /**
         * Performs the 2nd phase of construction.
         */
        void ConstructL();

        /**
         * Standard C++ destructor.
         */
        ~CxSPSortNamesListViewExtension();

        /**
         * Adds menu contents to menu pane if needed.
         *
         * @param aMenuPane Menu pane to which the items will be added.
         */
        void AddMenuContentsL(CEikMenuPane& aMenuPane);

        /**
         * If found changes old command to new/mapped
         *
         * @param aMenuPane Menu pane
         * @param aOldCommand command id to be changed
         */
        void ChangeCommandL(CEikMenuPane& aMenuPane, TInt aOldCommand);

    private: // Data

        /// Ref: command mapper
        MxSPCommandMapper* iMapper;
    };

#endif // __CXSPSORTNAMESLISTVIEWEXTENSION_H__

// End of File
