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
*     Logs "Settings" view container control class implementation
*
*/


#ifndef     __Logs_App_CLogsSettingsControlContainer_H__
#define     __Logs_App_CLogsSettingsControlContainer_H__

//  INCLUDES
#include "CLogsBaseControlContainer.h"

#include "LogsEng.hrh"



// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAknSettingStyleListBox;
class CLogsSettingsView;


// CLASS DECLARATION

/**
 * Logs "Settings" view container control class implementation.
 */
class   CLogsSettingsControlContainer :
            public CLogsBaseControlContainer
            
    {
    public:  // interface
        /**
         * Standard creation function.
         *
         * @param aSettingsView this control's creation view.
         * @param aRect client rect
         * @param aForGs Is created for general settings
         * @return new object
         */
        static CLogsSettingsControlContainer* NewL
                ( CLogsSettingsView* aSettingsView,
                  const TRect& aRect,
                  TBool aForGs );

        /**
         * Destructor. Deletes child controls.
         */
        ~CLogsSettingsControlContainer();

    private:
        /**
         * Constructor, second phase.
         *
         * @param aRect client rect.
         * @param aForGs 
         */
        void ConstructL( const TRect& aRect, 
                         TBool aForGs );

        /**
         * C++ constructor
         * @param aSettingsView this control's creation view.
         */
        CLogsSettingsControlContainer( CLogsSettingsView* aSettingsView );
    public:
        /**
         * Returns the iListBox.
         * @return listbox
         */
        CAknSettingStyleListBox* ListBox();

    public: // operations
        /**
         * Create empty listbox.
         */
        void CreateListBoxL();
        
        /**
         *  Update listbox contents by using resource and model information.
         */
        void UpdateListBoxContentL();

    private:  // from CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
         * Returns the child controls at aIndex.
         *
         * @param aIndex Control's index.
         *
         * @return Sub control from the aIndex.
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         *  Called when control's size changed.
         */
        void SizeChanged();

    public: // Called from Settings view (not from Gs appui as usual)
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    private:  // operations

        /**
         *  Makes Setting view's one line with the current information
         *
         *  @param  aItems              Array where to put the line information
         *  @param  aFirstResourceText  First line's resource id
         *  @param  aSecondResourceText Second line's resource id
         */
        void AddOneListBoxTextLineL( CDesCArrayFlat* aItems,
                                     TInt aFirstResourceText,
                                     TInt aSecondResourceText );
                                     
        /**
         *  Handles focus change events. This will hand over focus changes to list so that 
         *  focus animations are displayed on the list.
         */
        void FocusChanged(TDrawNow aDrawNow);
                                     
                                     

    private:  // data

        /// Own: ListBox control
        CAknSettingStyleListBox* iListBox;

        /// Ref: Pointer to the SettingsView
        CLogsSettingsView* iSettingsView;

        /// Own: The index of the line that has the focus.
        TInt iCurrentListBoxIndex;

        /// Own: The index of the top line that has the focus.
        TInt iCurrentListBoxTopIndex;
       
    };

#endif  // __Logs_App_CLogsSettingsControlContainer_H__

// End of File
