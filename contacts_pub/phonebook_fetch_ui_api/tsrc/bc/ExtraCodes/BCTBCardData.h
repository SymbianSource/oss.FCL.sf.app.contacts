/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*       Provides methods for the test dll.
*
*/








#ifndef __BCTBCARDDATA_H__
#define __BCTBCARDDATA_H__

//  INCLUDES
#include <E32BASE.H>

class CBCTBCardData :public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CBCTBCardData();

        
        /**
        * Destructor.
        */
        virtual ~CBCTBCardData();
        void ResetData();

    public:     // Data

        HBufC*  iLastName;
        HBufC*  iFirstName;
        HBufC*  iPhoneNumberStandard;
        HBufC*  iPhoneNumberHome;
        HBufC*  iPhoneNumberWork;
        HBufC*  iPagerNumber;
        
        HBufC*  iPhoneNumberMobile;
        HBufC*  iPhoneNumberMobileWork;
        HBufC*  iPhoneNumberMobileHome;
        
        HBufC*  iFaxNumber;
        HBufC*  iFaxNumberWork;
        HBufC*  iFaxNumberHome;

        HBufC*  iEmailAddress;
        HBufC*  iEmailAddressWork;
        HBufC*  iEmailAddressHome;

        HBufC*  iPostalAddress;
        HBufC*  iPostalAddressHome;
        HBufC*  iCompanyAddress;

        HBufC*  iURL;
        HBufC*  iURLWork;
        HBufC*  iURLHome;

        HBufC*  iJobTitle;
        HBufC*  iCompanyName;
        HBufC*  iDate;
        HBufC*  iNote;
        HBufC*  iPicture;
        HBufC*  iThumbnailImage;
        
        HBufC*  iPOBox;
        HBufC*  iExtensonAdr;
        HBufC*  iStreet;
        HBufC*  iPostalCode;
        HBufC*  iCity;
        HBufC*  iState;
        HBufC*  iCountry;

        HBufC*  iPOBoxHome;
        HBufC*  iExtensonAdrHome;
        HBufC*  iStreetHome;
        HBufC*  iPostalCodeHome;
        HBufC*  iCityHome;
        HBufC*  iStateHome;
        HBufC*  iCountryHome;

        HBufC*  iPOBoxWork;
        HBufC*  iExtensonAdrWork;
        HBufC*  iStreetWork;
        HBufC*  iPostalCodeWork;
        HBufC*  iCityWork;
        HBufC*  iStateWork;
        HBufC*  iCountryWork;
    };

#endif // __BCTBCARDDATA_H__
            
// End of File
