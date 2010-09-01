rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

makesis ccaapp.pkg
signsis ccaapp.sis ccaapp.sisx rd.cer rd-key.pem

makesis ccaapp_udeb.pkg
signsis ccaapp_udeb.sis ccaapp_udeb.sisx ..\..\internal\rom_stuff\certifications\rd.cer ..\..\internal\rom_stuff\certifications\rd-key.pem
