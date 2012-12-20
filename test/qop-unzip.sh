. .findqop.sh

if [ $# -gt 0 ]; then 
zipfile=$@
else
echo "input a zip file you want to extract"
read zipfile
fi

[ ! -f $zipfile ] && echo "$zipfile is not a file" && exit

echo "$qop -C unzip -o $1 -d /tmp 2>/dev/null"
$qop -C unzip -o $zipfile -d /tmp 2>/dev/null
