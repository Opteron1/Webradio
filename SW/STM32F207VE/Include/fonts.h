#ifndef _FONTS_H_
#define _FONTS_H_	1U

/*----- DEFINES -----*/
#define SPECIALFONT					(0)
#define SPECIALFONT_NAME			special_char_8x8
#define SPECIALFONT_START			(0x00)
#define SPECIALFONT_WIDTH			(8)
#define SPECIALFONT_HEIGHT			(8)

#define SMALLFONT					(0)
#define SMALLFONT_NAME				ascii_8x8
#define SMALLFONT_START				(0x20)
#define SMALLFONT_WIDTH				(8)
#define SMALLFONT_HEIGHT			(8)

#define NORMALFONT					(1)
#define NORMALFONT_NAME				ascii_8x16
#define NORMALFONT_START      		(0x20)
#define NORMALFONT_WIDTH      		(8)
#define NORMALFONT_HEIGHT     		(16)

#define LARGEFONT					(2)
#define LARGEFONT_NAME				ascii_16x16
#define LARGEFONT_START				(0x20)
#define LARGEFONT_WIDTH				(16)
#define LARGEFONT_HEIGHT			(16)

#define DIGITFONT					(3)
#define DIGITFONT_NAME				font_digit
#define DIGITFONT_START				(0x30)
#define DIGITFONT_WIDTH				(16)
#define DIGITFONT_HEIGHT			(20)

/*----- GLOBALS -----*/
extern unsigned char const special_char_8x8[24];
extern const unsigned char ascii_8x8[784];
extern const unsigned char ascii_8x16[1536];
extern const unsigned char ascii_16x16[3040];
extern const unsigned char font_digit[440];



#endif	/* _FONTS_H_ */
