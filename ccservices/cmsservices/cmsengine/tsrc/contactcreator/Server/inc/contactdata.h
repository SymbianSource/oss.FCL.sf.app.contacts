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

const TInt KNumberOfDefaultContacts     = 8;
const TInt KNumberOfDefaultImppContacts = 4;
const TInt KMobilePhoneCount            = 4;

const TInt KNumberOfXSPContacts         = 4;
const TInt KMobilePhoneCountXSP         = 2;


static const TText* const KFirstNameArray[] = {
    _S( "Veikko" ),
    _S( "Charles" ),
    _S( "Elvis" ),
    _S( "Urho" ),
    _S( "Tester1" ),
    _S( "Tester2" ),
    _S( "Tester3" ),
    _S( "Tester4" ) };

static const TText* const KLastNameArray[] = {
    _S( "Vennamo" ),
    _S( "Manson" ),
    _S( "Presley" ),
    _S( "Kekkonen" ),
    _S( "All" ),
    _S( "None" ),
    _S( "Ovi" ),
    _S( "Google" ) };

static const TText* const KMobileNumberArray[] = {
    _S( "+358504563271" ),
    _S( "+358440254892" ),
    _S( "+358402548963" ),
    _S( "+358500547894" ),
    _S( "+358504563275" ),
    _S( "+358440254896" ),
    _S( "+358402548967" ),
    _S( "+358500547898" ),
    _S( "+358504563279" ),
    _S( "+358440254810" ),
    _S( "+358402548911" ),
    _S( "+358500547812" ),
    _S( "+358504563213" ),
    _S( "+358440254814" ),
    _S( "+358402548915" ),
    _S( "+358500547816" ) };

static const TText* const KLandNumberArray[] = {
    _S( "+358345632712" ),
    _S( "+358192548921" ),
    _S( "+358162548963" ),
    _S( "+358275478942" ),
    _S( "+358275479010" ),
    _S( "+358275479021" ),
    _S( "+358275479035" ),
    _S( "+358275479044" ) };

static const TText* const KVideoNumberArray[] = {
    _S( "+358345632712" ),
    _S( "+35819254892" ),
    _S( "+358162548963" ),
    _S( "+35827547894" ) };


static const TText* const KEmailAddressArray[] = {
    _S( "huuhaa" ),
    _S( "charlie@manson.net" ),
    _S( "epresley@memphis.com" ),
    _S( "urkki@stasi.de" ) };

static const TText* const KVoIPAddressArray[] = {
    _S( "ECE_Demo:VoIP_1@ece.com" ),
    _S( "ECE_Demo:VoIP_2@ece.com" ),
    _S( "ECE_Demo:VoIP_3@ece.com" ),
    _S( "ECE_Demo:VoIP_4@ece.com" ),
    _S( "" ),
    _S( "" ),
    _S( "" ),
    _S( "" ) };

static const TText* const KSipAddressArray[] = {
    _S( "huuhaa" ),
    _S( "sip:charlie@manson.net" ),
    _S( "sip:epresley@memphis.com" ),
    _S( "sip:urkki@stasi.de" ) };

static const TText* const KThumbnailImageArray[] = {
    _S( "\\TestFramework\\cmstester\\kuva1.jpg" ),
    _S( "\\TestFramework\\cmstester\\kuva2.jpg" ),
    _S( "\\TestFramework\\cmstester\\kuva3.jpg" ),
    _S( "\\TestFramework\\cmstester\\kuva4.jpg" ) };

// The first two persons have an empty by purpose.
static const TText* const KImppAddressArray[] = {
    _S( "foo:veikko@foobar.com" ),
    _S( "msn:charles@manson.com" ),
    _S( "msn:epresley@memphis.com" ),
    _S( "msn:urkki@msn.com" ),
    _S( "msn:Chatter1@msn.com" ),
    _S( "" ),
    _S( "" ),
    _S( "" ) };


static const TText* const KGoogleAddressArray[] = {
    _S( "" ),
    _S( "" ),
    _S( "" ),
    _S( "" ),
    _S( "google:Tester1@google.com" ),
    _S( "" ),
    _S( "" ),
    _S( "google:Tester4@google.com" ) };

static const TText* const KOviAddressArray[] = {
    _S( "" ),
    _S( "" ),
    _S( "" ),
    _S( "" ),
    _S( "OVI:Tester1@ovi.com" ),
    _S( "" ),
    _S( "OVI:Tester3@ovi.com" ),
    _S( "" ) };

// ******************************************  XSP Contacts ****************************************************

static const TText* const KFirstNameArrayXSP[] = {
    _S( "XSP" ),
    _S( "XSP" ),
    _S( "XSP" ),
    _S( "XSP" ) };

static const TText* const KLastNameArrayXSP[] = {
    _S( "Contact1" ),
    _S( "Contact2" ),
    _S( "Contact3" ),
    _S( "Contact4" ) };

static const TText* const KMobileNumberArrayXSP[] = {
    _S( "+358504560001" ),
    _S( "+358504560002" ),
    _S( "+358504560003" ),
    _S( "+358504560004" ),
    _S( "+358504560005" ),
    _S( "+358504560006" ),
    _S( "+358504560007" ),
    _S( "+358504560008" ) };

static const TText* const KLandNumberArrayXSP[] =          {
    _S( "+358504560011" ),
    _S( "+358504560012" ),
    _S( "+358504560013" ),
    _S( "+358504560014" ) };

static const TText* const KEmailAddressArrayXSP[] = {
    _S( "contact1@xsp.com" ),
    _S( "contact2@xsp.com" ),
    _S( "contact3@xsp.com" ),
    _S( "contact4@xsp.com" ) };

// The first two persons have an empty by purpose.
static const TText* const KImppAddressArrayXSP[] = {
    _S( "xsp:veikko@foobar.com" ),
    _S( "msn:charles@manson.com" ),
    _S( "msn:epresley@memphis.com" ),
    _S( "msn:urkki@msn.com" ) };

#endif // __CONTACTDATA_H__

// End of File
