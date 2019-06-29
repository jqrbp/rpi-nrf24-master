#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <ctype.h>
#include "configUtils.h"

struct config_file_parameters params;

void init_parameters (struct config_file_parameters *_parms)
{
  strncpy (_parms->wrf_id, "wrf0", MAXSTRLEN);
  strncpy (_parms->mqtt_host, "localhost", MAXSTRLEN);
  _parms->mqtt_port = 1883;

  strncpy (_parms->mqtt_user, "mqttuser", MAXSTRLEN);
  strncpy (_parms->mqtt_pass, "mqttpass", MAXSTRLEN);
  strncpy (_parms->mqtt_cafile, "ca.crt", MAXSTRLEN);
  strncpy (_parms->mqtt_certfile, "client.crt", MAXSTRLEN);
  strncpy (_parms->mqtt_keyfile, "client.key", MAXSTRLEN);
  strncpy (_parms->mqtt_tlsver, "tlsv1.1", MAXSTRLEN);
  strncpy (_parms->nrf24_datarate, "RF24_1MBPS", MAXSTRLEN);
  _parms->nrf24_channel = 97;
}

char *trim (char * s)
{
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];

  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) ) s2--;
  *(s2+1) = '\0';

  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) ) s1++;

  /* Copy finished string */
  strcpy (s, s1);
  return s;
}


void parse_config_file(const char *_file, struct config_file_parameters *_parms) {
    char *s, buff[256];
    FILE *fp = fopen (_file, "r");
    printf("opening config file: %s\r\n", _file);

    if (fp == NULL) {
        printf("FAILED to open config file!");
        return;
    }

    /* Read next line */
    while ((s = fgets (buff, sizeof buff, fp)) != NULL) {
        /* Skip blank lines and comments */
        if (buff[0] == '\n' || buff[0] == '#')
            continue;

        /* Parse name/value pair from line */
        char name[MAXSTRLEN], value[MAXSTRLEN];
        s = strtok (buff, " =");
        if (s==NULL) continue;
        else strncpy (name, s, MAXSTRLEN);

        trim(name);

        s = strtok (NULL, " =");
        if (s==NULL) continue;
        else strncpy (value, s, MAXSTRLEN);

        trim (value);

        /* Copy into correct entry in parameters struct */
        if (strcmp(name, "wrf_id")==0) strncpy (_parms->wrf_id, value, MAXSTRLEN);
        else if (strcmp(name, "mqtt_host")==0) strncpy (_parms->mqtt_host, value, MAXSTRLEN);
        else if (strcmp(name, "mqtt_port")==0) _parms->mqtt_port = atoi(value);
        else if (strcmp(name, "mqtt_user")==0) strncpy (_parms->mqtt_user, value, MAXSTRLEN);
        else if (strcmp(name, "mqtt_pass")==0) strncpy (_parms->mqtt_pass, value, MAXSTRLEN);
        else if (strcmp(name, "mqtt_cafile")==0) strncpy (_parms->mqtt_cafile, value, MAXSTRLEN);
        else if (strcmp(name, "mqtt_certfile")==0) strncpy (_parms->mqtt_certfile, value, MAXSTRLEN);
        else if (strcmp(name, "mqtt_keyfile")==0) strncpy (_parms->mqtt_keyfile, value, MAXSTRLEN);
        else if (strcmp(name, "mqtt_tlsver")==0) strncpy (_parms->mqtt_tlsver, value, MAXSTRLEN);
        else if (strcmp(name, "nrf24_datarate")==0) strncpy (_parms->nrf24_datarate, value, MAXSTRLEN);
        else if (strcmp(name, "nrf24_channel")==0) _parms->nrf24_channel = atoi(value);
        else
            printf ("WARNING: %s/%s: Unknown name/value pair!\n", name, value);
    }

    /* Close file */
    fclose (fp);
}

void read_config_file(int argc, char *argv[]) {
    char config_file[MAXFILENAMELEN] = "rpi_nrf24_master.conf";
    if (argc > 1) strncpy(config_file, argv[1], strlen(argv[1]));

    printf ("Initializing parameters to default values...\n");
    init_parameters(&params);

    printf ("Reading config file...\n");
    parse_config_file(config_file, &params);

    printf ("config:\r\n");
    printf ("wrf_id: %s\r\n", params.wrf_id);
    printf ("mqtt_host: %s\r\n", params.mqtt_host);
    printf ("mqtt_port: %d\r\n", params.mqtt_port);
    printf ("mqtt_cafile: %s\r\n", params.mqtt_cafile);
    printf ("mqtt_certfile: %s\r\n", params.mqtt_certfile);
    printf ("mqtt_keyfile: %s\r\n", params.mqtt_keyfile);
    printf ("mqtt_tlsver: %s\r\n", params.mqtt_tlsver);
    printf ("nrf24_datarate: %s\r\n", params.nrf24_datarate);
    printf ("nrf24_channel: %d\r\n", params.nrf24_channel);
}

char* get_config_wrf_id(void) {
    return params.wrf_id;
}
char* get_config_mqtt_host(void) {
    return params.mqtt_host;
}
int get_config_mqtt_port(void) {
    return params.mqtt_port;
}
char* get_config_mqtt_user(void) {
    return params.mqtt_user;
}
char* get_config_mqtt_pass(void) {
    return params.mqtt_pass;
}
char *get_config_mqtt_cafile(void) {
    return params.mqtt_cafile;
}
char *get_config_mqtt_certfile(void) {
    return params.mqtt_certfile;
}
char *get_config_mqtt_keyfile(void) {
    return params.mqtt_keyfile;
}
char *get_config_mqtt_tlsver(void) {
    return params.mqtt_tlsver;
}
char* get_config_nrf24_datarate(void) {
    return params.nrf24_datarate;
}
int get_config_nrf24_channel(void) {
    return params.nrf24_channel;
}
