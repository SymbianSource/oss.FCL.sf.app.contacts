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
*       Utility class for phone number formatting.
*
*/


#ifndef __CPhoneNumberFormat_H__
#define __CPhoneNumberFormat_H__

//  INCLUDES
#include <e32base.h>
#include <AknUtils.h>
#include <NumberGrouping.h>

// CONSTANTS

// MACROS

// DATA TYPES
enum TFormatResult
    {
    EResultOK,          // No loss of precision on formatting.
    EResultTruncated,   // result descriptor truncated from the start.
    EResultClipped,     // result descriptor truncated and extra dots added to start
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFont;
class CEikFormattedCellListBox;
class CEikColumnListBox;

// CLASS DECLARATION

/**
*       Class for phone number formatting
*/
class CPhoneNumberFormat :public CBase
        {
        public:  // Constructors and destructor

        /**
        * Symbian OS constructor
        * @return new object
        */
        static CPhoneNumberFormat* NewL();

        /**
        * Destructor.
        */
        virtual ~CPhoneNumberFormat();  

        public: 
        /**
        * Strip DTMF post and prefix from phone number
        * @param aNumberToFormat, input buffer
        * @param aFormattedString, output buffer
        * @return KErrNone if number stripped ok or nothing to strip
        * @return KErrParameter if invalid number given for formatter. 
        *                       In this error situation output buffer = input buffer.
        */
        static TInt DTMFStrip( TPtrC aNumberToFormat, TDes& aFormattedString );

        /**
        * Identify DTMF character(w,W,p,P).
        */
        static TInt IsDtmf( TChar aChar );

        /**
        * Identify SS character(*,#).
        */
        static TInt IsSS( TChar aChar );


        /**
        * Phone number formatting
        * @param aNumberToFormat phone number in string to format
        * @param aFormattedString formatted number
        * @return   EResultOK           
        *           EResultTruncated
        *           EResultClipped
        */
        TFormatResult PhoneNumberFormat
                        (       TPtrC aNumberToFormat
                        ,       TDes& aFormattedString
                        ); 

        /**
        * Phone number formatting
        * @param aNumberToFormat phone number in string to format
        * @param aFormattedString formatted number
        * @param aFont 
        * @param aPixelWidth width in pixels, long numbers are clipped from the beginging 
        *                    of descriptor.
        * @return   EResultOK           
        *           EResultTruncated
        *           EResultClipped
        */
        TFormatResult PhoneNumberFormat
                        (       TPtrC aNumberToFormat
                        ,       TDes& aFormattedString
                        ,       const CFont* aFont
                        ,       TInt aPixelWidth
                        ); 

        TFormatResult PhoneNumberFormat
                        (       TPtrC aNumberToFormat
                        ,       TDes& aFormattedString
                        ,       AknTextUtils::TClipDirection aClipDirection
                        ,       CEikFormattedCellListBox *aListBox
                        ,       TInt aItemIndex
                        ,       TInt aSubCellNumber
                        ); 

        TFormatResult PhoneNumberFormat
                        (       TPtrC aNumberToFormat
                        ,       TDes& aFormattedString
                        ,       AknTextUtils::TClipDirection aClipDirection
                        ,       CEikColumnListBox *aListBox
                        ,       TInt aItemIndex
                        ,       TInt aColumnNumber
                        ); 

        private:    
        
                //Constructors are private by default
                CPhoneNumberFormat();
                        void ConstructL();
        
                TFormatResult DescriptorCopy( const TDesC& aSource, TDes& aDestination );
                
        private: // Data
                /// Own: grouping number grouping
                CPNGNumberGrouping* iNumberGrouping;
        };

#endif // __CPhoneNumberFormat_H__
            
// End of File
