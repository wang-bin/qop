. .findqop.sh

if [ $# -gt 0 ]; then
rarfile=$@
else
echo "input a rar file you want to extract"
read rarfile
fi

[ ! -f $rarfile ] && echo "$rarfile is not a file" && exit

ehco "unrar x -o+ $rarfile /tmp |$qop -x $rarfile 2>/dev/null"
unrar x -o+ $rarfile /tmp |$qop -x $rarfile 2>/dev/null

