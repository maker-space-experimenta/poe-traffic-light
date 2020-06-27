#include "mqttLogic.h"

#include <MQTT.h>
#include <ArduinoJson.h>

#include "eth.h"
#include "light.h"

#define DISCOVERY_PREFIX "homeassistant"

MQTTClient mqtt(512);
String clientStr, baseTopic;

void mqttSendAutoDiscovery() {
    DynamicJsonDocument payload(500);
    
    payload["~"] = baseTopic;
    payload["ret"] = false; //enable retain of messages on MQTT broker
    payload["name"] = clientStr;
    payload["unique_id"] = clientStr;
    payload["cmd_t"] = "~/set";
    payload["stat_t"] = "~/state";
    payload["schema"] = "json";

    payload["effect"] = true;
    
    JsonArray fx = payload.createNestedArray("effect_list");
    for(uint8_t i = 0; i < getStateCount(); i++) {
        fx.add(getStateNames()[i]);
    }

    // DEBUG.println("Payload:");
    // serializeJsonPretty(payload, DEBUG);

    String buf;
    serializeJson(payload, buf);
    mqtt.publish(String(baseTopic + "/config"), buf, true, 0); // retain = true
}

void mqttSendState() {
    DynamicJsonDocument payload(500);
    payload["state"] = (getCurrentStateId() > 0) ? "ON" : "OFF";
    payload["effect"] = getStateNames()[getCurrentStateId()];

    // DEBUG.println("State:");
    // serializeJsonPretty(payload, DEBUG);

    String buf;
    serializeJson(payload, buf);
    mqtt.publish(String(baseTopic + "/state"), buf, true, 0); // retain = true
}

void parseHAssCmd(String &payload) {
    DynamicJsonDocument doc(500);
    DeserializationError err = deserializeJson(doc, payload);
    if(err) {
        DEBUG.println("MQTT JSON payload parsing error");
        DEBUG.println(err.c_str());
        return;
    }

    lightSetEnable(doc["state"] == "ON");

    JsonVariant br = doc["brightness"];
    if(!br.isNull()) {
        // TODO brightness
    }

    JsonVariant fx = doc["effect"];
    if(!fx.isNull()) {
        lightSetState(fx.as<const char*>());
    }

    mqttSendState();
}

void mqttMessageHandler(String &topic, String &payload) {
    DEBUG.println("incoming: " + topic + " - " + payload);
    if(topic == baseTopic + "/set") {
        parseHAssCmd(payload);
    }
}

bool firstConnect = true;
void mqttConnect() {
    DEBUG.print("Connecting to broker");
    
    uint32_t initStart = millis();
    mqtt.setOptions(5, false, 1000);
    while(!mqtt.connect(clientStr.c_str(), MQTT_USER, MQTT_PASS)) {
        DEBUG.print(".");
        delay(250);
        if(millis() - initStart > 10000) {
            DEBUG.println("\nTimeout");
            return;
        }
    }
    DEBUG.println(" done.");
    mqtt.subscribe(baseTopic + "/set");

    if(!firstConnect) {
        mqttSendState();
    } 
}

void mqttInit() {
    mqtt.begin(BROKER_IP, net);
    mqtt.onMessage(mqttMessageHandler);

    clientStr = String("poe_trafficlight-") + String(macStr);
    baseTopic = DISCOVERY_PREFIX "/light/" + clientStr;

    mqttConnect();
    mqttSendAutoDiscovery();
    if(firstConnect) {
        firstConnect = false;
        delay(1); // somehow homeassistant doesn't recognize the initial state if it's sent too quickly after the config
        // mqttSendState(); // moved to main.cpp
    } 
}

void mqttLoop() {
    if(!mqtt.connected()) {
        mqttConnect();
    }
    mqtt.loop();
}