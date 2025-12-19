#ifndef _USB_STRUCT_H_
#define _USB_STRUCT_H_

#include <linux/types.h>
#include <linux/byteorder/generic.h>

#define DP_MAGIC        0x12345678u
#define DP_HDR_SIZE     7
#define DP_MAX_PAYLOAD  200

/* info: [7:4]=version(4bit), [3]=type(1bit), [2]=is_end(1bit), [1:0]=0 */
#define DP_INFO_VER_SHIFT 4
#define DP_INFO_VER_MASK  0xF0
#define DP_INFO_TYPE_MASK 0x08
#define DP_INFO_END_MASK  0x04

static inline u8 dp_get_ver(u8 info)  { return (info & DP_INFO_VER_MASK) >> DP_INFO_VER_SHIFT; }
static inline u8 dp_get_type(u8 info) { return (info & DP_INFO_TYPE_MASK) ? 1 : 0; }
static inline u8 dp_get_end(u8 info)  { return (info & DP_INFO_END_MASK)  ? 1 : 0; }

struct dp_hdr_wire {
    __le32 magic;   /* little-endian on wire */
    u8     info;
    __le16 cmd_len; /* payload length in BYTES: 0..DP_MAX_PAYLOAD */
} __attribute__((packed));

#endif /* _USB_STRUCT_H_ */