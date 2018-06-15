#ifndef FUSI_SDK_H
#define FUSI_SDK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#if defined(BUILDING_FUSI_SHARED)
#define SDK_EXTERN __declspec(dllexport)
#elif defined(USING_FUSI_SHARED)
#define SDK_EXTERN __declspec(dllimport)
#else
#define SDK_EXTERN
#endif
#else
#define SDK_EXTERN
#endif

typedef struct FusiDevice FusiDevice;

typedef struct {
    const char* mac;
    const char* name;
    const char* ip;
}FusiDeviceInfo;

typedef enum{
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
typedef void (*on_eeg_data)(const char* device_mac, EEGData* data);
typedef void (*on_eeg_stats)(const char* device_mac, EEGStats* stats);
typedef void (*on_oriention_change)(const char* device_mac, DeviceOrientation state);
typedef void (*on_error) (const char* device_mac, FusiError* err);
typedef void (*on_device_connection_change)(const char* device_mac, DeviceConnectionState event);
typedef void (*on_device_contact_state_change)(const char* device_mac, DeviceContactState state);

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
SDK_EXTERN char* get_name(FusiDevice* device);
SDK_EXTERN DeviceOrientation get_device_orientation(FusiDevice* device);
SDK_EXTERN DeviceContactState get_device_contact_state(FusiDevice* device);

// sync set callback operation
SDK_EXTERN int set_attention_callback(FusiDevice* device, on_attention cb);
SDK_EXTERN int set_eeg_data_callback(FusiDevice* device, on_eeg_data cb);
SDK_EXTERN int set_eeg_stats_callback(FusiDevice* device, on_eeg_stats cb);
SDK_EXTERN int set_device_oriention_callback(FusiDevice* device, on_oriention_change cb);
SDK_EXTERN int set_error_callback(FusiDevice* device, on_error cb);
SDK_EXTERN int set_device_connection_callback(FusiDevice* device, on_device_connection_change cb);
SDK_EXTERN int set_device_contact_state_callback(FusiDevice* device, on_device_contact_state_change cb);

// async device wifi setting operation
SDK_EXTERN int fusi_switch_wifi(FusiDeviceInfo info, WifiInfo winfo, on_switched_wifi cb);

// device AT cmd operations wifi setting operation
SDK_EXTERN int fusi_scan_wifi(FusiDeviceInfo info, on_wifi_scan_finished cb);
SDK_EXTERN int fusi_restart(FusiDeviceInfo info);

// SDK_EXTERN int fusi_config_wifi(FusiDeviceInfo device_info, WifiInfo wifi_info, const char* pswd);
SDK_EXTERN int fusi_check_wifi_connectivity(FusiDeviceInfo device_info);
SDK_EXTERN int fusi_set_station_mode(FusiDeviceInfo info, DeviceStationMode mode);

// async operation
SDK_EXTERN int fusi_devices_search(on_found_fusi_devices cb, int interval_ms);
SDK_EXTERN int fusi_connect(FusiDevice* device, on_device_connection_change cb, int auto_reconnect);
SDK_EXTERN int fusi_disconnect(FusiDevice*);
SDK_EXTERN int fusi_set_forehead_led_color(FusiDevice* device, int color);

SDK_EXTERN int hello(int num);

#ifdef __cplusplus
}
#endif
#endif
