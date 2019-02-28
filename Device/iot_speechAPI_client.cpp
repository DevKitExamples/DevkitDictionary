#include "iot_speechAPI_client.h"
#include "Arduino.h"
#include "http_client.h"
#include "AZ3166Wifi.h"
#include "parson.h"
#include <stdlib.h>
#include <string.h>

static char *current_token = NULL;

int iot_client_get_token(const char *serviceKey)
{
    time_t currentTime = time(NULL);
    if (currentTime == (time_t)-1 || currentTime < 1492333149)
    {
        Serial.println("Time does not appear to be working.");
        return -1;
    }

    HTTPClient tokenRequest = HTTPClient(HTTP_POST, COGNITIVE_API_TOKEN_URL);
    tokenRequest.set_header("Host", COGNITIVE_API_TOKEN_HOST);
    tokenRequest.set_header("Content-type", "application/x-www-form-urlencoded");
    tokenRequest.set_header("Content-Length", "0");
    tokenRequest.set_header("Ocp-Apim-Subscription-Key", serviceKey);
    const Http_Response *response = tokenRequest.send();

    if (response == NULL)
    {
        Serial.println("Token retrieval failed");
        return -1;
    }
    else if (response->status_code == 403)
    {
        Serial.println("Authentication failed, please check variable serviceKey");
        return -1;
    }
    else
    {
        const char *result = response->body;
        if (current_token != NULL)
        {
            free(current_token);
        }
        current_token = (char *)malloc(strlen(result) + 1);
        strcpy(current_token, result);
        return 0;
    }
}

char *iot_client_send_audio(const char *content, const char *serviceKey, int length)
{
    String bearer = "Bearer ";
    String auth = (bearer + current_token);
    HTTPClient conversionRequest = HTTPClient(HTTP_POST, INTERATIVE_SPEECH_URL);
    conversionRequest.set_header("Ocp-Apim-Subscription-Key", serviceKey);
    conversionRequest.set_header("Accept", "application/json");
    conversionRequest.set_header("Content-type", "audio/wav; codec="
                                                 "audio/pcm"
                                                 "; samplerate=8000");
    conversionRequest.set_header("Authorization", auth.c_str());
    const Http_Response *response = conversionRequest.send(content, length);
    
    if (response == NULL)
    {
        Serial.println("No response, error");
        return NULL;
    }
    Serial.println(response->body);
    JSON_Object *jsonObject = json_value_get_object(json_parse_string(response->body));
    if (jsonObject != NULL)
    {
        const char *json_Status = json_object_get_string(jsonObject, "RecognitionStatus");
        if (json_Status == NULL)
        {
            Serial.println("Unable to get string from json object for \"Status\"");
        }

        if (strcmp(json_Status, "Success") == 0)
        {
            char *json_DisplayText = (char *)json_object_get_string(jsonObject, "DisplayText");
            if (json_DisplayText == NULL)
            {
                Serial.println("Unable to get string from json object for \"DisplayText\"");
            }
            else
            {
                json_DisplayText[strlen(json_DisplayText) - 1] = 0;
                Serial.print("Word: ");
                Serial.println(json_DisplayText);
                char *ret = (char *)malloc(strlen(json_DisplayText) + 1);
                strcpy(ret, json_DisplayText);
                return ret;
            }
        }
        else
        {
            Serial.println("RecognitionStatus = Failed");
        }
    }
    return NULL;
}