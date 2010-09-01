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
*       Sort view implementation
*
*/


#ifndef __CXSPSORTVIEW_H__
#define __CXSPSORTVIEW_H__

//  INCLUDES
#include <e32base.h>
#include <eiklbo.h>
#include "CxSPBaseView.h"
#include "MGlobalNoteObserver.h"

//  FORWARD DECLARATIONS
class CPbk2UIExtensionView;
class CAknNavigationDecorator;
class CEikMenuPane;
class CxSPSortViewControl;
class CxSPViewIdChanger;
class CxSPLoader;

typedef CArrayPtrFlat<CxSPLoader> CxSPArray;

// CLASS DECLARATION
/**
 * Sort view for Extension Manager. Provides extension sorting UI
 * for user.
 */
class CxSPSortView : public CxSPBaseView, public MGlobalNoteObserver, public MEikListBoxObserver
    {
    public: // Constructor

        /**
         * Creates a new CxSPSortView.
         *
         * @param aViewIdChanger Reference to view id changer
         * @param aContactManager Reference to contact manager object
         * @param aView Reference to Phonebook2-created view instance
         * @param aExtensions array of extensions
         *
         * @return New instance of this class
         */
        static CxSPSortView* NewL( CxSPViewIdChanger& aViewIdChanger,
        								 MxSPContactManager& aContactManager,
        								 CPbk2UIExtensionView& aView,
        								 CxSPArray& aExtensions );

    public: // From MPbk2UIExtensionView through CxSPBaseView
		void DoDeactivate();
        void HandleCommandL( TInt aCommand );
        void HandleStatusPaneSizeChange();
        
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
    
   	private: // From CxSPBaseView

   		void DoActivateViewL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

   	private: // From MGlobalNoteObserver
        void GlobalNoteClosed( const TInt aResult );
        void GlobalNoteError( const TInt aErr );

   	private: // Implementation
        /**
         * Standard C++ constructor
         *
         * @param aMapper Reference to view id changer
         * @param aContactManager Reference to contact manager object
         * @param aView Reference to Phonebook2-created view instance
         * @param aExtensions array of extensions
         */
        CxSPSortView( CxSPViewIdChanger& aViewIdChanger,
    				 MxSPContactManager& aContactManager,
    				 CPbk2UIExtensionView& aView,
    				 CxSPArray& aExtensions );

        /**
         * Performs the 2nd phase of construction.
         */
        void ConstructL();

        /**
         * Standard C++ destructor.
         */
        ~CxSPSortView();
   
   	private:
        void UpdateCbasL( TInt aResourceId );

    private: // Data

    	/// Ref: View id changer
    	CxSPViewIdChanger& iViewIdChanger;

        /// Own: This view's control container
        CxSPSortViewControl* iContainer;

        /// Own: Used for navigation pane text
        CAknNavigationDecorator* iNaviDecorator;

        /// Ref: Array of extensions
        CxSPArray& iExtensions;
    };

#endif // __CXSPSORTVIEW_H__

// End of File
