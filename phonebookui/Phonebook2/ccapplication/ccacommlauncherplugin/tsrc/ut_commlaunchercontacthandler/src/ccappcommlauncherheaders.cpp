/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: implementation to help unit testing
*/


#include "ccappcommlauncherheaders.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCmsContactField::ItemL()
// -----------------------------------------------------------------------------
//
const CCmsContactFieldItem& CCmsContactField::ItemL( TInt /*aIndex*/) const 
    {
    // (*NULL) was allowed by symbian.So its didn't crash. 
    return *iCmsContactFieldItem;
    }

// -----------------------------------------------------------------------------
// CCmsContactField::Type()
// -----------------------------------------------------------------------------
//
CCmsContactFieldItem::TCmsContactField CCmsContactField::Type() const
    {
    CCmsContactFieldItem::TCmsContactField ret = 
            CCmsContactFieldItem::ECmsMobilePhoneWork;;
    switch ( iType )
        {
        // phone
        case PHONE:
            {
            ret = CCmsContactFieldItem::ECmsLandPhoneHome;
            break;
            }
        // email
        case EMAIL:
            {
            ret = CCmsContactFieldItem::ECmsEmailWork;
            break;
            }
       // voip
        case VOIP:
            {
            ret = CCmsContactFieldItem::ECmsVoipNumberHome;
            break;
            }
        // phone
        case PHONE2:
            {
            ret = CCmsContactFieldItem::ECmsMobilePhoneHome;
            break;
            }
        // email
        case EMAIL2:
            {
            ret = CCmsContactFieldItem::ECmsEmailGeneric;
            break;
            }
       // voip
        case VOIP2:
            {
            ret = CCmsContactFieldItem::ECmsVoipNumberWork;
            break;
            }
        // name
        case NAME:
            {
            ret = CCmsContactFieldItem::ECmsLastName;
            break;
            }
        // default phone
        default:
            {
            ret = CCmsContactFieldItem::ECmsMobilePhoneWork;
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CCmsContactField::HasDefaultAttribute()
// -----------------------------------------------------------------------------
//
TInt CCmsContactField::HasDefaultAttribute() const
    { 
    TInt ret ( KErrNotFound );
    switch ( iType )
        {
        // phone
        case PHONE:
            {
            ret = CCmsContactFieldItem::ECmsDefaultTypePhoneNumber;
            break;
            }
        // email
        case EMAIL:
            {
            ret = CCmsContactFieldItem::ECmsDefaultTypeEmail;
            break;
            }
        // voip
        case VOIP:
            {
            ret = CCmsContactFieldItem::ECmsDefaultTypeVoIP;
            break;
            }
       // mms
        default:
            {
            ret = CCmsContactFieldItem::ECmsDefaultTypeMms;
            break;
            }
        }

    return ret;
    
    }

// -----------------------------------------------------------------------------
// CCmsContactFieldInfo::~CCmsContactFieldInfo()
// -----------------------------------------------------------------------------
//
CCmsContactFieldInfo::~CCmsContactFieldInfo()
    {
    iArray->Close();
    delete iArray;
    }

// -----------------------------------------------------------------------------
// CCmsContactFieldInfo::NewL()
// -----------------------------------------------------------------------------
//
CCmsContactFieldInfo* CCmsContactFieldInfo::NewL()
    {
    CCmsContactFieldInfo* self = new (ELeave) CCmsContactFieldInfo();
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCmsContactFieldInfo::ConstructL()
// -----------------------------------------------------------------------------
//
void CCmsContactFieldInfo::ConstructL()
    {
    iArray = new (ELeave) RArray<CCmsContactFieldItem::TCmsContactField>();
    // phone
    iArray->Append( CCmsContactFieldItem::ECmsMobilePhoneHome );
    // email
    iArray->Append( CCmsContactFieldItem::ECmsEmailWork );
    // voip
    iArray->Append( CCmsContactFieldItem::ECmsVoipNumberHome );
    // not number
    iArray->Append( CCmsContactFieldItem::ECmsLastName );
    }

// -----------------------------------------------------------------------------
// CCmsContactFieldInfo::AddMoreFields()
// -----------------------------------------------------------------------------
//
void CCmsContactFieldInfo::AddMoreFields()
    {
    // phone
    iArray->Append( CCmsContactFieldItem::ECmsLandPhoneHome );
    iArray->Append( CCmsContactFieldItem::ECmsMobilePhoneGeneric );
    iArray->Append( CCmsContactFieldItem::ECmsMobilePhoneHome );
    // email
    iArray->Append( CCmsContactFieldItem::ECmsEmailGeneric );
    iArray->Append( CCmsContactFieldItem::ECmsEmailHome );
    // voip
    iArray->Append( CCmsContactFieldItem::ECmsVoipNumberWork );
    // im
    iArray->Append( CCmsContactFieldItem::ECmsImpp );
    // url
    iArray->Append( CCmsContactFieldItem::ECmsUrlHome );
    }

// -----------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::NewL()
// -----------------------------------------------------------------------------
//
CCCAppCommLauncherPlugin* CCCAppCommLauncherPlugin::NewL()
    {
    CCCAppCommLauncherPlugin* self = new(ELeave)CCCAppCommLauncherPlugin();
    return self;
    }

//  End of File
