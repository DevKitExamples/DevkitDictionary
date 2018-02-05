#ifndef _IOT_SPEECHAPI_CLIENT_H
#define _IOT_SPEECHAPI_CLIENT_H

#define COGNITIVE_API_TOKEN_URL "https://api.cognitive.microsoft.com/sts/v1.0/issueToken"
#define INTERATIVE_SPEECH_URL "https://speech.platform.bing.com/speech/recognition/interactive/cognitiveservices/v1?language=en-US&format=simple"

int iot_client_get_token(const char *key1);
char *iot_client_send_audio(const char *content, int length);

#endif