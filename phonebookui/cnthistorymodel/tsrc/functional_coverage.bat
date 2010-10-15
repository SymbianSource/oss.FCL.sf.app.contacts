pushd mt_cnthistorymodel
call del MON.sym
call del MON.dat
call del profile.txt

call qmake
call sbs reallyclean
call ctcwrap -i f -2comp -no-conf-check sbs.bat -c winscw_urel.test

call \epoc32\release\winscw\urel\mt_cnthistorymodel.exe
call ctcpost -p profile.txt
call ctc2html -i profile.txt -nsb
popd
