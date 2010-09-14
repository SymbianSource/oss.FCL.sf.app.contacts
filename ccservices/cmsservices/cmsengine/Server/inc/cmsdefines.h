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
* Description: 
*       
*
*/


#ifndef __CMSDEFINES__
#define __CMSDEFINES__

// INCLUDES
#include <e32base.h>
#include <vpbkeng.rsg>


//The types for voice calls
const TInt KCmsVoiceCallGroup[] = {   R_VPBK_FIELD_TYPE_LANDPHONEGEN,
                                      R_VPBK_FIELD_TYPE_LANDPHONEHOME,
                                      R_VPBK_FIELD_TYPE_LANDPHONEWORK,
                                      R_VPBK_FIELD_TYPE_MOBILEPHONEGEN,
                                      R_VPBK_FIELD_TYPE_MOBILEPHONEWORK,
                                      R_VPBK_FIELD_TYPE_MOBILEPHONEHOME };

//The types for messaging
const TInt KCmsMsgGroup[] =        {  R_VPBK_FIELD_TYPE_MOBILEPHONEGEN,
                                      R_VPBK_FIELD_TYPE_MOBILEPHONEWORK,
                                      R_VPBK_FIELD_TYPE_MOBILEPHONEHOME,
                                      R_VPBK_FIELD_TYPE_LANDPHONEGEN,
                                      R_VPBK_FIELD_TYPE_LANDPHONEHOME,
                                      R_VPBK_FIELD_TYPE_LANDPHONEWORK,
                                      R_VPBK_FIELD_TYPE_EMAILGEN,
                                      R_VPBK_FIELD_TYPE_EMAILHOME,
                                      R_VPBK_FIELD_TYPE_EMAILWORK };

//The types for VoIP calls
const TInt KCmsVoIPGroup[] =        { R_VPBK_FIELD_TYPE_VOIPWORK,
                                      R_VPBK_FIELD_TYPE_VOIPHOME,
                                      R_VPBK_FIELD_TYPE_VOIPGEN };

//The types for email addresses
const TInt KCmsEmailAddressGroup[] = { R_VPBK_FIELD_TYPE_EMAILGEN,
                                       R_VPBK_FIELD_TYPE_EMAILHOME,
                                       R_VPBK_FIELD_TYPE_EMAILWORK };

#endif  //__CMSDEFINES__

// End of File

