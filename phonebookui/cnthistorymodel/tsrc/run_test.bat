pushd mt_cnthistorymodel
call qmake
call sbs reallyclean
call sbs -c winscw_urel.test
call \epoc32\release\winscw\urel\mt_cnthistorymodel.exe -o c:\mt_cnthistorymodel.txt
popd