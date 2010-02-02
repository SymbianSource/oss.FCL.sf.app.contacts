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
*     Logs "Event list" view container control class implementation
*
*/


#ifndef     __Logs_App_CLogsEventListControlContainer_H__
#define     __Logs_App_CLogsEventListControlContainer_H__

//  INCLUDES
#include "CLogsBaseControlContainer.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogsEventListView;

// CLASS DECLARATION

/**
 * Logs "Event list" view container control class implementation.
 */
class CLogsEventListControlContainer : public CLogsBaseControlContainer
    {
    public:  // construction
        /**
         * Standard creation function.
         *
         * @param aView Parent view.
         * @param aRect rect size
         * @return new object
         */
        static CLogsEventListControlContainer* NewL
                                                (   CLogsEventListView* aView
                                                ,   const TRect& aRect
                                                );

        /**
         * Destructor. Deletes child controls.
         */
        ~CLogsEventListControlContainer();

    private:  // construction
        /**
         * C++ constructor
         * @param aView Parent view.
         */
        CLogsEventListControlContainer ( CLogsEventListView* aView );

        /**
         * Constructor, second phase.
         *
         * @param aRect rect size
         */
        void ConstructL( const TRect& aRect );

    public:
        /**
         * Returns the iListBox.
         * @return listbox 
         */
        CAknSingleGraphicHeadingStyleListBox* ListBox();
        
        /**
         * Returns the View.
         * @return view 
         */
        CLogsEventListView* View();

        /**
         * Update contents
         */
        void UpdateL();
     
    protected:  // from CCoeControl
        /**
         * Returns the child controls at aIndex.
         *
         * @param aIndex control index
         * @return Sub control from the aIndex.
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         *  Called when control's size changed.
         */
        void SizeChanged();

        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
    protected:  // CLogsBaseControlContainer
        /**
         *  Called from HandleResourceChange. Calls AddIconL funtion
         *  for each icon in the list box.
         *
         */
        void AddControlContainerIconsL();

    private:  // operations
        /**
         * Set the empty view text.
         */
        void UpdateEmptyViewTextL();

        /**
         * Inits the listbox top & current item index.
         */
        void InitListBoxL();
        
        /**
         *  Handles focus change events. This will hand over focus changes to list so that 
         *  focus animations are displayed on the list.
         */
        void FocusChanged(TDrawNow aDrawNow);
        

    private:  // data
        /// Own: ListBox control
        CAknSingleGraphicHeadingStyleListBox*   iListBox;

        /// Ref: Pointer to parent view.
        CLogsEventListView*                     iView;
    };

#endif  // __Logs_App_CLogsEventListControlContainer_H__


// End of File

