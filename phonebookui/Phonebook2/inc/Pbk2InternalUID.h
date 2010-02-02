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
* Description:  Phonebook 2 application internal UIDs.
*
*/


#ifndef PBK2INTERNALUID_H
#define PBK2INTERNALUID_H

/// Phonebook 2 application server UI service UID
#define KPbk2UIService 0x10207278

/// Phonebook 2 server application UID
#define KPbk2ServerAppUID3 0x10207277

/// Phonebook 2 UI Services ECom plugin
#define KPbk2UIServicesUID 0x101F87BE

/// Phonebook 2 UI Policy implementation DLL UID3
#define KPbk2UIPolicyUID3 0x101f87b2

/// Speed dial application UID3
#define KSpeedDialUid 0x1000590A

/// Phonebook 2 local variation settings file UID
#define KPbk2LocalVariationUID 0x101f859a

/// Custom field content type UID 1
#define KPbk2CustomContentTypeUid1 0x101f4cf1

/// Custom field content type UID 2
#define KPbk2CustomContentTypeUid2 0x101f4cf2

/// Custom field content type UID 3
#define KPbk2CustomContentTypeUid3 0x101F85A6

/// Custom field content type UID 4
#define KPbk2CustomContentTypeUid4 0x101F85A7

/// Implementation UIDs for AIW providers
#define KPbk2AiwCmdSelectImplementationUID 0x1020728e
#define KPbk2AiwCmdAssignImplementationUID 0x1020728f

/// Phonebook migration extension plugin interface UID
#define KPbk2MigrationSupportInterfaceUID 0x102823ea

/// Phonebook Migration support plugin
#define KPbk2MigrationSupportImplementationUID 0x102823eb

/// Phonebook 2 All contacts view sort order provider plugin implementation UID
#define KPbk2AllContactsViewSortOrderProviderImplementationUID    0x10283313

// Phonebook 2 Contacts RemoteContactLookup setting Extension implementation UID 
#define  KFscRclSettingExtensionImplementationUID             0x2001F464         

/// Control command item extension
#define KPbk2ControlCmdItemExtensionUID	0x2

/// Names list control extension extension uid
#define KPbk2ContactUiControlExtensionExtensionUID 0x3

/// Phonebook 2 Context launch request's custom view message uid
#define KPbk2ContextLaunchCustomMessageUID 0x20029F44

//
// Common system UIDs
// For some reason these are not available in .mmp usable form (macros)
// in Symbian OS SDK.
//
#define KEPOCNullUID 0x0
#define KEPOCAppUID2 0x100039ce
#define KEPOCStaticDllUID2 0x1000008d
#define KEComRecognitionUID2 0x10009D8D

#endif // PBK2INTERNALUID_H

// End of File
