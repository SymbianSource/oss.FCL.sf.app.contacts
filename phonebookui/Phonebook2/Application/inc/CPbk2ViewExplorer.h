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
* Description:  Phonebook 2 view explorer.
*
*/


#ifndef CPBK2VIEWEXPLORER_H
#define CPBK2VIEWEXPLORER_H

//  INCLUDES
#include <e32base.h>
#include <vwsdef.h> // TVwsViewId
#include <MPbk2ViewExplorer.h>

//  FORWARD DECLARATIONS
class CPbk2AppUi;
class CPbk2ViewState;
class CEikImage;
class CPbk2ViewGraph;
class MPbk2ViewActivationTransaction;

//  CLASS DECLARATION

/**
 * Phonebook 2 view explorer.
 * The view explorer is responsible for creating the 
 * view graph from resources and applying view graph changes from
 * UI extension. The application views are created by this class
 * from the view graph nodes and the application views can be 
 * switched.
 */
class CPbk2ViewExplorer : 
        public CBase,
        public MPbk2ViewExplorer
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aAppUi    Reference to Phonebook 2 application UI.
         * @return  A new instance of this class.
         */
        static CPbk2ViewExplorer* NewL(
                CPbk2AppUi& aAppUi );

        /**
         * Destructor.
         */
        ~CPbk2ViewExplorer();

    public: // Interface
        /**
         * Creates application views.
         */
         void CreateViewsL();

    public: // From MPbk2ViewExplorer
        TBool IsPhonebook2View(
                const TVwsViewId& aViewId ) const;
        void ActivatePhonebook2ViewL(
                TUid aViewId,
                const CPbk2ViewState* aViewState ) const;
        void ActivatePreviousViewL(
                const CPbk2ViewState* aViewState );
        MPbk2ViewActivationTransaction* HandleViewActivationLC(
                const TUid& aViewId,
                const TVwsViewId& aPrevViewId,
                const TDesC* aTitlePaneText,
                const CEikImage* aContextPanePicture,
                TUint aFlags );
        CPbk2ViewGraph& ViewGraph() const;

    private: // Implementation
        CPbk2ViewExplorer(
                CPbk2AppUi& aAppUi );
        void ConstructL();
        void ReadViewGraphL();

    private: // Data
        /// Ref: Phonebook 2 application UI
        CPbk2AppUi& iAppUi;
        /// Own: View navigation graph
        CPbk2ViewGraph* iViewGraph;
    };

#endif // CPBK2VIEWEXPLORER_H

// End of File
