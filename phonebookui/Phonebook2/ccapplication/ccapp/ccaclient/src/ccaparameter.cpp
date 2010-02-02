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
* Description:  This is a class for creating data of contact 
*                for launching CCA application.
*
*/

#include <e32std.h>

#include "ccaclientheaders.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCCAParameter::NewL
// ----------------------------------------------------------
//
EXPORT_C CCCAParameter* CCCAParameter::NewL( )
    {
    CCCAParameter* self = new (ELeave) CCCAParameter( );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------
// CCCAParameter::~CCCAParameter
// ----------------------------------------------------------
//
CCCAParameter::~CCCAParameter()
    {
    delete iContactData;
    }


// STREAMERS
// ---------------------------------------------------------
// CCCAParameter::ExternalizeL
// ---------------------------------------------------------
//
EXPORT_C void CCCAParameter::ExternalizeL( RWriteStream& aStream ) const
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::ExternalizeL()"));    

   	TInt dataProvidedFlag = 0;
    if ( iConnectionFlag != ENormal )
        {
        dataProvidedFlag |= EDataConnectionFlag;
        }
    if ( iContactDataFlag != EContactNone )
        {
        dataProvidedFlag |= EDataContactDataFlag;
        }
    if ( iContactData )
        {
        dataProvidedFlag |= EDataContactData;
        }
    if ( iUid !=  TUid::Null() )
        {
        dataProvidedFlag |= EDataViewUid;
        }
    if ( iVersion != KErrNotFound )
        {
        dataProvidedFlag |= EDataVersion;	
        }
    
    // write to stream
    aStream.WriteInt32L( dataProvidedFlag );
    if ( dataProvidedFlag & EDataConnectionFlag )
        {
        aStream.WriteInt32L( iConnectionFlag );
        }
    if ( dataProvidedFlag & EDataContactDataFlag )
        {
        aStream.WriteInt32L( iContactDataFlag );
        }
    if ( dataProvidedFlag & EDataContactData )
        {
        aStream.WriteInt32L(iContactData->Des().MaxLength());
        aStream << *iContactData;
        }	  
    if ( dataProvidedFlag & EDataViewUid )
        {
        aStream.WriteInt32L( iUid.iUid );
        }
    if ( dataProvidedFlag & EDataVersion )
        {
        aStream.WriteInt32L( iVersion );
        }

    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::ExternalizeL(): Done."));            
    return; 
    }

// ---------------------------------------------------------
// CCCAParameter::InternalizeL
// ---------------------------------------------------------
//    
EXPORT_C void CCCAParameter::InternalizeL( RReadStream& aStream )
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::InternalizeL()"));            

    TInt maxlen = 0;
    TInt dataProvidedFlag = aStream.ReadInt32L();

    if ( dataProvidedFlag & EDataConnectionFlag )
        {
        iConnectionFlag = (TConnectionFlags)aStream.ReadInt32L();
        }
    if ( dataProvidedFlag & EDataContactDataFlag )
        {
        iContactDataFlag = (TContactDataFlags)aStream.ReadInt32L();
        }
    if ( dataProvidedFlag & EDataContactData )
        {
        maxlen = aStream.ReadInt32L();
        
        delete iContactData;
        iContactData = NULL;
        
        iContactData = HBufC::NewL(aStream,maxlen);
        }
    if ( dataProvidedFlag & EDataViewUid )
        {
        iUid.iUid = aStream.ReadInt32L(); 
        }
    if ( dataProvidedFlag & EDataVersion )
        {
        iVersion = aStream.ReadInt32L();
        }

    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::InternalizeL(): Done."));                    
    return; 
    }

// ----------------------------------------------------------
// CCCAParameter::Close
// ----------------------------------------------------------
//
void CCCAParameter::Close( )
    {
    delete this;
    }

// ----------------------------------------------------------
// CCCAParameter::ConnectionFlag
// ----------------------------------------------------------
//
CCCAParameter::TConnectionFlags CCCAParameter::ConnectionFlag()
    {
    return iConnectionFlag; 
    }

// ----------------------------------------------------------
// CCCAParameter::ContactDataFlag
// ----------------------------------------------------------
//
CCCAParameter::TContactDataFlags CCCAParameter::ContactDataFlag()
    {
    return iContactDataFlag; 
    }

// ----------------------------------------------------------
// CCCAParameter::ContactDataL
// ----------------------------------------------------------
//
HBufC& CCCAParameter::ContactDataL()
    {    
    __ASSERT_ALWAYS( iContactData, User::Leave( KErrArgument ));                      
    return *iContactData; 
    }

// ----------------------------------------------------------
// CCCAParameter::LaunchedViewUid
// ----------------------------------------------------------
//
TUid CCCAParameter::LaunchedViewUid()
    {
	return iUid; 
    }

// ----------------------------------------------------------
// CCCAParameter::Version
// ----------------------------------------------------------
//
TInt32 CCCAParameter::Version()
    {
	return iVersion; 
    }

// ----------------------------------------------------------
// CCCAParameter::SetConnectionFlag
// ----------------------------------------------------------
//
void CCCAParameter::SetConnectionFlag( const TConnectionFlags aConnectionFlag )
    {
    iConnectionFlag = aConnectionFlag;
    return; 
    }

// ----------------------------------------------------------
// CCCAParameter::SetContactDataFlag
// ----------------------------------------------------------
//
void CCCAParameter::SetContactDataFlag( const TContactDataFlags aContactDataFlag )
    {
    iContactDataFlag = aContactDataFlag;
    return; 
    }


// ----------------------------------------------------------
// CCCAParameter::SetContactDataL
// ----------------------------------------------------------
//
void CCCAParameter::SetContactDataL( const TDesC& aContactData )
    {    
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::SetContactDataL()"));                    

     __ASSERT_ALWAYS(iContactDataFlag != EContactNone, User::Leave(KErrNotFound));   
     
    delete iContactData;
    iContactData = NULL;        
    iContactData = aContactData.AllocL();
    
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::SetContactDataL(): Done."));                    
    return; 
    }

// ----------------------------------------------------------
// CCCAParameter::SetLaunchedViewUid
// ----------------------------------------------------------
//
void CCCAParameter::SetLaunchedViewUid( const TUid aUid )
    {
    iUid = aUid;     
    }

// ----------------------------------------------------------
// CCCAParameter::CcaParameterExtension
// ----------------------------------------------------------
//
TAny* CCCAParameter::CcaParameterExtension( TUid aExtensionUid )
    {
    TAny* ret(NULL); 

    if( aExtensionUid == KMCCAConnectionExtUid )
        {
		TRAPD(err, ret =  static_cast<MCCAConnectionExt*>( CCCAConnection::NewL()));
        }

    return ret;
    }

// ----------------------------------------------------------
// CCCAParameter::ContactIdL
// ----------------------------------------------------------
//
EXPORT_C TInt32 CCCAParameter::ContactIdL()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::ContactIdL()"));                    

    __ASSERT_ALWAYS(iContactDataFlag == MCCAParameter::EContactId, User::Leave( KErrNotFound ));   

        
    TLex lex( *iContactData );
    TInt32 cntId( 0 );
    User::LeaveIfError( lex.Val( cntId ));

    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::ContactIdL(): Done."));                    
    return cntId;   
    }

// ----------------------------------------------------------
// CCCAParameter::ContactLinkL
// ----------------------------------------------------------
//
EXPORT_C HBufC8* CCCAParameter::ContactLinkL()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::ContactLinkL()"));                    

    __ASSERT_ALWAYS(iContactDataFlag == MCCAParameter::EContactLink, User::Leave( KErrNotFound ));                  
        
    HBufC8* eightBitLink = HBufC8::NewL( iContactData->Length() );
    eightBitLink->Des().Copy( *iContactData );
    
    CCA_DP(KCCAClientLoggerFile, CCA_L( "CCCAParameter::ContactLinkL(): Done."));                            
    return eightBitLink;
    }

// ----------------------------------------------------------
// CCCAParameter::CCCAParameter
// ----------------------------------------------------------
//
CCCAParameter::CCCAParameter(  ):
    iConnectionFlag( ENormal ),
    iContactDataFlag( EContactNone ),
    iUid( TUid::Null() ),
    iVersion( KErrNotFound )    
    {
    }

// ----------------------------------------------------------
// CCCAParameter::ConstructL
// ----------------------------------------------------------
//
void CCCAParameter::ConstructL( )
    {
    }

// End of file
