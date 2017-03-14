#!/bin/sh
#start
LOG_FILE=/var/log/matrix.log
FILE=/mnt/mmc1/server.conf
ADDRESS="address="
NETMASK="netmask="
GATEWAY="gateway="
REBOOT="reboot=true"
ifconfig wlan0:1 192.168.6.6
/usr/sbin/mini-httpd -C /etc/mini-httpd.conf &
cd /mnt/mmc1
insmod dht11.ko 
insmod sht15.ko
sleep 5 
while [ 1 ]
do
  IP_ALL=$( /sbin/ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d "addr:"|tr -d "地址:")
#  IP_MASK=$(/sbin/ifconfig |grep inet| sed -n '1p'|awk '{print $4}'|awk -F ':' '{print $2}')
  IP_MASK=$(route -n |grep 'U[ \t]' | head -n 1 | awk '{print $3}')
  IP_GW=$(/sbin/route -n | grep wlan0 | grep UG | awk '{print $2}')

echo "------------------------------------------------"
echo "address:$IP_ALL"
echo "netmask:$IP_MASK"
echo "gateway:$IP_GW"
  
  cat $FILE | while read  line ;do

        echo "$line" | grep -q "$ADDRESS"
        if [ $? = 0 ];then
           IPCfg=${line#*$ADDRESS}
	   echo "$IP_ALL" |grep -q "$IPCfg"
	   if [ $? != 0 ];then
		
               echo "address change to:$IPCfg" >> $LOG_FILE
               /sbin/ifconfig wlan0 $IPCfg
	       continue
           fi
	   
        fi
	echo "$line" | grep -q "$NETMASK"
        if [ $? = 0 ];then
           MaskCfg=${line#*$NETMASK}
           if [ "$MaskCfg" != "$IP_MASK" ];
           then
                 echo "netmask change to:$MaskCfg" >> $LOG_FILE
                 /sbin/ifconfig wlan0 netmask $MaskCfg
			continue
#	else echo "netmask is same"
           fi
	   
        fi
		
	echo "$line" | grep -q "$GATEWAY"
        if [ $? = 0 ];then
           GwCfg=${line#*$GATEWAY}
           if [ "$GwCfg" != "$IP_GW" ];
           then
                 echo "gateway change to:$GwCfg" >> $LOG_FILE

                 /sbin/route del default gw $IP_GW
                 /sbin/route add default gw $GwCfg
			continue
#	else echo "gateway is same"
           fi
        fi
	echo "$line" | grep -q "$REBOOT"
        if [ $? = 0 ];then
	sed -i '/'"$REBOOT"'/d' $FILE
	    echo " reboot now ......"
	  /sbin/reboot
	fi
  done
sleep 5
echo "------------------------------------------------------"
cd /mnt/mmc1
i1=`ps -ef |grep matrix-sht |grep -v "grep" |wc -l`   

if [ $i1 -eq 0 ];then
   echo "restart matrix-sht !"
   sleep 1


   killall matrix-sht
   chmod +x matrix-sht
   ./matrix-sht &
   sleep 5
  echo matrix-sht restart on `date` >> $LOG_FILE
fi


i2=`ps -ef |grep mini-httpd |grep -v "grep" |wc -l`                                       
                                                                                
if [ $i2 -eq 0 ];then                                                           
   echo "restart mini-httpd !"                                        
   sleep 1                                                                      
                                                                                
                                                                                
   killall mini-httpd                                                 
   
   /usr/sbin/mini-httpd -C /etc/mini-httpd.conf &                      
   sleep 5                                             
  echo mini-httpd restart on `date` >> $LOG_FILE
fi
done

