#include "_Plugin_Helper.h"

#ifdef USES_P023

// #######################################################################################################
// #################################### Plugin 023: OLED SSD1306 display #################################
// #######################################################################################################

/** Changelog:
 * 2023-10-16 tonhuisman: Bugfix: Template parsing stopped after initial display since previous updates :-(
 * 2023-03-18 tonhuisman: Show current on-display content on Devices page (75% size, omits trailing empty lines)
 *                        Manually set content via command: oled,x,y,<content> is included in the Devices page content
 *                        Make Interval optional
 *                        Move PLUGIN_READ and PLUGIN_WRITE code into P023_data_struct source
 *                        Several optimizations, like reading the template into memory only once
 * 2023-03-07 tonhuisman: Parse text to display without trimming off leading and trailing spaces
 * 2022-10-09 tonhuisman: Deduplicate code by moving the OLed I2C Address check to OLed_helper
 * 2022-10: Start changelog, latest on top.
 */

# include "src/PluginStructs/P023_data_struct.h"

// Sample templates
//  Temp: [DHT11#Temperature]   Hum:[DHT11#humidity]
//  DS Temp:[Dallas1#Temperature#R]
//  Lux:[Lux#Lux#R]
//  Baro:[Baro#Pressure#R]

# define PLUGIN_023
# define PLUGIN_ID_023         23
# define PLUGIN_NAME_023       "Display - OLED SSD1306"
# define PLUGIN_VALUENAME1_023 "OLED"

boolean Plugin_023(uint8_t function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
    {
      Device[++deviceCount].Number           = PLUGIN_ID_023;
      Device[deviceCount].Type               = DEVICE_TYPE_I2C;
      Device[deviceCount].VType              = Sensor_VType::SENSOR_TYPE_NONE;
      Device[deviceCount].Ports              = 0;
      Device[deviceCount].PullUpOption       = false;
      Device[deviceCount].InverseLogicOption = false;
      Device[deviceCount].FormulaOption      = false;
      Device[deviceCount].ValueCount         = 0;
      Device[deviceCount].SendDataOption     = false;
      Device[deviceCount].TimerOption        = true;
      Device[deviceCount].TimerOptional      = true;
      break;
    }

    case PLUGIN_GET_DEVICENAME:
    {
      string = F(PLUGIN_NAME_023);
      break;
    }

    case PLUGIN_GET_DEVICEVALUENAMES:
    {
      strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_023));
      break;
    }

    case PLUGIN_I2C_HAS_ADDRESS:
    case PLUGIN_WEBFORM_SHOW_I2C_PARAMS:
    {
      success = OLedI2CAddressCheck(function, event->Par1, F("i2c_addr"), PCONFIG(0));

      break;
    }

    # if FEATURE_I2C_GET_ADDRESS
    case PLUGIN_I2C_GET_ADDRESS:
    {
      event->Par1 = PCONFIG(0);
      success     = true;
      break;
    }
    # endif // if FEATURE_I2C_GET_ADDRESS

    case PLUGIN_WEBFORM_SHOW_GPIO_DESCR:
    {
      string  = F("Btn: ");
      string += formatGpioLabel(CONFIG_PIN3, false);
      success = true;
      break;
    }

    case PLUGIN_SET_DEFAULTS:
    {
      PCONFIG(4) = 1; // Default to Normal font width
      break;
    }

    case PLUGIN_WEBFORM_LOAD:
    {
      const int controllerValues[2] = { 0, 1 };
      OLedFormController(F("use_sh1106"), controllerValues, PCONFIG(5));

      OLedFormRotation(F("rotate"), PCONFIG(1));

      {
        const int optionValues3[3] = { 1, 3, 2 };
        OLedFormSizes(F("size"), optionValues3, PCONFIG(3));
      }
      {
        const __FlashStringHelper *options4[2] = { F("Normal"), F("Optimized") };
        const int optionValues4[2]             = { 1, 2 };
        addFormSelector(F("Font Width"), F("font_spacing"), 2, options4, optionValues4, PCONFIG(4));
      }
      {
        String strings[P23_Nlines];
        LoadCustomTaskSettings(event->TaskIndex, strings, P23_Nlines, P23_Nchars);

        for (int varNr = 0; varNr < 8; varNr++)
        {
          addFormTextBox(concat(F("Line "), varNr + 1), getPluginCustomArgName(varNr), strings[varNr], 64);
        }
      }

      // FIXME TD-er: Why is this using pin3 and not pin1? And why isn't this using the normal pin selection functions?
      addFormPinSelect(PinSelectPurpose::Generic_input, formatGpioName_input_optional(F("Display button")), F("taskdevicepin3"), CONFIG_PIN3);

      addFormNumericBox(F("Display Timeout"), F("ptimer"), PCONFIG(2));

      success = true;
      break;
    }

    case PLUGIN_WEBFORM_SAVE:
    {
      PCONFIG(0) = getFormItemInt(F("i2c_addr"));
      PCONFIG(1) = getFormItemInt(F("rotate"));
      PCONFIG(2) = getFormItemInt(F("ptimer"));
      PCONFIG(3) = getFormItemInt(F("size"));
      PCONFIG(4) = getFormItemInt(F("font_spacing"));
      PCONFIG(5) = getFormItemInt(F("use_sh1106"));


      // FIXME TD-er: This is a huge stack allocated object.
      char   deviceTemplate[P23_Nlines][P23_Nchars] = {};
      String error;

      for (uint8_t varNr = 0; varNr < P23_Nlines; varNr++) {
        if (!safe_strncpy(deviceTemplate[varNr], webArg(getPluginCustomArgName(varNr)), P23_Nchars)) {
          error += getCustomTaskSettingsError(varNr);
        }
      }

      if (error.length() > 0) {
        addHtmlError(error);
      }
      SaveCustomTaskSettings(event->TaskIndex, reinterpret_cast<const uint8_t *>(&deviceTemplate), sizeof(deviceTemplate));
      success = true;
      break;
    }

    case PLUGIN_INIT:
    {
      uint8_t type                           = 0;
      P023_data_struct::Spacing font_spacing = P023_data_struct::Spacing::normal;


      switch (PCONFIG(3)) {
        case 1:
          // 128x64
          type = P023_data_struct::OLED_128x64;
          break;
        case 2:
          type = P023_data_struct::OLED_64x48;
          break;
        case 3:
          type = P023_data_struct::OLED_128x32;
          break;
      }

      if (PCONFIG(1) == 2) {
        type |= P023_data_struct::OLED_rotated;
      }

      if (PCONFIG(4) > 0) {
        font_spacing = static_cast<P023_data_struct::Spacing>(PCONFIG(4));
      }

      initPluginTaskData(event->TaskIndex, new (std::nothrow) P023_data_struct(PCONFIG(0), type, font_spacing, PCONFIG(2), PCONFIG(5)));
      P023_data_struct *P023_data = static_cast<P023_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P023_data) {
        P023_data->StartUp_OLED(event);
        P023_data->clearDisplay();

        if (PCONFIG(1) == 2) {
          P023_data->sendCommand(0xA0 | 0x1); // SEGREMAP   //Rotate screen 180 deg
          P023_data->sendCommand(0xC8);       // COMSCANDEC  Rotate screen 180 Deg
        }

        P023_data->sendStrXY("ESP Easy ", 0, 0);

        if (validGpio(CONFIG_PIN3)) {
          pinMode(CONFIG_PIN3, INPUT_PULLUP);
        }
        success = true;
      }
      break;
    }

    case PLUGIN_TEN_PER_SECOND:
    {
      if (validGpio(CONFIG_PIN3)) {
        if (!digitalRead(CONFIG_PIN3)) {
          P023_data_struct *P023_data = static_cast<P023_data_struct *>(getPluginTaskData(event->TaskIndex));

          if (nullptr != P023_data) {
            P023_data->setDisplayTimer(PCONFIG(2));
          }
        }
      }
      break;
    }

    case PLUGIN_ONCE_A_SECOND:
    {
      P023_data_struct *P023_data = static_cast<P023_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P023_data) {
        P023_data->checkDisplayTimer();
      }
      break;
    }

    case PLUGIN_READ:
    {
      P023_data_struct *P023_data = static_cast<P023_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P023_data) {
        P023_data->plugin_read(event);
      }
      success = false;
      break;
    }

    case PLUGIN_WEBFORM_SHOW_VALUES:
    {
      P023_data_struct *P023_data =
        static_cast<P023_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P023_data) {
        success = P023_data->web_show_values();
      }
      break;
    }

    case PLUGIN_WRITE:
    {
      P023_data_struct *P023_data = static_cast<P023_data_struct *>(getPluginTaskData(event->TaskIndex));

      if (nullptr != P023_data) {
        success = P023_data->plugin_write(event, string);
      }
      break;
    }
  }
  return success;
}

#endif // USES_P023