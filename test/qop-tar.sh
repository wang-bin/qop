. .findqop.sh

if [ $# -gt 0 ]; then 
compress=$@
else
echo "input a directory you want to compress"
read compress
fi

[ ! -d $compress ] && echo "$compress does not exist" && exit


echo "$qop -C tar zcvvf /tmp/test.tgz $compress 2>/dev/null"
echo "Compressing...Showing progress for size..."
$qop -C tar zcvvf /tmp/test.tgz $compress 2>/dev/null

echo "$qop -C tar zcvf /tmp/test.tgz $compress 2>/dev/null"
echo "Compressing...Showing progress for number of files..."
$qop -C tar zcvf /tmp/test.tgz $compress 2>/dev/null

echo "$qop -C tar zxvvf /tmp/test.tgz -C /tmp 2>/dev/null
	Extracting...Showing progress for size..."
$qop -C tar zxvvf /tmp/test.tgz -C /tmp 2>/dev/null
