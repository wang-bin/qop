. .findqop.sh

if [ $# -gt 0 ]; then 
rarfile=$@
else
echo "input a rar file you want to extract"
read rarfile
fi

[ ! -f $rarfile ] && echo "$rarfile is not a file" && exit

ehco "$qop -C unrar x -o+ $rarfile /tmp 2>/dev/null"
$qop -C unrar x -o+ $rarfile /tmp 2>/dev/null
