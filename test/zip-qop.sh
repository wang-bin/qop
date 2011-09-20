. .findqop.sh

if [ $# -gt 0 ]; then
compress=$@
else
echo "input a directory you want to compress"
read compress
fi

[ ! -d $compress ] && echo "$compress does not exist" && exit

#some version of zip(3.0) do not have -y
rm -f /tmp/test.zip
echo "zip -rv -9 /tmp/test.zip $compress |$qop $compress -m 2>/dev/null
	Compressing...Showing progress for size..."
zip -rv -9 /tmp/test.zip $compress |$qop $compress -m 2>/dev/null

rm -f /tmp/test.zip
echo "zip -r -9 /tmp/test.zip $compress |$qop $compress -m 2>/dev/null
	Compressing...Showing progress for number of files..."
zip -rv -9 /tmp/test.zip $compress |$qop $compress -m 2>/dev/null
