. .findqop.sh

if [ $# -gt 0 ]; then
_7zfile=$@
else
echo "input a 7zip file you want to extract"
read _7zfile
fi

[ ! -f $_7zfile ] && echo "$_7zfile is not a file" && exit

echo "7z x -y $_7zfile |$qop -n -t7z -T $((`7z l $_7zfile |sed -n '$s/\(.*\), \(.*\)folders/\2/p'`+`7z l $_7zfile |sed -n '$s/\(.*\)  \(.*\)files.*/\2/p'`))"

7z x -y -o/tmp $_7zfile |$qop -n -t7z -T $((`7z l $_7zfile |sed -n '$s/\(.*\), \(.*\)folders/\2/p'`+`7z l $_7zfile |sed -n '$s/\(.*\)  \(.*\)files.*/\2/p'`))
