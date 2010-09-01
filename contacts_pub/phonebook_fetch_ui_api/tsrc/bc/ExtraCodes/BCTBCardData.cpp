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








// INCLUDE FILES
#include    "BCTBCardData.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CBCTBCardData::CBCTBCardData()
    {
    }
    
// Destructor
CBCTBCardData::~CBCTBCardData()
    {
    ResetData();
    }

/**
* Delete all dataitems.
*/ 
void CBCTBCardData::ResetData()
    {
    delete  iLastName;
    iLastName = NULL;

    delete  iFirstName;
    iFirstName = NULL;

    delete  iPhoneNumberStandard;
    iPhoneNumberStandard = NULL;
    
    delete  iPhoneNumberHome;
    iPhoneNumberHome = NULL;
    
    delete  iPhoneNumberWork;
    iPhoneNumberWork = NULL;
    
    delete  iPhoneNumberMobile;
    iPhoneNumberMobile = NULL;

    delete  iPhoneNumberMobileWork;
    iPhoneNumberMobileWork = NULL;
    
    delete  iPhoneNumberMobileHome;
    iPhoneNumberMobileHome = NULL;

    delete  iFaxNumber;
    iFaxNumber = NULL;
    
    delete  iFaxNumberWork;
    iFaxNumberWork = NULL;

    delete  iFaxNumberHome;
    iFaxNumberHome = NULL;

    delete  iPagerNumber;
    iPagerNumber = NULL;
    
    delete  iEmailAddress;
    iEmailAddress = NULL;
    
    delete  iEmailAddressWork;
    iEmailAddressWork = NULL;

    delete  iEmailAddressHome;
    iEmailAddressHome = NULL;

    delete  iPostalAddress;
    iPostalAddress = NULL;

    delete  iPostalAddressHome;
    iPostalAddressHome = NULL;
    
    delete  iURL;
    iURL = NULL;

    delete  iURLHome;
    iURLHome = NULL;

    delete  iURLWork;
    iURLWork = NULL;
    
    delete  iJobTitle;
    iJobTitle = NULL;
    
    delete  iCompanyName;
    iCompanyName = NULL;

    delete  iCompanyAddress;
    iCompanyAddress = NULL;
    
    delete  iDate;
    iDate = NULL;
    
    delete  iNote;
    iNote = NULL;
    
    delete  iPicture;
    iPicture = NULL;
    
    delete  iThumbnailImage;
    iThumbnailImage = NULL;

    delete iPOBox;
    iPOBox = NULL;

    delete iExtensonAdr;
    iExtensonAdr = NULL;

    delete iStreet;
    iStreet = NULL;

    delete iPostalCode;
    iPostalCode = NULL;

    delete iCity;
    iCity = NULL;

    delete iState;
    iState = NULL;

    delete iCountry;
    iCountry = NULL;

    delete iPOBoxHome;
    iPOBoxHome = NULL;

    delete iExtensonAdrHome;
    iExtensonAdrHome = NULL;

    delete iStreetHome;
    iStreetHome = NULL;

    delete iPostalCodeHome;
    iPostalCodeHome = NULL;

    delete iCityHome;
    iCityHome = NULL;

    delete iStateHome;
    iStateHome = NULL;

    delete iCountryHome;
    iCountryHome = NULL;

    delete iPOBoxWork;
    iPOBoxWork = NULL;

    delete iExtensonAdrWork;
    iExtensonAdrWork = NULL;

    delete iStreetWork;
    iStreetWork = NULL;

    delete iPostalCodeWork;
    iPostalCodeWork = NULL;

    delete iCityWork;
    iCityWork = NULL;

    delete iStateWork;
    iStateWork = NULL;

    delete iCountryWork;
    iCountryWork = NULL;
    }

//  End of File  
