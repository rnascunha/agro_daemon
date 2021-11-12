#include "esp_reset_reason.hpp"

const char* reset_reason_string(esp_reset_reason_t reason) noexcept
{
	switch(reason)
	{
		case esp_reset_reason_t::ESP_RST_UNKNOWN:
			return "not determined";
		case esp_reset_reason_t::ESP_RST_POWERON:
			return "power-on event";
		case esp_reset_reason_t::ESP_RST_EXT:
			return "external pin";
		case esp_reset_reason_t::ESP_RST_SW:
			return "software reset";
		case esp_reset_reason_t::ESP_RST_PANIC:
			return "exception/panic";
		case esp_reset_reason_t::ESP_RST_INT_WDT:
			return "interrupt watchdog";
		case esp_reset_reason_t::ESP_RST_TASK_WDT:
			return "task watchdog";
		case esp_reset_reason_t::ESP_RST_WDT:
			return "other watchdogs";
		case esp_reset_reason_t::ESP_RST_DEEPSLEEP:
			return "exiting deep sleep";
		case esp_reset_reason_t::ESP_RST_BROWNOUT:
			return "brownout";
		case esp_reset_reason_t::ESP_RST_SDIO:
			return "over SDIO";
		default:
		break;
	}

	return "not recognized";
}

const char* reset_reason_string_short(esp_reset_reason_t reason) noexcept
{
	switch(reason)
	{
		case esp_reset_reason_t::ESP_RST_UNKNOWN:
			return "ESP_RST_UNKNOWN";
		case esp_reset_reason_t::ESP_RST_POWERON:
			return "ESP_RST_POWERON";
		case esp_reset_reason_t::ESP_RST_EXT:
			return "ESP_RST_EXT";
		case esp_reset_reason_t::ESP_RST_SW:
			return "sESP_RST_SW";
		case esp_reset_reason_t::ESP_RST_PANIC:
			return "ESP_RST_PANIC";
		case esp_reset_reason_t::ESP_RST_INT_WDT:
			return "ESP_RST_INT_WDT";
		case esp_reset_reason_t::ESP_RST_TASK_WDT:
			return "ESP_RST_TASK_WDT";
		case esp_reset_reason_t::ESP_RST_WDT:
			return "ESP_RST_WDT";
		case esp_reset_reason_t::ESP_RST_DEEPSLEEP:
			return "ESP_RST_DEEPSLEEP";
		case esp_reset_reason_t::ESP_RST_BROWNOUT:
			return "ESP_RST_BROWNOUT";
		case esp_reset_reason_t::ESP_RST_SDIO:
			return "ESP_RST_SDIO";
		default:
		break;
	}

	return "not recognized";
}
