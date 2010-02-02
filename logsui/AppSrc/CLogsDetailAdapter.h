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
*     Adapts data from logsmodel to logs ui component (listbox) format
*
*/


#ifndef CLogsDetailAdapter_H
#define CLogsDetailAdapter_H

//  INCLUDES
#include <bamdesca.h>  // MDesCArray
#include <e32base.h>
#include <logwrap.h>

// CONSTANTS
// Adaptor buffer length
const TInt KLogsDetailsListAdaptorArrayLen = 100;

// FORWARD DECLARATIONS
class CEikLabel;
class MLogsEventGetter;
class MLogsGetEvent;
class CFont;
class CLogEvent;
class CPhoneNumberFormat;
class CLogsEngine;
class CAknSingleHeadingStyleListBox;
class CLogsDetailView;

// CLASS DECLARATION


/**
*  Adapts data from logsmodel to logs ui component (listbox) format.
*/
class CLogsDetailAdapter : public CBase, public MDesCArray
    {  
    private:

        enum TRowNumber
            {
            EFirstRow,
            ESecondRow
            };

    public:  // Constructors and destructor
                
       /**
        * Two-phased constructor.
        * @param aListBox pointer to listbox
        * @param aView pointer to view
        * @return new created object
        */
        static CLogsDetailAdapter* NewL( 
            CAknSingleHeadingStyleListBox* aListBox,
            CLogsDetailView* aView );

       /**
        * Destructor.
        */
        virtual ~CLogsDetailAdapter();

    private:
        
       /**
        * C++ default constructor.
        * @param aEngine pointer to Logs engine
        */
        CLogsDetailAdapter( CAknSingleHeadingStyleListBox* aListBox, CLogsDetailView* aView ); 


        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();


    public: // Functions from MDesCArray

        /**
         *  Returns number of entries in details window
         *
         *  @return TInt number of lines in listbox
         */
        TInt MdcaCount() const;
    
        /**
         *  Returns a TPtrC related with the position given by aIndex
         *
         *  @param  TInt    aIndex
         *  @return TPtrC16 listbox line descriptor
         */
        TPtrC16 MdcaPoint( TInt aIndex ) const;        

    public: 
        /**
         *  Sets the maximum width in pixels for the detail line. This needs to be called from controller's
         *  SizeChanged() method.
         *
         *  @param  TInt    aTextWidth maximum width in pixels
         */
        void SetLineWidth(TInt aTextWidth);
        
        /**
         *  Updates the display content.
         */
        void UpdateL();

    private: 
        
        /**
         * Direction Row, returns true
         * @param aEvent event for formatting
         * @return ETrue if direction can be shown for current event, 
         *         otherwise EFalse
         */
        TBool DirectionRow( const MLogsEventGetter* aEvent ) const;

        /**
         * Type Row, returns true
         * @param aEvent event for formatting
         * @return ETrue if type can be shown for current event, 
         *         otherwise EFalse
         */
        TBool TypeRow( const MLogsEventGetter* aEvent ) const;

        /**
         * Status Row, if not shown return False, else true. 
         * @param aEvent event for formatting
         * @return ETrue if status can be shown for current event, 
         *         otherwise EFalse
         */
        TBool StatusRow( const CLogEvent* aCLogEvent,
                         const MLogsEventGetter* aEvent ) const;


        /**
         * Duration Row, if not shown return False, else true. 
         * @param aEvent event for formatting 
         * @return ETrue if duration can be shown for current event, 
         *         otherwise EFalse
         */
        TBool DurationRow( const CLogEvent* aEvent ) const;

        /**
         * Amount Row, if not shown return False, else true. 
         * @param aEvent event for formatting 
         * @return ETrue if amount can be shown for current event, 
         *         otherwise EFalse
         */
        TBool AmountRow( const MLogsEventGetter* aEvent ) const;

        /**
         * Sent Amount Row, if not shown return EFalse, else ETrue. 
         * @param aEvent event for formatting 
         * @return ETrue if sent amount can be shown for current event, 
         *         otherwise EFalse
         */
        TBool SentAmountRow( const MLogsEventGetter* aEvent ) const;

        /**
         * Received Amount Row, if not shown return EFalse, else ETrue. 
         * @param aEvent event for formatting 
         * @return ETrue if received amount can be shown for current event, 
         *         otherwise EFalse
         */
        TBool ReceivedAmountRow( const MLogsEventGetter* aEvent ) const;

        /**
         * Tel Row, if not shown return False, else true. 
         * @param aLogsEvent event for formatting (CLogsEvent!)
         * @param aRowNumber row number if splitted
         * @return ETrue if tel number can be shown for current event, 
         *         otherwise EFalse
         */
        TBool TelRow( const MLogsEventGetter* aEvent, 
                        TRowNumber aRowNumber = EFirstRow ) const;

        /*
        * TimeL, wrapper for leavable functions in time formatting 
        * @param aTime to format
        */
        void TimeFormattingL( TTime& aTime ) const;

        /**
         * Split string or phone number into 2 lines if needed.
         * @param aEvent log event to get a number from
         * @param aFont used font
         * @param aMaxWidthInPixels max with of column in pixels
         * @param aPreferUri Use uri instead of phonenumber
         * @return ETrue if splitted, otherwise EFalse
         */
        TBool SplitNumber( const MLogsEventGetter* aEvent, 
                           const CFont& aFont, 
                           TInt aMaxWidthInPixels, 
                           TBool aPreferUri = EFalse ) const;

        //
        void ProcessPocAndVoipRows( TDes& aDes, 
                               TInt aIndex, 
                               const MLogsEventGetter* aEvent, 
                               const CLogEvent* aLogEvent ) const;
        void ProcessCallAndFaxRows( TDes& aDes, 
                               TInt aIndex, 
                               const MLogsEventGetter* aEvent, 
                               const CLogEvent* aLogEvent ) const;

        const CFont* LineFont();
        
        void Reverse(TDes& aText ) const;        

    private:    // Data

        // Ref: Pointer to CLogEngine
        CLogsEngine* iEngine;

        /// Ref: event getter
        MLogsGetEvent* iGetter;
        /// Own: Conversion buffer 
        TBuf<KLogsDetailsListAdaptorArrayLen> iBuffer;

        /// Own: Direction text
        HBufC* iDirection;
        /// Own: Type text
        HBufC* iType;
        /// Own: Status text
        HBufC* iStatus;
        /// Own: Duration text
        HBufC* iDuration;
        /// Own: Amounttext
        HBufC* iAmount;
        /// Own: Amount in kb's text
        HBufC* iAmountKB; 
        /// Own: phone nbr text
        HBufC* iNbr;

        /// Own: Private, unknown, payphone number text
        //HBufC* iPrivateNumber;
        HBufC* iUnknownNumber;
        //HBufC* iPayphoneNumber;        

        /// Own: Direction in
        HBufC* iDirectionIn;
        HBufC* iDirectionOut;
        HBufC* iDirectionMissed;

        /// Own: Event type
        HBufC* iEventTypeVoice;
        HBufC* iEventTypeSMS;
        HBufC* iEventTypeMMS;
        HBufC* iEventTypeGPRS;
        HBufC* iEventTypeData;
        HBufC* iEventTypeWLAN;
        HBufC* iEventTypeFax;
        HBufC* iEventTypeVideo;

        HBufC* iEventTypePoC;
        HBufC* iEventTypePoCInfo;        
        HBufC* iEventTypeVoIP;
        HBufC* iPoCAddr;
        HBufC* iVoIPCallFrom;
        HBufC* iVoIPCallTo;

        /// Own: Print status messages, shown in box
        HBufC* iEventStatusDelivered;
        HBufC* iEventStatusPending;
        HBufC* iEventStatusFailed;
        HBufC* iEventStatusSent;

        /// Own: Duration field format
        HBufC* iDurationFormat;
        
        /// Own: Label for clipping
        //CEikLabel* iLabel;

        /// Own: Event status 
        TLogString iEventStatusPendingTxt;
        TLogString iEventStatusSentTxt;
        TLogString iEventStatusFailedTxt;
        TLogString iEventStatusNoDeliveryTxt;
        TLogString iEventStatusDeliveredTxt;
        TLogString iEventStatusNotSentTxt;

        HBufC* iNumberFirstRow;
        HBufC* iNumberSecondRow;  
        __MUTABLE TBool  iNumberSplitted;    

        /// Own: Sent text
        HBufC* iSentAmount;
        /// Own: Received text
        HBufC* iReceivedAmount;

        /// Own: object with groupping, clipping and A&H functionality
        CPhoneNumberFormat* iPhoneNbrFormatter;

        //Maximum text width in pixels that fit in one row
        TInt iTextWidth;
        /// Ref: 
        const CFont* iListboxFont;  // not owned

        /// Ref: reference to the list box owned by the control container
        CAknSingleHeadingStyleListBox* iListBox;
        CLogsDetailView* iView;
        
        /// Own:
        CDesCArrayFlat* iDisplayRows;
        TInt iDisplayRowNumber;


    };

#endif      // CLogsDetailsAdapter_H
            
// End of File
