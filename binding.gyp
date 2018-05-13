{
  "targets": [
    {
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "C:\\Users\\pehzi\\source\\repos\\mqtt_addons\\nodejs\\Paho_MQTT\\include"
      ],
      "target_name": "mqtt",
      "sources": [ "main.cpp" ],
      "link_settings": {
       "libraries": [ "C:\\Users\\pehzi\\source\\repos\\mqtt_addons\\nodejs\\Paho_MQTT\\lib\\paho-mqtt3a.lib", 
       "C:\\Users\\pehzi\\source\\repos\\mqtt_addons\\nodejs\\Paho_MQTT\\lib\\paho-mqtt3as.lib", 
       "C:\\Users\\pehzi\\source\\repos\\mqtt_addons\\nodejs\\Paho_MQTT\\lib\\paho-mqtt3c.lib", 
       "C:\\Users\\pehzi\\source\\repos\\mqtt_addons\\nodejs\\Paho_MQTT\\lib\\paho-mqtt3cs.lib" ]
      }      
    }
  ]
}