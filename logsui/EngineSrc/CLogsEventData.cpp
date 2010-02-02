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
*     Implements interface for Logs event
*
*/

// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logsmspdudata.h>
#endif
#include "CLogsEventData.h"
#include "LogsApiConsts.h"
#include <AiwPoCParameters.h>  //epocnooptypeid for idatatype

_LIT8( KLogsDoubleDataFldNameDelimiter, "\t\t" );
const TInt KOneChar = 1;
const TInt KTwoChars = 2;


// ----------------------------------------------------------------------------
// CLogsEventData::NewL
// ----------------------------------------------------------------------------
//
CLogsEventData* CLogsEventData::NewL( const TDesC8 &aData, TBool aReadMessageParts ) 
    {
    CLogsEventData* self = new (ELeave) CLogsEventData( aData );
    CleanupStack::PushL( self );
    self->ConstructL( aReadMessageParts );
    CleanupStack::Pop( self );
    return self;
    }


// ----------------------------------------------------------------------------
// CLogsEventData::CLogsEventData
// ----------------------------------------------------------------------------
//
CLogsEventData::CLogsEventData( const TDesC8 &aData ) : 
    iTempDataFieldPtr( aData )
    {
    //NOTE It is NOT guaranteed that the TDesC8& got in as parameter is alive longer than 
    //during the call to this ConstructL. Therefore the same applies to  iTempDataFieldPtr we're using to point to it. 
    }

// ----------------------------------------------------------------------------
// CLogsEventData::~CLogsEventData
// ----------------------------------------------------------------------------
//
CLogsEventData::~CLogsEventData() 
    {
    delete iURL8;
    delete iProfile;
    delete iId;
    delete iMyAddress8;
    delete iDataField;
    delete iContactLink;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsEventData::ConstructL( TBool aReadMessageParts ) 
    {
    //All default values must already be set in case there is no data in the event's data field.
    iALS = EFalse;
    iCNAP = EFalse;
    iVT = EFalse;
    iPoC = EFalse;
    iVoIP = EFalse;
    iEmerg = EFalse;
    iDataSent = MAKE_TINT64(0,0);
    iDataReceived = MAKE_TINT64(0,0);
    iDataType = KErrNotFound;       //Provide EPoCNoOpTypeId if type is absent or non numeric    
    iMsgPartsNumber = 0;            //Meaningful only for sms
    iServiceId = 0;
    iContactLink = 0;
    iDataField = iTempDataFieldPtr.AllocL();
    iURL8 = HBufC8::NewL( 0 );      //We must have a valid HBufC object even if we would not put any data to it.
    iProfile = HBufC8::NewL( 0 );   
    iId = HBufC8::NewL( 0 );   
    iMyAddress8 = HBufC8::NewL( 0 );

    //1. Msg parts are written in different format in data field.
    if( aReadMessageParts )
        {
        SetMsgPartsNumber();
        }
    //2. else check if the sent & received data is in old format (this can be removed when creating 
    //   log data field entries is similar to format of other data field entries (i.e is tagged))
    else if( CheckNonTaggedData( iTempDataFieldPtr ) )
        {
        return;  //Found and processed legacy data.
        }
     //3. Otherwise check if tagged data is available.        
     else
        {
        CheckTaggedDataL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventData::SetMsgPartsNumber
//
// Read msg parts. They are written in format of TLogSmsPduData in Data field
// ----------------------------------------------------------------------------
//
void CLogsEventData::SetMsgPartsNumber() 
    {
    TPckgBuf<TLogSmsPduData> packedData;
    packedData.Copy( iTempDataFieldPtr.Ptr(), sizeof( TLogSmsPduData ) );
    iMsgPartsNumber = packedData().iTotal;
    return;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::CheckNonTaggedData
// ----------------------------------------------------------------------------
//
TBool CLogsEventData::CheckNonTaggedData( TPtrC8 aDataPtr ) 
    {
    TLex8 lex;
    TPtrC8 ptr;
    _LIT8(KDataSeparator,",");  //Separator for gprs data (old legacy non-tagged format for sent and received grps data)

    //Here were are interested only in data that begins with number (e.g. 123445,23445)
    if( aDataPtr.Length() < 1 )
        {
        return EFalse;      //No data
        }
    else
        {                   //At least 1 byte of data available. Check does it begin with number.
        TInt v;
        ptr.Set( aDataPtr.Left(1) );
        lex = ptr;
        if( lex.Val( v ) != KErrNone ) 
            {
            return EFalse;  //First byte does not contain number
            }
        }

    //Ok, data begins with number. Try to read a pair of comma separated numbers
    TInt separatorOffset = aDataPtr.Find( KDataSeparator );

    if( separatorOffset + 1 > aDataPtr.Length() || separatorOffset < 0 )   
        {
        return EFalse;  //No separator found. 
        }

    //Set iDataSent
    ptr.Set( aDataPtr.Left( separatorOffset ) );  //Let's inspect on the left side of field delimiter token
    lex = ptr;
    lex.Val( iDataSent );

    //Then reeceived data
    if( (separatorOffset + 1) >= aDataPtr.Length() )
        {
        return ETrue;  //Was no data after separator
        }

    //Set iDataReceived
    ptr.Set( aDataPtr.Mid(separatorOffset + 1, aDataPtr.Length() - separatorOffset - 1) );
    lex = ptr;
    lex.Val( iDataReceived );

    return ETrue;
    }


// ----------------------------------------------------------------------------
// CLogsEventData::CheckTaggedDataL
// ----------------------------------------------------------------------------
//
void CLogsEventData::CheckTaggedDataL() 
    {
    TInt dataLeft = iTempDataFieldPtr.Length();
    
    while( dataLeft > 0 )
        {
        TInt nextTokenStart = iTempDataFieldPtr.Find( KLogsDataFldNameDelimiter );
        TPtrC8 delim( iTempDataFieldPtr );
        
        // Check if there are double delimeter \t\t marks. 
        // If \t in the end of the string, dont' try to check the next one!
        if ( ( (nextTokenStart + KOneChar) < iTempDataFieldPtr.Length() ) && 
             ( iTempDataFieldPtr[ nextTokenStart + KOneChar ] == KLogsDataFldNameDelimiter()[ 0 ] ) )
            {
            // Contact link needs to be decoded. Each delimiter has been duplicated to logs data.
            TInt tokenStretch = 0;
            // Delimiter pairs are skipped here and correct token place or end of data is set.
            do
                {
                delim.Set( iTempDataFieldPtr.Mid( nextTokenStart + KTwoChars ) );
                tokenStretch = delim.Find( KLogsDataFldNameDelimiter );
                if ( KErrNotFound == tokenStretch )
                    {
                    // No more double delimiters of different data fields remaining, so take rest of data
                    tokenStretch = delim.Length();
                    }
                nextTokenStart += ( tokenStretch + KTwoChars );
                }while( iTempDataFieldPtr.Length() > nextTokenStart + KOneChar && 
                        iTempDataFieldPtr[ nextTokenStart + KOneChar ] == KLogsDataFldNameDelimiter()[ 0 ] );
            }
        
        if( nextTokenStart > iTempDataFieldPtr.Length() || nextTokenStart < 0 )
            {
            nextTokenStart = iTempDataFieldPtr.Length();
            }

        TPtrC8 nameValue = iTempDataFieldPtr.Left( nextTokenStart );  //Let's inspect on the left side of field delimiter token
        TPtrC8 name;
        TPtrC8 value;

        TInt delimiterStart = nameValue.Find( KLogsDataFldValueDelimiter );   
        if( delimiterStart > iTempDataFieldPtr.Length() || delimiterStart < 0 )
            {
            name.Set( nameValue );
            //No value. Initialised to null in above (TPtrC8 value)
            }
        else
            {
            name.Set( iTempDataFieldPtr.Left( delimiterStart ) );  
            TInt length = nameValue.Length() - delimiterStart - 1;
            value.Set( iTempDataFieldPtr.Mid( delimiterStart + 1, length) );//Mid(TInt aPos, TInt aLength)
            }

        //Below a minor attempt to slightly speed up the string comparisons: If value already found, no need to compare same name anymore.
        //Most likely there is VT, VOIP or POC tag in the beginning of data field if any tags.
        if( !iVT && name.Compare( KLogsDataFldTag_VT ) == 0 )
            {
            iVT = ETrue;
            }
        else if( !iCNAP  && name.Compare( KLogsDataFldTag_CNAP ) == 0 )
            {
            iCNAP = ETrue;
            }
        else if( !iEmerg && name.Compare( KLogsDataFldTag_Emergency ) == 0 )
            {
            iEmerg = ETrue;
            }
        else if( !iPoC && name.Compare( KLogsDataFldTag_POC ) == 0 )
            {
            iPoC = ETrue;
            }
        else if( !iVoIP && name.Compare( KLogsDataFldTag_IP ) == 0 )
            {
            iVoIP = ETrue;
            }
            //iDataType: internal PoC data (the value relates to the TPoCOperationTypeId enumeration)
        else if( iDataType == KErrNotFound && name.Compare( KLogsDataFldTag_Type ) == 0 )  //No negative numbers in value so 
            {                                                                             //this should not execute more than once
            TLex8 lex;
            lex = value;
            lex.Val( iDataType ); //TLex: if parsing error, iDataType remains in it's original value
            }
            //Consider optimising these (below) in case there's tag but no value (in those cases these comparisons 
            //are executed more than just once
        else if( !iURL8->Size() && name.Compare( KLogsDataFldTag_URL ) == 0 )
            {
            delete iURL8; //Exist for sure
            iURL8 = 0;
            iURL8 = HBufC8::NewL( value.Length() );
            TPtr8 ptr = iURL8->Des();
            ptr.Copy( value );  //Copy the data to our own instance of HBufC* that is alive as long as *this exist
            }
        else if( !iMyAddress8->Size() && name.Compare( KLogsDataFldTag_MA ) == 0 )
            {
            delete iMyAddress8; //Exist for sure
            iMyAddress8 = 0;
            iMyAddress8 = HBufC8::NewL( value.Length() );
            TPtr8 ptr = iMyAddress8->Des();
            ptr.Copy( value );  //Copy the data to our own instance of HBufC* that is alive as long as *this exist
            }

// Sawfish VoIP changes  >>>>
        else if( !iServiceId && name.Compare( KLogsDataFldTag_ServiceId ) == 0 )
            {
            // get the 'iServiceId' from the value
            TLex8 lex( value );
            TUint32 temp( iServiceId );
            TInt err = lex.Val( iServiceId , EDecimal );
            if( KErrNone != err )
                {
                //if an error occurred we leave the service id unchanged
                iServiceId = temp;
                }
            }
        else if ( !iContactLink 
                    && name.Compare( KLogsDataFldTag_ContactLink ) == 0 )
            {     
            iContactLink = value.AllocL();
            TPtr8 linkPtr( iContactLink->Des() );
            
            TInt doubleDelim = linkPtr.Find( KLogsDoubleDataFldNameDelimiter );
            // Remove duplicated delimiter(s)
            while( KErrNotFound != doubleDelim )
                {
                linkPtr.Delete( doubleDelim, KOneChar );
                doubleDelim = linkPtr.Find( KLogsDoubleDataFldNameDelimiter );
                }
            } 
// <<<< Sawfish VoIP changes 
        
        /*****************************************************************************            
        <the tags below are not needed currently, but may be needed in the future>
        else if( !iDataSent && name.Compare( KLogsDataFldTag_DataSent ) == 0 )
            {
            TLex8 lex;
            lex = value;
            lex.Val( iDataSent ); //TLex: if parsing error, iDataSent remains in it's original value
            }
        else if( !iDataReceived && name.Compare( KLogsDataFldTag_DataReceived ) == 0 )
            {
            TLex8 lex;
            lex = value;
            lex.Val( iDataReceived ); //TLex: if parsing error, iDataReceived remains in it's original value
            }
        else if( !iProfile->Size() && name.Compare( KLogsDataFldTag_Profile ) == 0 )
            {
            delete iProfile; 
            iProfile = 0;
            iProfile = HBufC8::NewL( value.Length() );
            TPtr8 ptr = iProfile->Des();
            ptr.Copy( value );  //Copy the data to our own instance of HBufC* that is alive as long as *this exist
            }
        else if( !iId->Size() && name.Compare( KLogsDataFldTag_ID ) == 0 )
            {
            delete iId; 
            iId = 0;
            iId = HBufC8::NewL( value.Length() );
            TPtr8 ptr = iId->Des();
            ptr.Copy( value );  //Copy the data to our own instance of HBufC* that is alive as long as *this exist
            }
        *****************************************************************************/            


        //Process remaining data
        dataLeft = iTempDataFieldPtr.Length() - nextTokenStart -1;  //Remaining data on the right side of token
        
        if( dataLeft > 0 )
            {
            nameValue.Set( iTempDataFieldPtr.Right( dataLeft ));    //Continue with remaining data on the right side of token
            iTempDataFieldPtr.Set( nameValue );               
            }
        }    
    }



// ----------------------------------------------------------------------------
// CLogsEventData::ALS
// ----------------------------------------------------------------------------
//
TBool CLogsEventData::ALS() const
    {
    return iALS;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::CNAP
// ----------------------------------------------------------------------------
//
TBool CLogsEventData::CNAP() const
    {
    return iCNAP;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::VT
// ----------------------------------------------------------------------------
//
TBool CLogsEventData::VT() const
    {
    return iVT;  
    }

// ----------------------------------------------------------------------------
// CLogsEventData::PoC
// ----------------------------------------------------------------------------
//
TBool CLogsEventData::PoC() const
    {
    return iPoC;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::VoIP
// ----------------------------------------------------------------------------
//
TBool CLogsEventData::VoIP() const
    {
    return iVoIP;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::Url
// ----------------------------------------------------------------------------
//
TDesC8& CLogsEventData::Url() const
    {
    return *iURL8;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::Profile
// ----------------------------------------------------------------------------
//
TDesC8& CLogsEventData::Profile() const
    {
    return *iProfile;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::Id
// ----------------------------------------------------------------------------
//
TDesC8& CLogsEventData::Id() const
    {
    return *iId;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::DataReceived
// ----------------------------------------------------------------------------
//
TInt64 CLogsEventData::DataReceived() const
    {
    return iDataReceived;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::DataSent
// ----------------------------------------------------------------------------
//
TInt64 CLogsEventData::DataSent() const
    {
    return iDataSent;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::Type
// ----------------------------------------------------------------------------
//
TInt CLogsEventData::Type() const
    {
    if( iDataType == KErrNotFound )
        {
        return EPoCNoOpTypeId;  //Provide EPoCNoOpTypeId if type is absent or non numeric    
        }
    else
        {
        return iDataType;        
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventData::MyAddress
// ----------------------------------------------------------------------------
//
TDesC8& CLogsEventData::MyAddress()
    {
    return *iMyAddress8;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::Emerg
// ----------------------------------------------------------------------------
//
TBool CLogsEventData::Emerg() const
    {
    return iEmerg;
    }

// ----------------------------------------------------------------------------
// CLogsEventData::MsgPartsNumber
//
// Msg parts. This info is meaningful only for sms events
// ----------------------------------------------------------------------------
//
TInt CLogsEventData::MsgPartsNumber()  const
    {
    return iMsgPartsNumber;
    }


// Sawfish VoIP changes >>>>
// ----------------------------------------------------------------------------
// CLogsEventData::ServiceId
//
// Returns the Service ID of the log event.
// ----------------------------------------------------------------------------
//
TUint32 CLogsEventData::ServiceId()  const
    {
    return iServiceId;
    }
    
// ----------------------------------------------------------------------------
// CLogsEventData::GetContactLink
//
// Getter for contactlink
// ----------------------------------------------------------------------------
//
TInt CLogsEventData::GetContactLink( TPtrC8& aContactLink )
    {
    aContactLink.Set( KNullDesC8 );
    TInt result = KErrNotFound;
    
    if ( iContactLink )
        {
    	aContactLink.Set( iContactLink->Des() );
    	result = KErrNone;
        }
        
    return result;
    }
    
// ----------------------------------------------------------------------------
// CLogsEventData::DataField
// 
// Returns the unparsed data field information.
// ----------------------------------------------------------------------------
//
TDesC8& CLogsEventData::DataField()  const
    {
    return *iDataField;
    }    
// <<<<  Sawfish VoIP changes 
