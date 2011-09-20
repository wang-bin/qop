. .findqop.sh

if [ $# -gt 0 ]; then 
compress=$@
else
echo "input a directory you want to compress"
read compress
fi

[ ! -d $compress ] && echo "$compress does not exist" && exit


echo "tar zcvvf /tmp/test.tgz $compress |$qop $compress -m 2>/dev/null
	Compressing...Showing progress for size..."
tar zcvvf /tmp/test.tgz $compress |$qop $compress -m 2>/dev/null

echo "tar zcvf /tmp/test.tgz $compress |qop $compress -n  2>/dev/null
	Compressing...Showing progress for number of files..."
tar zcvf /tmp/test.tgz $compress |$qop $compress -n  2>/dev/null

echo "tar zxvvf /tmp/test.tgz -C /tmp |$qop -x /tmp/test.tgz 2>/dev/null
	Extracting...Showing progress for size..."
tar zxvvf /tmp/test.tgz -C /tmp |$qop -x /tmp/test.tgz 2>/dev/null

echo "tar zxvf /tmp/test.tgz -C /tmp |$qop -x /tmp/test.tgz 2>/dev/null
	Extracting...Showing progress for number of files..."

echo `tar -zt </tmp/test.tgz |wc -l`
tar zxvf /tmp/test.tgz -C /tmp |$qop -T `tar -zt </tmp/test.tgz |wc -l` -n 2>/dev/null
