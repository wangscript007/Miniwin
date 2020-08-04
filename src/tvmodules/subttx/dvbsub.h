/* The object definition gives the position of the object in a region [7.2.5] */
#ifndef __DVB_SUBTITLE_H__
#define __DVB_SUBTITLE_H__
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <vlc_bits.h>

#ifdef __cplusplus
extern "C"{
#endif
typedef int64_t vlc_tick_t;
typedef struct dvbsub_objectdef_s {
    int i_id;
    int i_type;
    int i_x;
    int i_y;
    int i_fg_pc;
    int i_bg_pc;
    char *psz_text; /* for string of characters objects */

} dvbsub_objectdef_t;

/* The entry in the palette CLUT */
typedef struct dvbsub_color_s{
    uint8_t                 Y;
    uint8_t                 Cr;
    uint8_t                 Cb;
    uint8_t                 T;

} dvbsub_color_t;

/* The displays dimensions [7.2.1] */
typedef struct dvbsub_display_s {
    uint8_t                 i_id;
    uint8_t                 i_version;

    int                     i_width;
    int                     i_height;

    bool                    b_windowed;
    /* these values are only relevant if windowed */
    int                     i_x;
    int                     i_y;
    int                     i_max_x;
    int                     i_max_y;

} dvbsub_display_t;

/* [7.2.4] */
typedef struct dvbsub_clut_s {
    uint8_t                 i_id;
    uint8_t                 i_version;
    dvbsub_color_t          c_2b[4];
    dvbsub_color_t          c_4b[16];
    dvbsub_color_t          c_8b[256];

    struct dvbsub_clut_s    *p_next;

} dvbsub_clut_t;

/* The Region is an aera on the image [7.2.3]
 * with a list of the object definitions associated and a CLUT */
typedef struct dvbsub_region_s {
    int i_id;
    int i_version;
    int i_x;
    int i_y;
    int i_width;
    int i_height;
    int i_level_comp;
    int i_depth;
    int i_clut;

    uint8_t *p_pixbuf;

    int                    i_object_defs;
    dvbsub_objectdef_t     *p_object_defs;

    struct dvbsub_region_s *p_next;

} dvbsub_region_t;

/* The object definition gives the position of the object in a region */
typedef struct dvbsub_regiondef_s {
    int i_id;
    int i_x;
    int i_y;

} dvbsub_regiondef_t;

/* The page defines the list of regions [7.2.2] */
typedef struct {
    int i_id;
    int i_timeout; /* in seconds */
    int i_state;
    int i_version;

    int                i_region_defs;
    dvbsub_regiondef_t *p_region_defs;

} dvbsub_page_t;

typedef struct {
    bs_t               bs;

    /* Decoder internal data */
    int                i_id;
    int                i_ancillary_id;
    vlc_tick_t         i_pts;

    bool               b_absolute;
    int                i_spu_position;
    int                i_spu_x;
    int                i_spu_y;

    bool               b_page;
    dvbsub_page_t      *p_page;
    dvbsub_region_t    *p_regions;
    dvbsub_clut_t      *p_cluts;
    /* this is very small, so keep forever */
    dvbsub_display_t   display;
    dvbsub_clut_t      default_clut;
} decoder_sys_t;

typedef struct block_s{
    uint8_t*p_buffer; /**< Payload start */
    size_t i_buffer; /**< Payload length */
    size_t i_size; /**< Buffer total size */
    uint32_t i_flags;
    int64_t i_pts;
    int64_t i_dts;
}block_t;

typedef struct subpicture_s subpicture_t;
struct subpicture_s{
    int          i_channel;       /**< subpicture channel ID */
    int64_t      i_order;         /** an increasing unique number */
    //subpicture_t*p_next;          /**< next subtitle to be displayed */

    //subpicture_region_t *p_region;/**< region list composing this subtitle */
    int64_t      i_start;         /**< beginning of display date */
    int64_t      i_stop;          /**< end of display date */
    bool         b_ephemer;       /**< If this flag is set to true the subtitle  will be displayed until the next one appear */
    bool         b_fade;          /**< enable fading */
    bool         b_subtitle;      /**< the picture is a movie subtitle */
    bool         b_absolute;      /**< position is absolute */
    int          i_original_picture_width;  /**< original width of the movie */
    int          i_original_picture_height;/**< original height of the movie */
    int          i_alpha;                     
};
typedef  struct decoder_s decoder_t;
struct decoder_s {
    //VLC_COMMON_MEMBERS
    /* Module properties */
    //module_t *          p_module;
    decoder_sys_t *     p_sys;
    /* Input format ie from demuxer (XXX: a lot of field could be invalid) */
    //es_format_t         fmt_in;
    /* Output format of decoder/packetizer */
    //es_format_t         fmt_out;
    int(*pf_decode)(decoder_t *, block_t *p_block);
    void(*pf_flush)(decoder_t *);
    int(*pf_queue_sub)(decoder_t *, subpicture_t *);
    int(*pf_render)( decoder_t *p_dec,void*userdata);
    void*render_userdata;
};

/*****************************************************************************/
int  dvbsub_open( decoder_t *dec,int page_id,int ancillary_id );
void dvbsub_setrender(decoder_t*dec,int(*)(decoder_t*,void*),void*userdata);
void dvbsub_close( decoder_t * );
void dvbsub_flush( decoder_t *p_dec );
int  dvbsub_decode( decoder_t *, block_t * );
bool decode_time_stamp(int64_t *ts, const uint8_t *buf,uint32_t marker);
uint32_t dvbsub_ycbcr2argb(dvbsub_color_t*c);
#ifdef __cplusplus
}
#endif
#endif//
