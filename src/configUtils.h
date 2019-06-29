#ifndef __CONFIG_UTILS_H__
#define __CONFIG_UTILS_H__

#define MAXFILENAMELEN 100
#define MAXSTRLEN 80

struct config_file_parameters {
    char wrf_id[MAXSTRLEN];
    char mqtt_host[MAXSTRLEN];
    int mqtt_port;
    char mqtt_user[MAXSTRLEN];
    char mqtt_pass[MAXSTRLEN];
    char mqtt_cafile[MAXSTRLEN];
    char mqtt_certfile[MAXSTRLEN];
    char mqtt_keyfile[MAXSTRLEN];
    char mqtt_tlsver[MAXSTRLEN];
    char nrf24_datarate[MAXSTRLEN];
    int nrf24_channel;
};

void read_config_file(int argc, char *argv[]);
char* get_config_wrf_id(void);

char* get_config_mqtt_host(void);
int get_config_mqtt_port(void);
char* get_config_mqtt_user(void);
char* get_config_mqtt_pass(void);
char *get_config_mqtt_cafile(void);
char *get_config_mqtt_certfile(void);
char *get_config_mqtt_keyfile(void);
char *get_config_mqtt_tlsver(void);

char* get_config_nrf24_datarate(void);
int get_config_nrf24_channel(void);

#endif //__CONFIG_UTILS_H__