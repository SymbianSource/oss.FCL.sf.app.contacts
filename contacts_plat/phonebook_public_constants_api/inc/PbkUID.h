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
*    Common header for Phonebook application UIDs. All subcomponents should
*    use the UID constants defined in this file.
*
*/


#ifndef __PbkUID_H__
#define __PbkUID_H__

//  MACROS

// An UID reserved originally for Phonebook2 application.
// Can be used if both Phonebook1&Phonebook2 applications are wanted to exist
// at the same time. For SDK compatibility reasons the UID of Phonebook2 must
// be the same as the UID of Phonebook1.
// #define KPbkUID3 0x101f87b1
/// Phonebook application UID3
#define KPbkUID3 0x101f4cce


// Phonebook DLL UID3
#define KPbkDllUID3 0x101f4cce

// Sim PD application UID3
#define KSimPdUID3 0x101f4cf6

// SDN application UID3
#define KSdnUID3 0x101f4cf0

// Speeddial application UID3
#define KSpeedDialUid 0x1000590A

// Chinese find plugin UID
#define KFindPluginUID 0x101f85f6

// Phonebook local variation settings file UID
#define KPbkLocalVariationUID 0x101f859a

/// CPbkViewState Uid used with the view server
#define KPbkViewStateUid 0x101f4ccf

/// Custom field content type UID 1
#define KPbkCustomContentTypeUid1 0x101f4cf1

/// Custom field content type UID 2
#define KPbkCustomContentTypeUid2 0x101f4cf2

/// Custom field content type UID 3
#define KPbkCustomContentTypeUid3 0x101F85A6

/// Custom field content type UID 4
#define KPbkCustomContentTypeUid4 0x101F85A7

/// Custom field content type UID 5
#define KPbkCustomContentTypeUid5 0x101F8842

/// Custom field content type UID 6
#define KPbkCustomContentTypeUid6 0x101F8863

// Phonebook Engine extension plugin interface UID
#define KPbkEngineInterfaceUID 0x101f84fd

// Phonebook Engine Field Modification plugin
#define KPbkEngineFMPluginUID 0x101F859C

// Phonebook Engine extension plugin interface UID
#define KPbkSINDHandlerInterfaceUID 0x10207296

// Phonebook SIND Handler plugin
#define KPbkSINDHandlerPluginUID 0x10207297
// Default data string for ECom extension plugins
#define KPbkUiExtensionDefaultDataString "PbkUiExtension"

// Interface definition UID for ECom extension plugins
#define KPbkUiExtensionInterfaceDefinitionUID 0x101F85AF

// MMC UI extension implementation UID
#define KPbkMmcUiExtensionImplementationUID 0x101f85b0

// USIM UI extension implementation UID
#define KPbkUSimUiExtensionImplementationUID 0x101f85b1

// VoiceTag UI extension implementation UID
#define KPbkVoiceTagUiExtensionImplementationUID 0x101f85b2

// SyncML UI extension implementation UID
#define KPbkSyncMLUiExtensionImplementationUID 0x101f85b3

// CDMA UI extension implementation UID
#define KPbkCDMAUiExtensionImplementationUID 0x101f85c6

// Phonebook AIW providers ECom plugin
#define KPbkAiwProvidersPluginUID 0x101F85BA

// S60 specific contact field type containing text call object data
#define KUidContactFieldCodTextValue 0x101F8840

// S60 specific contact field type containing image call object data
#define KUidContactFieldCodImageValue 0x101F8841

//
// Common system UIDs
// For some reason these are not available in .mmp usable form (macros)
// in Symbian OS SDK.
//
#define KEPOCNullUID 0x0
#define KEPOCAppUID2 0x100039ce
#define KEPOCStaticDllUID2 0x1000008d
#define KEComRecognitionUID2 0x10009D8D

#endif // __PbkUID_H__

// End of File
