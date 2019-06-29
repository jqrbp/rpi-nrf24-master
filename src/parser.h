#ifndef __PARSER_H__
#define __PARSER_H__

void parseCommand(char* command, uint8_t _sIdx, int* returnValues, uint8_t *valLen);
bool parser_check_id(const char *_payld, const char *_id, unsigned int len);
void exec_http_postbody(const char *payload);

#endif //__PARSER_H__