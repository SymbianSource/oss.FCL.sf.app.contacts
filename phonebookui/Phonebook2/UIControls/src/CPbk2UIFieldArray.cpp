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
* Description:  Phonebook 2 contact editor dialog UI field array.
*
*/

#include "CPbk2UIFieldArray.h"
#include "CPbk2UIField.h"
#include <pbk2uicontrols.rsg>

// Phonebook 2
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include "MPbk2UIField.h"

// System includes
#include <barsc.h>
#include <barsread.h>
#include <featmgr.h>


// ---------------------------------------------------------------------------
// CPbk2UIFieldArray::CPbk2UIFieldArray
// ---------------------------------------------------------------------------
//
inline CPbk2UIFieldArray::CPbk2UIFieldArray(MPbk2UIFieldFactory& aFactory)
	:iFactory(aFactory)
    {
    }

// ---------------------------------------------------------------------------
// CPbk2UIFieldArray::ConstructL
// ---------------------------------------------------------------------------
//
inline void CPbk2UIFieldArray::ConstructL( const TDesC& aResFile, RFs& aRFs )
    {
    RPbk2LocalizedResourceFile resFile( &aRFs );
    resFile.OpenLC(KPbk2RomFileDrive,
                   KDC_RESOURCE_FILES_DIR, 
                   aResFile);

    TResourceReader reader;
    reader.SetBuffer( resFile.AllocReadLC( 
            R_PBK2_UI_FIELD_PROPERTIES_CONTACT_EDITOR ) );
 
    const TInt count = reader.ReadInt16();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2UIField* field = CPbk2UIField::NewL( reader );
        CleanupStack::PushL( field );
        iFieldsArr.AppendL( field );
        CleanupStack::Pop( field );
        }
    
    // R_PBK2_UI_FIELD_PROPERTIES_CONTACT_EDITOR, resFile
    CleanupStack::PopAndDestroy(2); 
    }

// ---------------------------------------------------------------------------
// CPbk2UIFieldArray::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CPbk2UIFieldArray* CPbk2UIFieldArray::NewL
        ( const TDesC& aResFile,
          RFs& aRFs, 
          MPbk2UIFieldFactory& aFactory )
    {
    CPbk2UIFieldArray* self = new(ELeave) CPbk2UIFieldArray( aFactory );
    CleanupStack::PushL(self);
    self->ConstructL( aResFile, aRFs);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CPbk2UIFieldArray::~CPbk2UIFieldArray
// ---------------------------------------------------------------------------
//
CPbk2UIFieldArray::~CPbk2UIFieldArray()
    {
    iFieldsArr.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CPbk2UIFieldArray::Count
// ---------------------------------------------------------------------------
//
TInt CPbk2UIFieldArray::Count() const
    {
    return iFieldsArr.Count();
    }

// ---------------------------------------------------------------------------
// CPbk2UIFieldArray::At
// ---------------------------------------------------------------------------
//
MPbk2UIField& CPbk2UIFieldArray::At
        (TInt aIndex) const
    {
    return *iFieldsArr[aIndex];
    }

// ---------------------------------------------------------------------------
// CPbk2UIFieldArray::GetUIFieldFactory
// ---------------------------------------------------------------------------
//
const MPbk2UIFieldFactory& CPbk2UIFieldArray::GetUIFieldFactory() const
    {
    return iFactory;
    }

// ---------------------------------------------------------------------------
// CPbk2UIFieldArray::AppendL
// ---------------------------------------------------------------------------
//
void CPbk2UIFieldArray::AppendL(MPbk2UIFieldArray& aUIFieldArray)
    {
    TInt count = aUIFieldArray.Count();
    for(TInt idx = 0; idx < count; idx++)
    	{
    	const MPbk2UIField&  field = aUIFieldArray.At(idx);
    	iFieldsArr.AppendL(&field);
    	}
    }

// ---------------------------------------------------------------------------
// CPbk2UIFieldArray::RemoveAt
// ---------------------------------------------------------------------------
//
void CPbk2UIFieldArray::RemoveAt( TInt aIndex )
    {
    iFieldsArr.Remove(aIndex);
    }


// End of File
