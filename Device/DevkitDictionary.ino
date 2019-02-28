#include "AZ3166WiFi.h"
#include "Arduino.h"
#include "RGB_LED.h"
#include "OLEDDisplay.h"
#include "dictionary_client.h"
#include "iot_speechAPI_client.h"
#include "AudioClassV2.h"

#define RGB_LED_BRIGHTNESS 16
#define LOOP_DELAY 100

//Required keys
static const char *SpeechServiceAppKey = "[Speech Service AppKey]";
static const char *OxfordDictionaryAppId = "[Oxford Dictionary AppId]";
static const char *OxfordDictionaryAppKey = "[Oxford Dictionary AppKey]";
//Required keys

//For execution of the code
static boolean hasWifi;
static int status = 0; //idle
static RGB_LED rgbLed;

static const int recordedDuration = 2;
static const int AUDIO_SIZE = ((32000 * recordedDuration) + 44);
static char waveFile[AUDIO_SIZE];
static AudioClass &Audio = AudioClass::getInstance();

static char *wordToSend = NULL;
static char *msgBody = NULL;
static int msgStart = 0;

static void InitWiFi()
{
    if (WiFi.begin() == WL_CONNECTED)
    {
        hasWifi = true;
    }
    else
    {
        Screen.clean();
        Screen.print(1, "No Wi-Fi           ");
        hasWifi = false;
    }
}

static void InitBoard(void)
{
    Screen.clean();
    Screen.print(0, "Azure IoT DevKit     ");
    Screen.print(2, "Initializing...      ");

    //Initialize the WiFi module
    Screen.print(3, " > WiFi              ");
    hasWifi = false;
    rgbLed.setColor(RGB_LED_BRIGHTNESS, 0, 0);
    InitWiFi();
    rgbLed.setColor(0, 0, 0);

    if (hasWifi == true)
    {
        enterIdleState();
    }
}

static void enterIdleState()
{
    status = 0;
    Screen.clean();
    Screen.print(0, "IoT Dictionary");
    Screen.print(1, "Start Record...");
    Screen.print(2, ">>Press A<<");
    Serial.println("Press A for definition");
    rgbLed.setColor(0, 0, 0);
}

void setup()
{
    Serial.begin(115200);
    InitBoard();
    Audio.format(8000, 16);
    dictionary_client_set_connection_string(OxfordDictionaryAppId, OxfordDictionaryAppKey);
}

static void ScrollDefinition(void)
{
    if (msgBody != NULL && msgBody[0] != 0 && digitalRead(USER_BUTTON_B) == LOW)
    {
        //Scroll it if Button A has been pressed
        msgStart += 16;
        if (msgStart >= strlen(msgBody))
        {
            msgStart = 0;
        }

        //Clean the msg screen
        Screen.print(1, " ");
        Screen.print(2, " ");
        Screen.print(3, " ");

        //Update it
        Screen.print(1, &msgBody[msgStart], true);
    }
}

static void enterRecordState()
{
    Screen.clean();
    Screen.print(0, "Start Recording");
    Screen.print(1, "Release A to send");
    Screen.print(2, "Max duraion: 2 sec");
    Serial.println("Release A to send    ");
    rgbLed.setColor(RGB_LED_BRIGHTNESS, 0, 0);
}

static void DoReceived()
{
    Screen.clean();
    Screen.print(0, wordToSend);
    Screen.print(1, msgBody, true);
    rgbLed.setColor(0, 0, RGB_LED_BRIGHTNESS);
    Serial.print("Definition: ");
    Serial.println(msgBody);
    Serial.println("Press B to scroll. Press A to look up another word.");
}

static void DoDictionaryRequest()
{

    Screen.print(3, "Dictionary<<");
    rgbLed.setColor(0, RGB_LED_BRIGHTNESS, 0);
    if (msgBody != NULL)
    {
        free(msgBody);
    }
    msgBody = dictionary_client_send_request(wordToSend);
}

static void DoSTTRequest()
{
    Screen.clean();
    Screen.print(0, "Processing ...     ");
    Screen.print(1, "Speech Service<<", true);
    rgbLed.setColor(0, RGB_LED_BRIGHTNESS, 0);
}

static void DoSpeechToTextRequest()
{
    Audio.stop();
    int size = Audio.convertToMono(waveFile, Audio.getCurrentSize(), 16);
    if (size > 44)
    {
        DoSTTRequest();
        iot_client_get_token(SpeechServiceAppKey);
        if (wordToSend != NULL)
        {
            free(wordToSend);
        }
        wordToSend = iot_client_send_audio(waveFile, SpeechServiceAppKey, size);
        if(wordToSend != NULL)
        {
            Screen.print(2, wordToSend);
            status = 2;
            return;
        }

    }
    Screen.print(2, "No Text Returned");   
    Serial.println("No Data Recorded! ");
    wordToSend = NULL;
    return;
}

void loop()
{
    // Wait until press button A to start record
    while (digitalRead(USER_BUTTON_A) == HIGH)
    {
        ScrollDefinition();
        delay(LOOP_DELAY);
    }
    Audio.startRecord(waveFile, AUDIO_SIZE);
    enterRecordState();

    // Wait until release A to send
    while (digitalRead(USER_BUTTON_A) == LOW) ;
    DoSpeechToTextRequest();
    if (wordToSend == NULL)
    {
        Serial.println("Speech to Text request failed");
        return;
    }
    DoDictionaryRequest();
    DoReceived();
    return;
}