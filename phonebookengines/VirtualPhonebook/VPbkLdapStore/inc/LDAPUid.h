/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains LDAP Provider Uid definitions.
*
*/

#ifndef __LDAPUID_H__
#define __LDAPUID_H__

// -----------------------------------------------------------------------------
// LDAP Provider application and dll uids
// -----------------------------------------------------------------------------
#define KLDAPTestEngineUID                  0x101F8F33
#define KLDAPSettingsUID                    0x101F8F34
#define KLDAPCentralRepositoryUID           0x101F8F35
#define KLDAPEngineUID                      0x101F8F3E
#define KLDAPUIExtensionUID                 0x101F8F3F

// -----------------------------------------------------------------------------
// LDAP Store domain (ECOM) implementation uid
// - Virtual Phonebook contact store domain interface implentation
// -----------------------------------------------------------------------------
#define KLDAPStoreDomainImplementationUID   0x101F8F36  

// -----------------------------------------------------------------------------
// LDAP Provider Client Provisioning (CP) and Device Management (DM) ECOM uids
// -----------------------------------------------------------------------------
#define KLDAPOmaCPImplementationUID         0x101F8F40
#define KLDAPOmaDMImplementationUID         0x101F8F41
#define KLDAPOmaCPDllUid3                   0x101F8FB7
#define KLDAPOmaDMDllUid3                   0x101F8FB6

// -----------------------------------------------------------------------------
// LDAP Provider finder interface and implementation uid
// -----------------------------------------------------------------------------
#define KLDAPFinderInterfaceUID             0x101F8F55
#define KLDAPFinderImplementationUID        0x101F8F56

// -----------------------------------------------------------------------------
// Central repository uids are in separate file (see specification)
// -----------------------------------------------------------------------------
#endif  // End of if __LDAPUID_H__
