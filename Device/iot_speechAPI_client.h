#ifndef _IOT_SPEECHAPI_CLIENT_H
#define _IOT_SPEECHAPI_CLIENT_H

/*Replace the following definitions from the value in https://docs.microsoft.com/en-us/azure/cognitive-services/speech-service/rest-apis*/
#define COGNITIVE_API_TOKEN_URL "https://westus.api.cognitive.microsoft.com/sts/v1.0/issueToken"
#define COGNITIVE_API_TOKEN_HOST "westus.api.cognitive.microsoft.com"
#define INTERATIVE_SPEECH_URL "https://westus.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1?language=en-US"

int iot_client_get_token(const char *serviceKey);
char *iot_client_send_audio(const char *content, const char *serviceKey, int length);

#endif