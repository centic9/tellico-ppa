########### next target ###############

SET(translators_STAT_SRCS
   adsimporter.cpp
   alexandriaexporter.cpp
   alexandriaimporter.cpp
   amcimporter.cpp
   audiofileimporter.cpp
   bibtexexporter.cpp
   bibteximporter.cpp
   bibtexmlexporter.cpp
   bibtexmlimporter.cpp
   boardgamegeekimporter.cpp
   ciwimporter.cpp
   collectorzimporter.cpp
   csvexporter.cpp
   csvimporter.cpp
   csvparser.cpp
   datacrowimporter.cpp
   dataimporter.cpp
   deliciousimporter.cpp
   discogsimporter.cpp
   ebookimporter.cpp
   exporter.cpp
   filelistingimporter.cpp
   freedb_util.cpp
   freedbimporter.cpp
   gcstarexporter.cpp
   gcstarimporter.cpp
   goodreadsimporter.cpp
   griffithimporter.cpp
   grs1importer.cpp
   htmlexporter.cpp
   importer.cpp
   librarythingimporter.cpp
   marcimporter.cpp
   onixexporter.cpp
   pdfimporter.cpp
   referencerimporter.cpp
   risimporter.cpp
   tellico_xml.cpp
   tellicoimporter.cpp
   tellicoxmlexporter.cpp
   tellicoxmlreader.cpp
   tellicozipexporter.cpp
   textimporter.cpp
   vinoxmlimporter.cpp
   xmlstatehandler.cpp
   xmphandler.cpp
   xsltexporter.cpp
   xslthandler.cpp
   xsltimporter.cpp
   )

add_library(translators STATIC ${translators_STAT_SRCS})
TARGET_LINK_LIBRARIES(translators
    collections
    tellicomodels
    core
    gui
    images
    utils
    ${TELLICO_BTPARSE_LIBS}
    ${TELLICO_CSV_LIBS}
    rtf2html-tellico
    )

TARGET_LINK_LIBRARIES(translators
    KF5::Archive
    KF5::JobWidgets
    KF5::Solid
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${LIBXSLT_EXSLT_LIBRARIES}
    )

IF( KF5Cddb_FOUND )
  TARGET_LINK_LIBRARIES(translators KF5::Cddb)
ELSEIF( Libkcddb_FOUND )
  TARGET_LINK_LIBRARIES(translators KDE::Libkcddb::kcddb)
ENDIF( KF5Cddb_FOUND)

IF( Poppler_Qt5_FOUND )
  TARGET_LINK_LIBRARIES(translators Poppler::Qt5)
ENDIF( Poppler_Qt5_FOUND )

IF( KF5FileMetaData_FOUND )
  TARGET_LINK_LIBRARIES(translators KF5::FileMetaData)
ENDIF( KF5FileMetaData_FOUND )

IF( Exempi_FOUND )
  TARGET_LINK_LIBRARIES(translators ${Exempi_LIBRARIES})
ENDIF( Exempi_FOUND )

IF( TAGLIB_FOUND )
  TARGET_LINK_LIBRARIES(translators ${TAGLIB_LIBRARIES})
ENDIF( TAGLIB_FOUND )

IF( CDIO_FOUND )
  TARGET_LINK_LIBRARIES(translators ${CDIO_LIBRARIES})
ENDIF( CDIO_FOUND )

ADD_DEPENDENCIES(translators tellico_config)

########### install files ###############

INSTALL(FILES bibtex-translation.xml DESTINATION ${TELLICO_DATA_INSTALL_DIR})
