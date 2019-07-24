#ifndef FUSI_SDK_H
#define FUSI_SDK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#ifdef BUILDING_FUSI_SHARED
#define SDK_EXTERN __declspec(dllexport)
#elif USING_FUSI_SHARED
#define SDK_EXTERN __declspec(dllimport)
#else
#define SDK_EXTERN
#endif
#else
#define SDK_EXTERN
#endif

//CFFI_DEF_START
typedef struct FusiDevice FusiDevice;

typedef struct {
    const char* mac;
    const char* name;
    const char* ip;
}FusiDeviceInfo;

typedef enum{
    DEVICE_MODE_OTA = -1,
    DEVICE_MODE_EEG = 0,
    DEVICE_MODE_IMPEDANCE_TEST_REF = 1,
    DEVICE_MODE_IMPEDANCE_TEST_RLD = 2
}DeviceMode;

typedef enum{
    DEVICE_STATION_MODE_AP = 0,
    DEVICE_STATION_MODE_STA = 1,
    DEVICE_STATION_MODE_APSTA = 2
}DeviceStationMode;

typedef struct{
    int size;
    double* data;
    double sample_rate;
    double pga;
}EEGData;

typedef struct{
    FusiDevice* device;    
    double delta;
    double theta;
    double alpha;
    double low_beta;
    double high_beta;
    double gamma;
}EEGStats;

typedef enum{
    ORIENTATION_UNKNOWN = 0,
    ORIENTATION_BOTTOM_UP = 1,
    ORIENTATION_LEFT_ARM_END_UP = 2,
    ORIENTATION_LEFT_ARM_END_DOWN = 3,
    ORIENTATION_TOP_UP = 4,
    ORIENTATION_LEFT_FACE_UP = 5,
    ORIENTATION_LEFT_FACE_DOWN = 6
}DeviceOrientation;

typedef struct {
    FusiDevice* device;    
    int code;
    const char* desc;
}FusiError;

typedef enum{
    CONNECTING = 0,
    CONNECTED = 1,
    INTERRUPTED = 2,
    DISCONNECTED = 3
}DeviceConnectionState;

typedef enum{
    CONTACT = 0,
    NO_CONTACT = 1,
    CHECKING_CONTACT_STATE = 2
}DeviceContactState;

typedef struct {
    const char* ssid;
    const char* mac;
    const char* encryption;
    const char* password;
}WifiInfo;

typedef enum {
    ERROR_FW_GENERAL = -13,
    ERROR_FW_DATA = -12,
    ERROR_FW_DATA_OTHER = -11,
    ERROR_FW_INFO = -10,
    ERROR_FW_INFO_OTHER = -9,
    ERROR_PACKET_PARSING = -8,
    ERROR_OTA_CRC = -7,
    ERROR_OTA_PARAM = -6,
    ERROR_OTA_CMD = -5,
    ERROR_OTA_AP_NOT_ALLOWED = -4,
    ERROR_OTA_DISABLED = -3,
    ERROR_BATTERY_LOW = -2,
    ERROR_CONNECTION_TIMEOUT = -1,
    OTA_BEGIN = 0,
    OTA_PREPARING = 1,
    OTA_CHECKING_FIRMWARE_INFO = 2,
    OTA_TRANSMITING_FIRMWARE_DATA = 3,
    OTA_COMPLETE = 4
}OTAStatus;

typedef void (*on_switched_wifi)(int result);

// if count == 0, no wifi
// else if count > 0, wifi pointer to an array of wifi
// else count < 0, error pointer to the FusiError
typedef void (*on_wifi_scan_finished)(WifiInfo* wifi, int count, FusiError* error);

// if count == 0, no device
// else if count > 0, device pointer to an array of device
// else count < 0, error pointer to the FusiError
typedef void (*on_found_fusi_devices)(FusiDeviceInfo* device, int count, FusiError* error);

// Device callbacks
typedef void (*on_attention)(const char* device_mac, double attention);
typedef void (*on_meditation)(const char* device_mac, double meditation);
typedef void (*on_blink)(const char* device_mac); //, int is_heavy_blink);
typedef void (*on_eeg_data)(const char* device_mac, EEGData* data);
typedef void (*on_eeg_stats)(const char* device_mac, EEGStats* stats);
typedef void (*on_oriention_change)(const char* device_mac, DeviceOrientation state);
typedef void (*on_error) (const char* device_mac, FusiError* err);
typedef void (*on_device_connection_change)(const char* device_mac, DeviceConnectionState event);
typedef void (*on_device_contact_state_change)(const char* device_mac, DeviceContactState state);

typedef void (*on_log)(const char* msg);


// Firmware update callbacks
// typedef void (*on_firmware_update_log)(const char* device_mac, const char* log);
typedef void (*on_firmware_update_status)(const char* device_mac, OTAStatus status, int progress);

// sync operation
SDK_EXTERN FusiDevice* fusi_device_create(FusiDeviceInfo info);
SDK_EXTERN int fusi_device_dispose(FusiDevice* device);

// sync get operation
SDK_EXTERN double get_battery_level(FusiDevice* device);
SDK_EXTERN double get_alpha_peak(FusiDevice* device);
SDK_EXTERN DeviceMode get_device_mode(FusiDevice* device);
SDK_EXTERN int is_impedance_test_enabled(FusiDevice* device);
SDK_EXTERN void set_impedance_test_enabled(FusiDevice* device, int enabled);
SDK_EXTERN int is_charging(FusiDevice* device);
SDK_EXTERN char* get_mac(FusiDevice* device);
SDK_EXTERN char* get_ip(FusiDevice* device);
SDK_EXTERN int set_ip(FusiDevice* device, const char* ip);

SDK_EXTERN char* get_name(FusiDevice* device);
SDK_EXTERN char* get_firmware_info(FusiDevice* device);
SDK_EXTERN char* get_firmware_version(FusiDevice* device);
SDK_EXTERN DeviceOrientation get_device_orientation(FusiDevice* device);
SDK_EXTERN DeviceContactState get_device_contact_state(FusiDevice* device);

// sync set callback operation
SDK_EXTERN int set_attention_callback(FusiDevice* device, on_attention cb);
SDK_EXTERN int set_meditation_callback(FusiDevice* device, on_meditation cb);
SDK_EXTERN int set_eeg_data_callback(FusiDevice* device, on_eeg_data cb);
SDK_EXTERN int set_eeg_stats_callback(FusiDevice* device, on_eeg_stats cb);
SDK_EXTERN int set_device_oriention_callback(FusiDevice* device, on_oriention_change cb);
SDK_EXTERN int set_error_callback(FusiDevice* device, on_error cb);
SDK_EXTERN int set_device_connection_callback(FusiDevice* device, on_device_connection_change cb);
SDK_EXTERN int set_device_contact_state_callback(FusiDevice* device, on_device_contact_state_change cb);
SDK_EXTERN int set_blink_callback(FusiDevice* device, on_blink cb);
SDK_EXTERN int set_log_callback(on_log cb);

// sync set firmware update callbacks
SDK_EXTERN int set_device_firmware_update_status_callback(FusiDevice* device, on_firmware_update_status cb);

// async device wifi setting operation
SDK_EXTERN int fusi_switch_wifi(FusiDeviceInfo info, WifiInfo winfo, on_switched_wifi cb);

// device AT cmd operations wifi setting operation
SDK_EXTERN int fusi_scan_wifi(FusiDeviceInfo info, on_wifi_scan_finished cb);
SDK_EXTERN int fusi_restart_device(FusiDeviceInfo info);

// SDK_EXTERN int fusi_config_wifi(FusiDeviceInfo device_info, WifiInfo wifi_info, const char* pswd);
SDK_EXTERN int fusi_check_wifi_connectivity(FusiDeviceInfo device_info);
SDK_EXTERN int fusi_set_station_mode(FusiDeviceInfo info, DeviceStationMode mode);

// async operation
SDK_EXTERN int fusi_devices_search(on_found_fusi_devices cb, int interval_ms);
SDK_EXTERN int fusi_connect(FusiDevice* device, on_device_connection_change cb, int auto_reconnect);
SDK_EXTERN int fusi_disconnect(FusiDevice*);
SDK_EXTERN int fusi_disconnect_ota(FusiDevice* device);
SDK_EXTERN int fusi_set_forehead_led_color(FusiDevice* device, int color);

// Firmware update APIs
SDK_EXTERN int fusi_update_firmware(FusiDevice* device,const char* firmware_data,int size, on_firmware_update_status status_cb); //, on_firmware_update_log log_cb
// SDK_EXTERN int fusi_ota_send_firmware_info(FusiDevice* device, int size, int crc);

SDK_EXTERN int hello(int num);
//CFFI_DEF_END

#ifdef __cplusplus
}
#endif
#endif
