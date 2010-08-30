#!/bin/sh
#Starting script for xiva-daemon-python

#Main options
ETCDIR='/etc'
NAME=$1
WORK_DIR=$ETCDIR/$NAME

XIVA_CONFIG=$WORK_DIR/xiva.conf
CUSTOM_FILE_TEMPLATE=xiva-*\.conf

LOCKFILE="/var/run/$NAME/xivadaemonstart.pid"
LOGFILE="/var/log/$NAME/xivadaemonstart.log"
RDELAY=1
EXIT_TIMEOUT=10


for CUSTOM_CONFIG in $WORK_DIR/$CUSTOM_FILE_TEMPLATE
do
    if [ -r $CUSTOM_CONFIG ]; then
	. $CUSTOM_CONFIG
    fi
done

if [ -e $XIVA_CONFIG  ]; then
    touch $LOCKFILE
    trap "rm $LOCKFILE 2>/dev/null" 0
    ps ax | grep "xivadaemonstart.sh $NAME$" | grep -v grep | awk '{print $1}' > $LOCKFILE
    if [ -d "$COREDIR" ]; then
	mkdir -p $COREDIR 2>/dev/null
    fi
    SLEEP_TIMEOUT=0
    while [ -f $LOCKFILE ]; do
	sleep $SLEEP_TIMEOUT
	STARTTIME=`date +%s`
	echo "/usr/bin/xivadaemon.py --config=$XIVA_CONFIG"
        /usr/bin/xivadaemon.py --config=$XIVA_CONFIG >> $LOGFILE 2>&1
        echo "on `date` ::xivadaemonstart.sh:: xivadaemonstart.sh on `hostname` restarted"  >>$LOGFILE
        NOWTIME=`date +%s`
	if [ $SLEEP_TIMEOUT -eq 0 -a $EXIT_TIMEOUT -gt 0 ]; then
	   WORKTIME=$(($NOWTIME-$STARTTIME))
	   if [ $WORKTIME -lt $EXIT_TIMEOUT ]; then
		echo "Fast xiva crash.  Exit 1"
		exit 1
	   fi
	fi
	SLEEP_TIMEOUT=$RDELAY
    done
else
    echo "Can not find $XIVA_CONFIG"
    exit 1
fi
