#include "dictionary_client.h"
#include "Arduino.h"
#include "http_client.h"
#include "AZ3166Wifi.h"
#include "parson.h"
#include <stdlib.h>

static const char *appIdInternal = NULL;
static const char *appKeysInternal = NULL;

static const char Oxford_Cert[] =
"-----BEGIN CERTIFICATE-----\r\nMIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\r\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\nd3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\r\n"
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\r\nMAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\r\n"
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\r\nRVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\r\n"
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\r\nPNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\r\n"
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\r\nIk5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\r\n"
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\r\nEsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\r\n"
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\r\nFLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\r\n"
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\r\neM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\r\n"
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\r\nYzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\r\n"
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\r\n+OkuE6N36B9K\r\n-----END CERTIFICATE-----\r\n";

int dictionary_client_set_connection_string(const char *appId, const char *appKeys)
{
    appIdInternal = appId;
    appKeysInternal = appKeys;
    return 0;
}

char *dictionary_client_send_request(const char *requiredWord)
{
    if (requiredWord == NULL || strlen(requiredWord) <= 0)
    {
        Serial.println("Invalid word");
        return NULL;
    }
    int len = snprintf(NULL, 0, "https://od-api.oxforddictionaries.com/api/v1/entries/en/%s/definitions", requiredWord);
    char *url = (char *)malloc(len + 1);
    snprintf(url, len + 1, "https://od-api.oxforddictionaries.com/api/v1/entries/en/%s/definitions", requiredWord);

    HTTPClient dictionaryRequest = HTTPClient(Oxford_Cert, HTTP_GET, url);
    dictionaryRequest.set_header("app_id", appIdInternal);
    dictionaryRequest.set_header("app_key", appKeysInternal);
    const Http_Response *response = dictionaryRequest.send();
    if (response != NULL && response -> status_code == 200)
    {
        JSON_Object *jsonObject = json_value_get_object(json_parse_string(response->body));
        const char * propertyName[4] = {"results", "lexicalEntries", "entries", "senses"};
        for (int i = 0; i < 4; ++i)
        {
            jsonObject = json_array_get_object(json_object_get_array(jsonObject, propertyName[i]), 0);
        }
        free(url);
        const char *result = json_array_get_string(json_object_get_array(jsonObject, "definitions"), 0);
        if(!result)
        {
            return NULL;
        }
        char *ret = (char *)malloc(strlen(result) + 1);
        strcpy(ret, result);
        return ret;
    }
    free(url);
    return NULL;
}