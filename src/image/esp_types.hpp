#ifndef AGRO_DAEMON_OTA_TYPES_HPP__
#define AGRO_DAEMON_OTA_TYPES_HPP__

#include <cstdint>
#include "../helper/packed.h"

/**
 * Types copied from 'esp_app_format.h' of ESP-IDF
 */

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif /* _MSC_VER */
/**
 * @brief ESP chip ID
 *
 */
typedef enum {
    ESP_CHIP_ID_ESP32 = 0x0000,  /*!< chip ID: ESP32 */                                                                /**< ESP_CHIP_ID_ESP32 */
    ESP_CHIP_ID_ESP32S2 = 0x0002, /*!< chip ID: ESP32-S2 */                                                           /**< ESP_CHIP_ID_ESP32S2 */
    ESP_CHIP_ID_ESP32S3 = 0x0004, /*!< chip ID: ESP32-S3 */                                                            /**< ESP_CHIP_ID_ESP32S3 */
    ESP_CHIP_ID_ESP32C3 = 0x0005, /*!< chip ID: ESP32-C3 */                                                            /**< ESP_CHIP_ID_ESP32C3 */
    ESP_CHIP_ID_INVALID = 0xFFFF /*!< Invalid chip ID (we defined it to make sure the esp_chip_id_t is 2 bytes size) *//**< ESP_CHIP_ID_INVALID */
} PACKED esp_chip_id_t;

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */

/**
 * @brief Main header of binary image
 */
PACK(typedef struct {
    uint8_t magic;              /*!< Magic word ESP_IMAGE_HEADER_MAGIC */
    uint8_t segment_count;      /*!< Count of memory segments */
    uint8_t spi_mode;           /*!< flash read mode (esp_image_spi_mode_t as uint8_t) */
    uint8_t spi_speed: 4;       /*!< flash frequency (esp_image_spi_freq_t as uint8_t) */
    uint8_t spi_size: 4;        /*!< flash chip size (esp_image_flash_size_t as uint8_t) */
    uint32_t entry_addr;        /*!< Entry address */
    uint8_t wp_pin;            /*!< WP pin when SPI pins set via efuse (read by ROM bootloader,
                                * the IDF bootloader uses software to configure the WP
                                * pin and sets this field to 0xEE=disabled) */
    uint8_t spi_pin_drv[3];     /*!< Drive settings for the SPI flash pins (read by ROM bootloader) */
    esp_chip_id_t chip_id;      /*!< Chip identification number */
    uint8_t min_chip_rev;       /*!< Minimum chip revision supported by image */
    uint8_t reserved[8];       /*!< Reserved bytes in additional header space, currently unused */
    uint8_t hash_appended;      /*!< If 1, a SHA256 digest "simple hash" (of the entire image) is appended after the checksum.
                                 * Included in image length. This digest
                                 * is separate to secure boot and only used for detecting corruption.
                                 * For secure boot signed images, the signature
                                 * is appended after this (and the simple hash is included in the signed data). */
}) esp_image_header_t;

/**
 * @brief Header of binary image segment
 */
typedef struct {
    uint32_t load_addr;     /*!< Address of segment */
    uint32_t data_len;      /*!< Length of data */
} esp_image_segment_header_t;

/**
 * @brief Description about application.
 */
typedef struct {
    std::uint32_t magic_word;        /*!< Magic word ESP_APP_DESC_MAGIC_WORD */
    std::uint32_t secure_version;    /*!< Secure version */
    std::uint32_t reserv1[2];        /*!< reserv1 */
    char version[32];           /*!< Application version */
    char project_name[32];      /*!< Project name */
    char time[16];              /*!< Compile time */
    char date[16];              /*!< Compile date*/
    char idf_ver[32];           /*!< Version IDF */
    std::uint8_t app_elf_sha256[32]; /*!< sha256 of elf file */
    std::uint32_t reserv2[20];       /*!< reserv2 */
} esp_app_desc_t;

// #undef PACK
// #undef PACKED

#endif /* AGRO_DAEMON_OTA_TYPES_HPP__ */
