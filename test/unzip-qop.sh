source ./.findqop.sh

if [ $# -gt 0 ]; then
zipfile=$@
else
echo "input a zip file you want to extract"
read zipfile
fi

[ ! -f $zipfile ] && echo "$zipfile is not a file" && exit

echo "unzip -o $zipfile -d /tmp |$qop -x $zipfile 2>/dev/null"
unzip -o $zipfile -d /tmp |$qop -x $zipfile 2>/dev/null
