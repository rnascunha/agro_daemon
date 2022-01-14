#ifndef AGRO_DAEMON_DEVICE_ESP_RESET_REASON_HPP__
#define AGRO_DAEMON_DEVICE_ESP_RESET_REASON_HPP__

/**
 * Reset reasons
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system.html?highlight=reset%20reason#_CPPv418esp_reset_reason_t
 */
enum class esp_reset_reason_t{
	ESP_RST_UNKNOWN = 0, 	///< Reset reason can not be determined.
	ESP_RST_POWERON,		///< Reset due to power-on event.
	ESP_RST_EXT, 			///< Reset by external pin (not applicable for ESP32)
	ESP_RST_SW,				///< Software reset via esp_restart.
	ESP_RST_PANIC,			///< Software reset due to exception/panic.
	ESP_RST_INT_WDT,		///< Reset (software or hardware) due to interrupt watchdog.
	ESP_RST_TASK_WDT,		///< Reset due to task watchdog.
	ESP_RST_WDT,			///< Reset due to other watchdogs.
	ESP_RST_DEEPSLEEP,		///< Reset after exiting deep sleep mode.
	ESP_RST_BROWNOUT,		///< Brownout reset (software or hardware)
	ESP_RST_SDIO			///< Reset over SDIO.
};

const char* reset_reason_string(esp_reset_reason_t) noexcept;
const char* reset_reason_string_short(esp_reset_reason_t) noexcept;

#endif /* AGRO_DAEMON_DEVICE_ESP_RESET_REASON_HPP__ */


