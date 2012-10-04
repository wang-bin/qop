#!/bin/bash
cat <<EOF
Test scripts for qop.
Wang Bin <wbsecg1@gmail.com>
2011-04-20

EOF

error()
{
  echo -e "\e[1;31merror:\e[m $1"
  exit 1
}


export PATH=../bin:$PATH

if [ -n "`uname -a |grep -E 'MINGW|CYGWIN'`" ];then
  qop="qop.exe"
  type -p $qop 2>/dev/null
  [ $? -ne 0 ] && qop="qop_win32.exe";
elif [ -n "`uname -a |grep 'Linux'`" ];then
  qop="qop"
  type -p $qop 2>/dev/null
  [ $? -ne 0 ] && qop="qop_linux"
  type -p $qop 2>/dev/null
  [ $? -ne 0 ] && qop="qop_linux_llvm"
fi

echo "$qop"
type -p $qop 2>/dev/null
[ $? -ne 0 ] && error "qop not found. Please set the PATH"

:<<COMMENT
qop="qop"; type -p $qop 2>/dev/null
[ $? -ne 0 ] && qop="qop.exe"; type -p $qop 2>/dev/null
[ $? -ne 0 ] && qop="qop-maemo5"; type -p $qop 2>/dev/null
[ $? -ne 0 ] && qop="qop-ezx"; type -p $qop 2>/dev/null
[ $? -ne 0 ] && echo 'qop not found. Please set the PATH' && exit
COMMENT
