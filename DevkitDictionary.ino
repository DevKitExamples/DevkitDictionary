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
static const char *key1 = "[Key 1 from Bing Speech API]";
static const char *appId = "[ApplicationId from the Dictionary API]";
static const char *appKeys = "[ApplicationKey from the Dictionary API]";
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
    Screen.print(1, "Press B for   ");
    Screen.print(2, "definition");
    Serial.println("Press B for definition   ");
    rgbLed.setColor(0, 0, 0);
}

void setup()
{
    Serial.begin(115200);
    InitBoard();
    Audio.format(8000, 16);
    dictionary_client_set_connection_string(appId, appKeys);
}

static void ScrollDefinition(void)
{
    if (msgBody[0] != 0 && digitalRead(USER_BUTTON_A) == LOW)
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
    Screen.print(0, "Release B to send\r\nMax duraion: 2 sec");
    Serial.println("Release B to send    ");
    rgbLed.setColor(RGB_LED_BRIGHTNESS, 0, 0);
}

static void DoReceived()
{
    Screen.clean();
    Screen.print(0, "Definition:");
    Screen.print(1, msgBody, true);
    rgbLed.setColor(0, 0, RGB_LED_BRIGHTNESS);
    Serial.println("Press A to scroll. Press B to look up another word.");
}

static void DoDictionaryRequest()
{
    Screen.print(0, "Processing ...     ");
    Screen.print(1, "Sending dictionary request", true);
    rgbLed.setColor(0, RGB_LED_BRIGHTNESS, 0);
    msgBody = dictionary_client_send_request(wordToSend);
}

static void DoSTTRequest()
{
    Screen.print(0, "Processing ...     ");
    Screen.print(1, "Sending text request", true);
    rgbLed.setColor(0, RGB_LED_BRIGHTNESS, 0);
}

static void DoSpeechToTextRequest()
{
    Audio.stop();
    int size = Audio.convertToMono(waveFile, Audio.getCurrentSize(), 16);
    if (size > 44)
    {
        DoSTTRequest();
        iot_client_get_token(key1);
        wordToSend = iot_client_send_audio(waveFile, size);
        status = 2;
    }
    else
    {
        Serial.println("No Data Recorded! ");
        wordToSend = NULL;
        return;
    }
}

void loop()
{
    // Wait until press button B to start record
    while (digitalRead(USER_BUTTON_B) == HIGH)
    {
        ScrollDefinition();
        delay(LOOP_DELAY);
    }
    Audio.startRecord(waveFile, AUDIO_SIZE);
    enterRecordState();

    // Wait until release B to send
    while (digitalRead(USER_BUTTON_B) == LOW) ;
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