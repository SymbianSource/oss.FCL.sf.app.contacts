/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*/


#ifndef VPBKCNTMODELUID_H
#define VPBKCNTMODELUID_H

//////////////////////////////////////////////////////////////////////////
//
// Field type UIDs
//
//////////////////////////////////////////////////////////////////////////

/// Custom field content type UID 1
#define KVPbkCustomContentTypeUid1           0x101f4cf1

/// Custom field content type UID 2
#define KVPbkCustomContentTypeUid2           0x101f4cf2

/// Custom field content type UID 3
#define KVPbkCustomContentTypeUid3           0x101F85A6

/// Custom field content type UID 4
#define KVPbkCustomContentTypeUid4           0x101F85A7

/// Custom field content type UID 5
#define KVPbkCustomContentTypeUid5           0x101F8842

/// Custom field content type UID 6
#define KVPbkCustomContentTypeUid6           0x101F8863

/// Custom field content type UID 7
#define KVPbkCustomContentTypeUid7           0x2001E63F

/// Custom field content type UID 8
#define KVPbkCustomContentTypeUid8           0x2001E640


/// default phone number and email field type
#define KIntVPbkDefaultFieldPref            KIntContactFieldVCardMapPREF
/// default video number field type
#define KIntVPbkDefaultFieldVideo           KVPbkCustomContentTypeUid3
/// default sms field type
#define KIntVPbkDefaultFieldSms             KVPbkCustomContentTypeUid1
/// default mms field type
#define KIntVPbkDefaultFieldMms             KVPbkCustomContentTypeUid2
/// default email over sms field type
#define KIntVPbkDefaultFieldEmailOverSms    KVPbkCustomContentTypeUid4
/// Integer UID for POC default.
#define KIntVPbkDefaultFieldPoc             KVPbkCustomContentTypeUid5
/// Integer UID for VOIP default.
#define KIntVPbkDefaultFieldVoip            KVPbkCustomContentTypeUid6
/// Integer UID for chat default.
#define KIntVPbkDefaultFieldChat            KVPbkCustomContentTypeUid7
/// Integer UID for url default.
#define KIntVPbkDefaultFieldOpenLink        KVPbkCustomContentTypeUid8

/**
 * Phonebook custom Field value.
 * Locationing privacy field type.
 */
#define KPbkUidContactFieldLocationPrivValue    0x101F4FF2

/**
 * Phonebook custom Field value.
 * Top Contact field type.
 */
#define KUidContactFieldTopContactValue 	0x200100E3

/**
 * Phonebook custom Field value.
 * IMPP field type.
 */
#define KUidContactFieldIMPPValue           0x200100E4

#endif // VPBKCNTMODELUID_H
//End of file

