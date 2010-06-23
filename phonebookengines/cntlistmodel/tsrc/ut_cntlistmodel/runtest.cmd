call del MON.sym
call del MON.dat
call del profile.txt

call qmake
call sbs reallyclean
call sbs -c winscw_udeb
call sbs -c winscw_udeb
call qmake
call ctcwrap -i d -C "EXCLUDE+moc_*.cpp" -C "EXCLUDE+ut_*.*" sbs -c winscw_udeb

call \epoc32\release\winscw\udeb\ut_cntlistmodel.exe -noprompt
call ctcpost MON.sym MON.dat -p profile.txt
call ctc2html -i profile.txt -nsb
