#ifndef AGRO_DAEMON_HELPER_PACKED_H__
#define AGRO_DAEMON_HELPER_PACKED_H__

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#else
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACKED
#else 
#define PACKED __attribute__((packed))
#endif

#endif /* AGRO_DAEMON_HELPER_PACKED_H__ */