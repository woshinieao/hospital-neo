#!/bin/sh
#start
FILE=/etc/network/interfaces-bak
ADDRESS=address


cd /mnt/mmc1
/usr/sbin/mini-httpd -C /etc/mini-httpd.conf &

while [ 1 ]
do
    IP_ALL=$( /sbin/ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d "addr:"|tr -d "......:")
    echo  "local ip is:$IP_ALL---"
    cat $FILE | while read  line ;do
    echo "$line" | grep -q "$ADDRESS"
    if [ $? = 0 ];then
        IPCfg=${line#*$ADDRESS}
        echo "config_ip:$IPCfg "
        if [ "$IPCfg" != "$IP_ALL" ];
        then
            echo "ip_changed :$IPCfg"
            /sbin/ifconfig eth0 $IPCfg
            continue
        fi
    fi
    echo "$line" | grep -q "$REBOOT"                                                                                                                      if [ $? = 0 ];then
      sed -i '/'"$REBOOT"'/d' $FILE
      echo " reboot now ......"
      /sbin/reboot
    fi                                                                                                   
  done                                                                                                                                                                                                                                                         
sleep 10


cd /mnt/mmc1
i1=` pgrep -f matrix-temp_humidity |wc -l`

if [ $i1 -eq 0 ];then
   echo "restart matrix-temp_humidity !"
   sleep 1
   
   
   killall matrix-temp_humidity
   chmod +x matrix-temp_humidity
   ./matrix-temp_humidity &
   sleep 5
  echo matrix-temp_humidity restart on `date` >> $LOG_FILE
fi




done
