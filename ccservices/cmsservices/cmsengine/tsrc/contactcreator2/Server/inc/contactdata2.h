/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       
*
*/


#ifndef __CONTACTDATA_H__
#define __CONTACTDATA_H__

#include <e32def.h>

const TInt KMobilePhoneCount							= 4;
#ifdef _MASS_TEST
const TInt KNumberOfContacts					        = 50;
#else
const TInt KNumberOfContacts					        = 5;
#endif

static const TText* const KFirstNameArray[] = 			{ _S( "Veikko" ), _S( "Charles" ), _S( "Elvis" ), _S( "Urho" ),  _S( "Charlie" ) };
static const TText* const KLastNameArray[] = 			{ _S( "Vennamo" ), _S( "Manson" ), _S( "Presley" ), _S( "Kekkonen" ),  _S( "Brown" ) };

static const TText* const KMobileNumberArray[] = 		{ _S( "+358504563271" ), _S( "+358440254892" ),
														  _S( "+358402548963" ), _S( "+358500547894" ),
														  _S( "+358504563275" ), _S( "+358440254896" ),
														  _S( "+358402548967" ), _S( "+358500547898" ),
														  _S( "+358504563279" ), _S( "+358440254810" ),
														  _S( "+358402548911" ), _S( "+358500547812" ),
														  _S( "+358504563213" ), _S( "+358440254814" ),
														  _S( "+358402548915" ), _S( "+358500547816" ),
                                                          _S( "+358501" ), _S( "+358502" ),
                                                          _S( "+358503" ), _S( "+358504" ) };


static const TText* const KLandNumberArray[] = 			{ _S( "+358345632712" ), _S( "+35819254892" ),
		                                                  _S( "+358162548963" ), _S( "+35827547894" ),
                                                          _S( "+35827547901" )};

static const TText* const KVideoNumberArray[] = 	    { _S( "+358345632712" ), _S( "+35819254892" ),
		                                                  _S( "+358162548963" ), _S( "+35827547894" ),
                                                          _S( "+35827547902" )};


static const TText* const KEmailAddressArray[] =        { _S( "huuhaa" ), _S( "charlie@manson.net" ),
													      _S( "epresley@memphis.com" ), _S( "urkki@stasi.de" ), 
                                                          _S( "charlie@brown.com" )};

static const TText* const KVoIPAddressArray[] =         { _S( "sip:VoIP_1@ece.com" ), _S( "sip:VoIP_2@ece.com" ),
													      _S( "sip:VoIP_3@ece.com" ), _S( "sip:VoIP_4@ece.com" ),
                                                          _S( "sip:VoIP_5@ece.com" )};

static const TText* const KSipAddressArray[] =    		{ _S( "huuhaa" ), _S( "sip:charlie@manson.net" ),
	      												  _S( "sip:epresley@memphis.com" ), _S( "sip:urkki@stasi.de" ), _S("") };

static const TText* const KThumbnailImageArray[] =    	{ _S( "\\TestFramework\\cmstester\\kuva1.jpg" ),
														  _S( "\\TestFramework\\cmstester\\kuva2.jpg" ),
														  _S( "\\TestFramework\\cmstester\\kuva3.jpg" ),
														  _S( "\\TestFramework\\cmstester\\kuva4.jpg" ),
                                                          _S("") };

// The first two persons have an empty by purpose.
static const TText* const KImppAddressArray[] =          { _S( "" ), _S( "" ),
        _S( "msn:epresley@memphis.com" ), _S( "msn:urkki@msn.com" ), _S( "msn:charlie@msn.com" ), };
          
// The first two persons have an empty by purpose.
static const TText* const KGtalkAddressArray[] =          { _S( "" ), _S( "" ),
          _S( "gtalk:elvis@gtalk.com" ), _S( "gtalk:urkki@gtalk.com" ), _S( "gtalk:charlie@gtalk.com" ) };          


#endif // __CONTACTDATA_H__

// End of File
