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
* Description:  Contact field data
*
*/


// INCLUDES
#include "MVPbkContactFieldTextData.h"
#include "MVPbkContactFieldBinaryData.h"
#include "MVPbkContactFieldDateTimeData.h"
#include "MVPbkContactFieldUriData.h"
#include "VPbkError.h"

EXPORT_C MVPbkContactFieldTextData& MVPbkContactFieldTextData::Cast
        (MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS( aFieldData.DataType() == EVPbkFieldStorageTypeText,
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch) );
    return static_cast<MVPbkContactFieldTextData&>(aFieldData);
    }

EXPORT_C const MVPbkContactFieldTextData& MVPbkContactFieldTextData::Cast
        (const MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS( aFieldData.DataType() == EVPbkFieldStorageTypeText,
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch) );
    return static_cast<const MVPbkContactFieldTextData&>(aFieldData);
    }

EXPORT_C MVPbkContactFieldBinaryData& MVPbkContactFieldBinaryData::Cast
        (MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS( aFieldData.DataType() == EVPbkFieldStorageTypeBinary,
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch) );
    return static_cast<MVPbkContactFieldBinaryData&>(aFieldData);
    }

EXPORT_C const MVPbkContactFieldBinaryData& MVPbkContactFieldBinaryData::Cast
        (const MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS( aFieldData.DataType() == EVPbkFieldStorageTypeBinary,
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch) );
    return static_cast<const MVPbkContactFieldBinaryData&>(aFieldData);
    }

EXPORT_C MVPbkContactFieldDateTimeData& MVPbkContactFieldDateTimeData::Cast
        (MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS( aFieldData.DataType() == EVPbkFieldStorageTypeDateTime,
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch) );
    return static_cast<MVPbkContactFieldDateTimeData&>(aFieldData);
    }

EXPORT_C const MVPbkContactFieldDateTimeData& MVPbkContactFieldDateTimeData::Cast
        (const MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS( aFieldData.DataType() == EVPbkFieldStorageTypeDateTime,
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch) );
    return static_cast<const MVPbkContactFieldDateTimeData&>(aFieldData);
    }

EXPORT_C MVPbkContactFieldUriData& MVPbkContactFieldUriData::Cast
        (MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS( aFieldData.DataType() == EVPbkFieldStorageTypeUri,
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch) );
    return static_cast<MVPbkContactFieldUriData&>(aFieldData);
    }

EXPORT_C const MVPbkContactFieldUriData& MVPbkContactFieldUriData::Cast
        (const MVPbkContactFieldData& aFieldData)
    {
    __ASSERT_ALWAYS( aFieldData.DataType() == EVPbkFieldStorageTypeUri,
        VPbkError::Panic(VPbkError::EFieldDataTypeMismatch) );
    return static_cast<const MVPbkContactFieldUriData&>(aFieldData);
    }

