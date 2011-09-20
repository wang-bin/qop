. .findqop.sh

if [ $# -gt 0 ]; then 
compress=$@
else
echo "input a directory you want to compress"
read compress
fi

[ ! -d $compress ] && echo "$compress does not exist" && exit

echo "$qop -C zip -ryv -9 /tmp/test.zip $compress 2>/dev/null
	Showing progress for size..."
$qop -C zip -ryv -9 /tmp/test.zip $compress 2>/dev/null

echo "$qop -C zip -ry -9 /tmp/test.zip $compress 2>/dev/null
	Showing progress for number of files..."
$qop -C zip -ry -9 /tmp/test.zip $compress 2>/dev/null
