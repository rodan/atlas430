#ifndef __WARNING_H__
#define __WARNING_H__

/**  quick fix for the following compiler warning:

warning: ISO C forbids an empty translation unit [-Wpedantic]

*/
extern char nonempty_translation_unit;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
///\}
