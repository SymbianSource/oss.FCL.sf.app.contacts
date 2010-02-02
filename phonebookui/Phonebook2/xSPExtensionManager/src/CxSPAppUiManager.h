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
*       Composite AppUi extension.
*
*/


#ifndef __CXSPAPPUIMANAGER_H__
#define __CXSPAPPUIMANAGER_H__

// INCLUDES
#include <e32base.h>
#include "MPbk2AppUiExtension.h"

// FORWARD DECLARATIONS
class CPbk2ViewGraph;
class MPbk2StartupMonitor;
class CPbk2TabGroupContainer;
class MxSPFactory;
class CxSPViewIdChanger;

/**
 * Composite of AppUi extensions.
 */
class CxSPAppUiManager : public CBase, public MPbk2AppUiExtension
    {
    public:
        /**
         * Creates new composite AppUI extension object with empty
         * set of contained AppUIs.
         *
         * @param aFactories Array of xSP extension factories
         * @param aViewIdChanger View id changer instance
         * @return New instance of this class
         */
        static CxSPAppUiManager* NewL( CArrayPtrFlat<MxSPFactory>& aFactories,
        							   CxSPViewIdChanger& aViewIdChanger );

        /**
         * Destructor.
         */
        ~CxSPAppUiManager();

    public: // From MPbk2AppUiExtension

        /**
         * Creates a Phonebook startup extension.
         *
         * @param aObserver the startup observer
         * @param aEngine Phonebook contact engine
         */
        void ExtensionStartupL( MPbk2StartupMonitor& aStartupMonitor );

        /**
         * Allows extensions to add tab groups dynamically.
         *
         * @param aNode View graph node
         * @param aAknTabGroup Avkon tab group
         * @param aTabIcons Tab icons data structure
         * @param aViewCount View count
         */
        void ApplyDynamicPluginTabGroupContainerChangesL(
                const CPbk2ViewNode& aNode,
                CAknTabGroup& aAknTabGroup,
                CPbk2IconInfoContainer& aTabIcons,
                TInt aViewCount );

    private: // From MPbk2AppUiExtension
        void DoRelease();

    public: // Implementation
		/**
         * Modifies view graph.
         *
         * @param aViewGraph view graph to be modified
         */
        void ApplyExtensionViewGraphChangesL( CPbk2ViewGraph& aViewGraph );

    private: // Implementation
        CxSPAppUiManager( CArrayPtrFlat<MxSPFactory>& aFactories,
        				  CxSPViewIdChanger& aViewIdChanger );

        /**
         * Performs the 2nd phase of the construction.
         */
		void ConstructL();

    private: // Data
    	// Ref: factories
        CArrayPtrFlat<MxSPFactory>& iFactories;

        /// Ref: view id changer
        CxSPViewIdChanger& iViewIdChanger;

        /// Total number of tab views
        TInt iTabViews;
        /// Total number of other views
        TInt iOtherViews;
    };


#endif // __CXSPAPPUIMANAGER_H__

// End of File
