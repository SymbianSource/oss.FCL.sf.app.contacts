/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Contains the array of virtual phonebook resource ids used
*                to map phonebook fields to XML tags.
*
*/

#ifndef PDCXMLMAPPING_H
#define PDCXMLMAPPING_H

// System includes 
#include <VPbkEng.rsg>

// User includes
#include "pdcstringconstants.h"     // PdcStringTable

// Constants

// Array of virtual phonebook resource ids. The order of the resource ids
// has to match to the order of strings in the string table. Any string in
// the string table that does not map to a resource id has to represented
// with a -1.
const TInt KFieldResIdLookupTable[] = 
    {
	/** EContact */
	-1,
	/** EFirstName */
	R_VPBK_FIELD_TYPE_FIRSTNAME,
	/** ELastName */
	R_VPBK_FIELD_TYPE_LASTNAME,
	/** ECompany */
	R_VPBK_FIELD_TYPE_COMPANYNAME,
	/** EJob */
	R_VPBK_FIELD_TYPE_JOBTITLE,
	/** EGeneral */
	R_VPBK_FIELD_TYPE_LANDPHONEHOME,
	/** EMobile */
	R_VPBK_FIELD_TYPE_MOBILEPHONEHOME,
	/** EVideoNo */
	R_VPBK_FIELD_TYPE_VIDEONUMBERHOME,
	/** EFax */
	R_VPBK_FIELD_TYPE_FAXNUMBERWORK ,
	/** EEmail */
	R_VPBK_FIELD_TYPE_EMAILHOME,
	/** EWvUserId */
	-1,
	/** EURL */
	R_VPBK_FIELD_TYPE_URLHOME,
	/** EVPbkNonVersitTopContact */
	-1
	//R_VPBK_FIELD_TYPE_TOPCONTACT
    };
	
#endif // PDCXMLMAPPING_H
