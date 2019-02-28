#ifndef _DICTIONARY_CLIENT_H
#define _DICTIONARY_CLIENT_H

int dictionary_client_set_connection_string(const char *appId, const char *appKeys);
char *dictionary_client_send_request(const char *requiredWord);

#endif