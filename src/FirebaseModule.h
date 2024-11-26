// FirebaseModule.h

#ifndef FIREBASE_MODULE_H
#define FIREBASE_MODULE_H

#include <Firebase_ESP_Client.h>

// Function to initialize Firebase
void initFirebase(const char* apiKey, const char* databaseUrl);

// Function to send data to Firebase
bool sendSensorData(const String& path, int value);

#endif // FIREBASE_MODULE_H