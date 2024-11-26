// FirebaseModule.cpp

#include "FirebaseModule.h"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOk = false;

void initFirebase(const char* apiKey, const char* databaseUrl) {
  config.api_key = apiKey;
  config.database_url = databaseUrl;

  // Sign up for anonymous authentication
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Firebase signup successful.");
    signupOk = true;
  } else {
    Serial.printf("Firebase signup error: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

bool sendSensorData(const String& path, int value) {
  if (Firebase.RTDB.setInt(&fbdo, path, value)) {
    // Serial.printf("Data sent successfully to %s : %d\n", path.c_str(), value);
    return true;
  } else {
    Serial.printf("Failed to send data to %s : %d\n", path.c_str(), value);
    Serial.println(fbdo.errorReason());
    return false;
  }
}