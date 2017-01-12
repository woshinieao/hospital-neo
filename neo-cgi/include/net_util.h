#ifndef NET_UTIL_INCLUDE_H
#define NET_UTIL_INCLUDE_H

#define DHCPC_EXEC			    "dhcpcd"
#define DHCPC_EXEC_PATH			"/opt/ipnc/dhcpcd/"DHCPC_EXEC
#define DHCPC_PID_FILE_PATH		""_PATH_VARRUN"dhcpcd-%s.pid"
#define RESOLV_CONF				"/etc/resolv.conf"
#define PROCNET_ROUTE_PATH		"/proc/net/route"

/**
* @brief Structure to save network status
*/
struct NET_CONFIG {
	unsigned char mac[6];
	in_addr_t	ifaddr;
	in_addr_t	netmask;
	in_addr_t	gateway;
	in_addr_t	dns;
};

int net_nic_up(char *ifname);
int net_nic_down(char *ifname);

int net_set_flag(char *ifname, short flag);
int net_clr_flag(char *ifname, short flag);
int net_get_flag(char *ifname);

in_addr_t net_get_ifaddr(char *ifname);
int net_set_ifaddr(char *ifname, in_addr_t addr);

struct in_addr net_get_ip(int skfd, char *ifname);

in_addr_t net_get_netmask(char *ifname);
int net_set_netmask(char *ifname, in_addr_t addr);

int net_get_hwaddr(char *ifname, unsigned char *mac);

int net_add_gateway(in_addr_t addr);
int net_del_gateway(in_addr_t addr);
int net_clean_gateway(void);
in_addr_t net_get_gateway(void);
int net_set_gateway(in_addr_t addr);

in_addr_t net_get_dns(void);
int net_set_dns(char *dnsname);

pid_t net_start_dhcpcd(char *ifname);
int net_renew_dhcpcd(pid_t pid);
void net_enable_dhcpcd(void);
void net_disable_dhcpcd(void);

int net_get_info(char *ifname, struct NET_CONFIG *netcfg);

int Check_Link_status( char *pInterface );
int Check_Link_speed( char *pInterface );

#endif
