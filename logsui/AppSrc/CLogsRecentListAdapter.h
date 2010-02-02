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
*     Adaptor to RecentModel, copies data from model listbox
*
*/


#ifndef __CLogsRecentListAdapter_H__
#define __CLogsRecentListAdapter_H__

//  INCLUDES
#include <bamdesca.h> 
#include <e32base.h>

#include "LogsConsts.h"
#include "LogsEng.hrh"
#include <AknUtils.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEikLabel;
class MLogsModel;
class MLogsEventGetter;
class MLogsUiControlExtension;
class CPhoneNumberFormat;
class CEikFormattedCellListBox;
class CLogsRecentListView;
class CLogsRecentListControlContainer;
class CLogsEngine;
//For ring duation feature
class MLogsSharedData;


// CLASS DECLARATION

/**
*  Adaptor to RecentModel, copies data from model listbox
*/
class CLogsRecentListAdapter : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aContainer
         * @param aAlsIconId icon id for ALS2 line calls.
         * @return new object 
         */
        static CLogsRecentListAdapter* NewL
            (   
            CLogsRecentListControlContainer* aContainer,
            TInt aAlsIconId );

        /**
         *  Destructor.
         */
        virtual ~CLogsRecentListAdapter();

    public: 
        /**
         *  Sets the maximum width in pixels for the detail line. This needs to be called from controller's
         *  SizeChanged() method.
         *
         *  @param  TInt    aTextWidth Maximum width in pixels
         *  @param  TInt    aNumOfTrailingIcons Number of icons needing extra space in the end of line.
         */
        void SetLineWidth(TInt aTextWidth, TInt aNumOfTrailingIcons);

        


    private:        
        /**
         * C++ default constructor.
         * @param aContainer
         * @param aAlsIconId icon id for ALS2 events
         */
        CLogsRecentListAdapter
            (   
            CLogsRecentListControlContainer* aContainer,
            TInt aAlsIconId );

        /**
         *  Symbian OS constructor.
         */
        void ConstructL();

        /**
         * Date and time localization
         * @param aEvent localize, stores result to iBuffer
         * @param aPtr Pointer to buffer which stores result 
         */ 
        void DateTimeLocalizationL( const MLogsEventGetter* aEvent ,TPtr& aPtr ) const;

    private: // from MDesCArray
        /**
         *  Returns number of entries
         *  @return TInt number of lines in list
         */
        TInt MdcaCount() const;

        /**
         *  Returns a TPtrC related with the position given by aIndex
         *  @param  aIndex list item
         *  @return item descriptor
         */
        TPtrC16 MdcaPoint( TInt aIndex ) const;  
        
    private:
        /**
         *  Returns a TPtrC related with the position given by aIndex
         *
         *  @param aDest destination string
         *  @param aDuplBufLength pixel length of "(dupl. count)"
         *  @param aDuplBuffer string to insert if duplicates "(dupl. count)"
         *  @param aNumIcons number of trailing icons (need space in the end of row)
         *  @param aOriginal original string
         */
        void BuildDisplayString
            (   TDes& aDest
            ,   TInt aDuplBufPixels
            ,   TDesC& aNonClippedDuplBuffer
            ,   TInt   aNumIcons
            ,   TPtrC aOriginal 
            ,   AknTextUtils::TClipDirection aClipDirection = AknTextUtils::EClipFromEnd
            ) const;

        const CFont* LineFont();
        
//For ring duation feature
        /** Append Ring Duration to the string 
        *   @param aEvent : an event information to be processed
        *          aPtr:    a pointer to the ouput string
        *   @return  None
        */
        void AppendRingDuration( const MLogsEventGetter& aEvent, TPtr& aPtr ) const;

        /** Format Ring Duration Value to 00:00
        *   @param aDesc :  An output string
        *          aSeconds:An input ring duration value in seconds
        *   @return  None
        */
        void FormatRingDuration( TDes& aDesc, TInt aSeconds ) const;
      
        /** Append 2 digit number. If it is one digit number, insert zero before the number 
        *   @param aDesc :  An output string
        *          aNum:    A number to be appended
        *   @return  None
        */
        void AppendNum( TDes& aDesc, TInt aNum ) const;

        /** Append char if it is valid
        *   @param aDesc :  An output string
        *          aCh:     A character to be appended
        *   @return  None
        */
        inline void AppendChar( TDes& aDesc, TChar aCh ) const;
  
        // no use currently, use it just incase the Phone's implementation changes
        /** Evaluate the icon type from virtual pbk eng field type
        *   @param aNumberFieldType:  a VPbk number field type
        *   @return  the icon type
        */
        RecentListIconArrayIcons NumberIconTypeFromVPbk( TInt aNumberFieldType ) const;
        
        /** Evaluate the icon type from phonebook field type
        *   @param aNumberFieldType:  a phone number field type
        *   @return  the icon type
        */
        RecentListIconArrayIcons NumberIconTypeFromPbkField( TInt aNumberFieldType ) const;
        

    private: //data
    
        /// Ref: reference to recent model
        MLogsModel*             iModel;

     
        
        //For ring duation feature
        /// a Pointer to shared data
        MLogsSharedData&        iSharedData;
        // A key getting from central repository to decide whether to show ring duration or not 
        TBool                   iShowRingDuration;
         

        /// Own: Conversion buffer 
        HBufC*                  iBuffer;
        /// Own: Resource string
        HBufC*                  iDateResource;  // QTN_DATE_WITHOUT_YEAR_WITH_ZERO 
        /// Own: Resource string
        HBufC*                  iTimeResource;  // R_LOGS_STM_TIME_LOC_STRING 
        /// Own: Strings for various phone number types
        HBufC*                  iPrivateNumber;
        HBufC*                  iUnknownNumber;
        HBufC*                  iPayphoneNumber;
        HBufC*                  iEmergencyCall;
        HBufC*                  iSATNumber;
        HBufC* iGroupCall;        

        /// Ref: control extension
        MLogsUiControlExtension* iControlExtension;

        /// Own: object with groupping, clipping and A&H functionality
        CPhoneNumberFormat*     iPhoneNumber;

        /// Ref: reference to the list box owned by the control container
        CEikFormattedCellListBox*   iListBox;
        
        /// Own: Offset to own icon .
        TInt iLastOwnIconOffset;

        /// Own: Array of TInt's containing maximum pixel length for displayded rows.
        TInt* iTextWidth;

        /// Ref: 
        const CFont* iListboxFont;  

        /// Ref:         
        CLogsEngine* iEngine;       
    };

#endif  // __CLogsRecentListAdapter_H__
           
// End of File
