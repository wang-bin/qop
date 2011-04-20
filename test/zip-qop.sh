source ./.findqop.sh

if [ $# -gt 0 ]; then
compress=$@
else
echo "input a directory you want to compress"
read compress
fi

[ ! -d $compress ] && echo "$compress does not exist" && exit

echo "zip -ryv -9 /tmp/test.zip $compress |$qop $compress -m 2>/dev/null
	Compressing...Showing progress for size..."
zip -ryv -9 /tmp/test.zip $compress |$qop $compress -m 2>/dev/null

echo "zip -ry -9 /tmp/test.zip $compress |$qop $compress -m 2>/dev/null
	Compressing...Showing progress for number of files..."
zip -ryv -9 /tmp/test.zip $compress |$qop $compress -m 2>/dev/null
