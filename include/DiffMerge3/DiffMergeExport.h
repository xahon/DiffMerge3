
#ifndef DIFFMERGE3_EXPORT_H
#define DIFFMERGE3_EXPORT_H

#ifdef DIFFMERGE3_STATIC_DEFINE
#  define DIFFMERGE3_EXPORT
#  define DIFFMERGE3_NO_EXPORT
#else
#  ifndef DIFFMERGE3_EXPORT
#    ifdef DiffMerge3_EXPORTS
        /* We are building this library */
#      define DIFFMERGE3_EXPORT 
#    else
        /* We are using this library */
#      define DIFFMERGE3_EXPORT 
#    endif
#  endif

#  ifndef DIFFMERGE3_NO_EXPORT
#    define DIFFMERGE3_NO_EXPORT 
#  endif
#endif

#ifndef DIFFMERGE3_DEPRECATED
#  define DIFFMERGE3_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef DIFFMERGE3_DEPRECATED_EXPORT
#  define DIFFMERGE3_DEPRECATED_EXPORT DIFFMERGE3_EXPORT DIFFMERGE3_DEPRECATED
#endif

#ifndef DIFFMERGE3_DEPRECATED_NO_EXPORT
#  define DIFFMERGE3_DEPRECATED_NO_EXPORT DIFFMERGE3_NO_EXPORT DIFFMERGE3_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef DIFFMERGE3_NO_DEPRECATED
#    define DIFFMERGE3_NO_DEPRECATED
#  endif
#endif

#ifndef DIFFMERGE_INLINE
    #define DIFFMERGE_INLINE inline
#endif

#endif /* DIFFMERGE3_EXPORT_H */
