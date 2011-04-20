source ./.findqop.sh

ehco "7z x -y test.7z |$qop -t7z -T $((`7z l test.7z |sed -n '$s/\(.*\), \(.*\)folders/\2/p'`+`7z l test.7z |sed -n '$s/\(.*\) \(.*\)files.*/\2/p'`))"

7z x -y $1 |$qop -t7z -T $((`7z l $1 |sed -n '$s/\(.*\), \(.*\)folders/\2/p'`+`7z l $1 |sed -n '$s/\(.*\) \(.*\)files.*/\2/p'`))
