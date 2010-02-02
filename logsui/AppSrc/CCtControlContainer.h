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
*     Logs "AoC & CT list" view container class implementation
*
*/


#ifndef     __LOGS_APP_CCTCONTROLCONTAINER_H__
#define     __LOGS_APP_CCTCONTROLCONTAINER_H__

//  INCLUDES
#include "CLogsBaseControlContainer.h"
#include "MSimInfoObserver.h"
#include "MLineStatusObserver.h"

#include "LogsAlsEnum.hrh"

// FORWARD DECLARATIONS
class CLogsBaseView;
class CAknDoubleLargeStyleListBox;
class MLogsAocUtil;
class MLogsCallStatus;
class MLogsSimInfo;

// CLASS DECLARATION

/**
 * Logs "CT list" view container class implementation.
 * CT = Call Timers
 */
class CCtControlContainer
	: public CLogsBaseControlContainer
	, public MSimInfoObserver
	, public MLineStatusObserver
    {
	public:  // interface
		/**
		 *  Standard creation function.
		 *
		 *  @param  aAppView    this control's application view.
		 *  @param  aRect       rect size
		 *  @return             new object of this class
		 */
		static CCtControlContainer* NewL
										(	CLogsBaseView* aAppView
										,	const TRect& aRect
										);

		/**
		 *  Destructor. Deletes child controls.
		 */
		~CCtControlContainer();

	private:
		/**
		 *  Constructor, second phase.
		 *
		 *  @param aRect rect size
		 */
		void ConstructL( const TRect& aRect);

		/**
		 * C++ default constructor
		 * @param aAppView, pointer to view
		 */
		CCtControlContainer( CLogsBaseView* aAppView );

	public: 
		/**
		 *  Returns the iListBox.
		 *
		 *  @return listbox control
		 */
		CAknDoubleLargeStyleListBox* ListBox();

		/**
		 *  Returns Aoc util API
		 *
		 *  @return MLogsAocUtil reference
		 */
		MLogsAocUtil& AocUtil() const;

		/**
		 *  Returns call status API
		 *
		 *  @return MLogsCallStatus reference
		 */
		MLogsCallStatus& CallStatus() const;

		/**
		 *  Updates the the listbox
		 */
		void UpdateListBoxContentL();

		/**
		 * Als setting query
		 * @param aAlsSetting current value
		 * @return ETrue if changed, else EFalse
		 */
		TBool AlsSettingQueryL( TAlsEnum & aAlsSetting );

		/**
		 * Shows call duration details
		 */
        void ShowDurationDetailsL( TInt aCommandId );		
        void DurationDetailLineL( CDesCArrayFlat& aItems, 
                                  TInt aResource, 
                                  TAlsEnum aLine, 
                                  TVoipEnum aVoip );

	protected:  // from CCoeControl
		/**
		 *  Returns the child controls at aIndex.
		 *
		 *  @param  aIndex  Index of the control.
		 *
		 *  @return Sub control from the aIndex.
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

	private: // MLineStatusObserver
		void LineStatusChangedL();

	private: // MSimInfoObserver
		void SimInfoNotificationL();

	private:  // operations
		/**
		 *  Timer handler function. Sets new timer if call is active for 
		 *  new update, calls UpdateListBoxContentL() to do actual 
		 *  listbox updating
		 *  @param aContainer, this pointer, allows deletion of timer.
		 *  @return always 0
		 */
		static TInt TimerLaunchedL( TAny* aContainer );

		/**
		 * Add line to array
		 * @param aItems array
		 * @param resource text
		 */
		void MakeOneLineL( CDesCArrayFlat& aItems, TInt aResourceText );
		
        /**
         *  Handles focus change events. This will hand over focus changes to list so that 
         *  focus animations are displayed on the list.
         */
        void FocusChanged(TDrawNow aDrawNow);

	private:  // data
		/// Ref: The application view whose control container this class is.
		CLogsBaseView*					iAppView;

		/// Own: ListBox style
		CAknDoubleLargeStyleListBox*	iListBox;

		/// Own: CPeriodic for updating view when call is active.
		CPeriodic* iPeriodic;        

		/// Own: Aoc Utility class
		MLogsAocUtil*					iAocUtil;

		/// Own: call status
		MLogsCallStatus*				iCallStatus;
		
		//Own: Pop-listbox for duration details info
		CAknSingleHeadingPopupMenuStyleListBox* iDurationDetailsPopupListBox;
	};

#endif  // __Logs_App_CCtControlContainer_H__

// End of File
