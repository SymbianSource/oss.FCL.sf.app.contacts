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


// INCLUDE FILES
#include <CxSPViewData.h>

// System includes
#include <s32mem.h>


/// Unnamed namespace for local definitions
namespace {

/**
 * Stream buffer which counts number of bytes written to it.
 */
class TCalcLengthStreamBuf : public MStreamBuf
    {
    public: // Construction

        /**
         * Constructor.
         */
        inline TCalcLengthStreamBuf() :
                iBytesWritten( 0 )
            {
            }

        /**
         * Returns the number of bytes written.
         *
         * @return  Number of bytes written.
         */
        inline TInt BytesWritten() const
            {
            return iBytesWritten;
            }

    private: // From MStreamBuf

        void DoWriteL( const TAny* /*aPtr*/, TInt aLength )
            {
            // Just count the bytes
            iBytesWritten += aLength;
            }

    private: // Data
        /// Ref: Count of bytes written to this streambuf
        TInt iBytesWritten;
    };


} /// namespace




CxSPViewData::CxSPViewData() :
        iEComId( 0 ),
        iOriginalViewId( KErrNotFound ),
        iIconId( KErrNotFound ),
        iMaskId( KErrNotFound )
    {
    }

CxSPViewData::~CxSPViewData()
    {
    delete iIconFile;
    delete iViewName;
    }

EXPORT_C CxSPViewData* CxSPViewData::NewL()
    {
    CxSPViewData* self = new ( ELeave ) CxSPViewData;
    return self;
    }

EXPORT_C CxSPViewData* CxSPViewData::NewL( const TDesC8& aBuf )
    {
    CxSPViewData* self = new ( ELeave ) CxSPViewData;
    CleanupStack::PushL( self );
    self->ConstructL( aBuf );
    CleanupStack::Pop( self );
    return self;    
    }

void CxSPViewData::ConstructL( const TDesC8& aBuf )
    {
    UnpackL( aBuf );
    }


EXPORT_C TUint32 CxSPViewData::EComId() const
	{
	return iEComId;
	}

EXPORT_C TInt32 CxSPViewData::OriginalViewId() const
	{
	return iOriginalViewId;
	}

EXPORT_C TInt32 CxSPViewData::IconId() const
	{
	return iIconId;
	}

EXPORT_C TInt32 CxSPViewData::MaskId() const
	{
	return iMaskId;
	}

EXPORT_C const TDesC& CxSPViewData::IconFile() const
	{
	if( iIconFile )
		{
		return *iIconFile;
		}
	else
		{
		return KNullDesC();
		}
	}

EXPORT_C const TDesC& CxSPViewData::ViewName() const
	{
	if( iViewName )
		{
		return *iViewName;
		}
	else
		{
		return KNullDesC();
		}
	}
	
EXPORT_C void CxSPViewData::SetEComId( TUint32 aEComId )
	{
	iEComId = aEComId;
	}

EXPORT_C void CxSPViewData::SetOriginalViewId( TInt32 aOriginalViewId )
	{
	iOriginalViewId = aOriginalViewId;
	}

EXPORT_C void CxSPViewData::SetIconId( TInt32 aIconId )
	{
	iIconId = aIconId;
	}

EXPORT_C void CxSPViewData::SetMaskId( TInt32 aMaskId )
	{
	iMaskId = aMaskId;
	}

EXPORT_C void CxSPViewData::SetIconFileL( const TDesC& aIconFile )
	{
	delete iIconFile;
	iIconFile = NULL;
	iIconFile = aIconFile.AllocL();
	}

EXPORT_C void CxSPViewData::SetViewNameL( const TDesC& aViewName )
	{
	{
	delete iViewName;
	iViewName = NULL;
	iViewName = aViewName.AllocL();
	}
	}

EXPORT_C void CxSPViewData::Reset()
    {
    iEComId = 0;
    iOriginalViewId = KErrNotFound;
    iIconId = KErrNotFound;
    iMaskId = KErrNotFound;
    delete iIconFile;
    iIconFile = NULL;
    delete iViewName;
    iViewName = NULL;
    }

EXPORT_C HBufC8* CxSPViewData::PackL() const
    {
    // Calculate needed buffer size using TCalcLengthStreamBuf
    TCalcLengthStreamBuf countBuf;     
    RWriteStream countStream( &countBuf );
    // This can't leave because we are using TCalcLengthStreamBuf
    ExternalizeL( countStream );

    // Allocate a buffer and a stream
    HBufC8* buf = HBufC8::NewLC( countBuf.BytesWritten() );
    TPtr8 bufPtr = buf->Des();
    RDesWriteStream stream( bufPtr );
    stream.PushL();

    // Write this object to buf through stream
    ExternalizeL( stream );

    stream.CommitL();
    CleanupStack::PopAndDestroy(); // stream
    CleanupStack::Pop(); // buf
    return buf;
    }

EXPORT_C void CxSPViewData::UnpackL( const TDesC8& aPack )
    {
    RDesReadStream stream( aPack );
    stream.PushL();
    InternalizeL( stream );
    CleanupStack::PopAndDestroy(); // stream
    }

void CxSPViewData::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iEComId );
    aStream.WriteInt32L( iOriginalViewId );
    aStream.WriteInt32L( iIconId );
    aStream.WriteInt32L( iMaskId );
    
    if( iIconFile )
    	{
    	aStream.WriteUint16L( iIconFile->Length() );
    	aStream.WriteL( *iIconFile );
    	}
   	else
   		{
   		aStream.WriteUint16L( KNullDesC().Length() );
   		aStream.WriteL( KNullDesC() );
   		}

	if( iViewName )
    	{
    	aStream.WriteUint16L( iViewName->Length() );
    	aStream.WriteL( *iViewName );
    	}
   	else
   		{
   		aStream.WriteUint16L( KNullDesC().Length() );
   		aStream.WriteL( KNullDesC() );
   		}    
    }

void CxSPViewData::InternalizeL( RReadStream& aStream )
    {
    Reset();

	iEComId = aStream.ReadUint32L();
	iOriginalViewId = aStream.ReadInt32L();
	iIconId = aStream.ReadInt32L();
	iMaskId = aStream.ReadInt32L();

	TInt length = aStream.ReadUint16L();
	iIconFile = HBufC::NewL( length );
	TPtr iconFilePtr = iIconFile->Des();
    aStream.ReadL( iconFilePtr, length );

	length = aStream.ReadUint16L();
	iViewName = HBufC::NewL( length );
	TPtr viewNamePtr = iViewName->Des();
    aStream.ReadL( viewNamePtr, length );    
    }

//  End of File
