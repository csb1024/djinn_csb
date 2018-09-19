SET(FLANDMARK_BASE_DIR "/home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark")
SET(FLANDMARK_BINARY_DIR "/home/sbchoi/org/djinn-1.0/tonic-suite/img/flandmark-master/libflandmark")

SET(FLANDMARK_INCLUDE_DIRS ${FLANDMARK_BASE_DIR})

SET(FLANDMARK_LINK_DIRS ${FLANDMARK_BINARY_DIR} ${FLANDMARK_BINARY_DIR}/Release)

SET(FLANDMARK_LIBRARIES flandmark_shared)

SET(FLANDMARK_FOUND 1)

