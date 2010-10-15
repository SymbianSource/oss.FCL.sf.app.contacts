/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testrunner.h"
#include <QtTest/QtTest>
#include <QDir>
#include <stdio.h>

const char testFunctionElement[] = "TestFunction";
const char incidentElement[] = "Incident";
const char descriptionElement[] = "Description";
const char nameAttr[] = "name";
const char typeAttr[] = "type";
const char fileAttr[] = "file";
const char lineAttr[] = "line";
const char attrValueFail[] = "fail";


TestRunner::TestRunner(const QString& name)
: mTestCount(0),
  mParsingIncidentElement(false),
  mParsingDescriptionElement(false),
  mCurrentTestFailed(false),
  mCurrentTestFailureLine(0)
{
 #ifdef __WINS__ 
    mTestRunParams.append(name);
  
    mTestRunParams.append("-xml");
    
    mTestRunParams.append("-o");
    mHomeDir = QDir::homePath();
    	
    mTestRunParams.append(QString()); // Initial result file name

    if (!mHomeDir.endsWith(QString::fromAscii("/")))
        mHomeDir += QString::fromAscii("/");
#endif 
}

TestRunner::~TestRunner()
{
}

int TestRunner::runTests(QObject& testObject)
{
    QString className(testObject.metaObject()->className());
    printf("Running tests for %s ... ", className.toUtf8().data());
#ifdef __WINS__    
    QString resultFileName = mHomeDir + className + ".xml";
    mTestRunParams.replace(mTestRunParams.count()-1,resultFileName);
    int errorsBefore = mErrors.count();
    int error = QTest::qExec(&testObject, mTestRunParams);
    parse(resultFileName);
    printf("Failures: %d\n",mErrors.count()-errorsBefore);
   	fflush(stdout);  	
#else
    int error = QTest::qExec(&testObject);
#endif
    
    return error;
}

void TestRunner::printResults()
{
    printf("\nTests executed: %d\n",mTestCount);
    if (mErrors.count() > 0) {
        printf("Failures (%d):\n", mErrors.count());
        foreach(QString error, mErrors) {
            printf("\n%s", error.toUtf8().data());
        }
        printf("\n");
    } else {
        printf("All passed.\n\n");
    }
    fflush(stdout);
#ifdef __WINS__       
    //To write in file
    QFile file("C:\\TestResult.txt");
    if(file.open(QIODevice::WriteOnly)) 
    {
        QTextStream ts( &file );
        ts << "Tests executed: " << mTestCount << "\n";
        if (mErrors.count() > 0)
        {
            ts <<"Failures : " << mErrors.count() << "\n";
            foreach(QString error, mErrors)
                {
                    ts << error.toUtf8().data();
                }
            ts << "\n";
        }
        else
        {
            ts<< "All passed.\n\n";
        }
        
        ts << endl;
        file.close();
    }
#endif      
}

void TestRunner::parse(const QString& fileName)
{
    QFile file(fileName);
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.parse(inputSource);
}

bool TestRunner::startElement(
    const QString& /*namespaceURI*/, 
    const QString& /*localName*/, 
    const QString& qName, 
    const QXmlAttributes& atts)
{
    if (qName == QString::fromAscii(testFunctionElement)) {
        mTestCount++;
        mCurrentTestName = atts.value(QString::fromAscii(nameAttr));
        return true;
    }
    if (qName == QString::fromAscii(incidentElement)) {
        mParsingIncidentElement = true;
        if (atts.value(QString::fromAscii(typeAttr)) == QString::fromAscii(attrValueFail)) {
            mCurrentTestFailed = true;
            mCurrentTestFile = atts.value(QString::fromAscii(fileAttr));
            mCurrentTestFailureLine = atts.value(QString::fromAscii(lineAttr)).toInt();
        }
        return true;
    }
    mParsingDescriptionElement =
        (qName == QString::fromAscii(descriptionElement));
    return true;
}

bool TestRunner::endElement(
    const QString& /*namespaceURI*/,
    const QString& /*localName*/,
    const QString& qName)
{
    if (qName == QString::fromAscii(incidentElement)) {
        mParsingIncidentElement = false;
        mCurrentTestFailed = false;
        return true;
    }
    if (qName == QString::fromAscii(descriptionElement)) {
        mParsingDescriptionElement = false;
    }    
    return true;
}

bool TestRunner::characters(const QString& ch)
{
    if (mParsingIncidentElement && 
        mParsingDescriptionElement &&
        mCurrentTestFailed) {
        QByteArray testResult = mCurrentTestName.toAscii() + " failed:\n";
        testResult += "File: ";
        testResult += mCurrentTestFile.toAscii();
        testResult += "\n";
        testResult += "Line: ";
        testResult += QByteArray::number(mCurrentTestFailureLine);
        testResult += "\n";
        testResult += "Reason: ";
        testResult += ch.toAscii();
        testResult += "\n";
        mErrors.append(QString::fromAscii(testResult.data()));
    }
    return true;
}

