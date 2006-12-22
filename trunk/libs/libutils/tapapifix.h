#ifndef __FIXTAPAPI_H
#define __FIXTAPAPI_H

#ifdef  __cplusplus
extern "C" {
#endif

void InitTAPAPIFix();

// missing items
#define ATTR_PARENTFOLDER	0xf0
#define ATTR_THISFOLDER		0xf1
#define ATTR_RECYCLEFOLDER	0xf3
#define ATTR_DELETEDFILE	0xff

#ifdef  __cplusplus
}
#endif

#endif
