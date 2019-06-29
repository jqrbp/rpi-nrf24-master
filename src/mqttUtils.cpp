#include <signal.h>
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <mosquitto.h>
#include "parser.h"
#include "nrf24Utils.h"
#include "configUtils.h"

char strbuff[15];

const char topicSet[21] = "homebridge/from/set/";
const char topicReach[34] = "homebridge/from/get/reachability/";

char clientid[24];
struct mosquitto *mosq;
int rc = 0;
int mqtt_thrun = 1;

static int run = 1;
static int run_old = 0;

void mqtt_signal_handle(int s) {
  run = 0;
  printf("waiting mqtt thread to exit\r\n");
  while(mqtt_thrun) sleep(5);
  printf("mqtt thread exited\r\n");
  exit(s);
}

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	printf("connect callback, rc=%d\n", result);
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
  char buff[50];
  char cmdChr = '0';
	bool match = 0;
  int data[3];
  const char *wrfid = (const char*)get_config_wrf_id();
  uint8_t dataLen;

	printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);
  sprintf(buff, "%s%s", topicSet, wrfid);
	mosquitto_topic_matches_sub(buff, message->topic, &match);
  if(match) cmdChr = 'S';
	else {
    sprintf(buff, "%s%s", topicReach, wrfid);
    mosquitto_topic_matches_sub(buff, message->topic, &match);
	  if (match) {
      cmdChr = 'A';
    }
  }

  if (match) {
		printf("got message for set/reach topic\n");
    if (parser_check_id((char*) message->payload, wrfid, message->payloadlen)) {
      parseCommand((char*) message->payload, strlen(wrfid) + 1, data, &dataLen);
      if (dataLen == 3) {
          sendMeshMasterCmd(cmdChr, (uint8_t)data[0], (uint8_t)data[1], (uint8_t)data[2]);
      }
    }
	}
}

void mqtt_publish_val(uint8_t _id, uint8_t _pin, int32_t _val) {
  if(run && !rc){
    sprintf(strbuff, "%s,%d,%d,%d!", (const char*)get_config_wrf_id(), (int)_id, (int)_pin, (int)_val);
    mosquitto_publish(mosq, NULL, "homebridge/to/set", strlen(strbuff), strbuff, 0, false);
  }
}

void mqtt_publish_reach(uint8_t _id) {
  if(run && !rc){
    sprintf(strbuff, "%s,%d!", (const char*)get_config_wrf_id(), (int)_id);
    mosquitto_publish(mosq, NULL, "homebridge/to/set/reachability", strlen(strbuff), strbuff, 0, false);
  }
}

void *mqtt_thread_func(void *arg) {
  while(mqtt_thrun) {
    if(run) {
      if(run ^ run_old) {
        printf("mqtt connected!\r\n");
      }
      rc = mosquitto_loop(mosq, -1, 1);
      if(run && rc){
        printf("connection error!\n");
        sleep(10);
        mosquitto_reconnect(mosq);
      }
    } else {
      if(run ^ run_old) {
        printf("stopping mqtt!\r\n");
      }
      mosquitto_destroy(mosq);
      mosquitto_lib_cleanup();
      mqtt_thrun = 0;
    }
    (run_old ^ run) ? run_old = run : NULL;
  }

  return NULL;
}

void mqtt_begin(void) {
  pthread_t mqtt_th;
  char buffReach[50];
  char buffSet[50];
  const char *wrfid = (const char*)get_config_wrf_id();

	signal(SIGINT, mqtt_signal_handle);
	signal(SIGTERM, mqtt_signal_handle);

	mosquitto_lib_init();

	memset(clientid, 0, 24);
	snprintf(clientid, 23, "%s%d", wrfid, getpid());
	mosq = mosquitto_new(clientid, true, 0);

  if(mosq){
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);
    mosquitto_username_pw_set(mosq, (const char*)get_config_mqtt_user(), (const char*)get_config_mqtt_pass());
    mosquitto_tls_insecure_set(mosq, false);
    mosquitto_tls_set(mosq, (const char*)get_config_mqtt_cafile(), NULL, (const char*)get_config_mqtt_certfile(), (const char*)get_config_mqtt_keyfile(), NULL);
    mosquitto_tls_opts_set(mosq, 1, (const char*)get_config_mqtt_tlsver(), NULL);
	  rc = mosquitto_connect(mosq, (const char*)get_config_mqtt_host(), get_config_mqtt_port(), 60);

    sprintf(buffReach, "%s%s", topicReach, wrfid);
    sprintf(buffSet, "%s%s", topicSet, wrfid);

		mosquitto_subscribe(mosq, NULL, buffReach, 0);
    mosquitto_subscribe(mosq, NULL, buffSet, 0);

    mqtt_thrun = 1 ;
    pthread_create(&mqtt_th,NULL,mqtt_thread_func,(void *)"starting mqtt thread ...");
  }
}
