# Devkit Dictionary

This project shows how to configure DevKit to use Azure Bing Speech API Service and oxford dictionary API. Then you can search dictionary on Devkit with voice. In this tutorial, you will learn how to:

* Create and use Azure Bing Speech API Service
* Register and use oxford dictionary API

The status flow of our program:

![Status flow](status-flow.png)

## Before you begin

To complete the steps in this tutorial, you need the following:

* Prepare your DevKit with [Getting Started Guide](https://microsoft.github.io/azure-iot-developer-kit/docs/get-started/).
* Upgrade to latest firmware (>= 1.3.0) with [Firmware Upgrading](https://microsoft.github.io/azure-iot-developer-kit/docs/firmware-upgrading/) tutorial.

## Set up the Environment

### Setting up the Azure Environment (Bing Speech API)

1. Log in to the [Azure portal](https://portal.azure.com/).
2. Create a Bing Speech API using the green + sign. Create a new F0 tier speech API if needed. Pin it to your dashboard for easy access.

![Create Bing Speech API](screenshots/create-BingSpeechAPI.png)

3. Once the speech API has been created, navigate to its main page. Click “Keys”. Copy Key 1, it will be needed for later use.

![Copy Bing Speech API key1](screenshots/copy-BingSpeechAPI-key1.png)


### Setting up the dictionary API (Dictionary)

1. Visit https://developer.oxforddictionaries.com/
2. Click the “GET STARTED” and then “FREE”. This will prompt you to enter some information about your project.
    * Application type: Learning
    * Platform: Other-> IoT
    * Agree to terms and conditions
3. The website will send you an email to activate your account. Please do so.
4. Sign into your account and click on “Credentials”
5. Copy the ApplicationId and ApplicationKey. They will be needed in the code. 

### Placing the keys into the code and upload program

1. Open the DevkitDictionary.ino file. Replace the first three variables with the keys copied earlier.

| Variable | Value |
| :------: | :----:|
| key1 | Key 1 from Bing Speech API |
| appId | ApplicationId from the Dictionary API |
| appKeys | ApplicationKey from the Dictionary API |

2. Use **Quick Open** in VS Code (Windows: `Ctrl+P`, macOS: `Cmd+P`) and type **task device-upload** to build and upload the code to the DevKit.

## How to Use

Press and hold the B button while saying the word. The recording time is 2 seconds. Once the request has been processed, the definition of the word will appear on the screen. To scroll, press A. To start another quest, press B and say the word.

## Coding Exercises

### Changing Dictionary Language

The dictionary language can also be changed. The URL in the function app (named string url) can be changed to a variety of languages, documented [Supported languages](https://developer.oxforddictionaries.com/documentation/languages)

### Changing Dictionary Type

[API Documentation](https://developer.oxforddictionaries.com/documentation) shows how to call each API and other functions provided by Oxford Dictionaries.
These include translations, synonyms (Thesaurus) and other functionalities. 


## Problems and feedback

If you encounter problems, you can find [FAQs](https://microsoft.github.io/azure-iot-developer-kit/docs/faq/) if you encounter problems or reach out to us from the channels below.
