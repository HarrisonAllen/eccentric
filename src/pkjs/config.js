module.exports = [
    {
      "type": "heading",
      "defaultValue": "eccentric configuration"
    },
    {
      "type": "text",
      "defaultValue": "stylize eccentric your way"
    },
    {
      "type": "section",
      "items": [
        {
          "type": "heading",
          "defaultValue": "Colors"
        },
        {
          "type": "color",
          "messageKey": "BackgroundOuterColor",
          "defaultValue": "0xFFFFFF",
          "label": "Outer Background Color",
          "capabilities": ["RECT"],
          "sunlight": false,
          "allowGray": true
        },
        {
          "type": "color",
          "messageKey": "BackgroundInnerColor",
          "defaultValue": "0xFFFFFF",
          "label": "Inner Background Color",
          "capabilities": ["RECT"],
          "sunlight": false,
          "allowGray": true
        },
        {
          "type": "color",
          "messageKey": "BackgroundInnerColor",
          "defaultValue": "0xFFFFFF",
          "label": "Background Color",
          "capabilities": ["ROUND"],
          "sunlight": false,
          "allowGray": true
        },
        {
          "type": "color",
          "messageKey": "HourColor",
          "defaultValue": "0x000000",
          "label": "Hour Color",
          "sunlight": false,
          "allowGray": true
        },
        {
          "type": "color",
          "messageKey": "MinuteColor",
          "defaultValue": "0xFFFFFF",
          "label": "Minute Color",
          "sunlight": false,
          "allowGray": true
        }
      ]
    },
    {
      "type": "section",
      "items": [
        {
          "type": "heading",
          "defaultValue": "Seconds"
        },
        {
          "type": "toggle",
          "messageKey": "SecondsEnabled",
          "label": "Enable Seconds",
          "defaultValue": false
        },
        {
          "type": "color",
          "messageKey": "SecondsColor",
          "defaultValue": "0x000000",
          "label": "Seconds Color",
          "sunlight": false,
          "allowGray": true
        }
      ]
    },
    {
      "type": "section",
      "items": [
        {
          "type": "heading",
          "defaultValue": "Battery"
        },
        {
          "type": "toggle",
          "messageKey": "BatteryEnabled",
          "label": "Enable Battery Indicator",
          "defaultValue": false
        },
        {
          "type": "color",
          "messageKey": "BatteryColor",
          "defaultValue": "0xAAAAAA",
          "label": "Battery Color",
          "sunlight": false,
          "allowGray": true
        }
      ]
    },
    {
      "type": "submit",
      "defaultValue": "Save Settings"
    }
  ];